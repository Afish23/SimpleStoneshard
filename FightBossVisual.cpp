#include "FightBossVisual.h"
#include <graphics.h>
#include <thread>
#include <chrono>
#include <string>
#include <cstdio>
#include <cstdarg>

// ����ÿ���ĵȴ�ʱ�䣬��λ����
const int step_sleep_ms = 1800;

// �ı����
void outtextxy_format(int x, int y, const wchar_t* fmt, ...) {
    wchar_t buf[256];
    va_list args;
    va_start(args, fmt);
    vswprintf(buf, sizeof(buf) / sizeof(wchar_t), fmt, args);
    va_end(args);
    outtextxy(x, y, buf);
}


void drawSkillSequence(
    int x, int y,
    const std::vector<std::string>& actions,
    int currentIdx
) {
    settextcolor(BLACK);
    settextstyle(18, 0, L"Consolas");
    outtextxy(x, 10, L"������־��"); // �����y=10
    int startIdx = 0;
    int endIdx = currentIdx;
    for (int i = startIdx; i <= endIdx && i < actions.size(); ++i) {
        if (i < endIdx)
            settextcolor(RGB(120, 120, 120));  // ���ͷ�
        else if (i == endIdx)
            settextcolor(RGB(255, 69, 0));     // ��ǰ
        wchar_t stepbuf[64];
        swprintf(stepbuf, 64, L"%2d. %hs", i + 1, actions[i].c_str());
        outtextxy(x, 38 + (i - startIdx) * 22, stepbuf); // ��38��ʼ����������
    }
}

// ���ӻ�����ս�����Զ�ģʽ�����Ӽ���������ʾ��
// ���� actions, currentActionIdx ����
void drawBattleAuto(
    int bossIdx, int turn, int totalBoss, int bossHp, int bossMaxHp,
    const std::vector<Skill>& skills, const std::vector<int>& cooldowns,
    int lastSkill, const wchar_t* msg,
    const std::vector<std::string>& actions,
    int currentActionIdx
) {
    cleardevice();
    setbkcolor(RGB(240, 240, 240));
    setfillcolor(RGB(30, 144, 255));
    fillrectangle(50, 350, 200, 500);
    settextcolor(BLACK);
    settextstyle(20, 0, L"Consolas");
    outtextxy_format(50, 330, L"Player (�Զ�)");

    setfillcolor(RGB(178, 34, 34));
    fillrectangle(500, 100, 650, 250);
    outtextxy_format(500, 80, L"Boss %d/%d", bossIdx + 1, totalBoss);

    // bossѪ��
    setfillcolor(GREEN);
    int showHp = bossHp > 0 ? bossHp : 0;
    fillrectangle(500, 60, 500 + showHp * 2, 80);
    outtextxy_format(500, 40, L"HP:%d/%d", showHp, bossMaxHp);

    // ���ܰ�ť/��ȴ
    for (int i = 0; i < skills.size(); ++i) {
        setfillcolor(lastSkill == i ? RGB(255, 215, 0) : LIGHTGRAY); // ��һ�����ܸ���
        fillrectangle(50 + i * 150, 520, 200 + i * 150, 570);
        outtextxy_format(60 + i * 150, 530, L"Skill%d", i + 1);
        outtextxy_format(60 + i * 150, 550, L"CD:%d", cooldowns[i]);
    }
    // ��Ϣ
    outtextxy_format(500, 400, L"%s", msg);

    // ------ �������� ------
    drawSkillSequence(220, 330, actions, currentActionIdx);
}

void fightBossVisualAuto(
    const vector<int>& bossHps,
    const vector<Skill>& skills,
    const vector<string>& actions
) {
    initgraph(800, 600);
    int totalBoss = bossHps.size();
    int bossIdx = 0;
    int bossHp = bossHps[0];
    int bossMaxHp = bossHps[0];
    int turn = 0;
    vector<int> cooldowns(skills.size(), 0);
    int lastSkill = -1;

    for (int ai = 0; ai < actions.size(); ++ai) {
        const auto& act = actions[ai];
        int actBoss, actTurn, actSkill;
        sscanf_s(act.c_str(), "Boss%d-%d-%d", &actBoss, &actTurn, &actSkill);
        if (actBoss - 1 != bossIdx) {
            bossIdx = actBoss - 1;
            bossHp = bossHps[bossIdx];
            bossMaxHp = bossHp;
            // turn ��������
        }
        int dmg = skills[actSkill - 1].dmg;
        if (dmg > bossHp) dmg = bossHp;
        bossHp -= dmg;
        lastSkill = actSkill - 1;
        for (int& cd : cooldowns) if (cd > 0) cd--;
        cooldowns[actSkill - 1] = skills[actSkill - 1].maxCd;
        ++turn;
        wchar_t buf[128];
        swprintf(buf, 128, L"�غ�%d��Skill%d ���%d�˺�", turn, actSkill, dmg);
        drawBattleAuto(bossIdx, turn, totalBoss, bossHp, bossMaxHp, skills, cooldowns, lastSkill, buf, actions, ai);
        std::this_thread::sleep_for(std::chrono::milliseconds(step_sleep_ms));
    }
    // �ܻغ�����ʾ
    wchar_t finalMsg[128];
    swprintf(finalMsg, 128, L"��ϲͨ������%d���غϻ�����boss��", turn);
    drawBattleAuto(bossIdx, turn, totalBoss, bossHp, bossMaxHp, skills, cooldowns, -1, finalMsg, actions, actions.size());
    system("pause");
    closegraph();
}