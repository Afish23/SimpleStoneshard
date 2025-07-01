#include "GreedyResourcePicker.h"


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



// 深度优先搜索寻找出口路径
vector<pair<int, int>> findPathDFS(
    const vector<vector<MazeCell>>& maze,
    const pair<int, int>& start,
    const pair<int, int>& end,
    bool avoidTraps)  // 添加参数控制是否避开陷阱
{
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
                !visited[nx][ny]) {

                char cellType = maze[nx][ny].type;

                // 根据参数决定是否避开陷阱和机关
                if (avoidTraps) {
                    if (cellType == 'T' || cellType == 'L') {
                        // 除非是终点，否则避开
                        if (nx != end.first || ny != end.second) {
                            continue;
                        }
                    }
                }

                if (isPassable(cellType)) {
                    visited[nx][ny] = true;
                    parent[nx][ny] = curr;
                    s.push({ nx, ny });
                }
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
    unordered_set<string>& visited,
    vector<pair<int, int>>& fullPath) {

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
                isPassable(maze[nx][ny].type))
            {
                // 避开非目标位置的陷阱和机关
                if ((maze[nx][ny].type == 'T' || maze[nx][ny].type == 'L') &&
                    (nx != target.first || ny != target.second))
                {
                    continue;
                }

                visitedMap[nx][ny] = true;
                parent[nx][ny] = curr;
                q.push({ nx, ny });
            }
        }
    }

    if (!found) {
        cout << "无法到达目标位置 (" << target.first << ", " << target.second << ")" << endl;

        // 尝试不避开陷阱的路径
        cout << "尝试穿越陷阱路径..." << endl;
        auto trapPath = findPathDFS(maze, playerPos, target, false);

        if (!trapPath.empty()) {
            cout << "找到穿越陷阱路径，共" << trapPath.size() << "步" << endl;

            // 沿路径移动
            for (const auto& nextPos : trapPath) {
                playerPos = nextPos;
                steps++;
                fullPath.push_back(playerPos);

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
                // ===== 添加机关处理 =====
                else if (cellType == 'L') {
                    bool unlocked = tryUnlockLocker(maze, playerPos, totalScore);
                    if (unlocked) {
                        cout << "机关已解锁，变为通路。";
                    }
                }
                // ===== 结束添加 =====
                // ... (其他类型处理不变)

                cout << " 当前位置: (" << playerPos.first << ", " << playerPos.second << ")"
                    << " 总得分: " << totalScore
                    << endl;

                // 检查是否到达目标
                if (playerPos == target) {
                    return true;
                }
            }
            return true;
        }

        return false;
    }

    // 重建路径
    vector<pair<int, int>> path;
    pair<int, int> curr = target;
    while (curr != playerPos) {
        path.push_back(curr);
        curr = parent[curr.first][curr.second];
    }
    reverse(path.begin(), path.end());

    // 获取目标位置类型（用于判断是否是出口）
    char targetType = maze[target.first][target.second].type;

    // 沿路径移动
    for (const auto& pos : path) {
        playerPos = pos;
        steps++;
        fullPath.push_back(playerPos);

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
        // ===== 添加机关处理 =====
        else if (cellType == 'L') {
            bool unlocked = tryUnlockLocker(maze, playerPos, totalScore);
            if (unlocked) {
                cout << "机关已解锁，变为通路。";
                maze[playerPos.first][playerPos.second].type = ' ';
            }
            else {
                // 解谜失败时，移除访问标记以便再次尝试
                string posKey = to_string(playerPos.first) + "," + to_string(playerPos.second);
                visited.erase(posKey); // 关键修复：允许再次访问该位置
            }

        }
        // ===== 结束添加 =====
        else if (cellType == 'B') {
            triggerBossFight(maze, playerPos, totalScore);
        }

        cout << " 当前位置: (" << playerPos.first << ", " << playerPos.second << ")"
            << " 总得分: " << totalScore
            << endl;

        // 标记为已访问
        string posKey = to_string(playerPos.first) + "," + to_string(playerPos.second);
        visited.insert(posKey);

        // 只有当目标不是出口时，才检查新资源
        if (targetType != 'E') {
            auto newResources = getVisibleResources(maze, playerPos, 1);

            // 过滤掉陷阱，只考虑金币和Boss
            vector<pair<pair<int, int>, char>> filteredNewResources;
            for (const auto& res : newResources) {
                if (res.second == 'G' || res.second == 'B') {
                    filteredNewResources.push_back(res);
                }
            }

            if (!filteredNewResources.empty()) {
                cout << "发现新资源，中断当前路径！" << endl;
                return true;
            }
        }
    }

    return true;
}

