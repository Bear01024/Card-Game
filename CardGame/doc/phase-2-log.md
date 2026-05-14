# 阶段 2 日志：数据层实现

**日期**: 2026-05-14  
**状态**: 编译通过，0 错误

## 完成内容

### 1. CardTypes.h — 枚举定义
- `CardSuitType`: CST_CLUBS(梅花♣), CST_DIAMONDS(方块♦), CST_HEARTS(红桃♥), CST_SPADES(黑桃♠)
- `CardFaceType`: CFT_ACE(1) ~ CFT_KING(13)，与设计文档的 thrift 定义一致

### 2. CardModel — 卡牌数据模型
- 字段: `_id`, `_face`, `_suit`, `_position`, `_isFaceUp`, `_isInPlayField`
- 辅助方法: `isRedSuit()` (♥♦为红色), `getFaceString()` (数字→字符串 A/J/Q/K)

### 3. GameModel — 游戏运行时数据
- `_playFieldCards` — 主牌区所有牌
- `_stackCards` — 手牌区所有牌
- `_topCardIndex` — 顶牌索引（-1=无顶牌）
- `getTopCard()`, `findCardById()` 查询方法

### 4. UndoModel — 回退数据模型
- `UndoActionType`: StackSwap / PlayFieldMatch
- `UndoRecord`: 记录 cardAId, cardBId, 移动前后位置, 旧顶牌索引
- `UndoModel`: 栈式 push/pop，`canUndo()` 判断是否可回退

### 5. 构建文件更新
- `CMakeLists.txt`: 添加 3 个 .cpp + 4 个 .h
- `CardGame.vcxproj`: 添加对应的 ClCompile 和 ClInclude

## 新增文件
| 文件 | 说明 |
|------|------|
| `Classes/models/CardTypes.h` | 花色/点数枚举 |
| `Classes/models/CardModel.h` | 卡牌数据模型声明 |
| `Classes/models/CardModel.cpp` | 卡牌数据模型实现 |
| `Classes/models/GameModel.h` | 游戏数据模型声明 |
| `Classes/models/GameModel.cpp` | 游戏数据模型实现 |
| `Classes/models/UndoModel.h` | 回退数据模型声明 |
| `Classes/models/UndoModel.cpp` | 回退数据模型实现 |

## 修改文件
| 文件 | 变更 |
|------|------|
| `CMakeLists.txt` | 添加 7 个模型源文件 |
| `proj.win32/CardGame.vcxproj` | 添加 ClCompile/ClInclude 条目 |

## 遇到的问题
- **MSVC 代码页 936 不支持 UTF-8 中文注释**：头文件中的中文注释导致编译器解析异常（C2550, C2535）。已移除中文注释改用英文标识符自说明。
- 链接器警告 LNK4075 为引擎项目已有配置，不影响运行。

## 编译结果
- Debug|Win32 — 0 错误，1 警告（LNK4075，引擎预配置）
- 卡牌模型 .obj 全部成功链接
