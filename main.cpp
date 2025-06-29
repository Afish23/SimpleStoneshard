#include "BossFightStrategy.h"
#include "GameObjects.h"
#include "GreedyResourcePicker.h"
#include "MazeGenerator.h"
#include "PuzzleSolver.h"
#include "ResourcePathPlanner.h"
#include "FightBossVisual.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
using namespace std;

int main() {

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

    return 0;
}