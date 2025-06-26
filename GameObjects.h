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
    int dmg;
    int maxCd;
    int curCd;
    Skill (int dmg = 0, int maxCd = 0);
};

class Player : public GameObject {
public:
    vector<Skill> skills;
    Player(int x = 0, int y = 0);
    void move(int dx, int dy);
    void addSkill(const Skill& skill);
};

class Boss  {
public:
    int hp;
    int goldDrop;
    bool defeated;
    // 只声明，不实现
    Boss( int hp = 200);
    void takeDamage(int dmg);
    bool isAlive() const;
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