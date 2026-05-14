# 程序设计文档

## 一、架构总览

### 1.1 分层架构（MVC + 扩展层）

```
HelloWorld (Scene, 入口壳)
  └── GameController (顶层协调器, 唯一的装配点)
        ├── GameModel           —— 运行时数据（playFieldCards, stackCards, topCardIndex）
        ├── PlayFieldController —— 桌面匹配逻辑
        ├── StackController     —— 手牌交换逻辑
        └── UndoManager         —— 撤销系统
              └── UndoModel     —— 撤销栈（std::vector<UndoRecord>）

视图层（被控制器驱动，不持有业务逻辑）:
  GameView → PlayFieldView + StackView
  PlayFieldView → CardView[]
  StackView    → CardView[] (手牌) + CardView (顶牌)

配置/服务层（无状态）:
  LevelConfigLoader  —— JSON → LevelConfig
  GameModelGenerator —— LevelConfig → GameModel
```

### 1.2 数据流

```
用户点击牌
  → CardView::onTouchBegan()
    → View 的回调 (PlayFieldView::_onCardClicked 或 StackView::_onHandCardClicked)
      → Controller::handle*Click()
        1. 校验（遮挡判定 / 匹配规则）
        2. 记录撤销（→ UndoManager::record*()）
        3. 运行动画（MoveTo + CallFunc）
        4. 更新视图层级（silent 方法 + layoutCards）
        5. 更新 GameModel
```

### 1.3 关键设计决策

| 决策 | 说明 |
|---|---|
| 坐标空间转换 | 主牌区牌在 PlayFieldView 坐标系；移动前通过 `convertToWorldSpace` → `convertToNodeSpace` 桥接两个视图 |
| Silent 方法 | `removeCardSilent` / `addHandCardSilent` 只操作内部 std::vector，不调用 `addChild`/`removeFromParent`，避免打断正在运行的 MoveTo 动画 |
| z-order 管理 | PlayFieldView 中 Y 越小 → z-order 越高（视觉上更靠前）；StackView 中手牌从左到右递增 |
| 撤销时机 | 在**动画之前**记录撤销（捕获"之前"状态），撤销时反向执行动画 + 模型恢复 |
| 撤销锁定 | UndoManager::_isUndoing 在撤销动画期间阻止再次撤销 |

### 1.4 文件注册

新增 .cpp/.h 文件时需要同时注册到：
- `CMakeLists.txt`（`GAME_SOURCE` / `GAME_HEADER`）
- `proj.win32/CardGame.vcxproj`（`<ClCompile>` / `<ClInclude>`）

---

## 二、如何新增一种卡牌

本章以新增"万能牌（Wild Card）"为例，展示需要在哪些层次做修改。

### 2.1 数据层：扩展 CardModel

**文件**: `Classes/models/CardModel.h`

在 CardModel 中添加新属性：

```cpp
// 新增：是否为万能牌
bool isWild() const { return _isWild; }
void setWild(bool wild) { _isWild = wild; }

private:
    bool _isWild;  // 构造函数中初始化为 false
```

如果这是一类全新的卡牌（而非现有牌的属性），可以创建派生类或新增一个 CardType 枚举：

```cpp
// Classes/models/CardTypes.h
enum class CardSpecialType {
    CST_NONE = -1,
    CST_WILD,       // 万能牌，可匹配任意面值
    CST_BLOCK,      // 阻挡牌，不可被匹配
};
```

然后在 CardModel 中加入 `CardSpecialType _specialType` 字段。

### 2.2 配置层：扩展 LevelConfig 和加载器

**文件**: `Classes/configs/models/LevelConfig.h`

```cpp
struct PlayfieldCardConfig {
    CardFaceType face;
    CardSuitType suit;
    cocos2d::Vec2 position;
    CardSpecialType specialType;  // 新增字段，默认 CST_NONE
};
```

**文件**: `Classes/configs/loaders/LevelConfigLoader.cpp`

在 JSON 解析中添加对 `specialType` 字段的读取：

```cpp
if (cardJson.HasMember("specialType")) {
    cardConfig.specialType = static_cast<CardSpecialType>(cardJson["specialType"].GetInt());
}
```

### 2.3 服务层：扩展 GameModelGenerator

**文件**: `Classes/services/GameModelGenerator.cpp`

在 `generateGameModel()` 中将新字段从 Config 传递到 Model：

```cpp
CardModel card(/* ... */);
card.setSpecialType(cfg.specialType);  // 传递特殊类型
```

### 2.4 视图层：扩展 CardView 渲染

**文件**: `Classes/views/CardView.cpp`

