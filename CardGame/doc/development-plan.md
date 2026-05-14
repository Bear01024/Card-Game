# 开发计划 — 纸牌游戏（需求一）v2

## 一、项目初始化

### 1.1 调整设计分辨率与窗口配置

修改 `AppDelegate.cpp`：
- 窗口大小：1080×2080
- 设计分辨率：1080×2080，`ResolutionPolicy::FIXED_WIDTH`
- 移除旧的 480×320 三档分辨率适配逻辑
- 初始场景改为 GameScene（新建）

### 1.2 建立 MVC 目录结构

在 `Classes/` 下创建：

```
Classes/
├── configs/
│   ├── models/          # 静态配置类 (LevelConfig, CardResConfig)
│   └── loaders/         # 配置加载器 (LevelConfigLoader)
├── models/              # 运行时数据 (CardModel, GameModel, UndoModel)
├── views/               # UI 组件 (CardView, PlayFieldView, StackView, GameView)
├── controllers/         # 控制器 (GameController, PlayFieldController, StackController)
├── managers/            # 状态管理器 (UndoManager)
├── services/            # 无状态服务 (GameModelGenerator)
└── utils/               # 工具类
```

### 1.3 更新构建设置

新增的 `.cpp`/`.h` 同步注册到 `CMakeLists.txt` 和 `proj.win32/CardGame.vcxproj`。

---

## 二、数据层 (models)

### 2.1 枚举与类型 (`CardTypes.h`)

```cpp
enum CardSuitType { CST_NONE = -1, CST_CLUBS, CST_DIAMONDS, CST_HEARTS, CST_SPADES };
enum CardFaceType { CFT_NONE = -1, CFT_ACE, CFT_TWO, CFT_THREE, CFT_FOUR, CFT_FIVE,
                    CFT_SIX, CFT_SEVEN, CFT_EIGHT, CFT_NINE, CFT_TEN,
                    CFT_JACK, CFT_QUEEN, CFT_KING };
```

花色颜色：♥♦ 红色，♠♣ 黑色。

### 2.2 CardModel — 卡牌数据

```cpp
class CardModel {
    int id;                  // 唯一标识
    CardFaceType face;       // 点数
    CardSuitType suit;       // 花色
    cocos2d::Vec2 position;  // 当前位置
    bool isFaceUp;           // 是否翻开
    bool isInPlayField;      // true=主牌区, false=手牌区
    // 序列化/反序列化
};
```

### 2.3 GameModel — 游戏运行时数据

```cpp
class GameModel {
    std::vector<CardModel> playFieldCards;  // 主牌区所有牌
    std::vector<CardModel> stackCards;      // 手牌区所有牌
    int topCardIndex;                       // 手牌区顶牌（右侧待匹配牌）在 stackCards 中的索引
    // 序列化/反序列化
};
```

### 2.4 UndoModel — 回退记录

```cpp
enum class UndoActionType { StackSwap, PlayFieldMatch };

struct UndoRecord {
    UndoActionType type;
    int cardAId;              // 操作的牌
    int cardBId;              // 关联的牌（如被替换的旧顶牌）
    cocos2d::Vec2 posABefore; // cardA 移动前的位置
    cocos2d::Vec2 posBBefore; // cardB 移动前的位置（如有）
};
```

回退栈：`std::vector<UndoRecord>`，每次操作 push，undo 时 pop 并逆向复原。

---

## 三、视图层 (views)

### 3.1 卡牌渲染说明

每张卡牌由 4 层图片叠加合成（基于 `res.zip`）：

```
card_general.png        ← 底框
  + small_number        ← 左上角（如 small_red_A.png）
  + suit icon           ← 右上角（如 heart.png）
  + big_number          ← 中间偏下（如 big_red_A.png）
```

颜色规则：♥♥ 红色用 `red` 系列；♠♣ 黑色用 `black` 系列。

### 3.2 CardView — 单张卡牌视图

