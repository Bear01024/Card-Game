#ifndef __LEVEL_CONFIG_LOADER_H__
#define __LEVEL_CONFIG_LOADER_H__

#include <string>
#include "configs/models/LevelConfig.h"

class LevelConfigLoader {
public:
    static LevelConfig loadFromFile(const std::string& filePath);
};

#endif // __LEVEL_CONFIG_LOADER_H__