如果需要不同的视觉表现（例如万能牌显示金色背景），在 `buildFaceUpSprites()` 中根据类型切换资源：

```cpp
if (model.isWild()) {
    _bgSprite->setTexture("card_wild.png");  // 万能牌专用底框
    // 不显示花色和数字，或显示特殊标记
    _suitSprite->setVisible(false);
    _smallNumSprite->setVisible(false);
    _bigNumSprite->setVisible(false);
}
```

### 2.5 控制器层：扩展匹配规则

**文件**: `Classes/controllers/PlayFieldController.cpp`

在 `canMatch()` 中添加万能牌逻辑：

```cpp
bool PlayFieldController::canMatch(const CardView* playFieldCard) const
{
    // ... 原有逻辑 ...
    
    // 万能牌：可匹配任意顶牌
    if (playFieldCard->getSpecialType() == CardSpecialType::CST_WILD) return true;
    
    // 阻挡牌：不可被匹配
    if (playFieldCard->getSpecialType() == CardSpecialType::CST_BLOCK) return false;
    
    // 顶牌是万能牌：任意主牌区牌均可匹配
    // （需要在 CardView 中暴露 getSpecialType()，且顶牌也可能是万能牌）
    
    // 原有面值差 ±1 规则
    int diff = std::abs(...);
    return diff == 1;
}
```

### 2.6 关卡 JSON：配置新牌

**文件**: `Resources/res/levels/level-2.json`

```json
{
  "Playfield": [
    { "face": 0, "suit": 2, "x": 300, "y": 900, "specialType": 1 }
  ],
  "Stack": [
    { "face": 0, "suit": 2 }
  ]
}
```

### 2.7 涉及文件清单

| 步骤 | 文件 | 改动 |
|---|---|---|
| 1 | `models/CardTypes.h` | 新增枚举值 |
| 2 | `models/CardModel.h/.cpp` | 新增属性 + getter/setter |
| 3 | `configs/models/LevelConfig.h` | Config 结构体加字段 |
| 4 | `configs/loaders/LevelConfigLoader.cpp` | JSON 解析新字段 |
| 5 | `services/GameModelGenerator.cpp` | Config → Model 传递 |
| 6 | `views/CardView.cpp` | 条件渲染新资源 |
| 7 | `controllers/PlayFieldController.cpp` | 匹配规则扩展 |
| 8 | `Resources/res/levels/*.json` | 关卡数据 |

---

## 三、如何新增一类回退操作

本章以新增"一次性撤销多步（Multi-Undo）"为例，展示回退系统的扩展点。

### 3.1 当前回退系统结构

```
UndoModel:
  ├── UndoActionType 枚举 { StackSwap, PlayFieldMatch }
  ├── UndoRecord 结构体 { type, cardAId, cardBId, posABefore, posBBefore, oldTopCardIndex, cardAWasInPlayField }
  └── std::vector<UndoRecord> (栈)

UndoManager:
  ├── recordStackSwap()      → 构建 UndoRecord → UndoModel::push()
  ├── recordPlayFieldMatch() → 构建 UndoRecord → UndoModel::push()
  ├── undo()                 → UndoModel::pop() → undoStackSwap() / undoPlayFieldMatch()
  └── _isUndoing 锁
```

### 3.2 新增步骤

#### 步骤 1：扩展 UndoActionType 枚举

**文件**: `Classes/models/UndoModel.h`

```cpp
enum class UndoActionType {
    StackSwap,
    PlayFieldMatch,
    MultiUndo       // 新增
};
```

#### 步骤 2：扩展 UndoRecord（如果需要额外数据）

**文件**: `Classes/models/UndoModel.h`

如果新回退类型需要保存更多信息，扩展 UndoRecord 结构体：

```cpp
struct UndoRecord {
    UndoActionType type;
    int cardAId;
    int cardBId;
    cocos2d::Vec2 posABefore;
    cocos2d::Vec2 posBBefore;
    int oldTopCardIndex;
    bool cardAWasInPlayField;
    
    // 新增字段（仅在 MultiUndo 类型时使用）
    std::vector<UndoRecord> batchRecords;  // 批量撤销记录
};
```

#### 步骤 3：在 UndoManager 中添加记录方法

**文件**: `Classes/managers/UndoManager.h`

```cpp
// 新增：批量记录
void recordMultiUndo(const std::vector<UndoRecord>& batch);
```

**文件**: `Classes/managers/UndoManager.cpp`

```cpp
void UndoManager::recordMultiUndo(const std::vector<UndoRecord>& batch)
{
    UndoRecord record;
    record.type = UndoActionType::MultiUndo;
    record.batchRecords = batch;
    _undoModel.push(record);
    if (_onStateChanged) _onStateChanged(_undoModel.canUndo());
}
```