- 继承 `cocos2d::Node`，内部组合 4 个 Sprite 子节点
- 构造函数传入 `CardModel&`，根据 face/suit 选择对应图片
- 正面：4 层完整合成；背面：仅显示 card_general（或专用牌背图）
- `setPosition()` 同步更新绑定的 CardModel.position
- 触摸事件监听，触发回调 `std::function<void(CardView*)> onCardClicked`

### 3.3 PlayFieldView — 主牌区视图

- 尺寸：1080×1500，位于屏幕上方
- 管理所有 playField 的 CardView，按初始 Y 坐标设置 z-order（Y 越小→越靠屏幕下方→z-order 越高→视觉上越靠前）
- **遮挡判定**：卡牌 A 被卡牌 B 遮挡当且仅当：
  1. B 的 z-order > A 的 z-order（B 在 A 上层）
  2. B 的包围盒与 A 的包围盒有重叠（矩形相交判定）
- 被遮挡的牌不响应触摸事件（或触摸时判定为不可操作并忽略）
- 暴露 `onPlayFieldCardClick(CardView*)` 回调
- 当牌被移除后，重新计算下层牌的遮挡状态并更新触摸可用性

### 3.4 StackView — 手牌区视图

- 尺寸：1080×580，位于屏幕下方
- 布局分为左右两部分：

```
  [左侧手牌区]          [间隙]    [右侧顶牌区]
  ┌──┐┌──┐┌──┐                  ┌────┐
  │♣2││♦K││♠5│  ...    ←→     │ ♥A │  ← 待匹配顶牌
  └──┘└──┘└──┘                  └────┘
   水平排列，部分层叠             单独放大展示
```

- 左侧手牌：水平排列、部分遮挡（后面牌被前面牌遮住一部分），全部翻开
- 右侧顶牌：单张独立显示，为当前待匹配的基准牌
- 左右之间有视觉间隙以区分
- 暴露两个回调：`onHandCardClick(CardView*)` / `onTopCardClick(CardView*)`

### 3.5 GameView — 游戏整体视图

- 组合 PlayFieldView + StackView + 回退按钮（UIButton）
- 回退按钮位于手牌区右侧附近
- 绑定 `onUndoClick` 回调

---

## 四、控制器层 (controllers)

### 4.1 GameController — 游戏主控制器

- 持有 `GameModel*`, `GameView*`, `PlayFieldController*`, `StackController*`, `UndoManager*`
- `startGame(levelId)` 流程：
  1. `LevelConfigLoader::loadLevelConfig(levelId)` → `LevelConfig`
  2. `GameModelGenerator::generateGameModel(config)` → `GameModel`
  3. 初始化子控制器和 UndoManager
  4. 创建 GameView 并关联各子 View
  5. 播放入场动画（如有）

### 4.2 PlayFieldController — 主牌区控制器

- 注册 PlayFieldView 的 `onPlayFieldCardClick` 回调
- `handleCardClick(CardView* cardView)`：
  1. **遮挡判定**：调用 `isCardClickable(cardView)` 检查该牌是否被上层牌遮挡。若被遮挡，忽略点击（可加一个微小的抖动反馈）
  2. 获取被点击牌的 face 值
  3. 获取当前手牌区顶牌的 face 值
  4. 判断 `abs(clickedCard.face - topCard.face) == 1`
  5. **匹配成功**：
     - 记录 UndoRecord（被点击牌原位置 + 旧顶牌位置）
     - 更新 Model：从 playFieldCards 移除该牌，插入 stackCards 并设为新 topCardIndex
     - 旧顶牌留在 stackCards 中但不再为 top（被"压"在下面）
     - 调用 PlayFieldView 播放：桌面牌 MoveTo 到右侧顶牌位置
     - 旧顶牌视觉上降低 z-order，被新牌遮挡
     - **重新计算**：移除牌后，原被该牌遮挡的下层牌需更新遮挡状态，变为可点击
- `isCardClickable(CardView*)` — 遍历所有 z-order 高于该牌的其他牌，判断包围盒是否相交

### 4.3 StackController — 手牌区控制器

