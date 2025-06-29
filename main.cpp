#include "BossFightStrategy.h"
#include "GameObjects.h"
#include "GreedyResourcePicker.h"
#include "MazeGenerator.h"
#include "PuzzleSolver.h"
#include "ResourcePathPlanner.h"
#include"FightBossVisual.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
using namespace std;

// 假设正确密码
vector<int> target = { 7, 5, 2 };
// 根据正确密码给出线索
vector<vector<int>> clues = {
    {-1, -1},        // 每位素数且不重复
    {3, 0},          // 第三位偶数
    {-1, 5, -1}      // 第二位是5
};

int main() {
    srand(time(0));
    int choice = 0;
    cout << "欢迎使用迷宫系统\n";
    cout << "1. 随机生成迷宫\n";
    cout << "2. 读取文件并开始游戏\n";
    cout << "请选择功能（输入数字1或2）：";
    cin >> choice;

    if (choice == 1) {
        int n;
        cout << "输入迷宫尺寸 (n×n, 最小7): ";
        cin >> n;
    
        if (n < 7) {
            cout << "尺寸不能小于7\n";
            return 1;
        }

        int goldCount = max(1, n / 4);
        int trapCount = max(1, n / 5);
        int lockerCount = max(1, n / 6);
        int bossCount = 1;

        pair<int, int> startPos, exitPos;
        cout << "请输入初始点坐标（x y形式，大于0，小于n - 1）：";
        cin >> startPos.first >> startPos.second;
        cout << "请输入终止点坐标（x y形式，大于0，小于n - 1）：";
        cin >> exitPos.first >> exitPos.second;

        auto maze = MazeGenerator::generateMaze(n, goldCount, trapCount, lockerCount, bossCount, startPos, exitPos);

        cout << "\n生成的迷宫如下：\n";
        MazeGenerator::printMaze(maze);

        // 写入json文件
        MazeGenerator::writeMazeToJson(maze, "maze.json");
        cout << "已保存到 maze.json\n";
    //srand(time(0));
    }
    else if (choice == 2) {
        pair<int, int> startPos, exitPos;//起止点坐标对
        // 读取json并实例化对象
        ifstream fin("maze.json");
        if (!fin) {
            cerr << "无法打开maze.json文件" << endl;
            return 1;
        }
        json j;
        fin >> j;

        auto maze_arr = j["maze"];
        int nn = maze_arr.size();
        int m = maze_arr[0].size();
        int start_x, start_y, end_x, end_y;

        vector<vector<MazeCell>> maze_objs(nn, vector<MazeCell>(m));
        // 填充迷宫数据
        for (int i = 0; i < nn; ++i) {
            for (int j2 = 0; j2 < m; ++j2) {
                char c = maze_arr[i][j2].get<string>()[0];
                maze_objs[i][j2].type = c;  // 直接设置单元格类型
                switch (c) {
                case 'S':  // 起点
                    start_x = i;
                    start_y = j2;
                    break;

                case 'E':  // 终点
                    end_x = i;
                    end_y = j2;
                    break;

                default:   // 其他情况无需操作
                    break;
                }
            }
        }

        // 输出可视化
        cout << "读取到的迷宫如下：\n";
        for (int i = 0; i < nn; ++i) {
            for (int j2 = 0; j2 < m; ++j2) {
                cout << maze_objs[i][j2].type << ' ';  // 直接访问单元格类型
            }
            cout << endl;
        }
        /*cout << start_x << ',' << start_y << endl;
        cout << end_x << ',' << end_y << endl;
        cout << goldCount << endl;
        cout << trapCount << endl;
        cout << lockerCount << endl;*/

        startPos.first = start_x;
        startPos.second = start_y;
        exitPos.first = end_x;
        exitPos.second = end_y;

        auto result = ResourcePathPlanner::findOptimalPath(maze_objs, startPos, exitPos);

        if (result.success) {
            cout << "\n找到最优路径！总价值: " << result.totalValue << endl;

            // 可视化显示
            auto markedMaze = ResourcePathPlanner::markPath(maze_objs, result.path);
            cout << "\n路径标记图 (* 表示路径):\n";
            MazeGenerator::printMaze(markedMaze);

            // 输出路径坐标
            cout << "\n路径点序列:\n";
            for (auto& p : result.path) {
                cout << "(" << p.first << "," << p.second << ") ";
            }
            cout << endl;
          //读取文件成功，成功生成最优路径后开始游戏
          // 读取JSON文件
    ifstream ifs("boss_case.json");
    nlohmann::json j;
    ifs >> j;

    // 解析Boss血量
    vector<int> bossHps = j["B"].get<vector<int>>();

    // 解析技能
    std::vector<Skill> skills;
    for (const auto& arr : j["PlayerSkills"]) {
        int damage = arr[0];
        int cooldown = arr[1];
        skills.emplace_back(damage, cooldown);

    }

    // 计算最优技能释放顺序
    BossFightStrategy bfs;
    auto result = bfs.minTurnSkillSequence(bossHps, skills);

    // 打印最优回合数和顺序到控制台（可选）
    //printf("min_turns: %d\n", result.first);
    //for (const auto& step : result.second) {
        //printf("%s\n", step.c_str());
    //}
    // 自动可视化播放整个战斗流程
    fightBossVisualAuto(bossHps, skills, result.second);
          

        }
        else {
            cout << "\n未找到有效路径！" << endl;
        }
    }
    else {
                cout << "无效输入，程序退出。\n";
    }
        return 0; 
    }