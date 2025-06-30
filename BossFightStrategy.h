#pragma once
#include <vector>
#include <string>
#include<tuple>
#include "GameObjects.h"
using namespace std;

// 状态节点
struct State {
    int bossIdx;                 // 当前BOSS编号
    int bossHp;                  // 当前BOSS剩余血量
    int turn;                    // 总用回合数
    vector<int> cooldowns;       // 技能冷却
    vector<string> actions;      // 技能序列

    bool operator<(const State& rhs) const {
        // 用于set/map判重
        return tie(bossIdx, bossHp, cooldowns) < tie(rhs.bossIdx, rhs.bossHp, rhs.cooldowns);
    }
};

class BossFightStrategy {
public:
    // 输入：Boss血量列表、玩家技能
    // 输出：最小回合数下的技能释放序列
    static pair<int, vector<string>> minTurnSkillSequence(
        const vector<int>& bossHps,
        const vector<Skill>& skills
    );
};