#include "BossFightStrategy.h"
#include <queue>
#include <set>
#include <climits>
#include <algorithm>
#include <tuple>

using namespace std;

// 估算剩余回合：当前boss血量/最大技能伤害
static int estimate_rounds(int bossHp, const vector<Skill>& skills) {
    int maxdmg = 0;
    for (const auto& s : skills) maxdmg = max(maxdmg, s.dmg);
    if (maxdmg == 0) return INT_MAX;
    return (bossHp + maxdmg - 1) / maxdmg;
}

struct Node {
    State state;
    int f; // 估价
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
    // 初始状态
    State start{ 0, bossHps[0], 0, vector<int>(skillCount, 0), {} };
    pq.push({ start, estimate_rounds(start.bossHp, skills) });
    while (!pq.empty()) {
        Node node = pq.top(); pq.pop();
        State s = node.state;
        // 终止（所有boss打完）
        if (s.bossIdx == bossCount) {
            if (s.turn < best) {
                best = s.turn;
                bestSeq = s.actions;
            }
            continue;
        }
        // 去重
        if (visited.count(s)) continue;
        visited.insert(s);

        // 剪枝
        int est = s.turn + estimate_rounds(s.bossHp, skills) + (bossCount - s.bossIdx - 1);
        if (est >= best) continue;

        // 尝试所有技能
        for (int i = 0; i < skillCount; ++i) {
            if (s.cooldowns[i] == 0) {
                State ns = s;
                int dmg = skills[i].dmg;
                // “不溢出”原则
                if (dmg > ns.bossHp) dmg = ns.bossHp;
                ns.bossHp -= dmg;
                ns.turn += 1;
                // 冷却推进
                for (int& cd : ns.cooldowns) if (cd > 0) cd--;
                ns.cooldowns[i] = skills[i].maxCd;
                ns.actions.push_back("Boss" + to_string(ns.bossIdx + 1) + "-" + to_string(ns.turn) + "-" + to_string(i));
                // Boss被击败，进入下一个
                if (ns.bossHp == 0) {
                    ns.bossIdx += 1;
                    if (ns.bossIdx < bossCount)
                        ns.bossHp = bossHps[ns.bossIdx];
                    else
                        ns.bossHp = 0; // 结束
                }
                int nf = ns.turn + (ns.bossHp ? estimate_rounds(ns.bossHp, skills) : 0) + (bossCount - ns.bossIdx - 1);
                pq.push({ ns, nf });
            }
        }
    }
    return { best, bestSeq };
}