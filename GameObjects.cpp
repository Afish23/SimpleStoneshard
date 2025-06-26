#include "GameObjects.h"
using namespace std;

GameObject::GameObject(int x, int y, char type)
    : x(x), y(y), type(type) {}

Skill::Skill(const string& name, int dmg, int maxCd,int curCd)
    : name(name), dmg(dmg), maxCd(maxCd),curCd(curCd) {}

Player::Player(int x, int y, int hp, int gold, int attackPower)
    : GameObject(x, y, 'P'), hp(hp), gold(gold), attackPower(attackPower) {}

void Player::move(int dx, int dy) {
    x += dx;
    y += dy;
}
void Player::addGold(int amount)
{
    gold += amount; 
}
void Player::takeDamage(int dmg) 
{
    hp -= dmg; if (hp < 0) hp = 0;
}
void Player::heal(int amount)
{
    hp += amount; 
}
void Player::normalAttack(Boss& boss)
{
    if (boss.isAlive()) {
        boss.takeDamage(attackPower);
        cout << "玩家对Boss造成了" << attackPower << "点伤害！" << endl;
    }
}
void Player::addSkill(const Skill& skill)
{ 
    skills.push_back(skill); 
}

Boss::Boss(int x, int y, int hp, int attackPower, int goldDrop, bool defeated, const vector<Skill>& skills)
    : GameObject(x, y, 'B'), hp(hp), attackPower(attackPower), goldDrop(goldDrop), defeated(defeated), skills(skills) {}

void Boss::takeDamage(int dmg)
{
    hp -= dmg;
    if (hp <= 0) { hp = 0; defeated = true; }
}
bool Boss::isAlive() const 
{
    return !defeated; 
}
void Boss::normalAttack(Player& player)
{
    if (isAlive()) {
        player.takeDamage(attackPower);
        cout << "Boss对玩家造成了" << attackPower << "点伤害！" << endl;
    }
}

void Boss::addBossSkills(const vector <Skill>& skill)
{
    skills.insert(skills.end(), skill.begin(), skill.end());
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