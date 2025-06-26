#include "BossFightStrategy.h"
#include "GameObjects.h"
#include "GreedyResourcePicker.h"
#include "MazeGenerator.h"
#include "PuzzleSolver.h"
#include "ResourcePathPlanner.h"
#include "FightBossVisual.h"
#include "Utils.h"
using namespace std;

int main() {
    //srand(time(0));

    //int n;
    //cout << "�����Թ��ߴ� (n��n, ��С7): ";
    //cin >> n;

    //if (n < 7) {
    //    cout << "�ߴ粻��С��7\n";
    //    return 1;
    //}

    //// ���ø���Ԫ������
    //int goldCount = max(1, n / 4);
    //int trapCount = max(1, n / 5);
    //int lockerCount = max(1, n / 6);
    //int bossCount = 1;

    //// �����Թ�
    //pair<int, int> startPos, exitPos;
    //auto maze = MazeGenerator::generateMaze(n, goldCount, trapCount, lockerCount, bossCount, startPos, exitPos);

    //// ��ӡ�Թ�
    //cout << "\n���ɵ��Թ����£�\n";
    //MazeGenerator::printMaze(maze);
    // ����BossѪ��
    vector<int> bossHps = { 40,60, 80 };

    // ���弼�ܣ��˺�, �����ȴ��
    vector<Skill> skills;
    skills.push_back(Skill(15, 2)); // ����1���˺�15����ȴ2
    skills.push_back(Skill(10, 0)); // ����2���˺�10����ȴ0
    skills.push_back(Skill(25, 3)); // ����3���˺�25����ȴ3

    // �������ż����ͷ�˳��
    BossFightStrategy bfs;
    auto result = bfs.minTurnSkillSequence(bossHps, skills);

    // ��ӡ���Żغ�����˳�򵽿���̨����ѡ��
    printf("��С�غ���: %d\n", result.first);
    for (const auto& step : result.second) {
        printf("%s\n", step.c_str());
    }

    // �Զ����ӻ���������ս������
    fightBossVisualAuto(bossHps, skills, result.second);
}