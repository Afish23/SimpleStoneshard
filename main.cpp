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
    // 定义Boss血量
    vector<int> bossHps = { 40,60, 80 };
    // 定义技能（伤害, 最大冷却）
    vector<Skill> skills;
    skills.push_back(Skill(15, 2)); // 技能1：伤害15，冷却2
    skills.push_back(Skill(10, 0)); // 技能2：伤害10，冷却0
    skills.push_back(Skill(25, 3)); // 技能3：伤害25，冷却3
    // 生成迷宫
    pair<int, int> startPos, exitPos;
    cout << "请输入初始点坐标（x y形式，大于0，小于n - 1）：";
    cin >> startPos.first >> startPos.second;
    cout << "请输入终止点坐标（x y形式，大于0，小于n - 1）：";
	cin >> exitPos.first >> exitPos.second;
    auto maze = MazeGenerator::generateMaze(n, goldCount, trapCount, lockerCount, bossCount, startPos, exitPos);

    // 计算最优技能释放顺序
    BossFightStrategy bfs;
    auto result = bfs.minTurnSkillSequence(bossHps, skills);

    // 打印最优回合数和顺序到控制台（可选）
    printf("最小回合数: %d\n", result.first);
    for (const auto& step : result.second) {
        printf("%s\n", step.c_str());
    }
    // 自动可视化播放整个战斗流程
    fightBossVisualAuto(bossHps, skills, result.second);
    // 1. 读取JSON文件      需进行拆分，当前思路，做主界面，分成两个功能，1，随机生成迷宫；2.读取文件并开始游戏
    ifstream fin("maze.json");
    if (!fin) {
        cerr << "无法打开maze.json文件" << endl;
        return 1;
    }
    json j;
    fin >> j;

    // 2. 获取迷宫数组
    auto maze_arr = j["maze"];
    int nn = maze_arr.size();
    int m = maze_arr[0].size();

    // 3. 初始化二维智能指针数组
    vector<vector<shared_ptr<GameObject>>> maze_objs(nn, vector<shared_ptr<GameObject>>(m, nullptr));

    // 4. 根据字符实例化对象
    for (int i = 0; i < nn; ++i) {
        for (int j2 = 0; j2 < m; ++j2) {
            char c = maze_arr[i][j2].get<string>()[0];
            maze_objs[i][j2] = MazeGenerator::createObject(c, i, j2);
        }
    }

    // 5. 输出测试
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