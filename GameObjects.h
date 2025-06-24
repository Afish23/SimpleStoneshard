#pragma once
// ��Ϸ������ģ�飬���û�����������ṹ
#include <iostream>

// ��Ϸ������࣬�������й�������
class GameObject {
public:
    int x, y;
    char type;
    GameObject(int x = 0, int y = 0, char type = '#');
    virtual ~GameObject() {}
};

// �����
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
    // ����չ�����
    void normalAttack(class Boss& boss);
};

// Boss��
class Boss : public GameObject {
public:
    int hp;
    int attackPower;
    int goldDrop;
    bool defeated;
    Boss(int x = 0, int y = 0, int hp = 200, int attackPower = 30, int goldDrop = 50, bool defeated = false);
    void takeDamage(int dmg);
    bool isAlive() const;
    // Boss�չ�����
    void normalAttack(Player& player);
};


// �����
class Gold : public GameObject {
public:
    int value;
    bool collected;
    Gold(int x = 0, int y = 0, int value = 10, bool collected=false);
    void collect();
};

// ������
class Track : public GameObject {
public:
    int damage;
    bool triggered;
    Track(int x = 0, int y = 0, int damage = 20, bool triggered=false);
    void trigger();
};

// ����/������
class Locker : public GameObject {
public:
    bool locked;

    Locker(int x = 0, int y = 0, bool locked = true);

   
    bool isOpen() const;
};