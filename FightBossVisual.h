#pragma once
#include <vector>
#include "GameObjects.h"

// 自动Boss战可视化（根据最优技能顺序自动播放）

void fightBossVisualAuto(const std::vector<int>& bossHps, const std::vector<Skill>& skills, const std::vector<std::string>& actions);

void outtextxy_format(int x, int y, const wchar_t* fmt, ...);

void drawBattleAuto(
    int bossIdx, int turn, int totalBoss, int bossHp, int bossMaxHp,
    const std::vector<Skill>& skills, const std::vector<int>& cooldowns,
    int lastSkill, const wchar_t* msg,
    const std::vector<std::string>& actions,
    int currentActionIdx
);
void drawSkillSequence(int x, int y,const std::vector<std::string>& actions,int currentIdx);