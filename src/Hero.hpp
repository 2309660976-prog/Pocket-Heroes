#pragma once

#include "Equipment.hpp"
#include "WalkAtlas.hpp"

#include <SFML/Graphics.hpp>

// Hero 类封装与英雄有关的数据和行为。
// 外部代码只需要告诉英雄“鼠标点在哪里”和“过去了多少时间”，
// 不需要直接操作英雄内部的矩形、目标位置或选中状态。
class Hero
{
public:
    // 创建英雄时必须提供出生位置。
    explicit Hero(sf::Vector2f startPosition);

    // 处理一次鼠标左键点击。
    // worldSize 用于限制目标位置，防止英雄走出游戏区域。
    void handleLeftClick(sf::Vector2f mousePosition, sf::Vector2u worldSize);

    // 根据这一帧经过的时间更新英雄位置。
    void update(float deltaTime);

    // 绘制目标标记和英雄。
    void draw(sf::RenderWindow& window) const;

    // 换装只修改英雄状态；绘制时会自动采用对应外观。
    void equipWeapon(Weapon newWeapon);
    void equipArmor(Armor newArmor);
    Weapon equippedWeapon() const;
    Armor equippedArmor() const;
    int attackPower() const;
    int defense() const;
    void selectAppearance(std::size_t index, const sf::Texture* texture);

private:
    // inline static constexpr 表示所有 Hero 对象共用同一份编译期常量。
    // 这些实现细节放在 private 中，外部代码不能随意修改。
    inline static constexpr sf::Vector2f size{56.f, 56.f};
    inline static constexpr float moveSpeed = 260.f;

    // 判断一个世界坐标是否落在英雄图形内部。
    bool contains(sf::Vector2f point) const;

    // 设置新的移动目标，并确保目标位于游戏区域内。
    void setMoveTarget(sf::Vector2f position, sf::Vector2u worldSize);

    sf::RectangleShape shape;
    sf::CircleShape targetMarker;
    sf::Vector2f targetPosition;
    bool selected{false};
    bool targetVisible{false};
    Weapon weapon{Weapon::WoodenSword};
    Armor armor{Armor::Cloth};
    std::size_t appearanceIndex{0};
    const sf::Texture* walkTexture{nullptr};
    float walkTime{0.f};
    bool facingRight{true};
};
