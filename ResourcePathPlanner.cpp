#include "ResourcePathPlanner.h"
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

    // 1. 统计所有金币编号
    std::vector<std::pair<int, int>> golds;
    std::map<std::pair<int, int>, int> gold_id;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (maze[i][j].type == GOLD) {
                int id = golds.size();
                golds.push_back({ i,j });
                gold_id[{i, j}] = id;
            }
    int gnum = golds.size();
    if (gnum > 20) { // 太多金币不建议用此写法
        std::cerr << "金币过多（" << gnum << "），不适合状态压缩写法！" << std::endl;
        return result;
    }

    // 2. DP: dp[x][y][mask]=最大价值
    struct State {
        int value, steps;
        std::pair<int, int> prev;
    };
    std::vector<std::vector<std::vector<State>>> dp(
        n, std::vector<std::vector<State>>(n, std::vector<State>(1 << gnum, { -10000, INT_MAX, {-1,-1} })));
    struct Node { int x, y, mask; };
    std::queue<Node> q;

    dp[start.first][start.second][0] = { 0, 0, {-1,-1} };
    q.push({ start.first, start.second, 0 });

    int debug_step = 0;
    while (!q.empty()) {
        Node cur = q.front(); q.pop();
        int x = cur.x, y = cur.y, mask = cur.mask;
        State s = dp[x][y][mask];
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx < 0 || nx >= n || ny < 0 || ny >= n) continue;
            if (maze[nx][ny].type == WALL) continue;
            int nmask = mask, add = 0;
            // 如果是金币格且没吃过
            if (maze[nx][ny].type == GOLD) {
                int id = gold_id.at({ nx,ny });
                if (!(nmask & (1 << id))) {
                    nmask |= (1 << id);
                    add += 5;
                }
            }
            else if (maze[nx][ny].type == TRAP) {
                add -= 3;
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

            //// ====== debug输出（仿你原来的格式，按当前nmask打印）======
            //std::cout << "\n=== debug: step " << (debug_step++) << ", mask=" << nmask << " ===\n";
            //for (int i = 0; i < n; ++i) {
            //    for (int j = 0; j < n; ++j) {
            //        if (maze[i][j].type == WALL)
            //            std::cout << "#####\t";
            //        else if (dp[i][j][nmask].value == -10000)
            //            std::cout << "X\t";
            //        else
            //            std::cout << dp[i][j][nmask].value << "\t";
            //    }
            //    std::cout << "\n";
            //}
            //std::cout << std::endl;
            //// ====== end debug ======
        }
    }

    // 3. 在终点所有mask中找最大价值最短路径
    int bestValue = -10000, bestSteps = INT_MAX, bestMask = -1;
    for (int mask = 0; mask < (1 << gnum); ++mask) {
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

    // 4. 回溯路径
    std::vector<std::pair<int, int>> path;
    int x = exit.first, y = exit.second, mask = bestMask;
    while (!(x == start.first && y == start.second)) {
        path.push_back({ x, y });
        auto prev = dp[x][y][mask].prev;
        // 如果当前格子是金币，mask回退
        if (maze[x][y].type == GOLD) {
            int id = gold_id[{x, y}];
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
            marked[pos.first][pos.second] = 'X';
        else if (marked[pos.first][pos.second] == TRAP)
            marked[pos.first][pos.second] = 'T';
    }
    return marked;
}