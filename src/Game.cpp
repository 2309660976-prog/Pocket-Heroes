#include "Game.hpp"

#include <optional>
#include <filesystem>
#include <string>

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
    // 先尝试从可执行文件所在的 build 目录找素材，再兼容项目根目录启动。
    std::filesystem::path assetDirectory = "assets/heroes";
    if (!std::filesystem::exists(assetDirectory))
        assetDirectory = "build/assets/heroes";
    for (std::size_t index = 0; index < heroWalkArts.size(); ++index)
        heroTextureReady[index] = heroTextures[index].loadFromFile(assetDirectory / heroWalkArts[index].file);
    hero.selectAppearance(0, heroTextureReady[0] ? &heroTextures[0] : nullptr);
    updateWindowTitle();
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

            // 底部八个头像格只切换角色，不向场景发出移动指令。
            if (mousePosition.y >= 445.f)
            {
                for (std::size_t index = 0; index < heroWalkArts.size(); ++index)
                {
                    if (sf::FloatRect({20.f + index * 115.f, 449.f}, {94.f, 76.f}).contains(mousePosition))
                    {
                        selectedHero = index;
                        hero.selectAppearance(index, heroTextureReady[index] ? &heroTextures[index] : nullptr);
                        updateWindowTitle();
                        break;
                    }
                }
                continue;
            }

            // 顶部四个装备格先于场景处理，点击装备不会误触发英雄移动。
            constexpr float buttonY = 20.f;
            constexpr float buttonSize = 44.f;
            const float buttonX[] = {20.f, 75.f, 150.f, 205.f};
            bool clickedEquipment = false;
            for (int index = 0; index < 4; ++index)
            {
                if (!sf::FloatRect({buttonX[index], buttonY}, {buttonSize, buttonSize}).contains(mousePosition))
                    continue;

                if (index < 2)
                    hero.equipWeapon(index == 0 ? Weapon::WoodenSword : Weapon::SteelSword);
                else
                    hero.equipArmor(index == 2 ? Armor::Cloth : Armor::Steel);

                updateWindowTitle();
                clickedEquipment = true;
                break;
            }
            if (clickedEquipment)
                continue;

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
    drawEquipmentBar();
    drawHeroRoster();

    // display 把这一帧绘制的内容一次性真正显示到屏幕。
    // 之前所有 draw 只是画到后台缓冲，display 才"亮出来"。
    window.display();
}

void Game::drawEquipmentBar()
{
    // 两格武器、两格护甲；金色边框表示当前穿戴的装备。
    const float buttonX[] = {20.f, 75.f, 150.f, 205.f};
    const sf::Color equipmentColors[] = {
        sf::Color(145, 90, 45), sf::Color(220, 230, 240),
        sf::Color(135, 105, 75), sf::Color(165, 180, 195)};
    for (int index = 0; index < 4; ++index)
    {
        sf::RectangleShape button({44.f, 44.f});
        button.setPosition({buttonX[index], 20.f});
        button.setFillColor(sf::Color(65, 70, 80));
        button.setOutlineThickness(3.f);
        const bool active = index < 2
            ? hero.equippedWeapon() == (index == 0 ? Weapon::WoodenSword : Weapon::SteelSword)
            : hero.equippedArmor() == (index == 2 ? Armor::Cloth : Armor::Steel);
        button.setOutlineColor(active ? sf::Color(245, 195, 70) : sf::Color(105, 110, 120));
        window.draw(button);

        sf::RectangleShape icon(index < 2 ? sf::Vector2f{8.f, 28.f} : sf::Vector2f{28.f, 24.f});
        icon.setPosition({buttonX[index] + (index < 2 ? 18.f : 8.f), index < 2 ? 28.f : 30.f});
        icon.setFillColor(equipmentColors[index]);
        window.draw(icon);
    }
}

void Game::updateWindowTitle()
{
    // 不依赖字体资源，窗口标题直接显示装备名称与当前数值。
    window.setTitle(std::string("Pocket Heroes | Hero: ") + heroWalkArts[selectedHero].name +
                    " | Weapon: " + weaponInfo(hero.equippedWeapon()).name +
                    " | Armor: " + armorInfo(hero.equippedArmor()).name +
                    " | ATK: " + std::to_string(hero.attackPower()) +
                    " DEF: " + std::to_string(hero.defense()));
}

void Game::drawHeroRoster()
{
    // 每格使用同一张动作图的右行第一帧；金边表示当前正在预览的英雄。
    for (std::size_t index = 0; index < heroWalkArts.size(); ++index)
    {
        const float left = 20.f + index * 115.f;
        sf::RectangleShape button({94.f, 76.f});
        button.setPosition({left, 449.f});
        button.setFillColor(sf::Color(55, 60, 70));
        button.setOutlineThickness(3.f);
        button.setOutlineColor(index == selectedHero ? sf::Color(245, 195, 70) : sf::Color(105, 110, 120));
        window.draw(button);
        if (!heroTextureReady[index])
            continue;
        const WalkFrame& frame = heroWalkArts[index].frames[4];
        sf::Sprite sprite(heroTextures[index], sf::IntRect({frame.x, frame.y}, {frame.width, frame.height}));
        sprite.setOrigin({static_cast<float>(frame.originX), static_cast<float>(frame.originY)});
        sprite.setScale({0.14f, 0.14f});
        sprite.setPosition({left + 47.f, 518.f});
        window.draw(sprite);
    }
}
