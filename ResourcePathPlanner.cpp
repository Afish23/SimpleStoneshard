#include "ResourcePathPlanner.h"

ResourcePathPlanner::Result ResourcePathPlanner::findOptimalPath(
    const vector<vector<MazeCell>>& maze,
    pair<int, int> start,
    pair<int, int> exit) {

    Result result;
    int n = maze.size();
    if (n == 0) return result;

    // 初始化DP表和路径记录
    vector<vector<DPState>> dp(n, vector<DPState>(n, {
    -10000,       // value
    0,             // keys
    true,          // hasWeapon
    vector<vector<bool>>(n, vector<bool>(n, false))  // collected
        }));
    vector<vector<pair<int, int>>> prev(n, vector<pair<int, int>>(n, { -1,-1 }));

    // 起点初始化
    dp[start.first][start.second] = {
        0,  // value
        0,  // keys
        true,  // hasWeapon
        vector<vector<bool>>(n, vector<bool>(n, false))  // collected
    };
    prev[exit.first][exit.second] = { -1, -1 };  // 初始化为无前驱
    // 强制终点价值
    const int EXIT_BASE_VALUE = 10000;
    dp[exit.first][exit.second].value = EXIT_BASE_VALUE;

    // Bellman-Ford算法变种
    bool updated = true;
    for (int k = 0; k < n * n && updated; k++) {
        updated = false;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (maze[i][j].type == WALL) continue;

                for (int d = 0; d < 4; d++) {
                    int ni = i + dx[d], nj = j + dy[d];
                    if (!isValidPosition(maze, ni, nj)) continue;
                    //cout << ni << " " << nj << endl;
                    // 深度拷贝当前状态
                    DPState newState = dp[i][j];

                    // 处理不同单元格类型
                    switch (maze[ni][nj].type) {
                    case GOLD:
                        if (!newState.collected[ni][nj]) {
                            newState.value += 5;
                            newState.collected[ni][nj] = true;
                        }
                        break;
                    case LOCKER:
                        newState.value += (newState.keys > 0) ? 2 : -5;
                        if (newState.keys > 0) newState.keys--;
                        break;
                    case BOSS:
                        newState.value += newState.hasWeapon ? 10 : -20;
                        newState.hasWeapon = false;
                        break;
                    default:
                        newState.value += getValue(maze[ni][nj].type);
                    }

                    // 状态更新条件
                    if (newState.value > dp[ni][nj].value&&(ni!=start.first&&nj!=start.second)) {
                        dp[ni][nj] = newState;
                        prev[ni][nj] = { i, j };
                        updated = true;
                    }
                    //cout << "\n--- 更新 (" << ni << "," << nj << ") ---\n";
                    //cout << "来自: (" << i << "," << j << ")\n";
                    //cout << "新值: " << newState.value << "\n";

                    //// 打印整个DP矩阵
                    //cout << "当前DP矩阵:\n";
                    //for (int x = 0; x < n; x++) {
                    //    for (int y = 0; y < n; y++) {
                    //        if (maze[x][y].type == WALL) {
                    //            cout << "#####\t"; // 墙的特殊标记
                    //        }
                    //        else {
                    //            cout << dp[x][y].value << "\t";
                    //        }
                    //    }
                    //    cout << "\n";
                    //}
                }
            }
        }
    }

    // 检查终点是否可达
    if (dp[exit.first][exit.second].value == INT_MIN) {
        cerr << "错误：终点不可达" << endl;
        return result;
    }

    // 在回溯路径前添加调试输出
    cout << "\n===== 调试 prev 数组 =====" << endl;
    // 检查终点是否有有效前驱
    if (prev[exit.first][exit.second] == pair<int, int>(-1, -1)) {
        cout << "错误：终点 (" << exit.first << "," << exit.second
            << ") 没有有效的前驱节点！" << endl;
    }
    else {
        cout << "终点 (" << exit.first << "," << exit.second
            << ") 的前驱是 (" << prev[exit.first][exit.second].first
            << "," << prev[exit.first][exit.second].second << ")" << endl;
    }

    // 打印所有非(-1,-1)的前驱关系
    cout << "\n所有有效的前驱关系：" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << "(" << i << "," << j << ") <- (" << prev[i][j].first << "," << prev[i][j].second << ")" << endl;
        }
    }

    vector<pair<int, int>> path;
    auto current = exit;
    set<pair<int, int>> visited;
    bool validPath = true;
    int stepCount = 0;
    const int maxSteps = n * n * 2;  // 防止无限循环

    while (current != start) {
        // 检查循环次数
        if (stepCount++ > maxSteps) {
            cerr << "警告：超过最大回溯步数" << endl;
            validPath = false;
            break;
        }

        // 检查是否形成环路
        if (visited.count(current)) {
            cerr << "警告：检测到路径环路 ("
                << current.first << "," << current.second << ")" << endl;
            validPath = false;
            break;
        }

        // 检查无效的上一个点
        if (prev[current.first][current.second] == pair<int, int>(-1, -1)) {
            cerr << "警告：遇到无效的上一点 ("
                << current.first << "," << current.second << ")" << endl;
            validPath = false;
            break;
        }

        path.push_back(current);
        visited.insert(current);
        current = prev[current.first][current.second];
    }

    if (validPath && current == start) {
        path.push_back(start);
        reverse(path.begin(), path.end());

        // 计算路径总价值（减去出口的基准值）
        int totalValue = dp[exit.first][exit.second].value - EXIT_BASE_VALUE;

        // 验证路径连续性
        for (size_t i = 1; i < path.size(); ++i) {
            int dx = abs(path[i].first - path[i - 1].first);
            int dy = abs(path[i].second - path[i - 1].second);
            if (dx + dy != 1) {
                cerr << "警告：路径不连续 between ("
                    << path[i - 1].first << "," << path[i - 1].second << ") and ("
                    << path[i].first << "," << path[i].second << ")" << endl;
                validPath = false;
                break;
            }
        }

        if (validPath) {
            result = { path, totalValue, true };
        }
    }

    // 如果路径无效，尝试返回部分路径（调试用）
    if (!result.success && !path.empty()) {
        cerr << "返回部分路径（仅用于调试）" << endl;
        reverse(path.begin(), path.end());
        result = { path, dp[exit.first][exit.second].value - EXIT_BASE_VALUE, false };
    }

    return result;
}

