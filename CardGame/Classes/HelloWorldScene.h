#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class GameView;
class GameController;

class HelloWorld : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init() override;
    CREATE_FUNC(HelloWorld);

private:
    GameView* _gameView;
    GameController* _gameController;
};

#endif // __HELLOWORLD_SCENE_H__
