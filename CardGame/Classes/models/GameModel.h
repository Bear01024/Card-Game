#ifndef __GAME_MODEL_H__
#define __GAME_MODEL_H__

#include <vector>
#include "CardModel.h"

class GameModel {
public:
    GameModel();

    std::vector<CardModel>& getPlayFieldCards() { return _playFieldCards; }
    const std::vector<CardModel>& getPlayFieldCards() const { return _playFieldCards; }

    std::vector<CardModel>& getStackCards() { return _stackCards; }
    const std::vector<CardModel>& getStackCards() const { return _stackCards; }

    int getTopCardIndex() const { return _topCardIndex; }
    void setTopCardIndex(int index) { _topCardIndex = index; }

    CardModel* getTopCard();
    const CardModel* getTopCard() const;

    CardModel* findCardById(int id);
    const CardModel* findCardById(int id) const;

private:
    std::vector<CardModel> _playFieldCards;
    std::vector<CardModel> _stackCards;
    int _topCardIndex;
};

#endif // __GAME_MODEL_H__
