#ifndef __GAME_CONTROLLER_H__
#define __GAME_CONTROLLER_H__

#include <string>

class GameView;
class GameModel;
class PlayFieldController;
class StackController;
class UndoManager;

class GameController {
public:
    GameController();
    ~GameController();

    void init(GameView* gameView);
    void startGame(const std::string& levelPath);

private:
    void clearGame();

    GameView* _gameView;
    GameModel* _gameModel;
    PlayFieldController* _playFieldController;
    StackController* _stackController;
    UndoManager* _undoManager;
};

#endif // __GAME_CONTROLLER_H__
