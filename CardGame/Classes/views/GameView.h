#ifndef __GAME_VIEW_H__
#define __GAME_VIEW_H__

#include "cocos2d.h"
#include "PlayFieldView.h"
#include "StackView.h"
#include <functional>

class GameView : public cocos2d::Node {
public:
    static GameView* create();

    virtual bool init() override;

    PlayFieldView* getPlayFieldView() { return _playFieldView; }
    StackView* getStackView() { return _stackView; }

    void setOnUndoClicked(std::function<void()> callback) { _onUndoClicked = callback; }
    void setUndoEnabled(bool enabled);

private:
    PlayFieldView* _playFieldView;
    StackView* _stackView;
    cocos2d::Label* _undoLabel;
    std::function<void()> _onUndoClicked;
};

#endif // __GAME_VIEW_H__
