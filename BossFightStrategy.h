#pragma once
#include <vector>
#include <string>
#include <tuple>
#include "GameObjects.h"

using namespace std;
//分支限界算法的“节点”或“状态”结构体。
struct State {
    int boss_hp;
    int player_res;
    int round;
    vector<int> cooldowns;
    vector<string> action_seq;

    bool operator<(const State& rhs) const {
        return tie(boss_hp, player_res, cooldowns) < tie(rhs.boss_hp, rhs.player_res, rhs.cooldowns);
    }
};
//封装“BOSS战最优解搜索”功能
class BossFightStrategy {
public:
    static vector<string> findMinTurnSkillSequence(
        int player_res,
        int boss_hp,
        const vector<Skill>& skills,
        int limit_round = 100
    );
};

//战斗主循环
void fightBoss(Player& player, Boss& boss);