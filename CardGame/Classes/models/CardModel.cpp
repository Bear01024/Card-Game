#include "CardModel.h"

CardModel::CardModel()
    : _id(-1)
    , _face(CardFaceType::CFT_NONE)
    , _suit(CardSuitType::CST_NONE)
    , _position(cocos2d::Vec2::ZERO)
    , _isFaceUp(true)
    , _isInPlayField(true)
{
}

CardModel::CardModel(int id, CardFaceType face, CardSuitType suit,
                     const cocos2d::Vec2& position, bool isFaceUp, bool isInPlayField)
    : _id(id)
    , _face(face)
    , _suit(suit)
    , _position(position)
    , _isFaceUp(isFaceUp)
    , _isInPlayField(isInPlayField)
{
}

bool CardModel::isRedSuit() const
{
    return _suit == CardSuitType::CST_HEARTS || _suit == CardSuitType::CST_DIAMONDS;
}

std::string CardModel::getFaceString() const
{
    switch (_face) {
        case CardFaceType::CFT_ACE:   return "A";
        case CardFaceType::CFT_JACK:  return "J";
        case CardFaceType::CFT_QUEEN: return "Q";
        case CardFaceType::CFT_KING:  return "K";
        default: return std::to_string(static_cast<int>(_face) + 1);
    }
}
