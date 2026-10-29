#pragma once

#include <SFML/Graphics.hpp>

// Monster 类表示游戏中的一个小怪。
// 目前它只管理自己的图形和绘制行为。
// 以后生命值、攻击力、移动速度等数据也可以继续放进这个类。
class Monster
{
public:
    // 构造函数：创建 Monster 对象时自动执行，用传入的位置初始化小怪。
    // explicit 用来阻止编译器进行意外的隐式类型转换。
    explicit Monster(sf::Vector2f position);

    // 把小怪绘制到指定窗口。
    // & 表示使用原来的窗口，不复制窗口对象。
    // 末尾的 const 表示此函数不会修改 Monster 自己的数据。
    void draw(sf::RenderWindow& window) const;

private:
    // private 成员只能由 Monster 类内部直接访问。
    // 外部代码不直接控制图形，这种保护内部数据的设计叫作“封装”。
    sf::RectangleShape shape;
};
