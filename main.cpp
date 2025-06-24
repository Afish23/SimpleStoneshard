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
    cout << "�������ʼ�����꣨x y��ʽ������0��С��n - 1����";
    cin >> startPos.first >> startPos.second;
    cout << "��������ֹ�����꣨x y��ʽ������0��С��n - 1����";
	cin >> exitPos.first >> exitPos.second;
    auto maze = MazeGenerator::generateMaze(n, goldCount, trapCount, lockerCount, bossCount, startPos, exitPos);

    // ��ӡ�Թ�
    cout << "\n���ɵ��Թ����£�\n";
    MazeGenerator::printMaze(maze);

    // 1. ��ȡJSON�ļ�      ����в�֣���ǰ˼·���������棬�ֳ��������ܣ�1����������Թ���2.��ȡ�ļ�����ʼ��Ϸ
    ifstream fin("maze.json");
    if (!fin) {
        cerr << "�޷���maze.json�ļ�" << endl;
        return 1;
    }
    json j;
    fin >> j;

    // 2. ��ȡ�Թ�����
    auto maze_arr = j["maze"];
    int nn = maze_arr.size();
    int m = maze_arr[0].size();

    // 3. ��ʼ����ά����ָ������
    vector<vector<shared_ptr<GameObject>>> maze_objs(nn, vector<shared_ptr<GameObject>>(m, nullptr));

    // 4. �����ַ�ʵ��������
    for (int i = 0; i < nn; ++i) {
        for (int j2 = 0; j2 < m; ++j2) {
            char c = maze_arr[i][j2].get<string>()[0];
            maze_objs[i][j2] = MazeGenerator::createObject(c, i, j2);
        }
    }

    // 5. �������
   /* for (int i = 0; i < nn; ++i) {
        for (int j2 = 0; j2 < m; ++j2) {
            if (maze_objs[i][j2])
                cout << maze_objs[i][j2]->type << ' ';
            else
                cout << "? ";
        }
        cout << endl;
    }*/

    return 0;
}