// 主函数（添加深度搜索模式）
void greedyResourceCollection(
    vector<vector<MazeCell>>& maze,
    pair<int, int> startPos,
    pair<int, int> exitPos)
{
    pair<int, int> playerPos = startPos;
    int totalScore = 0;
    int steps = 0;
    const int MAX_STEPS = 1000;

    // 记录完整路径
    vector<pair<int, int>> fullPath;
    fullPath.push_back(playerPos);  // 添加起点位置

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

        // 修改：过滤掉陷阱，只考虑金币和Boss
        vector<pair<pair<int, int>, char>> filteredResources;
        for (const auto& res : visibleResources) {
            if (res.second != 'T' && res.second != 'L') { // 排除陷阱和机关
                filteredResources.push_back(res);
            }
        }

        // 修改：使用过滤后的资源列表
        if (!filteredResources.empty()) {
            vector<pair<pair<int, int>, double>> resourceValues;
            for (const auto& res : filteredResources) {
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

            // 移动到目标位置（传递fullPath参数）
            if (!moveToPosition(maze, playerPos, targetPos, totalScore, steps, visited, fullPath)) {
                cout << "移动失败，尝试寻找其他路径..." << endl;

                // 如果移动失败，尝试寻找出口
                // 先尝试避开陷阱的路径
                auto path = findPathDFS(maze, playerPos, exitPos, true);
                if (path.empty()) {
                    cout << "无法避开陷阱，尝试穿越陷阱路径..." << endl;
                    path = findPathDFS(maze, playerPos, exitPos, false);
                }

                if (!path.empty()) {
                    cout << "找到替代路径，转向出口..." << endl;
                    // 传递fullPath参数
                    if (!moveToPosition(maze, playerPos, exitPos, totalScore, steps, visited, fullPath)) {
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

            // 使用DFS探索所有路径（包括死路）
            bool newResourceFound = false;
            exploreDFS(maze, playerPos, exitPos, totalScore, steps, visited, fullPath, newResourceFound);

            // 检查是否到达出口
            if (playerPos == exitPos) {
                break;
            }

            // 如果发现新资源，处理这些资源
            if (newResourceFound) {
                // 获取视野范围内的资源
                auto visibleResources = getVisibleResources(maze, playerPos, 1);

                // 过滤掉陷阱，只考虑金币和Boss
                vector<pair<pair<int, int>, char>> filteredResources;
                for (const auto& res : visibleResources) {
                    if (res.second != 'T' && res.second != 'L') { // 排除陷阱和机关
                        filteredResources.push_back(res);
                    }
                }

                // 如果有有效资源，处理它们
                if (!filteredResources.empty()) {
                    vector<pair<pair<int, int>, double>> resourceValues;
                    for (const auto& res : filteredResources) {
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

                    // 移动到目标位置（传递fullPath参数）
                    if (!moveToPosition(maze, playerPos, targetPos, totalScore, steps, visited, fullPath)) {
                        cout << "移动失败，继续深度搜索..." << endl;
                        // 如果移动失败，继续深度搜索
                        continue;
                    }
                }
            }
            // === 新增处理：未发现新资源时尝试前往出口 ===
            else {
                cout << "DFS探索未发现新资源，尝试前往出口..." << endl;

                // 先尝试避开陷阱的路径
                auto path = findPathDFS(maze, playerPos, exitPos, true);
                if (path.empty()) {
                    cout << "无法避开陷阱，尝试穿越陷阱路径..." << endl;
                    path = findPathDFS(maze, playerPos, exitPos, false);
                }

                if (!path.empty()) {
                    cout << "找到通往出口的路径，转向出口..." << endl;
                    // 传递fullPath参数
                    if (!moveToPosition(maze, playerPos, exitPos, totalScore, steps, visited, fullPath)) {
                        // 移动失败，继续尝试
                        cout << "移动失败，继续尝试其他路径..." << endl;
                    }
                }
                else {
                    cout << "无可行路径！" << endl;
                    break;
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

    // 打印带路径标记的迷宫
    printMazeWithPath(maze, fullPath);
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

bool tryUnlockLocker(vector<vector<MazeCell>>& maze,
    const pair<int, int>& pos,
    int& totalScore) {

    cout << "发现机关，开始解谜..." << endl;

    // 尝试读取密码线索文件
    ifstream ifs2("pwd_000.json");
    if (!ifs2) {
        cerr << "无法打开密码线索文件" << endl;
        cout << "机关解锁失败！" << endl;
        return false;
    }

    nlohmann::json j2;
    ifs2 >> j2;
    std::vector<std::vector<int>> clues = j2["C"].get<std::vector<std::vector<int>>>();
    std::string hashL = j2["L"].get<std::string>();

    // 解谜密码
    PasswordResult pwd_result = solve_password(clues, hashL);

    if (!pwd_result.password.empty()) {
        std::cout << "\n【谜题系统】自动推理并验证密码成功！" << std::endl;
        // 扣分：每次尝试扣5分
        int penalty = pwd_result.tries;
        std::cout << "正确密码为: " << pwd_result.password << std::endl;
        std::cout << "推理尝试次数: " << pwd_result.tries << std::endl;

        totalScore -= penalty;
        cout << "扣分: " << penalty << endl;

        // 解锁后变为通路
        maze[pos.first][pos.second].type = ' ';
        return true;
    }
    else {
        std::cout << "\n【谜题系统】密码推理失败，机关解锁失败！" << std::endl;
        return false;
    }
}



// 添加路径标记函数
void printMazeWithPath(const vector<vector<MazeCell>>& maze, const vector<pair<int, int>>& fullPath) {
    // 创建迷宫副本
    vector<vector<char>> mazeCopy(maze.size(), vector<char>(maze[0].size()));
    for (int i = 0; i < maze.size(); ++i) {
        for (int j = 0; j < maze[0].size(); ++j) {
            mazeCopy[i][j] = maze[i][j].type;
        }
    }

    // 标记路径
    for (const auto& pos : fullPath) {
        if (mazeCopy[pos.first][pos.second] != 'S' &&
            mazeCopy[pos.first][pos.second] != 'E') {
            mazeCopy[pos.first][pos.second] = '*';
        }
    }

    // 打印带路径标记的迷宫
    cout << "路径标记图 (* 表示路径):" << endl;
    for (int i = 0; i < mazeCopy.size(); ++i) {
        for (int j = 0; j < mazeCopy[0].size(); ++j) {
            cout << mazeCopy[i][j] << ' ';
        }
        cout << endl;
    }
}
void exploreDFS(
    vector<vector<MazeCell>>& maze,
    pair<int, int>& playerPos,
    const pair<int, int>& exitPos,
    int& totalScore,
    int& steps,
    unordered_set<string>& visited,
    vector<pair<int, int>>& fullPath,
    bool& newResourceFound)
{
    int n = maze.size();
    stack<pair<int, int>> s;
    vector<vector<pair<int, int>>> parent(n, vector<pair<int, int>>(n, { -1, -1 }));
    vector<vector<bool>> explored(n, vector<bool>(n, false));
    vector<vector<bool>> visitedInDFS(n, vector<bool>(n, false));

    // 方向数组
    vector<pair<int, int>> directions = { {-1,0}, {1,0}, {0,-1}, {0,1} };

    s.push(playerPos);
    explored[playerPos.first][playerPos.second] = true;
    visitedInDFS[playerPos.first][playerPos.second] = true;

    // 初始化新资源标志
    newResourceFound = false;

    while (!s.empty()) {
        auto curr = s.top();
        s.pop();

        // 移动到当前位置
        if (curr != playerPos) {
            playerPos = curr;
            steps++;
            fullPath.push_back(playerPos);

            // 标记为已访问（全局）
            string posKey = to_string(playerPos.first) + "," + to_string(playerPos.second);
            visited.insert(posKey);

            // === 关键修改：只检测资源但不收集 ===
            char cellType = maze[playerPos.first][playerPos.second].type;
            if (cellType == 'G' || cellType == 'B') {
                // 仅设置标志，不实际收集资源
                newResourceFound = true;
                cout << "发现资源 '" << cellType << "' 在位置 ("
                    << playerPos.first << ", " << playerPos.second << ")"
                    << endl;
            }
            // === 新增：处理陷阱 ===
            else if (cellType == 'T') {
                int value = getResourceValue(cellType);
                totalScore += value;
                cout << "触发陷阱，损失 " << (-value) << " 分。";
                maze[playerPos.first][playerPos.second].type = ' '; // 陷阱触发后移除
                cout << " 当前位置: (" << playerPos.first << ", " << playerPos.second << ")"
                    << " 总得分: " << totalScore
                    << endl;
            }
            else if (cellType == 'L') {
                // 只有在没有其他路径时才尝试解谜
                bool unlocked = tryUnlockLocker(maze, playerPos, totalScore);
                if (unlocked) {
                    cout << "机关已解锁，变为通路。";
                    maze[playerPos.first][playerPos.second].type = ' ';
                }
                else {
                    // 解谜失败时，移除DFS探索标记
                    explored[playerPos.first][playerPos.second] = false;
                    visitedInDFS[playerPos.first][playerPos.second] = false;
                }
            }
        }

        // 如果到达出口
        if (playerPos == exitPos) {
            cout << "DFS探索中发现出口位置" << endl;
            return;
        }

        // 检查视野内是否有新资源（金币或Boss）
        auto currentResources = getVisibleResources(maze, playerPos, 1);
        vector<pair<pair<int, int>, char>> filteredResources;
        for (const auto& res : currentResources) {
            if (res.second == 'G' || res.second == 'B') {
                filteredResources.push_back(res);
            }
        }
        if (!filteredResources.empty()) {
            cout << "发现视野内资源，中断深度搜索！" << endl;
            newResourceFound = true;
            return;
        }

        // 探索所有未访问的相邻位置
        bool foundUnvisited = false;
        vector<pair<int, int>> shuffled = directions;
        random_shuffle(shuffled.begin(), shuffled.end());

        // 首先尝试非陷阱路径
        for (const auto& dir : shuffled) {
            int nx = playerPos.first + dir.first;
            int ny = playerPos.second + dir.second;

            if (nx >= 0 && nx < n && ny >= 0 && ny < n &&
                !explored[nx][ny] &&
                isPassable(maze[nx][ny].type)) {

                // 避开陷阱和机关
                if (maze[nx][ny].type == 'T' || maze[nx][ny].type == 'L')
                    continue;

                if (!visitedInDFS[nx][ny]) {
                    parent[nx][ny] = playerPos;
                    explored[nx][ny] = true;
                    visitedInDFS[nx][ny] = true;
                    s.push({ nx, ny });
                    foundUnvisited = true;
                }
            }
        }

        // === 修改：如果没有非陷阱路径，优先考虑机关 ===
        if (!foundUnvisited) {
            // 优先考虑机关位置
            for (const auto& dir : shuffled) {
                int nx = playerPos.first + dir.first;
                int ny = playerPos.second + dir.second;

                if (nx >= 0 && nx < n && ny >= 0 && ny < n &&
                    !explored[nx][ny] &&
                    isPassable(maze[nx][ny].type) &&
                    maze[nx][ny].type == 'L') { // 只考虑机关

                    if (!visitedInDFS[nx][ny]) {
                        parent[nx][ny] = playerPos;
                        explored[nx][ny] = true;
                        visitedInDFS[nx][ny] = true;
                        s.push({ nx, ny });
                        foundUnvisited = true;
                        cout << "无安全路径，优先选择机关位置 (" << nx << ", " << ny << ")" << endl;
                    }
                }
            }

            // 如果没有找到机关，再考虑陷阱
            if (!foundUnvisited) {
                for (const auto& dir : shuffled) {
                    int nx = playerPos.first + dir.first;
                    int ny = playerPos.second + dir.second;

                    if (nx >= 0 && nx < n && ny >= 0 && ny < n &&
                        !explored[nx][ny] &&
                        isPassable(maze[nx][ny].type) &&
                        maze[nx][ny].type == 'T') { // 只考虑陷阱

                        if (!visitedInDFS[nx][ny]) {
                            parent[nx][ny] = playerPos;
                            explored[nx][ny] = true;
                            visitedInDFS[nx][ny] = true;
                            s.push({ nx, ny });
                            foundUnvisited = true;
                            cout << "无安全路径，选择陷阱位置 (" << nx << ", " << ny << ")" << endl;
                        }
                    }
                }
            }
        }

        // 回溯逻辑
        if (!foundUnvisited && !s.empty()) {
            playerPos = s.top();
            steps++;
            fullPath.push_back(playerPos);
            cout << "回溯到位置: (" << playerPos.first << ", " << playerPos.second << ")" << endl;
        }
    }

    //// 超时保护
    //if (steps > 1000) {
    //    cout << "DFS探索超时，强制退出" << endl;
    //}
}
