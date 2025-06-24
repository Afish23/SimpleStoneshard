#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class GameObject {
public:
    int x, y;
    char type;
    GameObject(int x = 0, int y = 0, char type = '#');
    virtual ~GameObject() {}
};

class Skill {
public:
    string name;
    int dmg;
    int cd;
    Skill(const string& name = "", int dmg = 0, int cd = 0);
};

class Player : public GameObject {
public:
    int hp;
    int gold;
    int attackPower;
    vector<Skill> skills;
    Player(int x = 0, int y = 0, int hp = 100, int gold = 0, int attackPower = 20);
    void move(int dx, int dy);
    void addGold(int amount);
    void takeDamage(int dmg);
    void heal(int amount);
    void normalAttack(class Boss& boss);
    void addSkill(const Skill& skill);
};

class Boss : public GameObject {
public:
    int hp;
    int attackPower;
    int goldDrop;
    bool defeated;
    vector<Skill> skills;
    // 只声明，不实现
    Boss(int x = 0, int y = 0, int hp = 200, int attackPower = 30, int goldDrop = 50, bool defeated = false, const vector<Skill>& skills = {});
    void takeDamage(int dmg);
    bool isAlive() const;
    void normalAttack(Player& player);
};

class Gold : public GameObject {
public:
    int value;
    bool collected;
    Gold(int x = 0, int y = 0, int value = 10, bool collected = false);
    void collect();
};

class Track : public GameObject {
public:
    int damage;
    bool triggered;
    Track(int x = 0, int y = 0, int damage = 20, bool triggered = false);
    void trigger();
};

class Locker : public GameObject {
public:
    bool locked;
    Locker(int x = 0, int y = 0, bool locked = true);
    bool isOpen() const;
};