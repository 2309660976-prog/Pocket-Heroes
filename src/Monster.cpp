#include "Monster.hpp"

// 冒号后面是“成员初始化列表”。
// 在进入构造函数的大括号之前，shape 会先被创建为 56×56 的矩形。
Monster::Monster(const sf::Vector2f position)
    : shape({56.f, 56.f})   // 成员初始化列表：创建 56×56 的红色矩形
{
    // 原点默认在矩形左上角；改成中心后，
    // setPosition(position) 设置的就是小怪中心的位置。
    // shape.getSize() 返回 56×56（Vector2f），除以 2 得 (28, 28)。
    shape.setOrigin(shape.getSize() / 2.f);
    shape.setPosition(position);

    // 暂时用红色方块代替小怪图片，以后可换成贴图。
    shape.setFillColor(sf::Color(205, 70, 70));
}

void Monster::draw(sf::RenderWindow& window) const
{
    // Monster 自己负责绘制内部的 shape。
    // main 只需要调用 draw，不需要知道小怪内部使用了什么图形。
    window.draw(shape);
}
