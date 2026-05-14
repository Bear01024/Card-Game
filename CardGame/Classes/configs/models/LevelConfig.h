#ifndef __LEVEL_CONFIG_H__
#define __LEVEL_CONFIG_H__

#include <vector>
#include <string>
#include "cocos2d.h"
#include "models/CardTypes.h"

struct PlayfieldCardConfig {
    CardFaceType face;
    CardSuitType suit;
    cocos2d::Vec2 position;
};

struct StackCardConfig {
    CardFaceType face;
    CardSuitType suit;
};

struct LevelConfig {
    std::string levelId;
    std::vector<PlayfieldCardConfig> playfield;
    std::vector<StackCardConfig> stack;
};

#endif // __LEVEL_CONFIG_H__
