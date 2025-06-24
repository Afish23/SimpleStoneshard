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
    cout << "输入迷宫尺寸 (n×n, 最小7): ";
    cin >> n;

    if (n < 7) {
        cout << "尺寸不能小于7\n";
        return 1;
    }

    // 配置各种元素数量
    int goldCount = max(1, n / 4);
    int trapCount = max(1, n / 5);
    int lockerCount = max(1, n / 6);
    int bossCount = 1;

    // 生成迷宫
    pair<int, int> startPos, exitPos;
    cout << "请输入初始点坐标（x>=1,y>=1）形式：";
    cin >> startPos.first >> startPos.second;
    cout << "请输入终止点坐标（x<=" << n - 2 << ",y<=" << n - 2 << "）形式：";
    cin >> exitPos.first >> exitPos.second;

    auto maze = MazeGenerator::generateMaze(n, goldCount, trapCount, lockerCount, bossCount, startPos, exitPos);

    // 打印迷宫
    cout << "\n生成的迷宫如下：\n";
    MazeGenerator::printMaze(maze);

    // 路径规划（修正了变量名，使用startPos和exitPos）
    auto result = ResourcePathPlanner::findOptimalPath(maze, startPos, exitPos);

    // 处理结果
    if (result.success) {
        cout << "\n找到最优路径！总价值: " << result.totalValue << endl;

        // 可视化显示
        auto markedMaze = ResourcePathPlanner::markPath(maze, result.path);
        cout << "\n路径标记图 (* 表示路径):\n";
        MazeGenerator::printMaze(markedMaze);

        // 输出路径坐标
        cout << "\n路径点序列:\n";
        for (auto& p : result.path) {
            cout << "(" << p.first << "," << p.second << ") ";
        }
        cout << endl;
    }
    else {
        cout << "\n未找到有效路径！" << endl;
    }
    cout << 1<<endl;
    return 0;  // 移除了重复的return语句
}