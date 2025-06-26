#include "BossFightStrategy.h"
#include "GameObjects.h"
#include "GreedyResourcePicker.h"
#include "MazeGenerator.h"
#include "PuzzleSolver.h"
#include "ResourcePathPlanner.h"
#include "Utils.h"
using namespace std;

int main() {
    //srand(time(0));

    //int n;
    //cout << "输入迷宫尺寸 (n×n, 最小7): ";
    //cin >> n;

    //if (n < 7) {
    //    cout << "尺寸不能小于7\n";
    //    return 1;
    //}

    //// 配置各种元素数量
    //int goldCount = max(1, n / 4);
    //int trapCount = max(1, n / 5);
    //int lockerCount = max(1, n / 6);
    //int bossCount = 1;

    //// 生成迷宫
    //pair<int, int> startPos, exitPos;
    //auto maze = MazeGenerator::generateMaze(n, goldCount, trapCount, lockerCount, bossCount, startPos, exitPos);

    //// 打印迷宫
    //cout << "\n生成的迷宫如下：\n";
    //MazeGenerator::printMaze(maze);
    Player play;
    Boss boss;
    Skill a("ltbyz", 50, 3,0);
    vector <Skill> b{ {"lanyue",50,2,0},{"huche",70,3,0}};
    play.addSkill(a);
    boss.addBossSkills(b);
    fightBoss(play, boss);
    return 0;
}