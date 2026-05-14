#include "GameModelGenerator.h"

GameModel GameModelGenerator::generateGameModel(const LevelConfig& config)
{
    GameModel model;
    int nextId = 1;

    // Generate playfield cards
    for (const auto& pc : config.playfield) {
        CardModel card(nextId++, pc.face, pc.suit, pc.position, true, true);
        model.getPlayFieldCards().push_back(card);
    }

    // Generate stack cards
    for (const auto& sc : config.stack) {
        CardModel card(nextId++, sc.face, sc.suit, cocos2d::Vec2::ZERO, true, false);
        model.getStackCards().push_back(card);
    }

    // Set initial top card: first stack card is the initial top
    if (!model.getStackCards().empty()) {
        model.setTopCardIndex(0);
    }

    return model;
}
