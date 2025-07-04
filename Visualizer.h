#pragma once
#include <vector>
#include <utility>
#include <string>
#include <functional>
#include "GameObjects.h"
#include <graphics.h>
#include"MazeGenerator.h"
#define COLOR_PATH RGB(0, 128, 255)
#define COLOR_GREEDY RGB(255, 153, 0)
#define COLOR_BOSS RGB(255, 0, 0)
#define COLOR_GOLD RGB(255, 215, 0)
#define COLOR_TRAP RGB(192, 0, 0)
#define COLOR_LOCKER RGB(128, 0, 255)
#define COLOR_START RGB(0, 255, 0)
#define COLOR_EXIT RGB(255, 0, 255)
#define COLOR_EMPTY RGB(240, 240, 240)
#define COLOR_WALL RGB(100, 100, 100)

class MazeVisualizer {
public:
    MazeVisualizer(int n, int cellSize = 50);
    ~MazeVisualizer();

    void drawMaze(const std::vector<std::vector<MazeCell>>& maze);
    void drawPath(const std::vector<std::pair<int, int>>& path, COLORREF color);
    void drawPlayer(const std::pair<int, int>& pos, COLORREF color = RGB(0, 0, 0));
    void showInfo(const std::string& info);

    void animatePath(const std::vector<std::pair<int, int>>& path,
        const std::vector<std::vector<MazeCell>>& maze,
        int delayMs = 250,
        COLORREF pathColor = COLOR_PATH,
        bool showPlayer = true);

    // 注意：maze要用引用，便于Boss战后实时反映格子变化
    void animateGreedy(const std::vector<std::pair<int, int>>& fullPath,
        std::vector<std::vector<MazeCell>>& maze,
        const std::vector<std::pair<int, int>>& bossSteps,
        int delayMs = 220,
        std::function<void(const std::pair<int, int>&)> bossCallback = nullptr);

private:
    int n;
    int cellSize;
    int margin = 40;
};