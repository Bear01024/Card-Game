# 阶段 3 日志：配置层实现

**日期**: 2026-05-14  
**状态**: 编译通过，0 错误

## 完成内容

### 1. LevelConfig — 关卡静态配置结构体
- `PlayfieldCardConfig`: face, suit, position (Vec2)
- `StackCardConfig`: face, suit
- `LevelConfig`: levelId + playfield 数组 + stack 数组

### 2. LevelConfigLoader — JSON 配置加载器
- 使用 cocos2d-x 内置的 rapidjson 解析 (external/json/)
- `loadFromFile(filePath)` — 静态方法，读取 JSON 并反序列化为 LevelConfig
- JSON 格式匹配设计文档中定义的关卡配置

### 3. CardTypes.h 枚举值对齐
- `CardFaceType` 改为 0-based（CFT_ACE=0, CFT_KING=12），与 JSON CardFace 值直接映射
- `CardModel::getFaceString()` 中 default 分支适配为 `+1` 偏移

### 4. 构建文件更新
- `CMakeLists.txt`: 添加 LevelConfigLoader.cpp + 2 个 .h
- `CardGame.vcxproj`: 添加对应的 ClCompile/ClInclude 条目

## 新增文件
| 文件 | 说明 |
|------|------|
| `Classes/configs/models/LevelConfig.h` | 关卡静态配置结构体 |
| `Classes/configs/loaders/LevelConfigLoader.h` | JSON 加载器声明 |
| `Classes/configs/loaders/LevelConfigLoader.cpp` | JSON 加载器实现（rapidjson） |

## 修改文件
| 文件 | 变更 |
|------|------|
| `Classes/models/CardTypes.h` | CFT_ACE 0→0, CFT_KING 13→12（与 JSON 对齐） |
| `Classes/models/CardModel.cpp` | getFaceString default 分支 +1 偏移 |
| `CMakeLists.txt` | 添加 3 个配置层源文件 |
| `proj.win32/CardGame.vcxproj` | 添加 ClCompile/ClInclude 条目 |

## 编译结果
- Debug|Win32 — 0 错误，1 警告（LNK4075，引擎预置）
