#pragma once
#include <vector>
#include <string>
#include<tuple>
#include "GameObjects.h"
using namespace std;

// ״̬�ڵ�
struct State {
    int bossIdx;                 // ��ǰBOSS���
    int bossHp;                  // ��ǰBOSSʣ��Ѫ��
    int turn;                    // ���ûغ���
    vector<int> cooldowns;       // ������ȴ
    vector<string> actions;      // ��������

    bool operator<(const State& rhs) const {
        // ����set/map����
        return tie(bossIdx, bossHp, cooldowns) < tie(rhs.bossIdx, rhs.bossHp, rhs.cooldowns);
    }
};

class BossFightStrategy {
public:
    // ���룺BossѪ���б���Ҽ���
    // �������С�غ����µļ����ͷ�����
    static pair<int, vector<string>> minTurnSkillSequence(
        const vector<int>& bossHps,
        const vector<Skill>& skills
    );
};