#pragma once
#include <vector>
#include <string>
#include <tuple>
#include "GameObjects.h"

using namespace std;
//��֧�޽��㷨�ġ��ڵ㡱��״̬���ṹ�塣
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
//��װ��BOSSս���Ž�����������
class BossFightStrategy {
public:
    static vector<string> findMinTurnSkillSequence(
        int player_res,
        int boss_hp,
        const vector<Skill>& skills,
        int limit_round = 100
    );
};

//ս����ѭ��
void fightBoss(Player& player, Boss& boss);