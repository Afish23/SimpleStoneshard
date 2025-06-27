#include "FightBossVisual.h"

// 播放每步的等待时间，单位毫秒
const int step_sleep_ms = 1800;

// 文本输出
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

    outtextxy(x, 10, L"战斗日志："); // 最顶部，y=10

    int startIdx = 0;
    int endIdx = currentIdx;
    for (int i = startIdx; i <= endIdx && i < actions.size(); ++i) {
        if (i < endIdx)
            settextcolor(RGB(120, 120, 120));  // 已释放
        else if (i == endIdx)
            settextcolor(RGB(255, 69, 0));     // 当前
        wchar_t stepbuf[64];
        swprintf(stepbuf, 64, L"%2d. %hs", i + 1, actions[i].c_str());
        outtextxy(x, 38 + (i - startIdx) * 22, stepbuf); // 从38开始，逐行向下
    }
}

// 可视化绘制战斗（自动模式，增加技能序列显示）
// 新增 actions, currentActionIdx 参数
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
    outtextxy_format(50, 330, L"Player (自动)");

    setfillcolor(RGB(178, 34, 34));
    fillrectangle(500, 100, 650, 250);
    outtextxy_format(500, 80, L"Boss %d/%d", bossIdx + 1, totalBoss);

    // boss血条
    setfillcolor(GREEN);
    int showHp = bossHp > 0 ? bossHp : 0;
    fillrectangle(500, 60, 500 + showHp * 2, 80);
    outtextxy_format(500, 40, L"HP:%d/%d", showHp, bossMaxHp);

    // 技能按钮/冷却
    for (int i = 0; i < skills.size(); ++i) {
        setfillcolor(lastSkill == i ? RGB(255, 215, 0) : LIGHTGRAY); // 上一步技能高亮
        fillrectangle(50 + i * 150, 520, 200 + i * 150, 570);
        outtextxy_format(60 + i * 150, 530, L"Skill%d", i + 1);
        outtextxy_format(60 + i * 150, 550, L"CD:%d", cooldowns[i]);
    }
    // 消息
    outtextxy_format(500, 400, L"%s", msg);

    // ------ 技能序列 ------
    drawSkillSequence(220, 330, actions, currentActionIdx);
}

void fightBossVisualAuto(
    const vector<int>& bossHps,
    const vector<Skill>& skills,
    const vector<string>& actions
) {
    initgraph(800, 600);
    setbkcolor(RGB(240, 240, 240));
    cleardevice();
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
            // turn 不再重置
        }
        int dmg = skills[actSkill - 1].dmg;
        if (dmg > bossHp) dmg = bossHp;
        bossHp -= dmg;
        lastSkill = actSkill - 1;
        for (int& cd : cooldowns) if (cd > 0) cd--;
        cooldowns[actSkill - 1] = skills[actSkill - 1].maxCd;
        ++turn;
        wchar_t buf[128];
        swprintf(buf, 128, L"回合%d：Skill%d 造成%d伤害", turn, actSkill, dmg);
        drawBattleAuto(bossIdx, turn, totalBoss, bossHp, bossMaxHp, skills, cooldowns, lastSkill, buf, actions, ai);
        std::this_thread::sleep_for(std::chrono::milliseconds(step_sleep_ms));
    }
    // 总回合数提示
    wchar_t finalMsg[128];
    swprintf(finalMsg, 128, L"恭喜通关你用%d个回合击败了boss！", turn);
    drawBattleAuto(bossIdx, turn, totalBoss, bossHp, bossMaxHp, skills, cooldowns, -1, finalMsg, actions, actions.size());
    closegraph();
}