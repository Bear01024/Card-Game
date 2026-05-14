#ifndef __PLAY_FIELD_VIEW_H__
#define __PLAY_FIELD_VIEW_H__

#include "cocos2d.h"
#include "CardView.h"
#include <vector>
#include <functional>

class PlayFieldView : public cocos2d::Node {
public:
    static PlayFieldView* create();

    virtual bool init() override;

    void addCard(CardView* cardView);
    void removeCard(CardView* cardView);
    void removeCardSilent(CardView* cardView);
    void addCardSilent(CardView* cardView);
    void removeAllCards();

    bool isCardClickable(CardView* cardView) const;
    void updateAllClickable();

    std::vector<CardView*> getCards() const { return _cardViews; }

    void setOnCardClicked(std::function<void(CardView*)> callback) { _onCardClicked = callback; }

private:
    void sortCardsByZOrder();

    std::vector<CardView*> _cardViews;
    std::function<void(CardView*)> _onCardClicked;
};

#endif // __PLAY_FIELD_VIEW_H__
