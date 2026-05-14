#ifndef __UNDO_MANAGER_H__
#define __UNDO_MANAGER_H__

#include "cocos2d.h"
#include "models/UndoModel.h"
#include <functional>

class GameModel;
class PlayFieldView;
class StackView;
class CardView;

class UndoManager {
public:
    UndoManager();

    void init(GameModel* model, PlayFieldView* playFieldView, StackView* stackView);

    void recordStackSwap(int clickedCardId, int oldTopCardId,
                         const cocos2d::Vec2& clickedPos, const cocos2d::Vec2& topPos,
                         int oldTopCardIndex);

    void recordPlayFieldMatch(int matchedCardId, int oldTopCardId,
                              const cocos2d::Vec2& matchedPos, const cocos2d::Vec2& topPos,
                              int oldTopCardIndex);

    void undo();
    bool canUndo() const;

    void setOnUndoStateChanged(std::function<void(bool)> callback) { _onStateChanged = callback; }

private:
    void undoStackSwap(const UndoRecord& record);
    void undoPlayFieldMatch(const UndoRecord& record);

    CardView* findCardView(int cardId) const;

    UndoModel _undoModel;
    GameModel* _model;
    PlayFieldView* _playFieldView;
    StackView* _stackView;
    bool _isUndoing;
    std::function<void(bool)> _onStateChanged;
};

#endif // __UNDO_MANAGER_H__
