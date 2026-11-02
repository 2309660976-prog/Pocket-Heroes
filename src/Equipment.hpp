#pragma once

// 装备数据与外观选择分开：数值由这里定义，Hero 只负责把当前装备画出来。
enum class Weapon { WoodenSword, SteelSword };
enum class Armor { Cloth, Steel };

struct EquipmentInfo
{
    const char* name;
    int attackBonus;
    int defenseBonus;
};

inline EquipmentInfo weaponInfo(const Weapon weapon)
{
    switch (weapon)
    {
        case Weapon::WoodenSword: return {"Wooden Sword", 1, 0};
        case Weapon::SteelSword: return {"Steel Sword", 3, 0};
    }
    return {"Unknown", 0, 0};
}

inline EquipmentInfo armorInfo(const Armor armor)
{
    switch (armor)
    {
        case Armor::Cloth: return {"Cloth Armor", 0, 1};
        case Armor::Steel: return {"Steel Armor", 0, 3};
    }
    return {"Unknown", 0, 0};
}
