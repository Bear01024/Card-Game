#include "GameModel.h"

GameModel::GameModel()
    : _topCardIndex(-1)
{
}

CardModel* GameModel::getTopCard()
{
    if (_topCardIndex >= 0 && _topCardIndex < static_cast<int>(_stackCards.size())) {
        return &_stackCards[_topCardIndex];
    }
    return nullptr;
}

const CardModel* GameModel::getTopCard() const
{
    if (_topCardIndex >= 0 && _topCardIndex < static_cast<int>(_stackCards.size())) {
        return &_stackCards[_topCardIndex];
    }
    return nullptr;
}

CardModel* GameModel::findCardById(int id)
{
    for (auto& card : _playFieldCards) {
        if (card.getId() == id) return &card;
    }
    for (auto& card : _stackCards) {
        if (card.getId() == id) return &card;
    }
    return nullptr;
}

const CardModel* GameModel::findCardById(int id) const
{
    for (auto& card : _playFieldCards) {
        if (card.getId() == id) return &card;
    }
    for (auto& card : _stackCards) {
        if (card.getId() == id) return &card;
    }
    return nullptr;
}
