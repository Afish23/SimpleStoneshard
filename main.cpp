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

    // 定义Boss血量
    vector<int> bossHps = { 40,60, 80 };
    // 定义技能（伤害, 最大冷却）
    vector<Skill> skills;
    skills.push_back(Skill(15, 2)); // 技能1：伤害15，冷却2
    skills.push_back(Skill(10, 0)); // 技能2：伤害10，冷却0
    skills.push_back(Skill(25, 3)); // 技能3：伤害25，冷却3

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

    return 0;
}