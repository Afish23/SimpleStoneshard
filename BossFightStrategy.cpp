#include "BossFightStrategy.h"
#include <queue>
#include <map>
#include <climits>
#include <algorithm>
#include <tuple>

using namespace std;
// 估算剩余回合数：BOSS血量/最大技能伤害（向上取整）
static int estimate_rounds(int boss_hp, const vector<Skill>& skills) {
    int max_dmg = 0;
    for (const auto& s : skills) max_dmg = max(max_dmg, s.dmg);
    if (max_dmg == 0) return INT_MAX;
    return (boss_hp + max_dmg - 1) / max_dmg;
}

// priority_queue 按照 f 从小到大排序（小的优先）
struct Node {
    State state;
    int f; // 估价函数
    bool operator<(const Node& rhs) const { return f > rhs.f; }
};

vector<string> BossFightStrategy::findMinTurnSkillSequence(
    int player_res,
    int boss_hp,
    const vector<Skill>& skills,
    int limit_round
) {
    int n = skills.size();
    priority_queue<Node> pq;
    map<State, int> visited;
    vector<string> best_seq;
    int best = INT_MAX;

    State start{ boss_hp, player_res, 0, vector<int>(n, 0), {} };
    pq.push({ start, estimate_rounds(boss_hp, skills) });

    while (!pq.empty()) {
        Node node = pq.top(); pq.pop();
        State& s = node.state;

        if (s.boss_hp <= 0) {
            if (s.round < best) {
                best = s.round;
                best_seq = s.action_seq;
            }
            continue;
        }
        if (s.round >= limit_round)
            continue;
        if (visited.count(s) && visited[s] <= s.round)
            continue;
        visited[s] = s.round;

        int est = s.round + estimate_rounds(s.boss_hp, skills);
        if (est >= best) continue;

        for (int i = 0; i < n; ++i) {
            if (s.cooldowns[i] == 0) {
                State ns = s;
                ns.boss_hp -= skills[i].dmg;
                ns.round += 1;
                for (int& cd : ns.cooldowns)
                    if (cd > 0) cd--;
                ns.cooldowns[i] = skills[i].maxCd;
                ns.action_seq.push_back(skills[i].name);
                int nf = ns.round + estimate_rounds(max(0, ns.boss_hp), skills);
                pq.push({ ns, nf });
            }
        }
    }
    return best_seq;
}

void fightBoss(Player& player, Boss& boss) {
    int turn = 1;
    cout << "===== BOSS 战开始！=====" << endl;
    while (player.hp > 0 && boss.isAlive()) {
        cout << "\n--- 回合 " << turn << " ---" << endl;
        cout << "玩家 HP: " << player.hp << "  Gold: " << player.gold << endl;
        cout << "BOSS HP: " << boss.hp << endl;
        for (int i = 0; i < (int)player.skills.size(); i++)
        {
            if (player.skills[i].curCd != 0) { player.skills[i].curCd--; }
        }
        // 显示玩家技能列表
        cout << "技能列表：" << endl;
        for (size_t i = 0; i < player.skills.size(); ++i) {
            cout << i + 1 << ". " << player.skills[i].name
                << " (伤害: " << player.skills[i].dmg
                << ", 当前冷却: " << player.skills[i].curCd << ", 最大冷却:"<< player.skills[i].maxCd<<")" << endl;
        }
        cout << "0. 普通攻击" << endl;

        // 玩家选择技能
        int choice = -1;
        do {
            cout << "选择技能编号进行攻击(0为普通攻击): ";
            cin >> choice;
            if(choice!=0&&player.skills[choice-1].curCd !=0)
            {
                cout << "技能正在冷却，无法使用！" << endl;
                choice = -1;
            }
            
        } while (choice < 0 || choice >(int)player.skills.size());

        if (choice == 0) {
            player.normalAttack(boss);
        }
        else {
            boss.takeDamage(player.skills[choice - 1].dmg);
            player.skills[choice-1].curCd = player.skills[choice-1].maxCd+1;
            cout << "玩家使用[" << player.skills[choice - 1].name << "]对Boss造成"
                << player.skills[choice - 1].dmg << "点伤害！" << endl;
        }

        // 检查BOSS是否被击败
        if (!boss.isAlive()) {
            cout << "\n恭喜！你击败了BOSS！获得金币：" << boss.goldDrop << endl;
            player.addGold(boss.goldDrop);
            break;
        }

        // Boss回合
        boss.normalAttack(player);

        // 检查玩家是否死亡
        if (player.hp <= 0) {
            cout << "\n你被BOSS击败了！游戏结束。" << endl;
            break;
        }
        ++turn;
    }
    cout << "===== 战斗结束 =====" << endl;
}