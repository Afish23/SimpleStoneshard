#include "BossFightStrategy.h"
#include <queue>
#include <map>
#include <climits>
#include <algorithm>
#include <tuple>

using namespace std;
// ����ʣ��غ�����BOSSѪ��/������˺�������ȡ����
static int estimate_rounds(int boss_hp, const vector<Skill>& skills) {
    int max_dmg = 0;
    for (const auto& s : skills) max_dmg = max(max_dmg, s.dmg);
    if (max_dmg == 0) return INT_MAX;
    return (boss_hp + max_dmg - 1) / max_dmg;
}

// priority_queue ���� f ��С��������С�����ȣ�
struct Node {
    State state;
    int f; // ���ۺ���
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
    cout << "===== BOSS ս��ʼ��=====" << endl;
    while (player.hp > 0 && boss.isAlive()) {
        cout << "\n--- �غ� " << turn << " ---" << endl;
        cout << "��� HP: " << player.hp << "  Gold: " << player.gold << endl;
        cout << "BOSS HP: " << boss.hp << endl;
        for (int i = 0; i < (int)player.skills.size(); i++)
        {
            if (player.skills[i].curCd != 0) { player.skills[i].curCd--; }
        }
        // ��ʾ��Ҽ����б�
        cout << "�����б�" << endl;
        for (size_t i = 0; i < player.skills.size(); ++i) {
            cout << i + 1 << ". " << player.skills[i].name
                << " (�˺�: " << player.skills[i].dmg
                << ", ��ǰ��ȴ: " << player.skills[i].curCd << ", �����ȴ:"<< player.skills[i].maxCd<<")" << endl;
        }
        cout << "0. ��ͨ����" << endl;

        // ���ѡ����
        int choice = -1;
        do {
            cout << "ѡ���ܱ�Ž��й���(0Ϊ��ͨ����): ";
            cin >> choice;
            if(choice!=0&&player.skills[choice-1].curCd !=0)
            {
                cout << "����������ȴ���޷�ʹ�ã�" << endl;
                choice = -1;
            }
            
        } while (choice < 0 || choice >(int)player.skills.size());

        if (choice == 0) {
            player.normalAttack(boss);
        }
        else {
            boss.takeDamage(player.skills[choice - 1].dmg);
            player.skills[choice-1].curCd = player.skills[choice-1].maxCd+1;
            cout << "���ʹ��[" << player.skills[choice - 1].name << "]��Boss���"
                << player.skills[choice - 1].dmg << "���˺���" << endl;
        }

        // ���BOSS�Ƿ񱻻���
        if (!boss.isAlive()) {
            cout << "\n��ϲ���������BOSS����ý�ң�" << boss.goldDrop << endl;
            player.addGold(boss.goldDrop);
            break;
        }

        // Boss�غ�
        boss.normalAttack(player);

        // �������Ƿ�����
        if (player.hp <= 0) {
            cout << "\n�㱻BOSS�����ˣ���Ϸ������" << endl;
            break;
        }
        ++turn;
    }
    cout << "===== ս������ =====" << endl;
}