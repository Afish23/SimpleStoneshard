#include "BossFightStrategy.h"
#include "GameObjects.h"
#include "GreedyResourcePicker.h"
#include "MazeGenerator.h"
#include "PuzzleSolver.h"
#include "ResourcePathPlanner.h"
#include "Utils.h"
using namespace std;

int main() {
    srand(time(0));

    int n;
    cout << "�����Թ��ߴ� (n��n, ��С7): ";
    cin >> n;

    if (n < 7) {
        cout << "�ߴ粻��С��7\n";
        return 1;
    }

    // ���ø���Ԫ������
    int goldCount = max(1, n / 4);
    int trapCount = max(1, n / 5);
    int lockerCount = max(1, n / 6);
    int bossCount = 1;

    // �����Թ�
    pair<int, int> startPos, exitPos;
    cout << "�������ʼ�����꣨x y��ʽ����";
    cin >> startPos.first >> startPos.second;
    cout << "��������ֹ�����꣨x y��ʽ����";
	cin >> exitPos.first >> exitPos.second;
    auto maze = MazeGenerator::generateMaze(n, goldCount, trapCount, lockerCount, bossCount, startPos, exitPos);

    // ��ӡ�Թ�
    cout << "\n���ɵ��Թ����£�\n";
    MazeGenerator::printMaze(maze);

    return 0;
}