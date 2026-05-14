#ifndef __STACK_VIEW_H__
#define __STACK_VIEW_H__

#include "cocos2d.h"
#include "CardView.h"
#include <vector>
#include <functional>

class StackView : public cocos2d::Node {
public:
    static StackView* create();

    virtual bool init() override;

    void addHandCard(CardView* cardView);
    void setTopCard(CardView* cardView);
    void removeHandCard(CardView* cardView);
    void removeHandCardSilent(CardView* cardView);
    void addHandCardSilent(CardView* cardView);
    void setTopCardSilent(CardView* cardView);
    void removeAllCards();

    CardView* getTopCardView() const { return _topCardView; }
    std::vector<CardView*> getHandCards() const { return _handCardViews; }
    CardView* getHandCardById(int id) const;
    CardView* findCardView(int id) const;
    CardView* getRightmostHandCard() const;

    void setOnHandCardClicked(std::function<void(CardView*)> callback) { _onHandCardClicked = callback; }

    void layoutCards();

    static const float STACK_WIDTH;
    static const float STACK_HEIGHT;
    static const float HAND_CARD_OFFSET;

private:
    void updateHandCardClickable();

    std::vector<CardView*> _handCardViews;
    CardView* _topCardView;
    std::function<void(CardView*)> _onHandCardClicked;
};

#endif // __STACK_VIEW_H__
