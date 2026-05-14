#include "StackView.h"

USING_NS_CC;

const float StackView::STACK_WIDTH = 1080.0f;
const float StackView::STACK_HEIGHT = 580.0f;
const float StackView::HAND_CARD_OFFSET = 65.0f;

StackView* StackView::create()
{
    StackView* view = new (std::nothrow) StackView();
    if (view && view->init()) {
        view->autorelease();
        return view;
    }
    CC_SAFE_DELETE(view);
    return nullptr;
}

bool StackView::init()
{
    if (!Node::init()) return false;
    _topCardView = nullptr;
    setContentSize(Size(STACK_WIDTH, STACK_HEIGHT));

    auto bg = LayerColor::create(Color4B(100, 60, 150, 255), STACK_WIDTH, STACK_HEIGHT);
    bg->setPosition(Vec2::ZERO);
    this->addChild(bg, -1);

    return true;
}

void StackView::addHandCard(CardView* cardView)
{
    if (!cardView) return;
    _handCardViews.push_back(cardView);
    this->addChild(cardView);

    cardView->setOnCardClicked([this](CardView* cv) {
        if (_onHandCardClicked) {
            _onHandCardClicked(cv);
        }
    });

    layoutCards();
}

void StackView::setTopCard(CardView* cardView)
{
    if (_topCardView) {
        _topCardView->removeFromParent();
    }
    _topCardView = cardView;
    if (_topCardView) {
        this->addChild(_topCardView);
    }
    layoutCards();
}

void StackView::removeHandCard(CardView* cardView)
{
    auto it = std::find(_handCardViews.begin(), _handCardViews.end(), cardView);
    if (it != _handCardViews.end()) {
        (*it)->removeFromParent();
        _handCardViews.erase(it);
        layoutCards();
    }
}

void StackView::removeHandCardSilent(CardView* cardView)
{
    auto it = std::find(_handCardViews.begin(), _handCardViews.end(), cardView);
    if (it != _handCardViews.end()) {
        _handCardViews.erase(it);
    }
}

void StackView::addHandCardSilent(CardView* cardView)
{
    if (!cardView) return;
    _handCardViews.push_back(cardView);
    // Do NOT call removeFromParent/addChild — card is already a child of StackView,
    // and that would stopAllActions(), killing any running MoveTo animation.
    cardView->setOnCardClicked([this](CardView* cv) {
        if (_onHandCardClicked) {
            _onHandCardClicked(cv);
        }
    });
}

void StackView::setTopCardSilent(CardView* cardView)
{
    // Do NOT detach/re-attach — card stays in the same parent, only the role changes.
    _topCardView = cardView;
}

void StackView::removeAllCards()
{
    for (auto* cv : _handCardViews) {
        cv->removeFromParent();
    }
    _handCardViews.clear();
    if (_topCardView) {
        _topCardView->removeFromParent();
        _topCardView = nullptr;
    }
}

CardView* StackView::getHandCardById(int id) const
{
    for (auto* cv : _handCardViews) {
        if (cv->getCardId() == id) return cv;
    }
    return nullptr;
}

CardView* StackView::findCardView(int id) const
{
    if (_topCardView && _topCardView->getCardId() == id) return _topCardView;

    for (auto* cv : _handCardViews) {
        if (cv->getCardId() == id) return cv;
    }

    // Search orphan children (face-down old top cards not in managed lists)
    for (auto* child : getChildren()) {
        CardView* cv = dynamic_cast<CardView*>(child);
        if (cv && cv->getCardId() == id) return cv;
    }

    return nullptr;
}

CardView* StackView::getRightmostHandCard() const
{
    if (_handCardViews.empty()) return nullptr;
    return _handCardViews.back();
}

void StackView::layoutCards()
{
    float areaHeight = STACK_HEIGHT;
    float areaWidth = STACK_WIDTH;
    float cardW = CardView::CARD_WIDTH;
    float cardH = CardView::CARD_HEIGHT;

    // Left side: horizontally staggered hand cards
    float startX = cardW / 2 + 20;
    float centerY = areaHeight / 2;

    for (size_t i = 0; i < _handCardViews.size(); ++i) {
        _handCardViews[i]->setPosition(Vec2(startX + i * HAND_CARD_OFFSET, centerY));
        _handCardViews[i]->setLocalZOrder(static_cast<int>(i));
    }

    // Right side: top card
    if (_topCardView) {
        float topX = areaWidth - cardW / 2 - 200;
        _topCardView->setPosition(Vec2(topX, centerY));
        _topCardView->setLocalZOrder(100);
    }
}
