#include "GameView.h"

USING_NS_CC;

GameView* GameView::create()
{
    GameView* view = new (std::nothrow) GameView();
    if (view && view->init()) {
        view->autorelease();
        return view;
    }
    CC_SAFE_DELETE(view);
    return nullptr;
}

bool GameView::init()
{
    if (!Node::init()) return false;

    Size screenSize(1080, 2080);

    _playFieldView = PlayFieldView::create();
    _playFieldView->setPosition(Vec2(0, 580));
    this->addChild(_playFieldView, 1);

    _stackView = StackView::create();
    _stackView->setPosition(Vec2(0, 0));
    this->addChild(_stackView, 1);

    // Undo label with touch
    _undoLabel = Label::createWithSystemFont("回退", "Arial", 40);
    _undoLabel->setPosition(Vec2(screenSize.width - 80, 250));
    _undoLabel->setTextColor(Color4B::BLACK);
    this->addChild(_undoLabel, 2);

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);
    listener->onTouchBegan = [this](Touch* touch, Event*) -> bool {
        Vec2 localPos = _undoLabel->convertToNodeSpace(touch->getLocation());
        Size size = _undoLabel->getContentSize();
        Rect rect(0, 0, size.width + 20, size.height + 10);
        rect.origin = Vec2(-10, -5);
        if (rect.containsPoint(localPos)) {
            if (_onUndoClicked) {
                _onUndoClicked();
            }
            return true;
        }
        return false;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, _undoLabel);

    return true;
}

void GameView::setUndoEnabled(bool enabled)
{
    if (_undoLabel) {
        _undoLabel->setTextColor(enabled ? Color4B::BLACK : Color4B::GRAY);
    }
}
