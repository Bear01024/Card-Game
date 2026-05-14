# 阶段 4 日志：CardView 卡牌渲染

**日期**: 2026-05-14  
**状态**: 编译通过，0 错误

## 完成内容

### 1. 资源准备
- res.zip 解压到 `Resources/res/`
- 卡牌底框: 182×282
- 花色图标: 43×43
- 大数字: 115×139
- 小数字: 38×46

### 2. CardView — 单张卡牌视图
- 继承 `cocos2d::Node`，4 层 Sprite 合成：
  - `card_general.png` — 底框 (z=0)
  - `small_{color}_{face}.png` — 左上角小数字 (z=1)
  - `suits/{suit}.png` — 右上角花色图标 (z=1)
  - `big_{color}_{face}.png` — 中间偏下大数字 (z=1)
- 颜色规则：♥♦ → `red`；♠♣ → `black`
- `bindModel(CardModel)` — 绑定数据模型并重建显示
- `setFaceUp(bool)` — 翻开/覆盖切换（覆盖时仅显示底框）
- 触摸事件：`EventListenerTouchOneByOne`，以卡牌矩形区域判定命中，触发 `onCardClicked` 回调

### 3. 图片路径命名规则
```
res/number/{big|small}_{red|black}_{A|2-10|J|Q|K}.png
res/suits/{heart|diamond|club|spade}.png
res/card_general.png
```

### 4. 构建文件更新
- `CMakeLists.txt`: 添加 CardView.cpp/.h（同时修复了重复的 GameModel.h 条目）
- `CardGame.vcxproj`: 添加对应的 ClCompile/ClInclude

## 新增文件
| 文件 | 说明 |
|------|------|
| `Classes/views/CardView.h` | 卡牌视图声明 |
| `Classes/views/CardView.cpp` | 卡牌视图实现（4层合成 + 触摸） |
| `Resources/res/` | 解压后的卡牌图片资源 |

## 修改文件
| 文件 | 变更 |
|------|------|
| `CMakeLists.txt` | 添加 CardView，修复重复条目 |
| `proj.win32/CardGame.vcxproj` | 添加 ClCompile/ClInclude |

## 编译结果
- Debug|Win32 — 0 错误，1 警告（LNK4075，引擎预置）

### 运行验证
- 程序启动正常，窗口创建成功
- HelloWorld 场景渲染正常（CardView 未在此阶段集成）
- 退出正常，无崩溃

## 备注
- 当前 CardView 未在任何场景中使用，仅确保编译通过。下一阶段在 PlayFieldView/StackView 中集成。
