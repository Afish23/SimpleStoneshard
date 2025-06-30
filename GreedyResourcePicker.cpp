#include "GreedyResourcePicker.h"

// 密码锁解谜函数（已有）
int solveCombinationLock() {
    // 回溯法破解三位密码锁
    // 实际实现会返回尝试次数
    return 3; // 示例值，实际应用中会返回真实尝试次数
}

// 触发Boss战的通用函数（新增）
void triggerBossFight(
    vector<vector<MazeCell>>& maze,
    pair<int, int>& playerPos,
    int& totalScore) {

    cout << "触发boss战" << endl;
    ifstream ifs("boss_case.json");
    if (!ifs) {
        cerr << "无法打开boss_case.json文件" << endl;
        return;
    }

    nlohmann::json j;
    ifs >> j;

    // 解析Boss血量
    vector<int> bossHps = j["B"].get<vector<int>>();

    // 解析技能
    std::vector<Skill> skills;
    for (const auto& arr : j["PlayerSkills"]) {
        int damage = arr[0];
        int cooldown = arr[1];
        skills.emplace_back(damage, cooldown);
    }

    // 计算最优技能释放顺序
    BossFightStrategy bfs;
    auto result = bfs.minTurnSkillSequence(bossHps, skills);

    // 自动可视化播放整个战斗流程
    fightBossVisualAuto(bossHps, skills, result.second);

    maze[playerPos.first][playerPos.second].type = ' '; // boss被击败后变为通路
    cout << "成功击败boss！当前位置: (" << playerPos.first << ", " << playerPos.second << ")"
        << " 总得分: " << totalScore << endl;
}

// 视野函数（固定3x3视野）
vector<pair<pair<int, int>, char>> getVisibleResources(
    const vector<vector<MazeCell>>& maze,
    const pair<int, int>& playerPos,
    int visionRadius) {

    vector<pair<pair<int, int>, char>> visibleResources;
    int n = maze.size();
    int x = playerPos.first;
    int y = playerPos.second;

    // 固定3x3视野范围
    for (int i = max(0, x - 1); i <= min(n - 1, x + 1); i++) {
        for (int j = max(0, y - 1); j <= min(n - 1, y + 1); j++) {
            // 跳过玩家自身位置
            if (i == x && j == y) continue;

            char cellType = maze[i][j].type;
            // 包括金币、陷阱和boss
            if (cellType == 'G' || cellType == 'T' || cellType == 'B') {
                visibleResources.push_back({ {i, j}, cellType });
            }
        }
    }
    return visibleResources;
}

// 机关解锁函数（密码锁破解）
bool tryUnlockLocker(vector<vector<MazeCell>>& maze,
    const pair<int, int>& pos,
    int& totalScore) {
    cout << "发现机关，开始解谜..." << endl;

    // 破解密码锁
    int attempts = solveCombinationLock();
    int penalty = attempts; // 每次尝试扣5分

    totalScore -= penalty;

    cout << "扣分: " << penalty << endl;

    maze[pos.first][pos.second].type = ' '; // 解锁后变为通路
    return true;
}

// 深度优先搜索寻找出口路径
vector<pair<int, int>> findPathDFS(
    const vector<vector<MazeCell>>& maze,
    const pair<int, int>& start,
    const pair<int, int>& end) {

    int n = maze.size();
    vector<vector<bool>> visited(n, vector<bool>(n, false));
    vector<vector<pair<int, int>>> parent(n, vector<pair<int, int>>(n, { -1, -1 }));
    stack<pair<int, int>> s;

    // 方向数组
    vector<pair<int, int>> directions = { {-1,0}, {1,0}, {0,-1}, {0,1} };

    visited[start.first][start.second] = true;
    s.push(start);

    bool found = false;

    while (!s.empty()) {
        auto curr = s.top();
        s.pop();

        // 如果到达终点
        if (curr == end) {
            found = true;
            break;
        }

        // 随机方向探索（模拟深度优先）
        vector<pair<int, int>> shuffled = directions;
        random_shuffle(shuffled.begin(), shuffled.end());

        for (const auto& dir : shuffled) {
            int nx = curr.first + dir.first;
            int ny = curr.second + dir.second;

            // 检查边界和是否可通行
            if (nx >= 0 && nx < n && ny >= 0 && ny < n &&
                !visited[nx][ny] &&
                isPassable(maze[nx][ny].type)) {

                visited[nx][ny] = true;
                parent[nx][ny] = curr;
                s.push({ nx, ny });
            }
        }
    }

    // 重建路径
    vector<pair<int, int>> path;
    if (found) {
        pair<int, int> curr = end;
        while (curr != start) {
            path.push_back(curr);
            curr = parent[curr.first][curr.second];
        }
        reverse(path.begin(), path.end());
    }

    return path;
}

