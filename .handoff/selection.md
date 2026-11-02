# 英雄选择与行走预览交接

适用项目：`E:\Pocket Heroes`。改动文件：`src/Game.cpp`、`src/Game.hpp`、`src/Hero.cpp`、`src/Hero.hpp`、`src/WalkAtlas.hpp`、`build.ps1`、`README.md` 和本记录。

底部八个头像格切换当前英雄；英雄原地以六帧每秒循环四个行走姿态，点击英雄再点击场景可检查实际向左/向右移动。逐帧取图范围与脚底锚点根据 `src/assets/heroes/walk-atlas.json` 写入 `src/WalkAtlas.hpp`，动作图仍在原位置。八张纹理由 `Game` 持有，`Hero` 只借用纹理，避免 Sprite 引用失效。构建脚本将八张动作图复制到 `build/assets/heroes`。

验证：使用本机 SFML 3.1.0、MSYS2 g++ 重新编译通过；`build/assets/heroes` 含八张动作图；程序启动两秒仍在运行。尚未人工检查每位英雄的动作是否自然、方向与装备是否始终一致。

限制：本次是“选择外观并预览行走”的游戏内原型。不同英雄尚未有独立技能或数值；已有装备选择器的数值仍可用，但动作图包含固定服装，局内换装仅以脚边色块表示。若调整原动作图或 `walk-atlas.json`，需同步更新 `src/WalkAtlas.hpp`。其他会话地图文件未纳入本次改动。
