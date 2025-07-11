﻿#include "ResourcePathPlanner.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <map>

namespace {
    const int dx[4] = { -1, 1, 0, 0 };
    const int dy[4] = { 0, 0, -1, 1 };
}

// 返回格子的价值（金币只能吃一次由状态机控制）
int ResourcePathPlanner::getValue(char cellType) {
    switch (cellType) {
    case GOLD: return 5;
    case TRAP: return -3;
    case PATH: return 0;
    case START:
    case EXIT: return 0;
    default: return 0;
    }
}

ResourcePathPlanner::Result ResourcePathPlanner::findOptimalPath(
    const std::vector<std::vector<MazeCell>>& maze,
    std::pair<int, int> start,
    std::pair<int, int> exit)
    
{
    Result result;
    int n = maze.size();
    if (n == 0) return result;

    // 1. 统计所有金币和陷阱编号
    std::map<std::pair<int, int>, int> special_id;
    int spnum = 0;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (maze[i][j].type == GOLD || maze[i][j].type == TRAP)
                special_id[{i, j}] = spnum++;
    /*if (spnum > 20) {
        std::cerr << "金币+陷阱过多（" << spnum << "），不适合状态压缩写法！" << std::endl;
        return result;
    }*/

    struct State {
        int value, steps;
        std::pair<int, int> prev;
        int prevMask;  // 新增字段
    };
    std::vector<std::vector<std::vector<State>>> dp(
        n, std::vector<std::vector<State>>(n, std::vector<State>(1 << spnum, { -10000, INT_MAX, {-1,-1}, -1})));
    struct Node { int x, y, mask; };
    std::queue<Node> q;

    dp[start.first][start.second][0] = { 0, 0, {-1,-1}, -1 };
    q.push({ start.first, start.second, 0 });

    while (!q.empty()) {
        Node cur = q.front(); q.pop();
        int x = cur.x, y = cur.y, mask = cur.mask;
        State s = dp[x][y][mask];
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx < 0 || nx >= n || ny < 0 || ny >= n) continue;
            if (maze[nx][ny].type == WALL) continue;
            int nmask = mask, add = 0;
            auto it = special_id.find({ nx,ny });
            if (it != special_id.end()) {
                int id = it->second;
                if (!(nmask & (1 << id))) {
                    nmask |= (1 << id);
                    if (maze[nx][ny].type == GOLD)
                        add += 50;
                    else if (maze[nx][ny].type == TRAP)
                        add -= 30;
                }
            }
            int nvalue = s.value + add;
            int nsteps = s.steps + 1;
            State& ref = dp[nx][ny][nmask];
            if (nvalue > ref.value ||
                (nvalue == ref.value && nsteps < ref.steps)) {
                ref.value = nvalue;
                ref.steps = nsteps;
                ref.prev = { x, y };
                ref.prevMask = mask;  // 存储前驱掩码
                q.push({ nx, ny, nmask });
                //cout << x<<" "<<y<<" "<<" "<<nx<<" "<<ny << endl;
            }
        }

    }

    // 3. 在终点所有mask中找最大价值最短路径
    int bestValue = -10000, bestSteps = INT_MAX, bestMask = -1;
    for (int mask = 0; mask < (1 << spnum); ++mask) {
        State& s = dp[exit.first][exit.second][mask];
        if (s.value > bestValue ||
            (s.value == bestValue && s.steps < bestSteps)) {
            bestValue = s.value;
            bestSteps = s.steps;
            bestMask = mask;
        }
    }
    if (bestValue == -10000) {
        result.success = false;
        return result;
    }

    std::vector<std::pair<int, int>> path;
    int x = exit.first, y = exit.second;
    int mask = bestMask;
    int length = 0;
    // 从终点回溯到起点
    while (x != -1 || y != -1) {  // 使用(-1,-1)作为起点前驱的终止条件
        // 添加当前节点到路径
        path.push_back({ x, y });
        length++;
        // 获取当前状态
        State& s = dp[x][y][mask];

        // 到达起点（前驱为(-1,-1)）时终止
        if (s.prev.first == -1 && s.prev.second == -1) {
            break;
        }

        // 更新为前驱状态
        int nextMask = s.prevMask;  // 使用存储的前驱掩码
        x = s.prev.first;
        y = s.prev.second;
        mask = nextMask;
    }
    cout << "路径长度为：" << length << endl;
    // 反转路径（当前是从终点到起点）
    std::reverse(path.begin(), path.end());

    // 设置结果
    result.success = true;
    result.path = path;
    result.totalValue = bestValue;

    return result;
}

std::vector<std::vector<MazeCell>> ResourcePathPlanner::markPath(
    const std::vector<std::vector<MazeCell>>& originalMaze,
    const std::vector<std::pair<int, int>>& path)
{
    // 创建原始迷宫的深拷贝
    std::vector<std::vector<MazeCell>> markedMaze = originalMaze;

    // 标记路径上的单元格
    for (const auto& pos : path) {
        int i = pos.first;
        int j = pos.second;

        // 根据单元格类型设置不同的标记
        if (markedMaze[i][j].type == PATH) {
            markedMaze[i][j].type = '*';  // 路径标记
        }
        else if (markedMaze[i][j].type == GOLD) {
            markedMaze[i][j].type = 'g';  // 金币标记
        }
        else if (markedMaze[i][j].type == TRAP) {
            markedMaze[i][j].type = 't';  // 陷阱标记
        }
        // 其他类型保持不变
    }

    return markedMaze;
}