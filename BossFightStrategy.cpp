#include "BossFightStrategy.h"
#include <queue>
#include <set>
#include <climits>
#include <algorithm>
#include <tuple>

using namespace std;

// ����ʣ��غϣ���ǰbossѪ��/������˺�
static int estimate_rounds(int bossHp, const vector<Skill>& skills) {
    int maxdmg = 0;
    for (const auto& s : skills) maxdmg = max(maxdmg, s.dmg);
    if (maxdmg == 0) return INT_MAX;
    return (bossHp + maxdmg - 1) / maxdmg;
}

struct Node {
    State state;
    int f; // ����
    bool operator<(const Node& rhs) const { return f > rhs.f; }
};

pair<int, vector<string>> BossFightStrategy::minTurnSkillSequence(
    const vector<int>& bossHps,
    const vector<Skill>& skills
) {
    int bossCount = bossHps.size(), skillCount = skills.size();
    priority_queue<Node> pq;
    set<State> visited;
    int best = INT_MAX;
    vector<string> bestSeq;
    // ��ʼ״̬
    State start{ 0, bossHps[0], 0, vector<int>(skillCount, 0), {} };
    pq.push({ start, estimate_rounds(start.bossHp, skills) });
    while (!pq.empty()) {
        Node node = pq.top(); pq.pop();
        State s = node.state;
        // ��ֹ������boss���꣩
        if (s.bossIdx == bossCount) {
            if (s.turn < best) {
                best = s.turn;
                bestSeq = s.actions;
            }
            continue;
        }
        // ȥ��
        if (visited.count(s)) continue;
        visited.insert(s);

        // ��֦
        int est = s.turn + estimate_rounds(s.bossHp, skills) + (bossCount - s.bossIdx - 1);
        if (est >= best) continue;

        // �������м���
        for (int i = 0; i < skillCount; ++i) {
            if (s.cooldowns[i] == 0) {
                State ns = s;
                int dmg = skills[i].dmg;
                // ���������ԭ��
                if (dmg > ns.bossHp) dmg = ns.bossHp;
                ns.bossHp -= dmg;
                ns.turn += 1;
                // ��ȴ�ƽ�
                for (int& cd : ns.cooldowns) if (cd > 0) cd--;
                ns.cooldowns[i] = skills[i].maxCd;
                ns.actions.push_back("Boss" + to_string(ns.bossIdx + 1) + "-" + to_string(ns.turn) + "-" + to_string(i));
                // Boss�����ܣ�������һ��
                if (ns.bossHp == 0) {
                    ns.bossIdx += 1;
                    if (ns.bossIdx < bossCount)
                        ns.bossHp = bossHps[ns.bossIdx];
                    else
                        ns.bossHp = 0; // ����
                }
                int nf = ns.turn + (ns.bossHp ? estimate_rounds(ns.bossHp, skills) : 0) + (bossCount - ns.bossIdx - 1);
                pq.push({ ns, nf });
            }
        }
    }
    return { best, bestSeq };
}