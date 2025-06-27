#include "FightBossVisual.h"

// 播放每步的等待时间，单位毫秒
const int step_sleep_ms = 1800;

// 绘制技能序列（显示在玩家头像旁边）
void drawSkillSequence(
    int x, int y,
    const std::vector<std::string>& actions,
    int currentIdx
) {
    settextcolor(BLACK);
    settextstyle(18, 0, "Consolas");
    outtextxy(x, y - 24, "技能序列：");
    for (int i = 0; i < actions.size(); ++i) {
        // 当前回合高亮，其前面用深色，其后用浅色
        if (i < currentIdx)
            settextcolor(RGB(120, 120, 120)); // 已释放
        else if (i == currentIdx)
            settextcolor(RGB(255, 69, 0));    // 当前
        else
            settextcolor(RGB(180, 180, 180)); // 未释放
        char stepbuf[64];
        sprintf(stepbuf, "%2d. %s", i + 1, actions[i].c_str());
        outtextxy(x, y + i * 22, stepbuf);
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
    setbkcolor(WHITE);
    setfillcolor(RGB(30, 144, 255));
    fillrectangle(50, 350, 200, 500);
    settextcolor(BLACK);
    settextstyle(20, 0, "Consolas");
    outtextxy_format(50, 330, "Player (自动)");

    setfillcolor(RGB(178, 34, 34));
    fillrectangle(500, 100, 650, 250);
    outtextxy_format(500, 80, "Boss %d/%d", bossIdx + 1, totalBoss);

    // boss血条
    setfillcolor(GREEN);
    int showHp = bossHp > 0 ? bossHp : 0;
    fillrectangle(500, 70, 500 + showHp * 2, 90);
    outtextxy_format(500, 60, "HP:%d/%d", showHp, bossMaxHp);

    // 技能按钮/冷却
    for (int i = 0; i < skills.size(); ++i) {
        setfillcolor(lastSkill == i ? RGB(255, 215, 0) : LIGHTGRAY); // 上一步技能高亮
        fillrectangle(50 + i * 150, 520, 200 + i * 150, 570);
        outtextxy_format(60 + i * 150, 530, "Skill%d", i + 1);
        outtextxy_format(60 + i * 150, 550, "CD:%d", cooldowns[i]);
    }
    // 消息
    outtextxy_format(250, 400, "%s", msg);

    // ------ 技能序列 ------
    drawSkillSequence(220, 330, actions, currentActionIdx);
}

void fightBossVisualAuto(
    const std::vector<int>& bossHps,
    const std::vector<Skill>& skills,
    const std::vector<std::string>& actions
) {
    initgraph(800, 600);
    int totalBoss = bossHps.size();
    int bossIdx = 0;
    int bossHp = bossHps[0];
    int bossMaxHp = bossHps[0];
    int turn = 0;
    std::vector<int> cooldowns(skills.size(), 0);
    int lastSkill = -1;

    for (int ai = 0; ai < actions.size(); ++ai) {
        const auto& act = actions[ai];
        int actBoss, actTurn, actSkill;
        sscanf_s(act.c_str(), "Boss%d-%d-%d", &actBoss, &actTurn, &actSkill);
        if (actBoss - 1 != bossIdx) {
            bossIdx = actBoss - 1;
            bossHp = bossHps[bossIdx];
            bossMaxHp = bossHp;
            // 冷却保持不变
            turn = 0;
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
        // 关键：传递 actions 和 ai
        drawBattleAuto(bossIdx, turn, totalBoss, bossHp, bossMaxHp, skills, cooldowns, lastSkill, buf, actions, ai);
        std::this_thread::sleep_for(std::chrono::milliseconds(step_sleep_ms));
    }
    drawBattleAuto(bossIdx, turn, totalBoss, bossHp, bossMaxHp, skills, cooldowns, -1, L"恭喜通关！", actions, actions.size());
    system("pause");
    closegraph();
}