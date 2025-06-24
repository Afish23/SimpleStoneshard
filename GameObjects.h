#pragma once
// 游戏对象定义模块，采用基类与派生类结构
#include <iostream>

// 游戏对象基类，含有所有共性属性
class GameObject {
public:
    int x, y;
    char type;
    GameObject(int x = 0, int y = 0, char type = '#');
    virtual ~GameObject() {}
};

// 玩家类
class Player : public GameObject {
public:
    int hp;
    int gold;
    int attackPower;
    Player(int x = 0, int y = 0, int hp = 100, int gold=0, int attackPower = 20);
    void move(int dx, int dy);
    void addGold(int amount);
    void takeDamage(int dmg);
    void heal(int amount);
    // 玩家普攻技能
    void normalAttack(class Boss& boss);
};

// Boss类
class Boss : public GameObject {
public:
    int hp;
    int attackPower;
    int goldDrop;
    bool defeated;
    Boss(int x = 0, int y = 0, int hp = 200, int attackPower = 30, int goldDrop = 50, bool defeated = false);
    void takeDamage(int dmg);
    bool isAlive() const;
    // Boss普攻技能
    void normalAttack(Player& player);
};


// 金币类
class Gold : public GameObject {
public:
    int value;
    bool collected;
    Gold(int x = 0, int y = 0, int value = 10, bool collected=false);
    void collect();
};

// 陷阱类
class Track : public GameObject {
public:
    int damage;
    bool triggered;
    Track(int x = 0, int y = 0, int damage = 20, bool triggered=false);
    void trigger();
};

// 机关/宝箱类
class Locker : public GameObject {
public:
    bool locked;

    Locker(int x = 0, int y = 0, bool locked = true);

   
    bool isOpen() const;
};