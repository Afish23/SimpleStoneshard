#include "GameObjects.h"
using namespace std;

GameObject::GameObject(int x, int y, char type)
    : x(x), y(y), type(type) {}

Skill::Skill(int dmg, int maxCd)
    : dmg(dmg), maxCd(maxCd), curCd(0) {}

Player::Player(int x, int y)
    : GameObject(x, y, 'P') {}

void Player::move(int dx, int dy) {
    x += dx;
    y += dy;
}


void Player::addSkill(const Skill& skill)
{
    skills.push_back(skill);
}

Boss::Boss(int hp)
    : hp(hp), defeated(false), goldDrop(10) {}

void Boss::takeDamage(int dmg)
{
    hp -= dmg;
    if (hp <= 0) { hp = 0; defeated = true; }
}
bool Boss::isAlive() const
{
    return !defeated;
}



Gold::Gold(int x, int y, int value, bool collected)
    : GameObject(x, y, 'G'), value(value), collected(collected) {}

void Gold::collect()
{
    collected = true;
}

Track::Track(int x, int y, int damage, bool triggered)
    : GameObject(x, y, 'T'), damage(damage), triggered(triggered) {}

void Track::trigger()
{
    triggered = true;
}

Locker::Locker(int x, int y, bool locked)
    : GameObject(x, y, 'L'), locked(locked) {}

bool Locker::isOpen() const
{
    return !locked;
}