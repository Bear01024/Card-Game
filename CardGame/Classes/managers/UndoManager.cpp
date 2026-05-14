#include "UndoManager.h"
#include "models/GameModel.h"
#include "views/PlayFieldView.h"
#include "views/StackView.h"
#include "views/CardView.h"

USING_NS_CC;

UndoManager::UndoManager()
    : _model(nullptr)
    , _playFieldView(nullptr)
    , _stackView(nullptr)
    , _isUndoing(false)
{
}

void UndoManager::init(GameModel* model, PlayFieldView* playFieldView, StackView* stackView)
{
    _model = model;
    _playFieldView = playFieldView;
    _stackView = stackView;
}

void UndoManager::recordStackSwap(int clickedCardId, int oldTopCardId,
                                   const Vec2& clickedPos, const Vec2& topPos,
                                   int oldTopCardIndex)
{
    UndoRecord record;
    record.type = UndoActionType::StackSwap;
    record.cardAId = clickedCardId;
    record.cardBId = oldTopCardId;
    record.posABefore = clickedPos;
    record.posBBefore = topPos;
    record.oldTopCardIndex = oldTopCardIndex;
    record.cardAWasInPlayField = false;
    _undoModel.push(record);
    if (_onStateChanged) _onStateChanged(_undoModel.canUndo());
}

void UndoManager::recordPlayFieldMatch(int matchedCardId, int oldTopCardId,
                                        const Vec2& matchedPos, const Vec2& topPos,
                                        int oldTopCardIndex)
{
    UndoRecord record;
    record.type = UndoActionType::PlayFieldMatch;
    record.cardAId = matchedCardId;
    record.cardBId = oldTopCardId;
    record.posABefore = matchedPos;
    record.posBBefore = topPos;
    record.oldTopCardIndex = oldTopCardIndex;
    record.cardAWasInPlayField = true;
    _undoModel.push(record);
    if (_onStateChanged) _onStateChanged(_undoModel.canUndo());
}

bool UndoManager::canUndo() const
{
    return _undoModel.canUndo() && !_isUndoing;
}

void UndoManager::undo()
{
    if (!canUndo() || !_model || !_playFieldView || !_stackView) return;

    _isUndoing = true;
    UndoRecord record = _undoModel.pop();
    if (_onStateChanged) _onStateChanged(false);

    CCLOG("UndoManager: undo %s — cardA=%d, cardB=%d, oldTopIdx=%d, undoStack=%zu",
          record.type == UndoActionType::StackSwap ? "StackSwap" : "PlayFieldMatch",
          record.cardAId, record.cardBId, record.oldTopCardIndex, _undoModel.size());

    if (record.type == UndoActionType::StackSwap) {
        undoStackSwap(record);
    } else if (record.type == UndoActionType::PlayFieldMatch) {
        undoPlayFieldMatch(record);
    }
}

CardView* UndoManager::findCardView(int cardId) const
{
    // Search in stack view: top card
    CardView* top = _stackView->getTopCardView();
    if (top && top->getCardId() == cardId) return top;

    // Search in stack view: hand cards
    CardView* hand = _stackView->getHandCardById(cardId);
    if (hand) return hand;

    // Search in stack view: orphan children (face-down old top cards)
    for (auto* child : _stackView->getChildren()) {
        CardView* cv = dynamic_cast<CardView*>(child);
        if (cv && cv->getCardId() == cardId) return cv;
    }

    return nullptr;
}

