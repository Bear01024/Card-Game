# Phase 8 Log — UndoManager Implementation

**Date**: 2026-05-14  
**Status**: Compiled and running, 0 errors

## Completed

- Created `Classes/managers/UndoManager.h/.cpp`:
  - Manages undo stack via `UndoModel`, supports StackSwap and PlayFieldMatch action types.
  - `recordStackSwap()` / `recordPlayFieldMatch()` — called by controllers before animating an action.
  - `undo()` — pops the most recent record and plays a 0.3s reverse animation.
  - `onUndoStateChanged` callback — notifies UI to enable/disable the Undo button.
  - `_isUndoing` flag — locks undo during animation to prevent double-trigger.
  - `findCardView(int id)` — searches top card, hand cards, and orphan children.

- Modified `PlayFieldController`:
  - Added `UndoManager*` member and `setUndoManager()`.
  - `handleCardClick()` records `PlayFieldMatch` undo before starting the match animation.

- Modified `StackController`:
  - Added `UndoManager*` member and `setUndoManager()`.
  - `handleHandCardClick()` records `StackSwap` undo before starting the swap animation.

- Modified `PlayFieldView`:
  - Added `addCardSilent(CardView*)` — adds a card to the managed list without calling `addChild()` (used when card is already in the scene graph, e.g. returning from stack to play field during undo).

- Modified `StackView`:
  - Added `findCardView(int id)` — searches `_topCardView`, `_handCardViews`, and orphan children (face-down old top cards not in any managed list).

- Modified `GameView`:
  - Added `setUndoEnabled(bool)` — toggles Undo label text color between WHITE (enabled) and GRAY (disabled).

- Modified `HelloWorldScene`:
  - Creates `UndoManager` and passes it to both controllers.
  - Wires `onUndoStateChanged` callback to update Undo button appearance.
  - Undo button starts disabled, becomes enabled after first recorded action.

- Updated build files:
  - `CMakeLists.txt` — added `UndoManager.cpp` / `UndoManager.h`.
  - `proj.win32/CardGame.vcxproj` — added `UndoManager.cpp` / `UndoManager.h`.

## Undo Flow

### Record

1. Controller detects a valid action (match or swap).
2. Captures card IDs, positions, and `oldTopCardIndex` from current state.
3. Pushes `UndoRecord` to `UndoModel`.

### Undo

1. User clicks Undo button → `UndoManager::undo()`.
2. Pops `UndoRecord`, sets `_isUndoing = true`.
3. Depending on type:
   - **StackSwap**: Animates the new top card back to its original hand position. After animation, restores it to hand cards, brings old top card back face-up as top card.
   - **PlayFieldMatch**: Animates the matched card from stack back to its original play field position (with coordinate space conversion). After animation, migrates it from StackView to PlayFieldView, restores old top card.
4. Updates model state (reverses the model changes).
5. Fires `onUndoStateChanged` callback.

## Build Result

- 0 errors, 4 warnings (pre-existing: C4819 code page, LNK4075 /EDITANDCONTINUE).
- Game launches successfully, undo button toggles correctly.

## Minor Fix (same session)

- Moved top card 140px left in `StackView::layoutCards()` (changed right margin from 60 to 200) to avoid overlap with Undo button.