// 移动函数（添加机关扣分）
bool moveToPosition(
    vector<vector<MazeCell>>& maze,
    pair<int, int>& playerPos,
    const pair<int, int>& target,
    int& totalScore,
    int& steps,
    unordered_set<string>& visited) {

    int n = maze.size();
    vector<vector<bool>> visitedMap(n, vector<bool>(n, false));
    vector<vector<pair<int, int>>> parent(n, vector<pair<int, int>>(n, { -1, -1 }));
    queue<pair<int, int>> q;

    vector<pair<int, int>> directions = { {-1,0}, {1,0}, {0,-1}, {0,1} };

    visitedMap[playerPos.first][playerPos.second] = true;
    q.push(playerPos);

    bool found = false;

    while (!q.empty()) {
        auto curr = q.front();
        q.pop();

        if (curr == target) {
            found = true;
            break;
        }

        for (const auto& dir : directions) {
            int nx = curr.first + dir.first;
            int ny = curr.second + dir.second;

            if (nx >= 0 && nx < n && ny >= 0 && ny < n &&
                !visitedMap[nx][ny] &&
                isPassable(maze[nx][ny].type)) {

                visitedMap[nx][ny] = true;
                parent[nx][ny] = curr;
                q.push({ nx, ny });
            }
        }
    }

    if (!found) {
        cout << "无法到达目标位置 (" << target.first << ", " << target.second << ")" << endl;
        return false;
    }

    // 重建路径（修复路径重建逻辑）
    vector<pair<int, int>> path;
    pair<int, int> curr = target;
    while (curr != playerPos) {
        path.push_back(curr);
        curr = parent[curr.first][curr.second];
    }
    reverse(path.begin(), path.end());

    // 沿路径移动
    for (const auto& pos : path) {
        playerPos = pos;
        steps++;

        // 检查是否到达出口
        if (maze[playerPos.first][playerPos.second].type == 'E') {
            cout << "恭喜！你已到达出口！" << endl;
            return true;
        }

        // 处理当前位置的单元格
        char cellType = maze[playerPos.first][playerPos.second].type;
        if (cellType == 'G' || cellType == 'T') {
            int value = getResourceValue(cellType);
            totalScore += value;

            if (cellType == 'T') {
                cout << "触发陷阱，损失 " << (-value) << " 分。";
            }
            else {
                cout << "收集资源 '" << cellType << "' 获得 " << value << " 分。";
            }
            maze[playerPos.first][playerPos.second].type = ' ';
        }
        else if (cellType == 'L') {
            bool unlocked = tryUnlockLocker(maze, playerPos, totalScore);
            if (unlocked) {
                cout << "机关已解锁，变为通路。";
            }
        }
        else if (cellType == 'B') {
            // 触发boss战（使用通用函数）
            triggerBossFight(maze, playerPos, totalScore);
        }

        cout << " 当前位置: (" << playerPos.first << ", " << playerPos.second << ")"
            << " 总得分: " << totalScore
            << endl;

        // 标记为已访问
        string posKey = to_string(playerPos.first) + "," + to_string(playerPos.second);
        visited.insert(posKey);

        // 每次移动后检查视野内是否有新资源
        auto newResources = getVisibleResources(maze, playerPos, 1);
        if (!newResources.empty()) {
            cout << "发现新资源，中断当前路径！" << endl;
            return true; // 中断当前路径但不终止游戏
        }
    }

    return true;
}

