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

// ========== 主函数（程序入口） ==========
int main() {
    srand(time(0));  // 初始化随机种子
    int n;
    cout << "输入迷宫尺寸 (n×n, 最小7): ";
    cin >> n;
    if (n < 7) {
        cout << "尺寸不能小于7\n";
        return 1;
    }

    vector<vector<MazeCell>> maze;
    pair<int, int> start, exit;
    while (true) {
        // 初始化为全通路
        maze.assign(n, vector<MazeCell>(n, MazeCell()));
        // 外围设为墙
        for (int i = 0; i < n; i++)
            maze[0][i].type = maze[n - 1][i].type = maze[i][0].type = maze[i][n - 1].type = WALL;
        // 分治生成迷宫
        divide(maze, 1, 1, n - 2, n - 2);
        // 随机选起点、终点
        vector<pair<int, int>> paths = get_all_paths(maze);
        if (paths.size() < 2) continue;  // 通路不足则重试
        random_shuffle(paths.begin(), paths.end());
        start = paths[0];
        exit = paths[1];
        maze[start.first][start.second].type = START;
        maze[exit.first][exit.second].type = EXIT;
        // 检查连通性，连通则退出循环
        if (is_connected(maze, start, exit)) break;
    }

    // 随机放置元素（金币、陷阱、储物柜、Boss）
    set<pair<int, int>> forbidden = { start, exit };  // 起点、终点禁止放置
    int gold_count = max(1, n / 4);
    int trap_count = max(1, n / 5);
    int locker_count = max(1, n / 6);
    int boss_count = 1;
    place_random_elements(maze, GOLD, gold_count, forbidden);
    place_random_elements(maze, TRAP, trap_count, forbidden);
    place_random_elements(maze, LOCKER, locker_count, forbidden);
    place_random_elements(maze, BOSS, boss_count, forbidden);

    // 打印最终迷宫
    cout << "\n生成的迷宫如下：\n";
    print_maze(maze);
	return 0;
}