int ResourcePathPlanner::getValue(char cellType) {
    switch (cellType) {
    case TRAP:  return -3;
    case PATH:  return -1;  // 鼓励走最短路径
    case START: return 0;
    case EXIT:  return 0;
    default:    return 0;
    }
}

// 正确的实现方式
bool ResourcePathPlanner::isValidPosition(
    const vector<vector<MazeCell>>& maze,
    int x,
    int y)
{
    //cout << 1 << endl;
    // 检查迷宫是否为空
    if (maze.empty() || maze[0].empty()) return false;

    // 严格坐标检查
    return (x >= 0) && (x < maze.size()) &&
        (y >= 0) && (y < maze[0].size()) &&
        (maze[x][y].type != WALL);
}

vector<vector<char>> ResourcePathPlanner::markPath(
    const vector<vector<MazeCell>>& originalMaze,
    const vector<pair<int, int>>& path) {

    vector<vector<char>> markedMaze(
        originalMaze.size(),
        vector<char>(originalMaze[0].size()));

    // 复制原始迷宫
    for (size_t i = 0; i < originalMaze.size(); i++) {
        for (size_t j = 0; j < originalMaze[i].size(); j++) {
            markedMaze[i][j] = originalMaze[i][j].type;
        }
    }

    // 增强路径标记
    for (const auto& pos : path) {
        char& c = markedMaze[pos.first][pos.second];
        if (c == PATH) c = '*';
        else if (c == GOLD) c = 'X';
        else if (c == BOSS) c = 'b';
        else if (c == LOCKER) c = 'L';
    }

    return markedMaze;
}

void ResourcePathPlanner::printDPState(
    const vector<vector<DPState>>& dp, int k) {
    cout << "===== Iteration " << k << " =====" << endl;
    for (const auto& row : dp) {
        for (const auto& cell : row) {
            if (cell.value == INT_MIN) cout << "[-∞]";
            else cout << "[" << cell.value
                << "|K" << cell.keys
                << "|W" << cell.hasWeapon << "]";
        }
        cout << endl;
    }
}