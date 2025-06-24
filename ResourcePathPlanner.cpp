#include "ResourcePathPlanner.h"

ResourcePathPlanner::Result ResourcePathPlanner::findOptimalPath(
    const std::vector<std::vector<MazeCell>>& maze,
    std::pair<int, int> start,
    std::pair<int, int> exit) {

    Result result;
    int n = maze.size();
    if (n == 0) return result;

    // 初始化DP表和路径记录
    std::vector<std::vector<int>> dp(n, std::vector<int>(n, INT_MIN));
    std::vector<std::vector<std::pair<int, int>>> prev(n, std::vector<std::pair<int, int>>(n, { -1, -1 }));

    dp[start.first][start.second] = 0;

    // Bellman-Ford算法变种
    bool updated = true;
    for (int k = 0; k < n * n && updated; k++) {
        updated = false;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (maze[i][j].type == WALL) continue;

                for (int d = 0; d < 4; d++) {
                    int ni = i + dx[d];
                    int nj = j + dy[d];

                    if (isValidPosition(maze, ni, nj)) {
                        int newValue = dp[i][j] + getValue(maze[ni][nj].type);
                        if (newValue > dp[ni][nj]) {
                            dp[ni][nj] = newValue;
                            prev[ni][nj] = { i, j };
                            updated = true;
                        }
                    }
                }
            }
        }
    }

    // 回溯路径
    std::vector<std::pair<int, int>> path;
    auto current = exit;

    while (current != start && current != std::pair<int, int>(-1, -1)) {
        path.push_back(current);
        current = prev[current.first][current.second];
    }

    if (current == start) {
        path.push_back(start);
        std::reverse(path.begin(), path.end());
        result.success = true;
        result.path = path;
        result.totalValue = dp[exit.first][exit.second];
    }

    return result;
}

int ResourcePathPlanner::getValue(char cellType) {
    switch (cellType) {
    case GOLD: return 5;
    case TRAP: return -3;
    case START:
    case EXIT:
    case PATH: return 0;
    default: return 0; // BOSS/LOCKER等不影响路径价值
    }
}

bool ResourcePathPlanner::isValidPosition(const std::vector<std::vector<MazeCell>>& maze,
    int x, int y) {
    return x >= 0 && x < maze.size() &&
        y >= 0 && y < maze[0].size() &&
        maze[x][y].type != WALL;
}

std::vector<std::vector<char>> ResourcePathPlanner::markPath(
    const std::vector<std::vector<MazeCell>>& originalMaze,
    const std::vector<std::pair<int, int>>& path) {

    std::vector<std::vector<char>> markedMaze(
        originalMaze.size(),
        std::vector<char>(originalMaze[0].size()));

    // 复制原始迷宫
    for (size_t i = 0; i < originalMaze.size(); i++) {
        for (size_t j = 0; j < originalMaze[i].size(); j++) {
            markedMaze[i][j] = originalMaze[i][j].type;
        }
    }

    // 标记路径
    for (const auto& pos : path) {
        if (markedMaze[pos.first][pos.second] == PATH) {
            markedMaze[pos.first][pos.second] = '*';
        }
    }

    return markedMaze;
}