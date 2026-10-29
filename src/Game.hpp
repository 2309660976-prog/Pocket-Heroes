#pragma once

#include "Hero.hpp"
#include "Monster.hpp"

#include <SFML/Graphics.hpp>

// Game 类是当前程序的最高层管理者。
// 它负责窗口和游戏循环，但不直接修改英雄或小怪的内部图形。
class Game
{
public:
    Game();

    // 启动游戏主循环。main 只需要调用这个公开函数。
    void run();

private:
    // 下面三个函数分别对应一帧的三个阶段：输入、更新、绘制。
    void processEvents();
    void update(float deltaTime);
    void render();

    inline static constexpr sf::Vector2u windowSize{960, 540};

    sf::RenderWindow window;
    Hero hero;
    Monster monster;
    sf::Clock clock;
};
