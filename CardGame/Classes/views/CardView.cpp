#include "CardView.h"
#include "models/CardModel.h"

USING_NS_CC;

const float CardView::CARD_WIDTH = 182.0f;
const float CardView::CARD_HEIGHT = 282.0f;

CardView* CardView::create(const CardModel& model)
{
    CardView* view = new (std::nothrow) CardView();
    if (view) {
        view->bindModel(model);
        view->autorelease();
        return view;
    }
    CC_SAFE_DELETE(view);
    return nullptr;
}

bool CardView::init()
{
    if (!Node::init()) return false;

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);
    listener->onTouchBegan = CC_CALLBACK_2(CardView::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void CardView::bindModel(const CardModel& model)
{
    _cardId = model.getId();
    _face = model.getFace();
    _suit = model.getSuit();
    _isFaceUp = model.isFaceUp();

    init();
    buildFaceUpSprites(model);
    updateDisplay();
    setContentSize(Size(CARD_WIDTH, CARD_HEIGHT));
}

void CardView::setFaceUp(bool faceUp)
{
    _isFaceUp = faceUp;
    updateDisplay();
}

bool CardView::isFaceUp() const
{
    return _isFaceUp;
}

void CardView::updateDisplay()
{
    if (_bgSprite) _bgSprite->setVisible(true);
    if (_smallNumSprite) _smallNumSprite->setVisible(_isFaceUp);
    if (_suitSprite) _suitSprite->setVisible(_isFaceUp);
    if (_bigNumSprite) _bigNumSprite->setVisible(_isFaceUp);
}

void CardView::buildFaceUpSprites(const CardModel& model)
{
    _bgSprite = Sprite::create("res/card_general.png");
    if (_bgSprite) {
        _bgSprite->setPosition(Vec2::ZERO);
        this->addChild(_bgSprite, 0);
    }

    std::string color = getColorStr(model);
    std::string faceStr = model.getFaceString();

    // Top-left small number
    std::string smallPath = "res/number/small_" + color + "_" + faceStr + ".png";
    _smallNumSprite = Sprite::create(smallPath);
    if (_smallNumSprite) {
        _smallNumSprite->setPosition(Vec2(-62, 108));
        this->addChild(_smallNumSprite, 1);
    }

    // Top-right suit icon
    std::string suitPath = "res/suits/" + getSuitImageName(static_cast<int>(model.getSuit())) + ".png";
    _suitSprite = Sprite::create(suitPath);
    if (_suitSprite) {
        _suitSprite->setPosition(Vec2(59, 109));
        this->addChild(_suitSprite, 1);
    }

    // Bottom-center big number
    std::string bigPath = "res/number/big_" + color + "_" + faceStr + ".png";
    _bigNumSprite = Sprite::create(bigPath);
    if (_bigNumSprite) {
        _bigNumSprite->setPosition(Vec2(0, -31));
        this->addChild(_bigNumSprite, 1);
    }
}

std::string CardView::getSuitImageName(int suit) const
{
    switch (static_cast<CardSuitType>(suit)) {
        case CardSuitType::CST_HEARTS:   return "heart";
        case CardSuitType::CST_DIAMONDS: return "diamond";
        case CardSuitType::CST_CLUBS:    return "club";
        case CardSuitType::CST_SPADES:   return "spade";
        default: return "heart";
    }
}

std::string CardView::getColorStr(const CardModel& model) const
{
    return model.isRedSuit() ? "red" : "black";
}

bool CardView::onTouchBegan(Touch* touch, Event* event)
{
    Vec2 localPos = convertToNodeSpace(touch->getLocation());
    Rect rect(0, 0, CARD_WIDTH, CARD_HEIGHT);
    rect.origin = Vec2(-CARD_WIDTH / 2, -CARD_HEIGHT / 2);

    if (rect.containsPoint(localPos)) {
        if (_onCardClicked) {
            _onCardClicked(this);
        }
        return true;
    }
    return false;
}