- 注册 StackView 的 `onHandCardClick` 和 `onTopCardClick` 回调
- **左侧手牌点击**（需求1）：
  1. 记录 UndoRecord（被点击手牌位置 + 顶牌位置）
  2. 更新 Model：交换被点击牌与 topCardIndex
  3. 调用 StackView 播放双向 MoveTo 动画：被点击牌 → 顶牌位，旧顶牌 → 被点击牌位

---

## 五、回退功能 (UndoManager)

- 作为 GameController 的成员，持有 `std::vector<UndoRecord>`
- `push(UndoRecord)` — 操作发生时压入
- `pop()` → `UndoRecord` — 回退时弹出最新记录
- `undo()`：
  1. 弹出栈顶 UndoRecord
  2. 根据 type 逆向复原：
     - `StackSwap`：两张牌 MoveTo 回各自原位置，复原 topCardIndex
     - `PlayFieldMatch`：桌面牌 MoveTo 回原 PlayField 位置，旧顶牌恢复为 topCardIndex
  3. 连续调用依次回退，直到栈空
- 回退按钮在栈空时置灰不可点击

---

## 六、配置加载与服务层

### 6.1 LevelConfig (`configs/models/LevelConfig.h`)

```cpp
struct PlayfieldCardConfig {
    CardFaceType face;
    CardSuitType suit;
    cocos2d::Vec2 position;
};

struct StackCardConfig {
    CardFaceType face;
    CardSuitType suit;
    // Position 均为 {0,0}，由 StackView 计算实际布局位置
};

struct LevelConfig {
    std::vector<PlayfieldCardConfig> playfield;
    std::vector<StackCardConfig> stack;
};
```

### 6.2 LevelConfigLoader (`configs/loaders/LevelConfigLoader.h`)

- 从 JSON 文件读取关卡配置
- JSON 示例（与需求文档一致）：
```json
{
    "Playfield": [
        {"CardFace": 12, "CardSuit": 0, "Position": {"x": 250, "y": 1000}},
        ...
    ],
    "Stack": [
        {"CardFace": 2, "CardSuit": 0, "Position": {"x": 0, "y": 0}},
        ...
    ]
}
```

### 6.3 GameModelGenerator (`services/GameModelGenerator.h`)

- `generateGameModel(LevelConfig)` → `GameModel*`
- 将静态配置转为运行时数据：分配 id、设置初始 isFaceUp（playField 待确认、stack 全 true）、指定初始 topCardIndex

---

## 七、实施顺序

| 阶段 | 内容 | 依赖 | 说明 |
|------|------|------|------|
| 1 | 调整分辨率、建立目录结构 | — | AppDelegate 改造 |
| 2 | CardTypes 枚举 + CardModel + GameModel + UndoModel | 1 | 纯数据层 |
| 3 | LevelConfig + LevelConfigLoader | 1 | JSON 解析 |
| 4 | CardView（4层合成 + 触摸） | 2 | 核心渲染 |
| 5 | PlayFieldView + StackView + GameView | 4 | StackView 注意左右分区布局 |
| 6 | GameModelGenerator | 2, 3 | 静态→运行时转换 |
| 7 | PlayFieldController + StackController | 5, 6 | 业务逻辑核心 |
| 8 | UndoManager + 回退按钮 | 7 | 回退功能 |
| 9 | GameController 组装 + 关卡 JSON 落地 | 7, 8 | 串联全部 |
| 10 | 联调测试 + 动画调优 | 全部 | 端到端验证 |

---

## 八、资源文件

- **res.zip** 解压到 `Resources/res/`，结构：
  - `res/card_general.png` — 卡牌底框
  - `res/suits/` — 花色图标（spade/heart/diamond/club.png）
  - `res/number/` — 数字字母
    - `big_red_*.png` / `big_black_*.png` — 中大数字（♥♦用red，♠♣用black）
    - `small_red_*.png` / `small_black_*.png` — 左上角小数字
- 关卡 JSON 放入 `Resources/res/levels/`
