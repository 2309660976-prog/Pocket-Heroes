#include "Game.hpp"

#include <optional>

// 冒号后的"成员初始化列表"：在进入构造函数大括号前，
// 按 Game.hpp 中成员声明的顺序依次创建 window、hero、monster。
// 这样比在函数体里赋值更高效（对象只创建一次，不先默认构造再赋值）。
Game::Game()
    : window(
          sf::VideoMode(windowSize),
          "Pocket Heroes - Click Hero, Then Click Destination",
          sf::Style::Default),
      hero({180.f, 270.f}),
      monster({760.f, 270.f})
{
    // 限制最高帧率 120 FPS：不限制的话游戏会满速空转，浪费 CPU/GPU。
    window.setFramerateLimit(120);
}

void Game::run()
{
    // 游戏主循环：只要窗口开着就反复执行"输入 → 更新 → 绘制"三步。
    // 每一轮循环就是一帧画面，window.isOpen() 为 false（窗口被关闭）时结束。
    // 三个步骤的职责分离：
    //   processEvents()  —— 处理鼠标、关闭等所有输入事件
    //   update(deltaTime) —— 让游戏对象随时间推进（如英雄移动）
    //   render()          —— 把当前状态绘制到窗口
    while (window.isOpen())
    {
        processEvents();
        update(clock.restart().asSeconds());
        render();
    }
}

void Game::processEvents()
{
    // window.pollEvent() 的返回类型是 std::optional<sf::Event>：
    // 有事件时装着 sf::Event 对象，没事件时是空盒子。
    // 每轮循环重新声明一个 event，取下一个事件；取到空就结束循环。
    // 用 while 而不是 if：一帧内可能积累多个事件，要全部处理完。
    while (const std::optional event = window.pollEvent())
    {
        // event->is<sf::Event::Closed>()："这个事件是关闭窗口事件吗？"
        // is 是 sf::Event 的成员函数模板，<类型> 是模板实参，返回 bool。
        // -> 能这样用，是因为 optional 重载了 operator->：
        // 先取出内部 sf::Event 的指针，再通过指针调用它的成员函数。
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }

        // getIf<类型>() 也是成员函数模板，但返回的是指针：
        // 事件类型匹配时返回指向内部事件的指针，不匹配时返回 nullptr。
        // 所以用 if (const auto* p = ...) 判断"非空"即可安全使用。
        if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
        {
            // 当前游戏只响应鼠标左键，其他按键直接跳过本轮处理。
            if (mousePressed->button != sf::Mouse::Button::Left)
            {
                continue;
            }

            // 鼠标事件给的是窗口像素坐标，mapPixelToCoords 转成游戏世界坐标。
            // 具体如何处理这次点击，Hero 自己负责，Game 不关心细节。
            const sf::Vector2f mousePosition = window.mapPixelToCoords(mousePressed->position);
            hero.handleLeftClick(mousePosition, windowSize);
        }
    }
}

void Game::update(const float deltaTime)
{
    // deltaTime 是"上一帧到这一帧经过的秒数"（clock.restart() 测得）。
    // Game 只负责"什么时候更新"，具体怎么动由 Hero::update 自己决定。
    hero.update(deltaTime);
}

void Game::render()
{
    // 清空上一帧画面，涂上背景色，避免新旧画面叠加成残影。
    window.clear(sf::Color(30, 35, 45));

    // 每个对象都"自己画自己"：把 window 引用传进去即可。
    // 绘制顺序决定遮挡顺序：后绘制的对象显示在更上层，
    // 所以先画怪物、再画英雄，英雄会盖在怪物上面。
    monster.draw(window);
    hero.draw(window);

    // display 把这一帧绘制的内容一次性真正显示到屏幕。
    // 之前所有 draw 只是画到后台缓冲，display 才"亮出来"。
    window.display();
}
