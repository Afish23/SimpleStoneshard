#include "BossFightStrategy.h"
#include "GameObjects.h"
#include "GreedyResourcePicker.h"
#include "MazeGenerator.h"
#include "PuzzleSolver.h"
#include "ResourcePathPlanner.h"
#include "Utils.h"
#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <set>
using namespace std;

// ========== ��������������ڣ� ==========
int main() {
    srand(time(0));  // ��ʼ���������
    int n;
    cout << "�����Թ��ߴ� (n��n, ��С7): ";
    cin >> n;
    if (n < 7) {
        cout << "�ߴ粻��С��7\n";
        return 1;
    }

    vector<vector<MazeCell>> maze;
    pair<int, int> start, exit;
    while (true) {
        // ��ʼ��Ϊȫͨ·
        maze.assign(n, vector<MazeCell>(n, MazeCell()));
        // ��Χ��Ϊǽ
        for (int i = 0; i < n; i++)
            maze[0][i].type = maze[n - 1][i].type = maze[i][0].type = maze[i][n - 1].type = WALL;
        // ���������Թ�
        divide(maze, 1, 1, n - 2, n - 2);
        // ���ѡ��㡢�յ�
        vector<pair<int, int>> paths = get_all_paths(maze);
        if (paths.size() < 2) continue;  // ͨ·����������
        random_shuffle(paths.begin(), paths.end());
        start = paths[0];
        exit = paths[1];
        maze[start.first][start.second].type = START;
        maze[exit.first][exit.second].type = EXIT;
        // �����ͨ�ԣ���ͨ���˳�ѭ��
        if (is_connected(maze, start, exit)) break;
    }

    // �������Ԫ�أ���ҡ����塢�����Boss��
    set<pair<int, int>> forbidden = { start, exit };  // ��㡢�յ��ֹ����
    int gold_count = max(1, n / 4);
    int trap_count = max(1, n / 5);
    int locker_count = max(1, n / 6);
    int boss_count = 1;
    place_random_elements(maze, GOLD, gold_count, forbidden);
    place_random_elements(maze, TRAP, trap_count, forbidden);
    place_random_elements(maze, LOCKER, locker_count, forbidden);
    place_random_elements(maze, BOSS, boss_count, forbidden);

    // ��ӡ�����Թ�
    cout << "\n���ɵ��Թ����£�\n";
    print_maze(maze);
	return 0;
}