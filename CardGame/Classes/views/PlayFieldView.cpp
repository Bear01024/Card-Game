#include "PlayFieldView.h"

USING_NS_CC;

PlayFieldView* PlayFieldView::create()
{
    PlayFieldView* view = new (std::nothrow) PlayFieldView();
    if (view && view->init()) {
        view->autorelease();
        return view;
    }
    CC_SAFE_DELETE(view);
    return nullptr;
}

bool PlayFieldView::init()
{
    if (!Node::init()) return false;
    setContentSize(Size(1080, 1500));
    return true;
}

void PlayFieldView::addCard(CardView* cardView)
{
    if (!cardView) return;
    _cardViews.push_back(cardView);
    this->addChild(cardView);

    cardView->setOnCardClicked([this](CardView* cv) {
        if (isCardClickable(cv) && _onCardClicked) {
            _onCardClicked(cv);
        }
    });

    sortCardsByZOrder();
}

void PlayFieldView::removeCard(CardView* cardView)
{
    auto it = std::find(_cardViews.begin(), _cardViews.end(), cardView);
    if (it != _cardViews.end()) {
        (*it)->removeFromParent();
        _cardViews.erase(it);
        updateAllClickable();
    }
}

void PlayFieldView::removeCardSilent(CardView* cardView)
{
    auto it = std::find(_cardViews.begin(), _cardViews.end(), cardView);
    if (it != _cardViews.end()) {
        _cardViews.erase(it);
        updateAllClickable();
    }
}

void PlayFieldView::addCardSilent(CardView* cardView)
{
    if (!cardView) return;
    _cardViews.push_back(cardView);

    cardView->setOnCardClicked([this](CardView* cv) {
        if (isCardClickable(cv) && _onCardClicked) {
            _onCardClicked(cv);
        }
    });

    sortCardsByZOrder();
}

void PlayFieldView::removeAllCards()
{
    for (auto* cv : _cardViews) {
        cv->removeFromParent();
    }
    _cardViews.clear();
}

bool PlayFieldView::isCardClickable(CardView* cardView) const
{
    if (!cardView) return false;
    int myZ = cardView->getLocalZOrder();
    Rect myRect = cardView->getBoundingBox();

    for (auto* other : _cardViews) {
        if (other == cardView) continue;
        if (other->getLocalZOrder() <= myZ) continue;
        Rect otherRect = other->getBoundingBox();
        if (myRect.intersectsRect(otherRect)) {
            return false;
        }
    }
    return true;
}

void PlayFieldView::updateAllClickable()
{
    sortCardsByZOrder();
}

void PlayFieldView::sortCardsByZOrder()
{
    // Z-order follows array index: later cards (higher index) have higher Z
    // and cover earlier cards — inverse stacking.
    for (size_t i = 0; i < _cardViews.size(); ++i) {
        _cardViews[i]->setLocalZOrder(static_cast<int>(i));
    }
}
