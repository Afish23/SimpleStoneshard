#include "BossFightStrategy.h"
#include "GameObjects.h"
#include "GreedyResourcePicker.h"
#include "MazeGenerator.h"
#include "PuzzleSolver.h"
#include "ResourcePathPlanner.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
using namespace std;

int main() {
    srand(time(0));
    int choice = 0;
    cout << "��ӭʹ���Թ�ϵͳ\n";
    cout << "1. ��������Թ�\n";
    cout << "2. ��ȡ�ļ�����ʼ��Ϸ\n";
    cout << "��ѡ���ܣ���������1��2����";
    cin >> choice;

    if (choice == 1) {
        int n;
        cout << "�����Թ��ߴ� (n��n, ��С7): ";
        cin >> n;

        if (n < 7) {
            cout << "�ߴ粻��С��7\n";
            return 1;
        }

        int goldCount = max(1, n / 4);
        int trapCount = max(1, n / 5);
        int lockerCount = max(1, n / 6);
        int bossCount = 1;

        pair<int, int> startPos, exitPos;
        cout << "�������ʼ�����꣨x y��ʽ������0��С��n - 1����";
        cin >> startPos.first >> startPos.second;
        cout << "��������ֹ�����꣨x y��ʽ������0��С��n - 1����";
        cin >> exitPos.first >> exitPos.second;

        auto maze = MazeGenerator::generateMaze(n, goldCount, trapCount, lockerCount, bossCount, startPos, exitPos);

        cout << "\n���ɵ��Թ����£�\n";
        MazeGenerator::printMaze(maze);

        // д��json�ļ�
        MazeGenerator::writeMazeToJson(maze, "maze.json");
        cout << "�ѱ��浽 maze.json\n";
    }
    else if (choice == 2) {
        // ��ȡjson��ʵ��������
        ifstream fin("maze.json");
        if (!fin) {
            cerr << "�޷���maze.json�ļ�" << endl;
            return 1;
        }
        json j;
        fin >> j;

        auto maze_arr = j["maze"];
        int nn = maze_arr.size();
        int m = maze_arr[0].size();

        vector<vector<shared_ptr<GameObject>>> maze_objs(nn, vector<shared_ptr<GameObject>>(m, nullptr));
        for (int i = 0; i < nn; ++i) {
            for (int j2 = 0; j2 < m; ++j2) {
                char c = maze_arr[i][j2].get<string>()[0];
                maze_objs[i][j2] = MazeGenerator::createObject(c, i, j2);
            }
        }

        // ������ӻ�
        cout << "��ȡ�����Թ����£�\n";
        for (int i = 0; i < nn; ++i) {
            for (int j2 = 0; j2 < m; ++j2) {
                if (maze_objs[i][j2])
                    cout << maze_objs[i][j2]->type << ' ';
                else
                    cout << "? ";
            }
            cout << endl;
        }

        // ����������������Ϸ��ѭ��������������
        // ...
    }
    else {
        cout << "��Ч���룬�����˳���\n";
    }

    return 0;
}