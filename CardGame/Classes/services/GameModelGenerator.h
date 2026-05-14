#ifndef __GAME_MODEL_GENERATOR_H__
#define __GAME_MODEL_GENERATOR_H__

#include "configs/models/LevelConfig.h"
#include "models/GameModel.h"

class GameModelGenerator {
public:
    static GameModel generateGameModel(const LevelConfig& config);
};

#endif // __GAME_MODEL_GENERATOR_H__
