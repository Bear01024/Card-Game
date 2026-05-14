#ifndef __STACK_CONTROLLER_H__
#define __STACK_CONTROLLER_H__

#include "cocos2d.h"

class StackView;
class GameModel;
class CardView;
class UndoManager;

class StackController {
public:
    StackController();

    void init(GameModel* model, StackView* stackView);
    void setUndoManager(UndoManager* undoManager) { _undoManager = undoManager; }

    void handleHandCardClick(CardView* cardView);

private:
    GameModel* _model;
    StackView* _stackView;
    UndoManager* _undoManager;
};

#endif // __STACK_CONTROLLER_H__
