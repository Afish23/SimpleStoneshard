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
    vector<int> bossHps = { 19,17,14,19 };

    // ���弼�ܣ��˺�, �����ȴ��
    vector<Skill> skills;
    skills.push_back(Skill(6, 2)); 
    skills.push_back(Skill(9, 5)); 
    skills.push_back(Skill(5, 3));
    skills.push_back(Skill(4, 3));
    skills.push_back(Skill(2, 0));
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