void UndoManager::undoStackSwap(const UndoRecord& record)
{
    CardView* cardA = findCardView(record.cardAId); // now top card
    CardView* cardB = findCardView(record.cardBId); // now face-down orphan

    if (!cardA) {
        CCLOG("UndoManager: undoStackSwap failed — cardA id=%d not found", record.cardAId);
        _isUndoing = false;
        return;
    }

    auto* sv = _stackView;
    auto restore = CallFunc::create([this, cardA, cardB, record, sv]() {
        // Remove cardA from top card role
        sv->setTopCardSilent(nullptr);

        // Add cardA back to hand cards
        sv->addHandCardSilent(cardA);

        // Restore cardB as top card
        if (cardB) {
            cardB->setFaceUp(true);
            cardB->setLocalZOrder(100);
            cardB->setOnCardClicked(nullptr);  // top card needs no touch callback
            sv->setTopCardSilent(cardB);
        }

        sv->layoutCards();

        // Restore model state
        _model->setTopCardIndex(record.oldTopCardIndex);

        _isUndoing = false;
        CCLOG("UndoManager: StackSwap undone — card id=%d to hand, card id=%d to top, pfCards=%zu, stackCards=%zu, topIdx=%d",
              record.cardAId, record.cardBId,
              _model->getPlayFieldCards().size(),
              _model->getStackCards().size(),
              _model->getTopCardIndex());
        if (_onStateChanged) _onStateChanged(_undoModel.canUndo());
    });

    cardA->runAction(Sequence::create(
        EaseOut::create(MoveTo::create(0.3f, record.posABefore), 2.0f),
        restore,
        nullptr));
}

void UndoManager::undoPlayFieldMatch(const UndoRecord& record)
{
    CardView* cardA = findCardView(record.cardAId); // now top card in stack
    CardView* cardB = findCardView(record.cardBId); // now face-down orphan in stack

    if (!cardA) {
        CCLOG("UndoManager: undoPlayFieldMatch failed — cardA id=%d not found", record.cardAId);
        _isUndoing = false;
        return;
    }

    // Convert posABefore (PlayFieldView local space) to StackView local space for animation
    Vec2 targetWorld = _playFieldView->convertToWorldSpace(record.posABefore);
    Vec2 targetInStack = _stackView->convertToNodeSpace(targetWorld);

    auto* sv = _stackView;
    auto* pf = _playFieldView;

    auto migrate = CallFunc::create([this, cardA, cardB, record, sv, pf]() {
        // Clear pointer before moving card to another parent
        sv->setTopCardSilent(nullptr);

        // Move cardA from StackView to PlayFieldView
        Vec2 worldPos = sv->convertToWorldSpace(cardA->getPosition());
        cardA->removeFromParentAndCleanup(false);
        pf->addChild(cardA);

        Vec2 posInPlayField = pf->convertToNodeSpace(worldPos);
        cardA->setPosition(posInPlayField);

        pf->addCardSilent(cardA);
        pf->updateAllClickable();

        // Restore cardB as top card
        if (cardB) {
            cardB->setFaceUp(true);
            cardB->setLocalZOrder(100);
            cardB->setOnCardClicked(nullptr);  // top card needs no touch callback
            sv->setTopCardSilent(cardB);
        }

        sv->layoutCards();

        // Update model: save card data before erasing (pointer invalidated by erase)
        CardModel* cardAModel = _model->findCardById(record.cardAId);
        if (cardAModel) {
            cardAModel->setInPlayField(true);
            cardAModel->setPosition(posInPlayField);

            CardModel saved = *cardAModel;  // copy before pointer is invalidated
            auto& stackCards = _model->getStackCards();
            stackCards.erase(std::remove_if(stackCards.begin(), stackCards.end(),
                [&record](const CardModel& c) { return c.getId() == record.cardAId; }),
                stackCards.end());

            _model->getPlayFieldCards().push_back(saved);
        }

        _model->setTopCardIndex(record.oldTopCardIndex);

        _isUndoing = false;
        CCLOG("UndoManager: PlayFieldMatch undone — card id=%d to play field, card id=%d to top, pfCards=%zu, stackCards=%zu, topIdx=%d",
              record.cardAId, record.cardBId,
              _model->getPlayFieldCards().size(),
              _model->getStackCards().size(),
              _model->getTopCardIndex());
        if (_onStateChanged) _onStateChanged(_undoModel.canUndo());
    });

    cardA->runAction(Sequence::create(
        EaseOut::create(MoveTo::create(0.3f, targetInStack), 2.0f),
        migrate,
        nullptr));
}
