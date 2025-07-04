
#include "GreedyResourcePicker.h"




// �޸ĺ��Bossս������������ս�������֧���ӳٷ�������
bool triggerBossFight(
    vector<vector<MazeCell>>& maze,
    pair<int, int>& playerPos,
    int& totalScore,
    bool delayScoreUpdate = false)  // �������������Ƿ��ӳٸ��·���
{
    // ����ԭʼ״̬
    const int originalScore = totalScore;
    const char originalCellType = maze[playerPos.first][playerPos.second].type;

    cout << "����bossս" << endl;
    ifstream ifs("boss_case.json");
    if (!ifs) {
        cerr << "�޷���boss_case.json�ļ�" << endl;
        return false;
    }

    nlohmann::json j;
    ifs >> j;

    // ����BossѪ��
    vector<int> bossHps = j["B"].get<vector<int>>();

    // ��������
    std::vector<Skill> skills;
    for (const auto& arr : j["PlayerSkills"]) {
        int damage = arr[0];
        int cooldown = arr[1];
        skills.emplace_back(damage, cooldown);
    }

    // �������ż����ͷ�˳��
    BossFightStrategy bfs;
    auto result = bfs.minTurnSkillSequence(bossHps, skills);
    int turnsUsed = result.first;

    // �Զ����ӻ���������ս������
    fightBossVisualAuto(bossHps, skills, result.second);

    // ���ݲ���������������ʱ��
    if (!delayScoreUpdate) {
        totalScore -= turnsUsed;
        maze[playerPos.first][playerPos.second].type = ' ';
    }

    // ���ս������
    cout << "Bossս���� " << turnsUsed << " �غϣ��۳� " << turnsUsed << " �֣�" << endl;
    cout << "�ɹ�����boss����ǰλ��: (" << playerPos.first << ", " << playerPos.second << ")" << endl;

    // ����ս������ͷ����仯
    return true;
}

// ����������������ӳٵķ�������
void finalizeBossFight(
    vector<vector<MazeCell>>& maze,
    pair<int, int> bossPos,
    int& totalScore,
    int turnsUsed)
{
    totalScore -= turnsUsed;
    maze[bossPos.first][bossPos.second].type = ' ';
    cout << "Bossս�����Ӧ�ã��۳� " << turnsUsed << " �֣���ǰ�ܷ�: " << totalScore << endl;
}

// ��Ұ�������̶�3x3��Ұ��
vector<pair<pair<int, int>, char>> getVisibleResources(
    const vector<vector<MazeCell>>& maze,
    const pair<int, int>& playerPos,
    int visionRadius) {

    vector<pair<pair<int, int>, char>> visibleResources;
    int n = maze.size();
    int x = playerPos.first;
    int y = playerPos.second;

    // �̶�3x3��Ұ��Χ
    for (int i = max(0, x - 1); i <= min(n - 1, x + 1); i++) {
        for (int j = max(0, y - 1); j <= min(n - 1, y + 1); j++) {
            // �����������λ��
            if (i == x && j == y) continue;

            char cellType = maze[i][j].type;
            // ������ҡ������boss
            if (cellType == 'G' || cellType == 'T' || cellType == 'B') {
                visibleResources.push_back({ {i, j}, cellType });
            }
        }
    }
    return visibleResources;
}



