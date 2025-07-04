#include "Visualizer.h"
#include <thread>
#include <chrono>
#include <unordered_set>
#include <map>
#include <functional>

MazeVisualizer::MazeVisualizer(int n, int cellSize) : n(n) {
    // 计算合适的cellSize使迷宫能适应800x600窗口
    int maxPossibleCellSize = min(
        (WINDOW_WIDTH - 2 * margin) / n,
        (WINDOW_HEIGHT - 2 * margin) / n
    );
    this->cellSize = min(cellSize, maxPossibleCellSize);

    // 固定窗口大小为800x600
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT, SHOWCONSOLE);
    setbkcolor(WHITE);
    cleardevice();
}

MazeVisualizer::~MazeVisualizer() {
    closegraph();
}

void MazeVisualizer::drawMaze(const std::vector<std::vector<MazeCell>>& maze) {
    cleardevice();
    // 计算居中偏移量
    int mazeWidth = n * cellSize;
    int mazeHeight = n * cellSize;
    int offsetX = (WINDOW_WIDTH - mazeWidth) / 2;
    int offsetY = (WINDOW_HEIGHT - mazeHeight) / 2;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            int x = offsetX + j * cellSize;
            int y = offsetY + i * cellSize;
            char type = maze[i][j].type;
            COLORREF color = COLOR_EMPTY;
            switch (type) {
            case '#': color = COLOR_WALL; break;
            case 'G': color = COLOR_GOLD; break;
            case 'T': color = COLOR_TRAP; break;
            case 'L': color = COLOR_LOCKER; break;
            case 'B': color = COLOR_BOSS; break;
            case 'S': color = COLOR_START; break;
            case 'E': color = COLOR_EXIT; break;
            case '*': color = COLOR_PATH; break;
            default: color = COLOR_EMPTY; break;
            }
            setfillcolor(color);
            solidrectangle(x, y, x + cellSize, y + cellSize);

            settextcolor(BLACK);
            setbkmode(TRANSPARENT);
#ifdef UNICODE
            wchar_t label[2] = { type, 0 };
            outtextxy(x + cellSize / 3, y + cellSize / 3, label);
#else
            char label[2] = { type, 0 };
            outtextxy(x + cellSize / 3, y + cellSize / 3, label);
#endif
        }
}

void MazeVisualizer::drawPath(const std::vector<std::pair<int, int>>& path, COLORREF color) {
    int mazeWidth = n * cellSize;
    int mazeHeight = n * cellSize;
    int offsetX = (WINDOW_WIDTH - mazeWidth) / 2;
    int offsetY = (WINDOW_HEIGHT - mazeHeight) / 2;

    for (auto& p : path) {
        int x = offsetX + p.second * cellSize;
        int y = offsetY + p.first * cellSize;
        setfillcolor(color);
        solidrectangle(x, y, x + cellSize, y + cellSize);
    }
}

void MazeVisualizer::drawPlayer(const std::pair<int, int>& pos, COLORREF color) {
    int mazeWidth = n * cellSize;
    int mazeHeight = n * cellSize;
    int offsetX = (WINDOW_WIDTH - mazeWidth) / 2;
    int offsetY = (WINDOW_HEIGHT - mazeHeight) / 2;

    int x = offsetX + pos.second * cellSize + cellSize / 6;
    int y = offsetY + pos.first * cellSize + cellSize / 6;
    setfillcolor(color);
    fillcircle(x + cellSize / 3, y + cellSize / 3, cellSize / 4);
}

void MazeVisualizer::showInfo(const std::string& info) {
    settextcolor(BLUE);
    setbkmode(OPAQUE);
    setbkcolor(WHITE);
#ifdef UNICODE
    std::wstring winfo(info.begin(), info.end());
    outtextxy(margin, 10, winfo.c_str());
#else
    outtextxy(margin, 10, info.c_str());
#endif
}

void MazeVisualizer::animatePath(const std::vector<std::pair<int, int>>& path,
    const std::vector<std::vector<MazeCell>>& maze,
    int delayMs, COLORREF pathColor, bool showPlayer) {
    for (size_t i = 0; i < path.size(); ++i) {
        drawMaze(maze);
        std::vector<std::pair<int, int>> subPath(path.begin(), path.begin() + i + 1);
        drawPath(subPath, pathColor);
        if (showPlayer)
            drawPlayer(path[i]);
        FlushBatchDraw();
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }
}

void MazeVisualizer::animateGreedy(
    const vector<pair<int, int>>& fullPath,
    vector<vector<MazeCell>>& maze,
    const vector<pair<int, int>>& bossSteps,
    int delayMs,
    function<void(const pair<int, int>&)> bossCallback)
{
    unordered_set<string> bossPositions;
    for (auto& pos : bossSteps) {
        bossPositions.insert(to_string(pos.first) + "," + to_string(pos.second));
    }

    for (size_t i = 0; i < fullPath.size(); ++i) {
        auto& pos = fullPath[i];
        drawMaze(maze);

        // 绘制路径
        vector<pair<int, int>> subPath(fullPath.begin(), fullPath.begin() + i + 1);
        drawPath(subPath, COLOR_GREEDY);

        // 检查是否是Boss位置
        string posKey = to_string(pos.first) + "," + to_string(pos.second);
        if (bossPositions.count(posKey) && maze[pos.first][pos.second].type == 'B') {
            drawPlayer(pos, COLOR_BOSS);
            FlushBatchDraw(); // 确保在回调前绘制完成

            if (bossCallback) {
                bossCallback(pos); // 触发Boss战斗

                // 战斗结束后重新绘制当前状态
                drawMaze(maze);
                drawPath(subPath, COLOR_GREEDY);
                drawPlayer(pos);
                FlushBatchDraw();
            }
        }
        else {
            drawPlayer(pos);
        }

        FlushBatchDraw();
        this_thread::sleep_for(chrono::milliseconds(delayMs));
    }
}