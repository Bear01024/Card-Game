#include "LevelConfigLoader.h"
#include "cocos2d.h"
#include "json/document.h"

USING_NS_CC;

static int faceFromInt(int v)
{
    if (v < 0 || v >= static_cast<int>(CardFaceType::CFT_NUM_CARD_FACE_TYPES)) {
        return static_cast<int>(CardFaceType::CFT_NONE);
    }
    return v;
}

static int suitFromInt(int v)
{
    if (v < 0 || v >= static_cast<int>(CardSuitType::CST_NUM_CARD_SUIT_TYPES)) {
        return static_cast<int>(CardSuitType::CST_NONE);
    }
    return v;
}

LevelConfig LevelConfigLoader::loadFromFile(const std::string& filePath)
{
    LevelConfig config;

    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filePath);
    std::string content = FileUtils::getInstance()->getStringFromFile(fullPath);

    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseDefaultFlags>(content.c_str());

    if (doc.HasParseError()) {
        CCLOG("LevelConfigLoader: JSON parse error in %s", filePath.c_str());
        return config;
    }

    if (doc.HasMember("Playfield") && doc["Playfield"].IsArray()) {
        const auto& arr = doc["Playfield"];
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
            const auto& item = arr[i];
            PlayfieldCardConfig pc;
            pc.face = static_cast<CardFaceType>(faceFromInt(item["CardFace"].GetInt()));
            pc.suit = static_cast<CardSuitType>(suitFromInt(item["CardSuit"].GetInt()));
            pc.position.x = item["Position"]["x"].GetFloat();
            pc.position.y = item["Position"]["y"].GetFloat();
            config.playfield.push_back(pc);
        }
    }

    if (doc.HasMember("Stack") && doc["Stack"].IsArray()) {
        const auto& arr = doc["Stack"];
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
            const auto& item = arr[i];
            StackCardConfig sc;
            sc.face = static_cast<CardFaceType>(faceFromInt(item["CardFace"].GetInt()));
            sc.suit = static_cast<CardSuitType>(suitFromInt(item["CardSuit"].GetInt()));
            config.stack.push_back(sc);
        }
    }

    return config;
}
