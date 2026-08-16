# Pocket Heroes

这是 Pocket Heroes 的第一个 SFML 3 学习原型。目前只实现一个最基础、但以后可以继续扩展的操作循环：

1. 鼠标左键点击蓝色方块，选中英雄。
2. 鼠标左键点击场景中的其他位置。
3. 英雄平滑移动到金色圆圈处。

## 编译和运行

在项目目录打开 PowerShell：

```powershell
.\build.ps1
.\build\PocketHeroes.exe
```

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

主要代码位于 `src/main.cpp`。移动速度由 `moveSpeed` 控制，窗口大小由 `windowSize` 控制。

## 后续小步骤

下一步可以添加一个红色小怪：点击英雄后再点击小怪，英雄自动接近小怪，并在进入攻击距离后停止。之后再逐步加入生命值和普通攻击。