// �����������Ѱ�ҳ���·��
vector<pair<int, int>> findPathDFS(
    const vector<vector<MazeCell>>& maze,
    const pair<int, int>& start,
    const pair<int, int>& end,
    bool avoidTraps)  // ��Ӳ��������Ƿ�ܿ�����
{
    int n = maze.size();
    vector<vector<bool>> visited(n, vector<bool>(n, false));
    vector<vector<pair<int, int>>> parent(n, vector<pair<int, int>>(n, { -1, -1 }));
    stack<pair<int, int>> s;

    // ��������
    vector<pair<int, int>> directions = { {-1,0}, {1,0}, {0,-1}, {0,1} };

    visited[start.first][start.second] = true;
    s.push(start);

    bool found = false;

    while (!s.empty()) {
        auto curr = s.top();
        s.pop();

        // ��������յ�
        if (curr == end) {
            found = true;
            break;
        }

        // �������̽����ģ��������ȣ�
        vector<pair<int, int>> shuffled = directions;
        random_shuffle(shuffled.begin(), shuffled.end());

        for (const auto& dir : shuffled) {
            int nx = curr.first + dir.first;
            int ny = curr.second + dir.second;

            // ���߽���Ƿ��ͨ��
            if (nx >= 0 && nx < n && ny >= 0 && ny < n &&
                !visited[nx][ny]) {

                char cellType = maze[nx][ny].type;

                // ���ݲ��������Ƿ�ܿ�����ͻ���
                if (avoidTraps) {
                    if (cellType == 'T' || cellType == 'L') {
                        // �������յ㣬����ܿ�
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

    // �ؽ�·��
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

// �ƶ���������ӻ��ؿ۷֣�
bool moveToPosition(
    vector<vector<MazeCell>>& maze,
    pair<int, int>& playerPos,
    const pair<int, int>& target,
    int& totalScore,
    int& steps,
    unordered_set<string>& visited,
    vector<pair<int, int>>& fullPath,
    vector<pair<int, int>>& bossSteps) {

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
                // �ܿ���Ŀ��λ�õ�����ͻ���
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
        cout << "�޷�����Ŀ��λ�� (" << target.first << ", " << target.second << ")" << endl;

        // ���ȳ��Դ������ص�·�����ܿ����壩
        cout << "���ȳ��Դ������ص�·����..." << endl;
        vector<pair<int, int>> lockPath;

        // ʹ���Զ����߼����ɱܿ����嵫������ص�·��
        vector<pair<int, int>> directions = { {-1,0}, {1,0}, {0,-1}, {0,1} };
        vector<vector<bool>> visitedDFS(n, vector<bool>(n, false));
        vector<vector<pair<int, int>>> parentDFS(n, vector<pair<int, int>>(n, { -1, -1 }));
        stack<pair<int, int>> s;

        visitedDFS[playerPos.first][playerPos.second] = true;
        s.push(playerPos);

        bool foundLockPath = false;

        while (!s.empty()) {
            auto curr = s.top();
            s.pop();

            if (curr == target) {
                foundLockPath = true;
                break;
            }

            vector<pair<int, int>> shuffled = directions;
            random_shuffle(shuffled.begin(), shuffled.end());

            for (const auto& dir : shuffled) {
                int nx = curr.first + dir.first;
                int ny = curr.second + dir.second;

                if (nx >= 0 && nx < n && ny >= 0 && ny < n &&
                    !visitedDFS[nx][ny] &&
                    isPassable(maze[nx][ny].type)) {

                    // �ܿ����壨������Ŀ��λ�ã�
                    if (maze[nx][ny].type == 'T' &&
                        (nx != target.first || ny != target.second)) {
                        continue;
                    }

                    // ����ͨ������
                    visitedDFS[nx][ny] = true;
                    parentDFS[nx][ny] = curr;
                    s.push({ nx, ny });
                }
            }
        }

        // �ؽ�����·��
        if (foundLockPath) {
            pair<int, int> curr = target;
            while (curr != playerPos) {
                lockPath.push_back(curr);
                curr = parentDFS[curr.first][curr.second];
            }
            reverse(lockPath.begin(), lockPath.end());
        }

        if (!lockPath.empty()) {
            cout << "�ҵ��������ص�·������" << lockPath.size() << "��" << endl;

            // ��·���ƶ�
            for (const auto& nextPos : lockPath) {
                playerPos = nextPos;
                steps++;
                fullPath.push_back(playerPos);

                // ����ǰλ�õĵ�Ԫ��
                char cellType = maze[playerPos.first][playerPos.second].type;

                if (cellType == 'G' || cellType == 'T') {
                    int value = getResourceValue(cellType);
                    totalScore += value;

                    if (cellType == 'T') {
                        cout << "�������壬��ʧ " << (-value) << " �֡�";
                    }
                    else {
                        cout << "�ռ���Դ '" << cellType << "' ��� " << value << " �֡�";
                    }
                    maze[playerPos.first][playerPos.second].type = ' ';
                }
                // ===== ��ӻ��ش��� =====
                else if (cellType == 'L') {
                    bool unlocked = tryUnlockLocker(maze, playerPos, totalScore);
                    if (unlocked) {
                        cout << "�����ѽ�������Ϊͨ·��";
                    }
                }
                else if (cellType == 'B') {
                    // ����¼Bossλ�ã�������ս��
                    string posKey = to_string(playerPos.first) + "," + to_string(playerPos.second);
                    if (visited.find(posKey) == visited.end()) {
                        bossSteps.push_back(playerPos);
                        visited.insert(posKey);
                        cout << "����Bossλ��(" << playerPos.first << "," << playerPos.second
                            << ")�����ڿ��ӻ�ʱ����ս��" << endl;
                    }
                }
                // ===== ������� =====
                // ===== ��������Ұ����Դ�����ռ� =====
                auto visibleResources = getVisibleResources(maze, playerPos, 1);
                for (const auto& res : visibleResources) {
                    pair<int, int> resPos = res.first;
                    char resType = res.second;
                    string posKey = to_string(resPos.first) + "," + to_string(resPos.second);

                    // �����ѷ��ʹ�����Դ
                    if (visited.find(posKey) != visited.end()) continue;

                    // �ռ����
                    if (resType == 'G') {
                        int value = getResourceValue(resType);
                        totalScore += value;
                        maze[resPos.first][resPos.second].type = ' '; // �Ƴ����
                        visited.insert(posKey); // ���Ϊ�ѷ���

                        cout << "���ֲ��ռ���Ұ�ڽ��("
                            << resPos.first << "," << resPos.second
                            << ") ���" << value << "�֣�";
                    }
                    // ���Bossλ�ã�������������
                    else if (resType == 'B') {
                        cout << "����Bossλ��("
                            << resPos.first << "," << resPos.second << ")!";
                    }
                }
                // ===== �������� =====
                // ... (�������ʹ�����)

                cout << " ��ǰλ��: (" << playerPos.first << ", " << playerPos.second << ")"
                    << " �ܵ÷�: " << totalScore
                    << endl;

                // ����Ƿ񵽴�Ŀ��
                if (playerPos == target) {
                    return true;
                }
            }
            return true;
        }

        // ����������ص�·�������У��ٳ��Դ��������·��
        cout << "���Դ�Խ����·��..." << endl;
        auto trapPath = findPathDFS(maze, playerPos, target, false);

        if (!trapPath.empty()) {
            cout << "�ҵ���Խ����·������" << trapPath.size() << "��" << endl;

            // ��·���ƶ�
            for (const auto& nextPos : trapPath) {
                playerPos = nextPos;
                steps++;
                fullPath.push_back(playerPos);

                // ����ǰλ�õĵ�Ԫ��
                char cellType = maze[playerPos.first][playerPos.second].type;

                if (cellType == 'G' || cellType == 'T') {
                    int value = getResourceValue(cellType);
                    totalScore += value;

                    if (cellType == 'T') {
                        cout << "�������壬��ʧ " << (-value) << " �֡�";
                    }
                    else {
                        cout << "�ռ���Դ '" << cellType << "' ��� " << value << " �֡�";
                    }
                    maze[playerPos.first][playerPos.second].type = ' ';
                }
                // ===== ��ӻ��ش��� =====
                else if (cellType == 'L') {
                    bool unlocked = tryUnlockLocker(maze, playerPos, totalScore);
                    if (unlocked) {
                        cout << "�����ѽ�������Ϊͨ·��";
                    }
                }
                else if (cellType == 'B') {
                    // ����¼Bossλ�ã�������ս��
                    string posKey = to_string(playerPos.first) + "," + to_string(playerPos.second);
                    if (visited.find(posKey) == visited.end()) {
                        bossSteps.push_back(playerPos);
                        visited.insert(posKey);
                        cout << "����Bossλ��(" << playerPos.first << "," << playerPos.second
                            << ")�����ڿ��ӻ�ʱ����ս��" << endl;
                    }
                }
                // ===== ������� =====
                // ===== ��������Ұ����Դ�����ռ� =====
                auto visibleResources = getVisibleResources(maze, playerPos, 1);
                for (const auto& res : visibleResources) {
                    pair<int, int> resPos = res.first;
                    char resType = res.second;
                    string posKey = to_string(resPos.first) + "," + to_string(resPos.second);

                    // �����ѷ��ʹ�����Դ
                    if (visited.find(posKey) != visited.end()) continue;

                    // �ռ����
                    if (resType == 'G') {
                        int value = getResourceValue(resType);
                        totalScore += value;
                        maze[resPos.first][resPos.second].type = ' '; // �Ƴ����
                        visited.insert(posKey); // ���Ϊ�ѷ���

                        cout << "���ֲ��ռ���Ұ�ڽ��("
                            << resPos.first << "," << resPos.second
                            << ") ���" << value << "�֣�";
                    }
                    // ���Bossλ�ã�������������
                    else if (resType == 'B') {
                        cout << "����Bossλ��("
                            << resPos.first << "," << resPos.second << ")!";
                    }
                }
                // ===== �������� =====
                // ... (�������ʹ�����)

                cout << " ��ǰλ��: (" << playerPos.first << ", " << playerPos.second << ")"
                    << " �ܵ÷�: " << totalScore
                    << endl;

                // ����Ƿ񵽴�Ŀ��
                if (playerPos == target) {
                    return true;
                }
            }
            return true;
        }

        return false;
    }

    // �ؽ�·��
    vector<pair<int, int>> path;
    pair<int, int> curr = target;
    while (curr != playerPos) {
        path.push_back(curr);
        curr = parent[curr.first][curr.second];
    }
    reverse(path.begin(), path.end());

    // ��ȡĿ��λ�����ͣ������ж��Ƿ��ǳ��ڣ�
    char targetType = maze[target.first][target.second].type;

    // ��·���ƶ�
    for (const auto& pos : path) {
        playerPos = pos;
        steps++;
        fullPath.push_back(playerPos);

        // ����Ƿ񵽴����
        if (maze[playerPos.first][playerPos.second].type == 'E') {
            cout << "��ϲ�����ѵ�����ڣ�" << endl;
            return true;
        }

        // ����ǰλ�õĵ�Ԫ��
        char cellType = maze[playerPos.first][playerPos.second].type;
        if (cellType == 'G' || cellType == 'T') {
            int value = getResourceValue(cellType);
            totalScore += value;

            if (cellType == 'T') {
                cout << "�������壬��ʧ " << (-value) << " �֡�";
            }
            else {
                cout << "�ռ���Դ '" << cellType << "' ��� " << value << " �֡�";
            }
            maze[playerPos.first][playerPos.second].type = ' ';
        }
        // ===== ��ӻ��ش��� =====
        else if (cellType == 'L') {
            bool unlocked = tryUnlockLocker(maze, playerPos, totalScore);
            if (unlocked) {
                cout << "�����ѽ�������Ϊͨ·��";
                maze[playerPos.first][playerPos.second].type = ' ';
            }
            else {
                // ����ʧ��ʱ���Ƴ����ʱ���Ա��ٴγ���
                string posKey = to_string(playerPos.first) + "," + to_string(playerPos.second);
                visited.erase(posKey); // �ؼ��޸��������ٴη��ʸ�λ��
            }

        }
        // ===== ������� =====
      /*  else if (cellType == 'B') {
            triggerBossFight(maze, playerPos, totalScore);
        }*/

        cout << " ��ǰλ��: (" << playerPos.first << ", " << playerPos.second << ")"
            << " �ܵ÷�: " << totalScore
            << endl;

        // ���Ϊ�ѷ���
        string posKey = to_string(playerPos.first) + "," + to_string(playerPos.second);
        visited.insert(posKey);

        // ֻ�е�Ŀ�겻�ǳ���ʱ���ż������Դ
        if (targetType != 'E') {
            auto newResources = getVisibleResources(maze, playerPos, 1);

            // ���˵����壬ֻ���ǽ�Һ�Boss
            vector<pair<pair<int, int>, char>> filteredNewResources;
            for (const auto& res : newResources) {
                if (res.second == 'G' || res.second == 'B') {
                    filteredNewResources.push_back(res);
                }
            }

            if (!filteredNewResources.empty()) {
                cout << "��������Դ���жϵ�ǰ·����" << endl;
                return true;
            }
        }
    }

    return true;
}

// ������������������ģʽ��
void greedyResourceCollection(
    vector<vector<MazeCell>>& maze,
    pair<int, int> startPos,
    pair<int, int> exitPos,
    vector<pair<int, int>>& fullPath,
    vector<pair<int, int>>& bossSteps,
    int& totalScore  // ��ӷ������ò���
) {
    pair<int, int> playerPos = startPos;
    totalScore = 0;
    int steps = 0;
    const int MAX_STEPS = 1000;

    fullPath.clear();
    fullPath.push_back(playerPos);  // ������λ��

    bossSteps.clear();

    unordered_set<string> visited;
    visited.insert(to_string(playerPos.first) + "," + to_string(playerPos.second));

    cout << "��ʼ��Դ�ռ�����ʼλ��: ("
        << playerPos.first << ", " << playerPos.second << ")"
        << endl;

    while (steps < MAX_STEPS) {
        if (playerPos == exitPos) {
            cout << "��ϲ�����ѵ�����ڣ�" << endl;
            break;
        }

        // ��ȡ��Ұ��Χ�ڵ���Դ���̶�3x3��
        auto visibleResources = getVisibleResources(maze, playerPos, 1);

        // ���˵����壬ֻ���ǽ�Һ�Boss
        vector<pair<pair<int, int>, char>> filteredResources;
        for (const auto& res : visibleResources) {
            if (res.second != 'T' && res.second != 'L') { // �ų�����ͻ���
                filteredResources.push_back(res);
            }
        }

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

            cout << "���� " << steps + 1 << ": ѡ��Ŀ�� ("
                << targetPos.first << ", " << targetPos.second
                << ") ���� '" << targetType
                << "' �Լ۱�: " << resourceValues[0].second << endl;

            // �ƶ���Ŀ��λ�ã�ֻ��¼·����Boss�㣩
            bool moveSuccess = moveToPosition(maze, playerPos, targetPos, totalScore, steps, visited, fullPath, bossSteps);            // ��ֱ�Ӵ���Bossս��ֻ��¼Boss��
            if (targetType == 'B') {
                bossSteps.push_back(targetPos);
            }
            if (!moveSuccess) {
                cout << "�ƶ�ʧ�ܣ�����Ѱ������·��..." << endl;

                // ����ƶ�ʧ�ܣ�����Ѱ�ҳ���
                auto path = findPathDFS(maze, playerPos, exitPos, true);
                if (path.empty()) {
                    cout << "�޷��ܿ����壬���Դ�Խ����·��..." << endl;
                    path = findPathDFS(maze, playerPos, exitPos, false);
                }

                if (!path.empty()) {
                    cout << "�ҵ����·����ת�����..." << endl;
                    if (!moveToPosition(maze, playerPos, targetPos, totalScore, steps, visited, fullPath, bossSteps)) {
                        break;
                    }
                }
                else {
                    cout << "�޿���·����" << endl;
                    break;
                }
            }
        }
        // �������ģʽ����Ұ������Դ��
        else {
            cout << "\n===== �����������ģʽ =====" << endl;
            bool newResourceFound = false;
            exploreDFS(maze, playerPos, exitPos, totalScore, steps, visited, fullPath, newResourceFound);
            cout << "===== ����������� =====" << endl;
            // ����Ƿ񵽴����
            if (playerPos == exitPos) {
                break;
            }

            // �����������Դ��������Щ��Դ
            if (newResourceFound) {
                auto visibleResources = getVisibleResources(maze, playerPos, 1);
                vector<pair<pair<int, int>, char>> filteredResources;
                for (const auto& res : visibleResources) {
                    if (res.second != 'T' && res.second != 'L') {
                        filteredResources.push_back(res);
                    }
                }

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

                    cout << "���� " << steps + 1 << ": ѡ��Ŀ�� ("
                        << targetPos.first << ", " << targetPos.second
                        << ") ���� '" << targetType
                        << "' �Լ۱�: " << resourceValues[0].second << endl;

                    bool moveSuccess = moveToPosition(maze, playerPos, targetPos, totalScore, steps, visited, fullPath, bossSteps);
                    if (targetType == 'B') {
                        bossSteps.push_back(targetPos);
                    }
                    if (!moveSuccess) {
                        cout << "�ƶ�ʧ�ܣ������������..." << endl;
                        continue;
                    }
                }
            }
            // === ��������δ��������Դʱ����ǰ������ ===
            else {
                cout << "DFS̽��δ��������Դ������ǰ������..." << endl;
                auto path = findPathDFS(maze, playerPos, exitPos, true);
                if (path.empty()) {
                    cout << "�޷��ܿ����壬���Դ�Խ����·��..." << endl;
                    path = findPathDFS(maze, playerPos, exitPos, false);
                }

                if (!path.empty()) {
                    cout << "�ҵ�ͨ�����ڵ�·����ת�����..." << endl;
                    if (!moveToPosition(maze, playerPos, exitPos, totalScore, steps, visited, fullPath, bossSteps)) {
                        cout << "�ƶ�ʧ�ܣ�������������·��..." << endl;
                    }
                }
                else {
                    cout << "�޿���·����" << endl;
                    break;
                }
            }
        }

        if (steps >= MAX_STEPS) {
            cout << "�ﵽ��������ȴ�Bossս���..." << endl;
            break;
        }
    }
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
    // ǽ����ͨ��
    if (c == '#') {
        return false;
    }
    // �������Ͷ���ͨ�У��յء���ҡ����塢boss�����صȣ�
    return true;
}

bool tryUnlockLocker(vector<vector<MazeCell>>& maze,
    const pair<int, int>& pos,
    int& totalScore) {

    cout << "���ֻ��أ���ʼ����..." << endl;

    // ���Զ�ȡ���������ļ�
    ifstream ifs2("pwd_000.json");
    if (!ifs2) {
        cerr << "�޷������������ļ�" << endl;
        cout << "���ؽ���ʧ�ܣ�" << endl;
        return false;
    }

    nlohmann::json j2;
    ifs2 >> j2;
    std::vector<std::vector<int>> clues = j2["C"].get<std::vector<std::vector<int>>>();
    std::string hashL = j2["L"].get<std::string>();

    // ��������
    PasswordResult pwd_result = solve_password(clues, hashL);

    if (!pwd_result.password.empty()) {
        std::cout << "\n������ϵͳ���Զ�������֤����ɹ���" << std::endl;
        // �۷֣�ÿ�γ��Կ�5��
        int penalty = pwd_result.tries;
        std::cout << "��ȷ����Ϊ: " << pwd_result.password << std::endl;
        std::cout << "�����Դ���: " << pwd_result.tries << std::endl;

        penalty -= 1;
        totalScore -= penalty;
        cout << "�۷�: " << penalty << endl;

        // �������Ϊͨ·
        maze[pos.first][pos.second].type = ' ';
        return true;
    }
    else {
        std::cout << "\n������ϵͳ����������ʧ�ܣ����ؽ���ʧ�ܣ�" << std::endl;
        return false;
    }
}



// ���·����Ǻ���
void printMazeWithPath(const vector<vector<MazeCell>>& maze, const vector<pair<int, int>>& fullPath) {
    // �����Թ�����
    vector<vector<char>> mazeCopy(maze.size(), vector<char>(maze[0].size()));
    for (int i = 0; i < maze.size(); ++i) {
        for (int j = 0; j < maze[0].size(); ++j) {
            mazeCopy[i][j] = maze[i][j].type;
        }
    }

    // ���·��
    for (const auto& pos : fullPath) {
        if (mazeCopy[pos.first][pos.second] != 'S' &&
            mazeCopy[pos.first][pos.second] != 'E') {
            mazeCopy[pos.first][pos.second] = '*';
        }
    }

    // ��ӡ��·����ǵ��Թ�
    cout << "·�����ͼ (* ��ʾ·��):" << endl;
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

    // ��������
    vector<pair<int, int>> directions = { {-1,0}, {1,0}, {0,-1}, {0,1} };

    s.push(playerPos);
    explored[playerPos.first][playerPos.second] = true;
    visitedInDFS[playerPos.first][playerPos.second] = true;

    // �����ʼ��Ϣ
    cout << "��DFS��ʼ����λ��(" << playerPos.first << ", " << playerPos.second << ")\n";

    // ��ʼ������Դ��־
    newResourceFound = false;

    while (!s.empty()) {
        auto curr = s.top();
        s.pop();

        // �����ǰλ����Ϣ
        cout << "��DFSλ�á���ǰ: (" << curr.first << ", " << curr.second << ")";
        if (curr != playerPos) {
            cout << " (����)" << endl;
        }
        else {
            cout << " (ǰ��)" << endl;
        }

        // �ƶ�����ǰλ��
        if (curr != playerPos) {
            playerPos = curr;
            steps++;
            fullPath.push_back(playerPos);

            // ���Ϊ�ѷ��ʣ�ȫ�֣�
            string posKey = to_string(playerPos.first) + "," + to_string(playerPos.second);
            visited.insert(posKey);

            // ����ƶ���Ϣ
            cout << "  -> �ƶ���(" << playerPos.first << ", " << playerPos.second << ")";
            cout << " ����: " << steps << endl;

            // === �ؼ��޸ģ�ֻ�����Դ�����ռ� ===
            char cellType = maze[playerPos.first][playerPos.second].type;
            if (cellType == 'G' || cellType == 'B') {
                // �����ñ�־����ʵ���ռ���Դ
                newResourceFound = true;
                cout << "  ������Դ '" << cellType << "'" << endl;
            }
            // === �������������� ===
            else if (cellType == 'T') {
                int value = getResourceValue(cellType);
                totalScore += value;
                cout << "  �������壬��ʧ " << (-value) << " �֡�" << endl;
                maze[playerPos.first][playerPos.second].type = ' '; // ���崥�����Ƴ�
            }
            else if (cellType == 'L') {
                // ֻ����û������·��ʱ�ų��Խ���
                cout << "  ���ֻ��أ����Խ���..." << endl;
                bool unlocked = tryUnlockLocker(maze, playerPos, totalScore);
                if (unlocked) {
                    cout << "  ���ؽ����ɹ���" << endl;
                    maze[playerPos.first][playerPos.second].type = ' ';
                }
                else {
                    // ����ʧ��ʱ���Ƴ�DFS̽�����
                    explored[playerPos.first][playerPos.second] = false;
                    visitedInDFS[playerPos.first][playerPos.second] = false;
                    cout << "  ���ؽ���ʧ�ܣ�" << endl;
                }
            }
        }

        // ����������
        if (playerPos == exitPos) {
            cout << "��DFS���������ֳ���λ��(" << playerPos.first << ", " << playerPos.second << ")" << endl;
            return;
        }

        // �����Ұ���Ƿ�������Դ����һ�Boss��
        auto currentResources = getVisibleResources(maze, playerPos, 1);
        vector<pair<pair<int, int>, char>> filteredResources;
        for (const auto& res : currentResources) {
            if (res.second == 'G' || res.second == 'B') {
                filteredResources.push_back(res);
            }
        }
        if (!filteredResources.empty()) {
            cout << "��DFS�жϡ�������Ұ����Դ!" << endl;
            newResourceFound = true;
            return;
        }

        // ̽������δ���ʵ�����λ��
        bool foundUnvisited = false;
        vector<pair<int, int>> shuffled = directions;
        random_shuffle(shuffled.begin(), shuffled.end());

        // ����ھ�̽����Ϣ
        cout << "  ̽���ھ�: ";

        // ���ȳ��Է�����·��
        for (const auto& dir : shuffled) {
            int nx = playerPos.first + dir.first;
            int ny = playerPos.second + dir.second;

            if (nx >= 0 && nx < n && ny >= 0 && ny < n &&
                !explored[nx][ny] &&
                isPassable(maze[nx][ny].type)) {

                // �ܿ�����ͻ���
                if (maze[nx][ny].type == 'T' || maze[nx][ny].type == 'L')
                    continue;

                if (!visitedInDFS[nx][ny]) {
                    parent[nx][ny] = playerPos;
                    explored[nx][ny] = true;
                    visitedInDFS[nx][ny] = true;
                    s.push({ nx, ny });
                    foundUnvisited = true;
                    cout << "(" << nx << "," << ny << ":" << maze[nx][ny].type << ") "; // ����ھ���Ϣ
                }
            }
        }

        // === �޸ģ����û�з�����·�������ȿ��ǻ��� ===
        if (!foundUnvisited) {
            cout << "\n  �ް�ȫ·�������Ի���... ";
            // ���ȿ��ǻ���λ��
            for (const auto& dir : shuffled) {
                int nx = playerPos.first + dir.first;
                int ny = playerPos.second + dir.second;

                if (nx >= 0 && nx < n && ny >= 0 && ny < n &&
                    !explored[nx][ny] &&
                    isPassable(maze[nx][ny].type) &&
                    maze[nx][ny].type == 'L') { // ֻ���ǻ���

                    if (!visitedInDFS[nx][ny]) {
                        parent[nx][ny] = playerPos;
                        explored[nx][ny] = true;
                        visitedInDFS[nx][ny] = true;
                        s.push({ nx, ny });
                        foundUnvisited = true;
                        cout << "(" << nx << "," << ny << ":" << maze[nx][ny].type << ") ";
                    }
                }
            }

            // ���û���ҵ����أ��ٿ�������
            if (!foundUnvisited) {
                cout << "\n  �޻���·������������... ";
                for (const auto& dir : shuffled) {
                    int nx = playerPos.first + dir.first;
                    int ny = playerPos.second + dir.second;

                    if (nx >= 0 && nx < n && ny >= 0 && ny < n &&
                        !explored[nx][ny] &&
                        isPassable(maze[nx][ny].type) &&
                        maze[nx][ny].type == 'T') { // ֻ��������

                        if (!visitedInDFS[nx][ny]) {
                            parent[nx][ny] = playerPos;
                            explored[nx][ny] = true;
                            visitedInDFS[nx][ny] = true;
                            s.push({ nx, ny });
                            foundUnvisited = true;
                            cout << "(" << nx << "," << ny << ":" << maze[nx][ny].type << ") ";
                        }
                    }
                }
            }
        }
        cout << endl; // �����ھ����

        // �����߼� - ��ϸ���ÿһ��
        if (!foundUnvisited && !s.empty()) {
            auto next = s.top();

            // ���������Ϣ
            cout << "�����ݿ�ʼ����(" << playerPos.first << "," << playerPos.second
                << ") �� (" << next.first << "," << next.second << ")" << endl;

            // �������·��
            vector<pair<int, int>> backtrackPath;
            pair<int, int> current = playerPos;

            // �ؽ��ӵ�ǰλ�õ�����Ŀ��λ�õ�·��
            while (current != next) {
                // ��ȡ��ǰ�ڵ�ĸ��ڵ�
                pair<int, int> parentPos = parent[current.first][current.second];

                // ȷ�����ڵ���Ч
                if (parentPos.first == -1 && parentPos.second == -1) {
                    cout << "  �����桿�Ҳ�����(" << current.first << "," << current.second
                        << ")��(" << next.first << "," << next.second << ")��·��" << endl;
                    break;
                }

                // ��Ӹ��ڵ㵽����·��
                backtrackPath.push_back(parentPos);
                current = parentPos;
            }

            // �ػ���·���ƶ�
            for (const auto& pos : backtrackPath) {
                playerPos = pos;
                steps++;
                fullPath.push_back(playerPos);

                // ���ÿһ������
                cout << "  -> ���ݵ�(" << playerPos.first << "," << playerPos.second << ")";
                cout << " ����: " << steps << endl;

                // ������ݹ����п�������������
                char cellType = maze[playerPos.first][playerPos.second].type;
                if (cellType == 'T') {
                    int value = getResourceValue(cellType);
                    totalScore += value;
                    cout << "  �������壬��ʧ " << (-value) << " �֡�" << endl;
                    maze[playerPos.first][playerPos.second].type = ' ';
                }
            }

            // ����̽��״̬
            explored[playerPos.first][playerPos.second] = true;
            visitedInDFS[playerPos.first][playerPos.second] = true;

            cout << "�����ݽ���������Ŀ��λ��(" << playerPos.first << "," << playerPos.second << ")" << endl;
        }
    }

    cout << "��DFS������ջ�ѿգ��������\n";
}