// 主函数（添加深度搜索模式）
void greedyResourceCollection(
    vector<vector<MazeCell>>& maze,
    pair<int, int> startPos,
    pair<int, int> exitPos,
    int visionRadius) {

    pair<int, int> playerPos = startPos;
    int totalScore = 0;
    int steps = 0;
    const int MAX_STEPS = 1000;

    unordered_set<string> visited;
    visited.insert(to_string(playerPos.first) + "," + to_string(playerPos.second));

    cout << "开始资源收集，起始位置: ("
        << playerPos.first << ", " << playerPos.second << ")"
        << endl;

    while (steps < MAX_STEPS) {
        if (playerPos == exitPos) {
            cout << "恭喜！你已到达出口！" << endl;
            break;
        }

        // 获取视野范围内的资源（固定3x3）
        auto visibleResources = getVisibleResources(maze, playerPos, 1);

        if (!visibleResources.empty()) {
            vector<pair<pair<int, int>, double>> resourceValues;
            for (const auto& res : visibleResources) {
                pair<int, int> pos = res.first;
                char type = res.second;
                int value = getResourceValue(type);
                int distance = manhattanDistance(playerPos, pos);

                double valuePerDistance = (distance > 0) ?
                    static_cast<double>(value) / distance : value;

                resourceValues.push_back({ pos, valuePerDistance });
            }

            sort(resourceValues.begin(), resourceValues.end(),
                [](const auto& a, const auto& b) {
                    return a.second > b.second;
                });

            pair<int, int> targetPos = resourceValues[0].first;
            char targetType = maze[targetPos.first][targetPos.second].type;

            cout << "步骤 " << steps + 1 << ": 选择目标 ("
                << targetPos.first << ", " << targetPos.second
                << ") 类型 '" << targetType
                << "' 性价比: " << resourceValues[0].second << endl;

            // 移动到目标位置
            if (!moveToPosition(maze, playerPos, targetPos, totalScore, steps, visited)) {
                cout << "移动失败，尝试寻找其他路径..." << endl;

                // 如果移动失败，尝试寻找出口
                auto path = findPathDFS(maze, playerPos, exitPos);
                if (!path.empty()) {
                    cout << "找到替代路径，转向出口..." << endl;
                    if (!moveToPosition(maze, playerPos, exitPos, totalScore, steps, visited)) {
                        break;
                    }
                }
                else {
                    cout << "无可行路径！" << endl;
                    break;
                }
            }
        }
        // 深度搜索模式（视野内无资源）
        else {
            cout << "视野内无资源，转入深度搜索模式..." << endl;

            // 使用DFS寻找出口路径
            vector<pair<int, int>> path = findPathDFS(maze, playerPos, exitPos);

            if (path.empty()) {
                cout << "无法找到通往出口的路径！" << endl;
                break;
            }

            cout << "找到深度搜索路径，共" << path.size() << "步。" << endl;

            // 沿路径移动
            for (const auto& nextPos : path) {
                // 移动到下一个位置
                playerPos = nextPos;
                steps++;

                // 检查是否到达出口
                if (playerPos == exitPos) {
                    cout << "恭喜！你已到达出口！" << endl;
                    break;
                }

                // 处理当前位置的单元格
                char cellType = maze[playerPos.first][playerPos.second].type;
                if (cellType == 'G' || cellType == 'T') {
                    int value = getResourceValue(cellType);
                    totalScore += value;

                    if (cellType == 'T') {
                        cout << "触发陷阱，损失 " << (-value) << " 分。";
                    }
                    else {
                        cout << "收集资源 '" << cellType << "' 获得 " << value << " 分。";
                    }
                    maze[playerPos.first][playerPos.second].type = ' ';
                }
                else if (cellType == 'L') {
                    bool unlocked = tryUnlockLocker(maze, playerPos, totalScore);
                    if (unlocked) {
                        cout << "机关已解锁，变为通路。";
                    }
                }
                else if (cellType == 'B') {
                    // 触发boss战（使用通用函数）
                    triggerBossFight(maze, playerPos, totalScore);
                }

                cout << " 当前位置: (" << playerPos.first << ", " << playerPos.second << ")"
                    << " 总得分: " << totalScore
                    << endl;

                // 标记为已访问
                string posKey = to_string(playerPos.first) + "," + to_string(playerPos.second);
                visited.insert(posKey);

                // 检查视野内是否出现新资源
                auto newResources = getVisibleResources(maze, playerPos, 1);
                if (!newResources.empty()) {
                    cout << "发现新资源，中断深度搜索！" << endl;
                    break; // 中断深度搜索，返回主循环
                }
            }
        }

        // 检查移动后是否到达出口
        if (playerPos == exitPos) {
            break;
        }
    }

    cout << "游戏结束。总步数: " << steps
        << " 总得分: " << totalScore
        << endl;
}

int getResourceValue(char cellType) {
    int value;
    if (cellType == 'G')
    {
        value = 50;
    }
    else if (cellType == 'T')
    {
        value = -30;
    }
    else value = 0;
    return value;
}

int manhattanDistance(const pair<int, int>& a, const pair<int, int>& b) {
    return abs(a.first - b.first) + abs(a.second - b.second);
}

bool isPassable(char c) {
    // 墙不可通行
    if (c == '#') {
        return false;
    }
    // 其他类型都可通行（空地、金币、陷阱、boss、机关等）
    return true;
}