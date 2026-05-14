#include "HelloWorldScene.h"
#include "views/GameView.h"
#include "controllers/GameController.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

bool HelloWorld::init()
{
    if (!Scene::init()) return false;

    _gameView = GameView::create();
    this->addChild(_gameView);

    _gameController = new GameController();
    _gameController->init(_gameView);
    _gameController->startGame("res/levels/level-1.json");

    return true;
}
