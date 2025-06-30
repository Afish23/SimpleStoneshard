#include "FightBossVisual.h"

// ����ÿ���ĵȴ�ʱ�䣬��λ����
const int step_sleep_ms = 1800;
IMAGE playerImg;
const int MAX_BOSS_PIC = 5;
IMAGE bossImgs[MAX_BOSS_PIC];
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

    outtextxy(x, 10, L"ս����־��"); // �����y=10

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
    putimage(50, 350, &playerImg, SRCCOPY);
    settextcolor(BLACK);
    settextstyle(20, 0, L"Consolas");
    outtextxy_format(50, 330, L"Player (�Զ�)");

    // BossͼƬ
    int bossPicIdx = bossIdx;
    if (bossPicIdx >= 0 && bossPicIdx < MAX_BOSS_PIC) {
        putimage(500, 100, &bossImgs[bossPicIdx], SRCCOPY);
    }
    outtextxy_format(500, 80, L"Boss %d/%d", bossIdx + 1, totalBoss);

    // BossѪ������
    setfillcolor(GREEN);
    int showHp = bossHp > 0 ? bossHp : 0;
    fillrectangle(500, 260, 500 + showHp * 2, 280);
    outtextxy_format(500, 240, L"HP:%d/%d", showHp, bossMaxHp);

    // ���ܰ�ť/��ȴ/�˺�����Ŵ�0��ʼ
    for (int i = 0; i < skills.size(); ++i) {
        setfillcolor(lastSkill == i ? RGB(255, 215, 0) : LIGHTGRAY); // ��һ�����ܸ���
        fillrectangle(50 + i * 150, 520, 200 + i * 150, 570);
        outtextxy_format(100 + i * 150, 530, L"Skill%d", i); // ���ܱ�Ŵ�0��ʼ
        outtextxy_format(80 + i * 150, 550, L"CD:%d", cooldowns[i]);
        outtextxy_format(120 + i * 150, 550, L"�˺�:%d", skills[i].dmg); // ���������˺���ʾ
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
    
    loadimage(&playerImg, L"player.png", 150, 150);  // �ڶ��������ɵ���Ϊ����Ҫ�Ŀ��
    // ����BossͼƬ
    for (int i = 0; i < MAX_BOSS_PIC; ++i) {
        wchar_t filename[32];
        swprintf(filename, 32, L"boss%d.png", i + 1); // boss1.png, boss2.png, ...
        loadimage(&bossImgs[i], filename, 150, 150);   // �ɸ���ʵ����Ҫ�������
    }
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
        int ret = sscanf_s(act.c_str(), "Boss%d-%d-%d", &actBoss, &actTurn, &actSkill);
        assert(ret == 3);
        if (actBoss - 1 != bossIdx) {
            bossIdx = actBoss - 1;
            bossHp = bossHps[bossIdx];
            bossMaxHp = bossHp;
            // turn ��������
        }
        // ��Ҫ -1��actSkill ���� 0-based �±�
        if (actSkill < 0 || actSkill >= skills.size()) {
            // ���������� continue
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