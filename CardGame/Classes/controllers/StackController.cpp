#include "StackController.h"
#include "managers/UndoManager.h"
#include "views/StackView.h"
#include "views/CardView.h"
#include "models/GameModel.h"

USING_NS_CC;

StackController::StackController()
    : _model(nullptr)
    , _stackView(nullptr)
    , _undoManager(nullptr)
    , _isAnimating(false)
{
}

void StackController::init(GameModel* model, StackView* stackView)
{
    _model = model;
    _stackView = stackView;

    _stackView->setOnHandCardClicked([this](CardView* cv) {
        handleHandCardClick(cv);
    });
}

void StackController::handleHandCardClick(CardView* cardView)
{
    if (!_model || !_stackView) return;

    // Guard: reject clicks during animation
    if (_isAnimating) {
        CCLOG("StackController: card id=%d ignored (animating)", cardView->getCardId());
        return;
    }

    // Only the rightmost hand card can be used (requirement 1)
    CardView* rightmost = _stackView->getRightmostHandCard();
    if (cardView != rightmost) {
        CCLOG("StackController: card id=%d is not rightmost, ignored", cardView->getCardId());
        return;
    }

    _isAnimating = true;
    CardView* oldTop = _stackView->getTopCardView();

    // Record undo before animation
    if (_undoManager) {
        int oldTopId = oldTop ? oldTop->getCardId() : -1;
        Vec2 oldTopPos = oldTop ? oldTop->getPosition() : Vec2::ZERO;
        _undoManager->recordStackSwap(
            cardView->getCardId(),
            oldTopId,
            cardView->getPosition(),
            oldTopPos,
            _model->getTopCardIndex()
        );
    }

    // Target: top card position in StackView
    Vec2 targetPos = oldTop ? oldTop->getPosition() : _stackView->getPosition();

    auto* sv = _stackView;
    auto migrate = CallFunc::create([this, cardView, oldTop, sv]() {
        sv->removeHandCardSilent(cardView);

        if (oldTop) {
            sv->setTopCardSilent(nullptr);
            oldTop->setFaceUp(false);
            oldTop->setLocalZOrder(0);
            oldTop->setOnCardClicked(nullptr);
        }

        cardView->setOnCardClicked(nullptr);
        sv->setTopCardSilent(cardView);
        sv->layoutCards();

        // Update model
        int clickedIdx = -1;
        auto& stackCards = _model->getStackCards();
        for (size_t i = 0; i < stackCards.size(); ++i) {
            if (stackCards[i].getId() == cardView->getCardId()) {
                clickedIdx = static_cast<int>(i);
                break;
            }
        }
        if (clickedIdx >= 0) {
            _model->setTopCardIndex(clickedIdx);
        }

        _isAnimating = false;
        CCLOG("StackController: swap! hand card id=%d (face=%d) -> new top card, stackCards=%zu",
              cardView->getCardId(),
              static_cast<int>(cardView->getFace()),
              _model->getStackCards().size());
    });

    cardView->runAction(Sequence::create(
        EaseOut::create(MoveTo::create(0.3f, targetPos), 2.0f),
        migrate,
        nullptr));
}