#### 步骤 4：实现回退逻辑

**文件**: `Classes/managers/UndoManager.cpp`

在 `undo()` 的 switch 中增加分支：

```cpp
void UndoManager::undo()
{
    // ... 现有逻辑 ...
    
    if (record.type == UndoActionType::MultiUndo) {
        undoMultiUndo(record);
    }
}

void UndoManager::undoMultiUndo(const UndoRecord& record)
{
    // 逆序回退批量记录
    for (auto it = record.batchRecords.rbegin(); it != record.batchRecords.rend(); ++it) {
        if (it->type == UndoActionType::StackSwap) {
            undoStackSwap(*it);  // 复用现有逻辑
        } else if (it->type == UndoActionType::PlayFieldMatch) {
            undoPlayFieldMatch(*it);
        }
    }
    _isUndoing = false;
    if (_onStateChanged) _onStateChanged(_undoModel.canUndo());
}
```

> **注意**：逐条回退时每条都会播放动画。如果要一次性回退，可以跳过动画直接恢复位置，
> 或使用 Sequence 将所有动画串联。选择取决于产品需求。

#### 步骤 5：在控制器中触发记录

**文件**: 例如新增一个 "批量操作按钮" 的控制器

```cpp
// 在批量操作前收集所有待操作的 UndoRecord
std::vector<UndoRecord> batch;
for (auto& card : selectedCards) {
    UndoRecord r;
    r.type = UndoActionType::PlayFieldMatch;
    r.cardAId = card->getCardId();
    // ... 填充其他字段
    batch.push_back(r);
}
_undoManager->recordMultiUndo(batch);
```

### 3.3 更复杂的回退类型——"RemoveCard" 示例

如果要新增一种"从主牌区移除一张牌"的操作（原本不存在于游戏中）：

**步骤 1** — 在 `UndoActionType` 中增加 `RemoveCard`

**步骤 2** — 在 `UndoRecord` 中存储必要信息：
- `cardAId`：被移除的牌
- `posABefore`：移除前的位置
- `cardAWasInPlayField` = true

**步骤 3** — 实现 `recordRemoveCard()` 和 `undoRemoveCard()`：

```cpp
void UndoManager::recordRemoveCard(int cardId, const Vec2& position)
{
    UndoRecord record;
    record.type = UndoActionType::RemoveCard;
    record.cardAId = cardId;
    record.posABefore = position;
    record.cardAWasInPlayField = true;
    _undoModel.push(record);
}

void UndoManager::undoRemoveCard(const UndoRecord& record)
{
    CardModel* model = _model->findCardById(record.cardAId);
    if (!model) { _isUndoing = false; return; }
    
    // 重新创建 CardView 并放回原位置
    auto cv = CardView::create(*model);
    cv->setPosition(record.posABefore);
    _playFieldView->addChild(cv);
    _playFieldView->addCardSilent(cv);
    
    // 恢复模型状态
    model->setInPlayField(true);
    model->setPosition(record.posABefore);
    _model->getPlayFieldCards().push_back(*model);
    
    _isUndoing = false;
}
```

### 3.4 涉及文件清单（以 RemoveCard 为例）

| 步骤 | 文件 | 改动 |
|---|---|---|
| 1 | `models/UndoModel.h` | 枚举 +1，可能扩展 UndoRecord |
| 2 | `managers/UndoManager.h` | 新增 `recordRemoveCard()` |
| 3 | `managers/UndoManager.cpp` | 实现 record + undo 方法 |
| 4 | 相关 Controller | 调用 recordRemoveCard() |

---

## 四、扩展检查清单（Checklist）

无论是新增卡牌类型还是回退类型，都需要过一遍以下清单：

- [ ] **数据层** — Model 是否需要新字段？枚举是否需要新值？
- [ ] **配置层** — LevelConfig 和 LevelConfigLoader 是否需要同步？
- [ ] **服务层** — GameModelGenerator 是否需要传递新字段？
- [ ] **视图层** — 新牌类型是否需要新渲染资源？（图片放入 `Resources/res/`）
- [ ] **控制器层** — 业务规则是否需要扩展？（匹配条件、点击响应）
- [ ] **撤销层** — 新操作是否需要可撤销？如果需要，更新 UndoActionType + UndoManager
- [ ] **关卡 JSON** — 测试数据是否覆盖新功能？
- [ ] **构建文件** — 如果新增了 .cpp/.h，是否注册到 CMakeLists.txt 和 .vcxproj？
