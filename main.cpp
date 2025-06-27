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

    // 璇诲彇JSON鏂囦欢
    ifstream ifs("boss_case.json");
    nlohmann::json j;
    ifs >> j;

    // 瑙ｆ瀽Boss琛�閲�
    vector<int> bossHps = j["B"].get<vector<int>>();

    // 瑙ｆ瀽鎶�鑳�
    std::vector<Skill> skills;
    for (const auto& arr : j["PlayerSkills"]) {
        int damage = arr[0];
        int cooldown = arr[1];
        skills.emplace_back(damage, cooldown);
    }
    // 计算最优技能释放顺序
    BossFightStrategy bfs;
    auto result = bfs.minTurnSkillSequence(bossHps, skills);

    // 鎵撳嵃鏈�浼樺洖鍚堟暟鍜岄『搴忓埌鎺у埗鍙帮紙鍙�夛級
    printf("min_turns: %d\n", result.first);
    for (const auto& step : result.second) {
        printf("%s\n", step.c_str());
    }
    // 鑷姩鍙鍖栨挱鏀炬暣涓垬鏂楁祦绋�
    fightBossVisualAuto(bossHps, skills, result.second);

    return 0;
}