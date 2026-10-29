#include "Hero.hpp"

#include <algorithm>
#include <cmath>

Hero::Hero(const sf::Vector2f startPosition)
    : shape(size),       // 形状矩形：尺寸用类的静态常量 size
      targetMarker(9.f), // 目标标记：半径 9 的圆
      targetPosition(startPosition) // 初始目标 = 出生位置，游戏开始时不动
{
    // 外观设置都归 Hero 自己管，构造函数里一次配好。

    // 原点改到矩形中心：之后 setPosition 设置的就是"英雄中心"坐标，
    // 判断是否点到英雄、移动计算都按中心来，更直观。
    shape.setOrigin(size / 2.f);
    shape.setPosition(startPosition);
    shape.setFillColor(sf::Color(70, 135, 230));   // 蓝色填充 (R,G,B)

    // 4 像素描边初始为透明（看不见），选中后才变成金色——"选中反馈"。
    shape.setOutlineThickness(4.f);
    shape.setOutlineColor(sf::Color::Transparent);

    // 目标标记做成"空心圆环"：原点在圆心，内部透明，
    // 只保留 3 像素宽的金色描边，看起来就是个金色圆圈。
    targetMarker.setOrigin({9.f, 9.f});
    targetMarker.setFillColor(sf::Color::Transparent);
    targetMarker.setOutlineThickness(3.f);
    targetMarker.setOutlineColor(sf::Color(245, 195, 70));
}

void Hero::handleLeftClick(const sf::Vector2f mousePosition, const sf::Vector2u worldSize)
{
    // contains 返回 bool（点到英雄了吗？）。两种结局：
    //   true  → 只选中英雄，不设置移动目标；
    //   false → 继续往下判断"是否已选中"，已选中才设置新目标。
    // return 提前结束函数，后面的代码不会执行。
    if (contains(mousePosition))
    {
        selected = true;
        shape.setOutlineColor(sf::Color(245, 195, 70));
        return;
    }

    // 没点到英雄，且之前也没选中过 → 这次点击被忽略（空点击不产生命令）。
    if (!selected)
    {
        return;
    }

    setMoveTarget(mousePosition, worldSize);
}

void Hero::update(const float deltaTime)
{
    // offset = 目标位置 − 当前位置：从英雄指向目标的向量（有方向、有长度）。
    const sf::Vector2f offset = targetPosition - shape.getPosition();
    // 勾股定理求向量长度 = 英雄到目标的直线距离。
    const float distance = std::sqrt(offset.x * offset.x + offset.y * offset.y);
    // 本帧能走多远 = 每秒速度 × 这一帧的秒数。
    // 乘以 deltaTime 是为了和帧率解耦：无论 30 还是 120 FPS，
    // 每秒钟移动的总距离都恒为 moveSpeed，不会时快时慢。
    const float movement = moveSpeed * deltaTime;

    // distance 为 0（已在目标点）时不能执行 offset / distance（除以零会崩溃），
    // 所以先短路返回，不再移动。
    if (distance <= 0.f)
    {
        return;
    }

    // 剩余距离不足一帧能走的量：直接定位到目标点，
    // 避免"每帧越过目标一点又折返"造成来回抖动。
    // 到达后隐藏金色圆环（目的地已到，提示失去意义）。
    if (distance <= movement)
    {
        shape.setPosition(targetPosition);
        targetVisible = false;
        return;
    }

    // 还远，走一部分：offset / distance 把向量"单位化"（长度变 1、方向不变），
    // 再乘本帧移动距离 = 这一帧朝目标方向走的位移。move 在当前位置上累加。
    shape.move(offset / distance * movement);
}

void Hero::draw(sf::RenderWindow& window) const
{
    if (targetVisible)
    {
        window.draw(targetMarker);
    }

    window.draw(shape);
}

bool Hero::contains(const sf::Vector2f point) const
{
    // shape.getGlobalBounds() 返回一个 sf::FloatRect 对象（英雄的包围矩形）。
    // 函数返回值是类对象，所以可以继续用 . 调用它的成员函数，
    // .contains(point) 判断"point 是否落在矩形内部"，返回 bool。
    // 整句等价于拆成两行：
    //   const sf::FloatRect bounds = shape.getGlobalBounds();
    //   return bounds.contains(point);
    return shape.getGlobalBounds().contains(point);
}

void Hero::setMoveTarget(const sf::Vector2f position, const sf::Vector2u worldSize)
{
    // 半宽/半高 = 英雄中心到边缘的距离，用作边界限制值。
    const float halfWidth = size.x / 2.f;
    const float halfHeight = size.y / 2.f;

    // std::clamp(值, 最小, 最大) 把值限制在区间内，防止目标点太靠边，
    // 否则英雄中心移动到边缘时，半个身子会跑到窗口外面。
    // 注意：worldSize.x 是 unsigned int，clamp 三个参数都是 float，
    // 所以要用 static_cast<float> 先转类型，模板才能统一推导 T = float。
    targetPosition.x = std::clamp(position.x, halfWidth, static_cast<float>(worldSize.x) - halfWidth);
    targetPosition.y = std::clamp(position.y, halfHeight, static_cast<float>(worldSize.y) - halfHeight);

    // 金色圆环移到新目标点，并标记为可见。
    targetMarker.setPosition(targetPosition);
    targetVisible = true;
}
