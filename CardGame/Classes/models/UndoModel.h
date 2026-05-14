#ifndef __UNDO_MODEL_H__
#define __UNDO_MODEL_H__

#include "cocos2d.h"
#include <vector>

enum class UndoActionType {
    StackSwap,
    PlayFieldMatch
};

struct UndoRecord {
    UndoActionType type;

    int cardAId;
    int cardBId;

    cocos2d::Vec2 posABefore;
    cocos2d::Vec2 posBBefore;

    int oldTopCardIndex;
    bool cardAWasInPlayField;
};

class UndoModel {
public:
    UndoModel();

    void push(const UndoRecord& record);
    UndoRecord pop();
    bool canUndo() const;
    void clear();
    size_t size() const;

private:
    std::vector<UndoRecord> _records;
};

#endif // __UNDO_MODEL_H__
