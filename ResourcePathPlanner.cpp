#include "ResourcePathPlanner.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <map>

namespace {
    const int dx[4] = { -1, 1, 0, 0 };
    const int dy[4] = { 0, 0, -1, 1 };
}

// ���ظ��ӵļ�ֵ�����ֻ�ܳ�һ����״̬�����ƣ�
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

    // 1. ͳ�����н�Һ�������
    std::map<std::pair<int, int>, int> special_id;
    int spnum = 0;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (maze[i][j].type == GOLD || maze[i][j].type == TRAP)
                special_id[{i, j}] = spnum++;
    if (spnum > 20) {
        std::cerr << "���+������ࣨ" << spnum << "�������ʺ�״̬ѹ��д����" << std::endl;
        return result;
    }

    struct State {
        int value, steps;
        std::pair<int, int> prev;
    };
    std::vector<std::vector<std::vector<State>>> dp(
        n, std::vector<std::vector<State>>(n, std::vector<State>(1 << spnum, { -10000, INT_MAX, {-1,-1} })));
    struct Node { int x, y, mask; };
    std::queue<Node> q;

    dp[start.first][start.second][0] = { 0, 0, {-1,-1} };
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
                        add += 5;
                    else if (maze[nx][ny].type == TRAP)
                        add -= 3;
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
                q.push({ nx, ny, nmask });
            }
        }
    }

    // 3. ���յ�����mask��������ֵ���·��
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

    // 4. ����·��
    std::vector<std::pair<int, int>> path;
    int x = exit.first, y = exit.second, mask = bestMask;
    while (!(x == start.first && y == start.second)) {
        path.push_back({ x, y });
        auto prev = dp[x][y][mask].prev;
        // �����ǰ����������㣨��һ����壩��mask����
        auto it = special_id.find({ x, y });
        if (it != special_id.end()) {
            int id = it->second;
            if (mask & (1 << id)) mask ^= (1 << id);
        }
        int tx = prev.first, ty = prev.second;
        x = tx, y = ty;
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());

    result.success = true;
    result.path = path;
    result.totalValue = bestValue;
    return result;
}

std::vector<std::vector<char>> ResourcePathPlanner::markPath(
    const std::vector<std::vector<MazeCell>>& originalMaze,
    const std::vector<std::pair<int, int>>& path)
{
    std::vector<std::vector<char>> marked(
        originalMaze.size(),
        std::vector<char>(originalMaze[0].size()));
    for (size_t i = 0; i < originalMaze.size(); i++)
        for (size_t j = 0; j < originalMaze[i].size(); j++)
            marked[i][j] = originalMaze[i][j].type;

    for (const auto& pos : path) {
        if (marked[pos.first][pos.second] == PATH)
            marked[pos.first][pos.second] = '*';
        else if (marked[pos.first][pos.second] == GOLD)
            marked[pos.first][pos.second] = 'g';
        else if (marked[pos.first][pos.second] == TRAP)
            marked[pos.first][pos.second] = 't';
    }
    return marked;
}