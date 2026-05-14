#include "PlayFieldController.h"
#include "managers/UndoManager.h"
#include "views/PlayFieldView.h"
#include "views/StackView.h"
#include "views/CardView.h"
#include "models/GameModel.h"

USING_NS_CC;

PlayFieldController::PlayFieldController()
    : _model(nullptr)
    , _playFieldView(nullptr)
    , _stackView(nullptr)
    , _undoManager(nullptr)
    , _isAnimating(false)
{
}

void PlayFieldController::init(GameModel* model, PlayFieldView* playFieldView, StackView* stackView)
{
    _model = model;
    _playFieldView = playFieldView;
    _stackView = stackView;

    _playFieldView->setOnCardClicked([this](CardView* cv) {
        handleCardClick(cv);
    });
}

void PlayFieldController::handleCardClick(CardView* cardView)
{
    if (!_model || !_playFieldView || !_stackView) return;

    // Guard: reject clicks during animation
    if (_isAnimating) {
        CCLOG("PlayFieldController: card id=%d ignored (animating)", cardView->getCardId());
        return;
    }

    if (!isCardClickable(cardView)) {
        CCLOG("PlayFieldController: card id=%d is covered, ignored", cardView->getCardId());
        return;
    }

    if (!canMatch(cardView)) {
        const CardModel* topCard = _model->getTopCard();
        CCLOG("PlayFieldController: card id=%d (face=%d) cannot match top card id=%d (face=%d)",
              cardView->getCardId(),
              static_cast<int>(cardView->getFace()),
              topCard ? topCard->getId() : -1,
              topCard ? static_cast<int>(topCard->getFace()) : -1);
        return;
    }

    _isAnimating = true;
    CardView* oldTop = _stackView->getTopCardView();

    // Record undo before animation
    if (_undoManager) {
        int oldTopId = oldTop ? oldTop->getCardId() : -1;
        Vec2 oldTopPos = oldTop ? oldTop->getPosition() : Vec2::ZERO;
        _undoManager->recordPlayFieldMatch(
            cardView->getCardId(),
            oldTopId,
            cardView->getPosition(),
            oldTopPos,
            _model->getTopCardIndex()
        );
    }

    // Convert target position (top card in StackView) to PlayFieldView coordinate space
    Vec2 targetWorld = _stackView->convertToWorldSpace(
        oldTop ? oldTop->getPosition() : Vec2::ZERO);
    Vec2 targetInPF = _playFieldView->convertToNodeSpace(targetWorld);

    auto* pf = _playFieldView;
    auto* sv = _stackView;
    auto migrate = CallFunc::create([this, cardView, oldTop, pf, sv]() {
        cardView->removeFromParentAndCleanup(false);
        sv->addChild(cardView);

        // Clear stale callback — card is no longer in play field
        cardView->setOnCardClicked(nullptr);

        pf->removeCardSilent(cardView);
        if (oldTop) {
            sv->setTopCardSilent(nullptr);
            oldTop->setFaceUp(false);
            oldTop->setLocalZOrder(0);
            oldTop->setOnCardClicked(nullptr);
        }
        sv->setTopCardSilent(cardView);
        sv->layoutCards();

        // Refresh clickability of remaining play-field cards
        pf->updateAllClickable();

        // Update model: save card data before erasing (pointer invalidated by erase)
        CardModel* clickedModel = _model->findCardById(cardView->getCardId());
        if (clickedModel) {
            CardModel saved = *clickedModel;  // copy before pointer is invalidated
            auto& pfCards = _model->getPlayFieldCards();
            pfCards.erase(std::remove_if(pfCards.begin(), pfCards.end(),
                [cardView](const CardModel& c) { return c.getId() == cardView->getCardId(); }),
                pfCards.end());

            saved.setInPlayField(false);
            _model->getStackCards().push_back(saved);
            _model->setTopCardIndex(static_cast<int>(_model->getStackCards().size()) - 1);
        }

        _isAnimating = false;
        CCLOG("PlayFieldController: match! card id=%d (face=%d) -> new top card, pfCards=%zu, stackCards=%zu",
              cardView->getCardId(),
              static_cast<int>(cardView->getFace()),
              _model->getPlayFieldCards().size(),
              _model->getStackCards().size());
    });

    cardView->runAction(Sequence::create(
        EaseOut::create(MoveTo::create(0.3f, targetInPF), 2.0f),
        migrate,
        nullptr));
}

bool PlayFieldController::isCardClickable(CardView* cardView) const
{
    return _playFieldView && _playFieldView->isCardClickable(cardView);
}

bool PlayFieldController::canMatch(const CardView* playFieldCard) const
{
    if (!_model) return false;
    const CardModel* topCard = _model->getTopCard();
    if (!topCard || !playFieldCard) return false;

    int diff = std::abs(static_cast<int>(playFieldCard->getFace()) - static_cast<int>(topCard->getFace()));
    return diff == 1;
}
