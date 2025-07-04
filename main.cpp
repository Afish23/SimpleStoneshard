#include "BossFightStrategy.h"
#include "GameObjects.h"
#include "GreedyResourcePicker.h"
#include "MazeGenerator.h"
#include "PuzzleSolver.h"
#include "ResourcePathPlanner.h"
#include "FightBossVisual.h"
#include "Utils.h"
#include "Visualizer.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <filesystem>
#include <cstdint>
#include <functional>
using namespace std;

// json头文件，如果你用nlohmann::json
#include "json.hpp"
using json = nlohmann::json;

// Boss战回调，主控弹窗并分数递减
int totalScore = 0; // 全局分数

struct GameState {
    int totalScore = 0;
    bool bossDefeated = false;
} gameState;



void bossFightCallback(vector<vector<MazeCell>>& maze, pair<int, int>& playerPos) {
    if (maze[playerPos.first][playerPos.second].type == 'B') {
        triggerBossFight(maze, playerPos, totalScore,false);
        // 更新迷宫状态
        maze[playerPos.first][playerPos.second].type = ' ';
    }
}

int main() {
    srand(time(0));
    while (true) {
        int choice = 0;
        cout << "欢迎使用迷宫系统\n";
        cout << "1. 随机生成迷宫\n";
        cout << "2. 读取文件并开始游戏\n";
        cout << "3. 退出程序\n";
        cout << "请选择功能（输入数字1/2/3）：";
        cin >> choice;

        if (choice == 1) {
            int n;
            cout << "输入迷宫尺寸 (n×n, 最小7): ";
            cin >> n;

            if (n < 7) {
                cout << "尺寸不能小于7\n";
                cout << "按回车键返回主界面...";
                cin.ignore();
                cin.get();
                system("cls");
                continue;
            }

            int goldCount = max(1, n / 2);
            int trapCount = max(1, n / 5);
            int lockerCount = max(1, n / 6);
            int bossCount = 1;
            int flag = 0;
            pair<int, int> startPos, exitPos;
            cout << "是否手动指定起点和终点？（0：否 1：是 ）" << endl;
            cin >> flag;
            if (flag == 1) {
                cout << "请输入初始点坐标（x y形式，大于0，小于n - 1）：";
                cin >> startPos.first >> startPos.second;
                cout << "请输入终止点坐标（x y形式，大于0，小于n - 1）：";
                cin >> exitPos.first >> exitPos.second;
            }

            auto maze = MazeGenerator::generateMaze(n, goldCount, trapCount, lockerCount, bossCount, startPos, exitPos);
            cout << "\n生成的迷宫如下：\n";
            MazeGenerator::printMaze(maze);
            MazeGenerator::writeMazeToJson(maze, "maze.json");

            cout << "已保存到 maze.json\n";
            cout << "按回车键返回主界面...";
            cin.ignore();
            cin.get();
            system("cls");
        }
        else if (choice == 2) {
            pair<int, int> startPos, exitPos;
            ifstream fin("maze.json");
            if (!fin) {
                cerr << "无法打开maze.json文件" << endl;
                cout << "按回车键返回主界面...";
                cin.ignore();
                cin.get();
                system("cls");
                continue;
            }
            json j;
            fin >> j;
            fin.close();
            auto maze_arr = j["maze"];
            int nn = maze_arr.size();
            int m = maze_arr[0].size();
            int start_x, start_y, end_x, end_y;

            // 创建初始迷宫对象
            vector<vector<MazeCell>> initialMaze(nn, vector<MazeCell>(m));
            for (int i = 0; i < nn; ++i) {
                for (int j2 = 0; j2 < m; ++j2) {
                    char c = maze_arr[i][j2].get<string>()[0];
                    initialMaze[i][j2].type = c;
                    switch (c) {
                    case 'S':
                        start_x = i;
                        start_y = j2;
                        break;
                    case 'E':
                        end_x = i;
                        end_y = j2;
                        break;
                    default:
                        break;
                    }
                }
            }

            cout << "读取到的迷宫如下：\n";
            for (int i = 0; i < nn; ++i) {
                for (int j2 = 0; j2 < m; ++j2) {
                    cout << initialMaze[i][j2].type << ' ';
                }
                cout << endl;
            }

            startPos.first = start_x;
            startPos.second = start_y;
            exitPos.first = end_x;
            exitPos.second = end_y;

            // 复制一份迷宫用于动态规划
            auto dpMaze = initialMaze;
            auto greedyMaze = initialMaze;  // 创建全新的副本
            auto result = ResourcePathPlanner::findOptimalPath(dpMaze, startPos, exitPos);

            if (result.success) {
                cout << "\n找到最优路径！总价值: " << result.totalValue << endl;

                // 可视化DP最优路径 - 使用初始迷宫
                MazeVisualizer vis1(initialMaze.size());
                vis1.drawMaze(initialMaze);  // 这里确保使用初始迷宫
                vis1.animatePath(result.path, initialMaze, 120, COLOR_PATH);  // 这里也使用初始迷宫

                vis1.showInfo("动态规划最优路径已显示，按任意键继续贪心演示...");
                system("pause");

                // ---- 贪心部分 ----
                // 使用初始迷宫状态
                
                vector<pair<int, int>> fullPath;
                vector<pair<int, int>> bossSteps;
                totalScore = 0; // 全局分数清零

                greedyResourceCollection(greedyMaze, startPos, exitPos, fullPath, bossSteps, totalScore);

                // 创建新的可视化器
                MazeVisualizer vis2(greedyMaze.size());
                vis2.drawMaze(greedyMaze);  // 确保使用贪心算法处理后的迷宫

                // 添加一个标志位表示游戏是否真正结束
                bool gameReallyEnded = false;

                // 修改动画回调
                vector<pair<int, int>> pendingBossFights;
                vector<int> bossTurnsUsed; // 存储每个Boss战的回合数

                vis2.animateGreedy(
                    fullPath, initialMaze, bossSteps, 100, // 使用 initialMaze 而非 greedyMaze
                    [&](const pair<int, int>& pos) {
                        if (greedyMaze[pos.first][pos.second].type == 'B') {
                            // 创建非const副本用于修改
                            pair<int, int> nonConstPos = pos;

                            // 读取Boss战斗数据
                            ifstream ifs("boss_case.json");
                            if (!ifs) {
                                cerr << "无法打开boss_case.json文件" << endl;
                                return;
                            }

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
                            int turns = result.first;

                            // 显示战斗动画
                            fightBossVisualAuto(bossHps, skills, result.second);

                            // 记录战斗结果
                            pendingBossFights.push_back(nonConstPos);
                            bossTurnsUsed.push_back(turns);

                            // 立即更新迷宫状态
                            totalScore -= turns;
                            greedyMaze[pos.first][pos.second].type = ' ';
                            vis2.drawMaze(greedyMaze);
                            FlushBatchDraw();

                            cout << "触发Boss战 at (" << pos.first << "," << pos.second << ")" << endl;
                        }
                    }
                );

                cout << "最终得分: " << totalScore << endl;

                vis2.showInfo("贪心收集过程已演示完，按任意键结束...");
                system("pause");
            }
            else {
                cout << "\n未找到有效路径！" << endl;
            }
            cout << "按回车键返回主界面...";
            cin.ignore();
            cin.get();
            system("cls");
        }
        else if (choice == 3) {
            cout << "程序已退出。\n";
            break;
        }
        else {
            cout << "无效输入，请重新选择。\n";
            cin.clear();
            cin.ignore(10000, '\n');
        }
    }
    return 0;
}