#include "GameController.h"
#include "views/GameView.h"
#include "views/CardView.h"
#include "PlayFieldController.h"
#include "StackController.h"
#include "managers/UndoManager.h"
#include "configs/loaders/LevelConfigLoader.h"
#include "services/GameModelGenerator.h"

USING_NS_CC;

GameController::GameController()
    : _gameView(nullptr)
    , _gameModel(nullptr)
    , _playFieldController(nullptr)
    , _stackController(nullptr)
    , _undoManager(nullptr)
{
}

GameController::~GameController()
{
    clearGame();
}

void GameController::init(GameView* gameView)
{
    _gameView = gameView;
}

void GameController::clearGame()
{
    delete _playFieldController;
    _playFieldController = nullptr;

    delete _stackController;
    _stackController = nullptr;

    delete _undoManager;
    _undoManager = nullptr;

    delete _gameModel;
    _gameModel = nullptr;
}

void GameController::startGame(const std::string& levelPath)
{
    if (!_gameView) return;

    clearGame();

    if (_gameView->getPlayFieldView()) {
        _gameView->getPlayFieldView()->removeAllCards();
    }
    if (_gameView->getStackView()) {
        _gameView->getStackView()->removeAllCards();
    }

    // Load level
    LevelConfig config = LevelConfigLoader::loadFromFile(levelPath);
    _gameModel = new GameModel(GameModelGenerator::generateGameModel(config));

    auto* playField = _gameView->getPlayFieldView();
    auto* stack = _gameView->getStackView();

    // Populate PlayField
    for (auto& card : _gameModel->getPlayFieldCards()) {
        auto cv = CardView::create(card);
        cv->setPosition(card.getPosition());
        playField->addCard(cv);
    }

    // Populate Stack
    int topIdx = _gameModel->getTopCardIndex();
    auto& stackCards = _gameModel->getStackCards();

    for (size_t i = 0; i < stackCards.size(); ++i) {
        if (static_cast<int>(i) == topIdx) {
            stack->setTopCard(CardView::create(stackCards[i]));
        } else {
            stack->addHandCard(CardView::create(stackCards[i]));
        }
    }

    // Init UndoManager
    _undoManager = new UndoManager();
    _undoManager->init(_gameModel, playField, stack);

    // Init controllers
    _playFieldController = new PlayFieldController();
    _playFieldController->init(_gameModel, playField, stack);
    _playFieldController->setUndoManager(_undoManager);

    _stackController = new StackController();
    _stackController->init(_gameModel, stack);
    _stackController->setUndoManager(_undoManager);

    // Undo button setup
    _gameView->setUndoEnabled(false);

    _undoManager->setOnUndoStateChanged([this](bool canUndo) {
        _gameView->setUndoEnabled(canUndo);
    });

    _gameView->setOnUndoClicked([this]() {
        if (_undoManager && _undoManager->canUndo()) {
            _undoManager->undo();
        }
    });
}
