#pragma once

#include "Hero.hpp"
#include "Monster.hpp"

#include <SFML/Graphics.hpp>
#include <array>

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
    void drawEquipmentBar();
    void updateWindowTitle();
    void drawHeroRoster();

    inline static constexpr sf::Vector2u windowSize{960, 540};

    sf::RenderWindow window;
    Hero hero;
    Monster monster;
    sf::Clock clock;
    // 纹理必须比使用它的 Sprite 活得更久，统一由 Game 保存。
    std::array<sf::Texture, 8> heroTextures;
    std::array<bool, 8> heroTextureReady{};
    std::size_t selectedHero{0};
};
