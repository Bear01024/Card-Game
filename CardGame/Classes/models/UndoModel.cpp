#include "UndoModel.h"

UndoModel::UndoModel()
{
}

void UndoModel::push(const UndoRecord& record)
{
    _records.push_back(record);
}

UndoRecord UndoModel::pop()
{
    UndoRecord record = _records.back();
    _records.pop_back();
    return record;
}

bool UndoModel::canUndo() const
{
    return !_records.empty();
}

void UndoModel::clear()
{
    _records.clear();
}

size_t UndoModel::size() const
{
    return _records.size();
}
