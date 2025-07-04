#include "FightBossVisual.h"

// 播放每步的等待时间，单位毫秒
const int step_sleep_ms = 900;

IMAGE playerImg, playerAttackImg;
IMAGE bejingtu;
const int MAX_BOSS_PIC = 5;
IMAGE bossImgs[MAX_BOSS_PIC];

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
    putimage(0, 0, &bejingtu, SRCCOPY); // 先画背景
    setbkcolor(RGB(240, 240, 240));
    putimage(50, 350, &playerImg, SRCCOPY);
    settextcolor(BLACK);
    settextstyle(20, 0, L"Consolas");
    outtextxy_format(50, 330, L"Player (自动)");

    // Boss图片
    int bossPicIdx = bossIdx;
    if (bossPicIdx >= 0 && bossPicIdx < MAX_BOSS_PIC) {
        putimage(500, 100, &bossImgs[bossPicIdx], SRCCOPY);
    }
    outtextxy_format(500, 80, L"Boss %d/%d", bossIdx + 1, totalBoss);

    // Boss血条下移
    setfillcolor(GREEN);
    int showHp = bossHp > 0 ? bossHp : 0;
    fillrectangle(500, 260, 500 + showHp * 2, 280);
    outtextxy_format(500, 240, L"HP:%d/%d", showHp, bossMaxHp);

    // 技能按钮/冷却/伤害，编号从0开始
    for (int i = 0; i < skills.size(); ++i) {
        setfillcolor(lastSkill == i ? RGB(255, 215, 0) : LIGHTGRAY); // 上一步技能高亮
        fillrectangle(50 + i * 150, 520, 200 + i * 150, 570);
        outtextxy_format(100 + i * 150, 530, L"Skill%d", i); // 技能编号从0开始
        outtextxy_format(80 + i * 150, 550, L"CD:%d", cooldowns[i]);
        outtextxy_format(120 + i * 150, 550, L"伤害:%d", skills[i].dmg); // 新增技能伤害显示
    }
    // 消息
    outtextxy_format(500, 400, L"%s", msg);

    // ------ 技能序列 ------
    drawSkillSequence(220, 330, actions, currentActionIdx);
}

// 动画函数：人物移动到Boss旁边并击打（支持贴靠Boss侧边）
// 支持人物从(fromX, fromY)移动到(toX, toY)，Boss在(bossX, bossY)
void animatePlayerAttack(int fromX, int toX, int fromY, int toY, int bossX, int bossY, int bossIdx) {
    int steps = 20;
    int delay = 5; // 每帧5毫秒，移动更快
    // 1. 人物移动到boss旁边
    for (int i = 0; i <= steps; ++i) {
        cleardevice();
        putimage(0, 0, &bejingtu, SRCCOPY); // 加背景
        // Boss图
        if (bossIdx >= 0 && bossIdx < MAX_BOSS_PIC)
            putimage(bossX, bossY, &bossImgs[bossIdx], SRCCOPY);
        // 人物
        int px = fromX + (toX - fromX) * i / steps;
        int py = fromY + (toY - fromY) * i / steps;
        putimage(px, py, &playerImg, SRCCOPY);
        FlushBatchDraw();
        Sleep(delay);
    }
    // 2. 攻击动作
    cleardevice();
    putimage(0, 0, &bejingtu, SRCCOPY); // 加背景
    if (bossIdx >= 0 && bossIdx < MAX_BOSS_PIC)
        putimage(bossX, bossY, &bossImgs[bossIdx], SRCCOPY);
    putimage(toX, toY, &playerAttackImg, SRCCOPY);
    FlushBatchDraw();
    Sleep(300); // 攻击动作停顿300毫秒

    // 3. 恢复站立
    cleardevice();
    putimage(0, 0, &bejingtu, SRCCOPY); // 加背景
    if (bossIdx >= 0 && bossIdx < MAX_BOSS_PIC)
        putimage(bossX, bossY, &bossImgs[bossIdx], SRCCOPY);
    putimage(toX, toY, &playerImg, SRCCOPY);
    FlushBatchDraw();
    Sleep(100); // 站立停顿
}

void fightBossVisualAuto(
    const std::vector<int>& bossHps,
    const std::vector<Skill>& skills,
    const std::vector<std::string>& actions
) {
    loadimage(&playerImg, L"player.png", 150, 150);
    loadimage(&playerAttackImg, L"player_attack.png", 150, 150);

    // 加载Boss图片
    for (int i = 0; i < MAX_BOSS_PIC; ++i) {
        wchar_t filename[32];
        swprintf(filename, 32, L"boss%d.png", i + 1); // boss1.png, boss2.png, ...
        loadimage(&bossImgs[i], filename, 150, 150);   // 可根据实际需要调整宽高
    }
    initgraph(800, 600);
    loadimage(&bejingtu, L"bejing.png", 800, 600); // 800x600为窗口大小
    cleardevice();
    putimage(0, 0, &bejingtu, SRCCOPY);

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
        int ret = sscanf_s(act.c_str(), "Boss%d-%d-%d", &actBoss, &actTurn, &actSkill);
        assert(ret == 3);
        if (actBoss - 1 != bossIdx) {
            bossIdx = actBoss - 1;
            bossHp = bossHps[bossIdx];
            bossMaxHp = bossHp;
            // turn 不再重置
        }
        // 不要 -1，actSkill 就是 0-based 下标
        if (actSkill < 0 || actSkill >= skills.size()) {
            // 错误处理，比如 continue
            continue;
        }
        int dmg = skills[actSkill].dmg;
        if (dmg > bossHp) dmg = bossHp;
        bossHp -= dmg;
        lastSkill = actSkill;
        for (int& cd : cooldowns) if (cd > 0) cd--;
        cooldowns[actSkill] = skills[actSkill].maxCd;
        ++turn;
        wchar_t buf[128];
        swprintf(buf, 128, L"回合%d：Skill%d 造成%d伤害", turn, actSkill, dmg);

        // --- 动画：人物移动到Boss旁边并击打 ---
        // 以图片宽150为例，人物Y与boss顶端对齐
        int playerW = 150, playerH = 150;
        int bossW = 150, bossH = 150;
        int bossX = 500, bossY = 100;
        int playerStartX = 50, playerStartY = 350;
        int playerEndX = bossX - playerW;
        int playerEndY = bossY;

        animatePlayerAttack(playerStartX, playerEndX, playerStartY, playerEndY, bossX, bossY, bossIdx);

        // --- 刷新战斗界面 ---
        drawBattleAuto(bossIdx, turn, totalBoss, bossHp, bossMaxHp, skills, cooldowns, lastSkill, buf, actions, ai);
        std::this_thread::sleep_for(std::chrono::milliseconds(step_sleep_ms));
    }
    // 总回合数提示
    wchar_t finalMsg[128];
    swprintf(finalMsg, 128, L"恭喜通关你用%d个回合击败了boss！", turn);
    drawBattleAuto(bossIdx, turn, totalBoss, bossHp, bossMaxHp, skills, cooldowns, -1, finalMsg, actions, actions.size());
    system("pause");
    //closegraph();
}