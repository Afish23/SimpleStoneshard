#include "GameObjects.h"
using namespace std;

GameObject::GameObject(int x, int y, char type)
{
	this->x = x;
	this->y = y;
	this->type = type;
}

inline Player::Player(int x, int y, int hp, int gold,int attackPower)
{
	this->x = x;
	this->y = y;
	this->hp = hp;
	this->gold = gold;
	this->attackPower = attackPower;
}

void Player::move(int dx, int dy)
{
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
// 玩家普攻实现（需放在Boss定义后）
inline void Player::normalAttack(Boss& boss) {
	if (boss.isAlive()) {
		boss.takeDamage(attackPower);
		cout << "玩家对Boss造成了" << attackPower << "点伤害！" << endl;
	}
}

Boss::Boss(int x, int y, int hp, int attackPower, int goldDrop, bool defeated)
{
	this->x = x;
	this->y = y;
	this->hp = hp;
	this->attackPower = attackPower;
	this->goldDrop = goldDrop;
	this->defeated = defeated;
}

void Boss::takeDamage(int dmg)
{
	hp -= dmg;
	if (hp <= 0) {
		hp = 0;
		defeated = true;
	}
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

Gold::Gold(int x, int y, int value, bool collected)
{
	this->x = x;
	this->y = y;
	this->value = value;
	this->collected = collected;
}

void Gold::collect()
{
	collected = true;
}

Track::Track(int x, int y, int damage, bool triggered)
{
	this->x = x;
	this->y = y;
	this->damage = damage;
	this->triggered = triggered;
}

void Track::trigger()
{
	triggered = true;
}

Locker::Locker(int x, int y, bool locked)
{
	this->x = x;
	this->y = y;
	this->locked = locked;
}

bool Locker::isOpen() const
{
	return !locked;
}
