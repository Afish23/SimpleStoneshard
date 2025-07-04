#include "Visualizer.h"
#include <thread>
#include <chrono>
#include <unordered_set>
#include <map>
#include <functional>

MazeVisualizer::MazeVisualizer(int n, int cellSize) : n(n), cellSize(cellSize) {
    initgraph(n * cellSize + 2 * margin, n * cellSize + 2 * margin, SHOWCONSOLE);
    setbkcolor(WHITE);
    cleardevice();
}

MazeVisualizer::~MazeVisualizer() {
    closegraph();
}

void MazeVisualizer::drawMaze(const std::vector<std::vector<MazeCell>>& maze) {
    cleardevice();
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            int x = margin + j * cellSize;
            int y = margin + i * cellSize;
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
    for (auto& p : path) {
        int x = margin + p.second * cellSize;
        int y = margin + p.first * cellSize;
        setfillcolor(color);
        solidrectangle(x, y, x + cellSize, y + cellSize);
    }
}

void MazeVisualizer::drawPlayer(const std::pair<int, int>& pos, COLORREF color) {
    int x = margin + pos.second * cellSize + cellSize / 6;
    int y = margin + pos.first * cellSize + cellSize / 6;
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
    // 建立Boss位置索引
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
            if (bossCallback) {
                bossCallback(pos); // 这会触发Boss战斗
            }
        }
        else {
            drawPlayer(pos);
        }

        FlushBatchDraw();
        this_thread::sleep_for(chrono::milliseconds(delayMs));
    }
}