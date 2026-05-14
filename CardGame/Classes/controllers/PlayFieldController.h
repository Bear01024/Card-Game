#ifndef __PLAY_FIELD_CONTROLLER_H__
#define __PLAY_FIELD_CONTROLLER_H__

#include "cocos2d.h"

class PlayFieldView;
class StackView;
class GameModel;
class CardView;
class UndoManager;

class PlayFieldController {
public:
    PlayFieldController();

    void init(GameModel* model, PlayFieldView* playFieldView, StackView* stackView);
    void setUndoManager(UndoManager* undoManager) { _undoManager = undoManager; }

    void handleCardClick(CardView* cardView);

private:
    bool isCardClickable(CardView* cardView) const;
    bool canMatch(const CardView* playFieldCard) const;

    GameModel* _model;
    PlayFieldView* _playFieldView;
    StackView* _stackView;
    UndoManager* _undoManager;
    bool _isAnimating;
};

#endif // __PLAY_FIELD_CONTROLLER_H__
