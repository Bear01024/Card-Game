#ifndef __CARD_VIEW_H__
#define __CARD_VIEW_H__

#include "cocos2d.h"
#include "models/CardTypes.h"
#include <functional>

class CardModel;

class CardView : public cocos2d::Node {
public:
    static CardView* create(const CardModel& model);
    virtual bool init() override;

    void bindModel(const CardModel& model);
    void setFaceUp(bool faceUp);
    bool isFaceUp() const;

    int getCardId() const { return _cardId; }
    CardFaceType getFace() const { return _face; }
    CardSuitType getSuit() const { return _suit; }

    void setOnCardClicked(std::function<void(CardView*)> callback) { _onCardClicked = callback; }

    static const float CARD_WIDTH;
    static const float CARD_HEIGHT;

private:
    void updateDisplay();
    void buildFaceUpSprites(const CardModel& model);
    std::string getSuitImageName(int suit) const;
    std::string getColorStr(const CardModel& model) const;

    cocos2d::Sprite* _bgSprite;
    cocos2d::Sprite* _smallNumSprite;
    cocos2d::Sprite* _suitSprite;
    cocos2d::Sprite* _bigNumSprite;

    int _cardId;
    CardFaceType _face;
    CardSuitType _suit;
    bool _isFaceUp;

    std::function<void(CardView*)> _onCardClicked;

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
};

#endif // __CARD_VIEW_H__
