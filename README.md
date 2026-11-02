# Pocket Heroes

这是 Pocket Heroes 的 SFML 3 学习原型。现在可以在游戏窗口中选择八位英雄并查看左右行走动作：

1. 点击窗口底部的八个头像格切换英雄，金色边框表示当前英雄。
2. 英雄会原地循环播放四帧动作，便于检查迈步；点击英雄，再点击场景位置，可以观察实际向左或向右移动。
3. 点击左上角前两个格子切换木剑/钢剑，后两个格子切换布甲/钢甲。

窗口标题显示当前英雄、装备与攻击防御数值。角色动作图里已经画有固定服装和武器，因此目前换装只在角色脚边用两个色块提示；要让衣服和武器真正跟随每一帧变化，还需要绘制对齐的装备图层。战斗、背包和存档尚未实现。

## 编译和运行

在项目目录打开 PowerShell：

```powershell
.\build.ps1
.\build\PocketHeroes.exe
```

也可以在资源管理器中双击 `build\PocketHeroes.exe`。游戏采用 Windows 图形程序模式，不会另外弹出命令提示符；从 PowerShell 启动时，原来的 PowerShell 窗口仍会保持打开。

脚本已经根据当前电脑上的安装位置配置了默认值。以后移动 SFML 或更换编译器时，可以这样指定路径：

```powershell
$env:CXX = "D:\msys2\ucrt64\bin\g++.exe"
$env:SFML_ROOT = "D:\你的SFML目录\SFML-3.1.0"
.\build.ps1
```

## 这一版学到了什么

- `sf::RenderWindow`：创建游戏窗口。
- `sf::RectangleShape`：创建代表英雄的小方块。
- `sf::Event::MouseButtonPressed`：接收鼠标点击。
- `getGlobalBounds().contains(...)`：判断是否点中了英雄。
- `sf::Clock` 和 `deltaTime`：让移动速度不受帧率影响。
- `目标位置 - 当前位置`：计算英雄的移动方向。

代码已经按照职责拆分：

- `src/main.cpp`：程序入口，只创建并启动游戏。
- `src/Game.hpp` 和 `src/Game.cpp`：窗口、输入、更新和绘制流程。
- `src/Hero.hpp` 和 `src/Hero.cpp`：英雄的外观、选中和移动逻辑。
- `src/Equipment.hpp`：装备类型和数值定义。
- `src/WalkAtlas.hpp`：根据 `src/assets/heroes/walk-atlas.json` 整理的逐帧裁切与脚底锚点。
- `src/Monster.hpp` 和 `src/Monster.cpp`：小怪自己的数据和绘制逻辑。

这种设计让每个类只管理自己的职责，main 不需要知道对象内部如何实现。

## 和朋友一起开发

从克隆仓库、配置 SFML 到提交修改和发起 Pull Request，参见 [协作指南](docs/协作指南.md)。

## 后续小步骤

下一步可以实现：点击英雄后再点击红色小怪，英雄自动接近小怪，并在进入攻击距离后停止。之后再逐步加入生命值和普通攻击。
