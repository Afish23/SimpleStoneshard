#pragma once
#include <vector>
#include <graphics.h>
#include <thread>
#include <chrono>
#include <string>
#include <cstdio>
#include <cstdarg>

#include "GameObjects.h"

// �Զ�Bossս���ӻ����������ż���˳���Զ����ţ�

void fightBossVisualAuto(const std::vector<int>& bossHps, const std::vector<Skill>& skills, const std::vector<std::string>& actions);

// �ı����
inline void outtextxy_format(int x, int y, const char* fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    outtextxy(x, y, buf);
}

void drawBattleAuto(
    int bossIdx, int turn, int totalBoss, int bossHp, int bossMaxHp,
    const std::vector<Skill>& skills, const std::vector<int>& cooldowns,
    int lastSkill, const wchar_t* msg,
    const std::vector<std::string>& actions,
    int currentActionIdx
);
void drawSkillSequence(int x, int y, const std::vector<std::string>& actions, int currentIdx);