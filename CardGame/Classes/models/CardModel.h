#ifndef __CARD_MODEL_H__
#define __CARD_MODEL_H__

#include "cocos2d.h"
#include "CardTypes.h"

class CardModel {
public:
    CardModel();
    CardModel(int id, CardFaceType face, CardSuitType suit,
              const cocos2d::Vec2& position, bool isFaceUp, bool isInPlayField);

    int getId() const { return _id; }
    CardFaceType getFace() const { return _face; }
    CardSuitType getSuit() const { return _suit; }
    cocos2d::Vec2 getPosition() const { return _position; }
    bool isFaceUp() const { return _isFaceUp; }
    bool isInPlayField() const { return _isInPlayField; }

    void setId(int id) { _id = id; }
    void setFace(CardFaceType face) { _face = face; }
    void setSuit(CardSuitType suit) { _suit = suit; }
    void setPosition(const cocos2d::Vec2& pos) { _position = pos; }
    void setFaceUp(bool faceUp) { _isFaceUp = faceUp; }
    void setInPlayField(bool inPlayField) { _isInPlayField = inPlayField; }

    bool isRedSuit() const;
    std::string getFaceString() const;

private:
    int _id;
    CardFaceType _face;
    CardSuitType _suit;
    cocos2d::Vec2 _position;
    bool _isFaceUp;
    bool _isInPlayField;
};

#endif // __CARD_MODEL_H__
