#include "GreedyResourcePicker.h"


// ����Bossս��ͨ�ú�����������
void triggerBossFight(
    vector<vector<MazeCell>>& maze,
    pair<int, int>& playerPos,
    int& totalScore) {

    cout << "����bossս" << endl;
    ifstream ifs("boss_case.json");
    if (!ifs) {
        cerr << "�޷���boss_case.json�ļ�" << endl;
        return;
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
    int turnsUsed = result.first; // ��ȡʵ��ʹ�õĻغ���
    // �Զ����ӻ���������ս������
    fightBossVisualAuto(bossHps, skills, result.second);
    totalScore -= turnsUsed; // ÿ���غϿ�1��
    cout << "Bossս���� " << turnsUsed << " �غϣ��۳� " << turnsUsed << " �֣�" << endl;
    maze[playerPos.first][playerPos.second].type = ' '; // boss�����ܺ��Ϊͨ·
    cout << "�ɹ�����boss����ǰλ��: (" << playerPos.first << ", " << playerPos.second << ")"
        << " �ܵ÷�: " << totalScore << endl;
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

        // ���Բ��ܿ������·��
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
                // ===== ������� =====
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
        else if (cellType == 'B') {
            triggerBossFight(maze, playerPos, totalScore);
        }

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
    pair<int, int> exitPos)
{
    pair<int, int> playerPos = startPos;
    int totalScore = 0;
    int steps = 0;
    const int MAX_STEPS = 1000;

    // ��¼����·��
    vector<pair<int, int>> fullPath;
    fullPath.push_back(playerPos);  // ������λ��

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

        // �޸ģ����˵����壬ֻ���ǽ�Һ�Boss
        vector<pair<pair<int, int>, char>> filteredResources;
        for (const auto& res : visibleResources) {
            if (res.second != 'T' && res.second != 'L') { // �ų�����ͻ���
                filteredResources.push_back(res);
            }
        }

        // �޸ģ�ʹ�ù��˺����Դ�б�
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

            // �ƶ���Ŀ��λ�ã�����fullPath������
            if (!moveToPosition(maze, playerPos, targetPos, totalScore, steps, visited, fullPath)) {
                cout << "�ƶ�ʧ�ܣ�����Ѱ������·��..." << endl;

                // ����ƶ�ʧ�ܣ�����Ѱ�ҳ���
                // �ȳ��Աܿ������·��
                auto path = findPathDFS(maze, playerPos, exitPos, true);
                if (path.empty()) {
                    cout << "�޷��ܿ����壬���Դ�Խ����·��..." << endl;
                    path = findPathDFS(maze, playerPos, exitPos, false);
                }

                if (!path.empty()) {
                    cout << "�ҵ����·����ת�����..." << endl;
                    // ����fullPath����
                    if (!moveToPosition(maze, playerPos, exitPos, totalScore, steps, visited, fullPath)) {
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
            cout << "��Ұ������Դ��ת���������ģʽ..." << endl;

            // ʹ��DFS̽������·����������·��
            bool newResourceFound = false;
            exploreDFS(maze, playerPos, exitPos, totalScore, steps, visited, fullPath, newResourceFound);

            // ����Ƿ񵽴����
            if (playerPos == exitPos) {
                break;
            }

            // �����������Դ��������Щ��Դ
            if (newResourceFound) {
                // ��ȡ��Ұ��Χ�ڵ���Դ
                auto visibleResources = getVisibleResources(maze, playerPos, 1);

                // ���˵����壬ֻ���ǽ�Һ�Boss
                vector<pair<pair<int, int>, char>> filteredResources;
                for (const auto& res : visibleResources) {
                    if (res.second != 'T' && res.second != 'L') { // �ų�����ͻ���
                        filteredResources.push_back(res);
                    }
                }

                // �������Ч��Դ����������
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

                    // �ƶ���Ŀ��λ�ã�����fullPath������
                    if (!moveToPosition(maze, playerPos, targetPos, totalScore, steps, visited, fullPath)) {
                        cout << "�ƶ�ʧ�ܣ������������..." << endl;
                        // ����ƶ�ʧ�ܣ������������
                        continue;
                    }
                }
            }
            // === ��������δ��������Դʱ����ǰ������ ===
            else {
                cout << "DFS̽��δ��������Դ������ǰ������..." << endl;

                // �ȳ��Աܿ������·��
                auto path = findPathDFS(maze, playerPos, exitPos, true);
                if (path.empty()) {
                    cout << "�޷��ܿ����壬���Դ�Խ����·��..." << endl;
                    path = findPathDFS(maze, playerPos, exitPos, false);
                }

                if (!path.empty()) {
                    cout << "�ҵ�ͨ�����ڵ�·����ת�����..." << endl;
                    // ����fullPath����
                    if (!moveToPosition(maze, playerPos, exitPos, totalScore, steps, visited, fullPath)) {
                        // �ƶ�ʧ�ܣ���������
                        cout << "�ƶ�ʧ�ܣ�������������·��..." << endl;
                    }
                }
                else {
                    cout << "�޿���·����" << endl;
                    break;
                }
            }
        }

        // ����ƶ����Ƿ񵽴����
        if (playerPos == exitPos) {
            break;
        }
    }



    cout << "��Ϸ�������ܲ���: " << steps
        << " �ܵ÷�: " << totalScore
        << endl;

    // ��ӡ��·����ǵ��Թ�
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

    // ��ʼ������Դ��־
    newResourceFound = false;

    while (!s.empty()) {
        auto curr = s.top();
        s.pop();

        // �ƶ�����ǰλ��
        if (curr != playerPos) {
            playerPos = curr;
            steps++;
            fullPath.push_back(playerPos);

            // ���Ϊ�ѷ��ʣ�ȫ�֣�
            string posKey = to_string(playerPos.first) + "," + to_string(playerPos.second);
            visited.insert(posKey);

            // === �ؼ��޸ģ�ֻ�����Դ�����ռ� ===
            char cellType = maze[playerPos.first][playerPos.second].type;
            if (cellType == 'G' || cellType == 'B') {
                // �����ñ�־����ʵ���ռ���Դ
                newResourceFound = true;
                cout << "������Դ '" << cellType << "' ��λ�� ("
                    << playerPos.first << ", " << playerPos.second << ")"
                    << endl;
            }
            // === �������������� ===
            else if (cellType == 'T') {
                int value = getResourceValue(cellType);
                totalScore += value;
                cout << "�������壬��ʧ " << (-value) << " �֡�";
                maze[playerPos.first][playerPos.second].type = ' '; // ���崥�����Ƴ�
                cout << " ��ǰλ��: (" << playerPos.first << ", " << playerPos.second << ")"
                    << " �ܵ÷�: " << totalScore
                    << endl;
            }
            else if (cellType == 'L') {
                // ֻ����û������·��ʱ�ų��Խ���
                bool unlocked = tryUnlockLocker(maze, playerPos, totalScore);
                if (unlocked) {
                    cout << "�����ѽ�������Ϊͨ·��";
                    maze[playerPos.first][playerPos.second].type = ' ';
                }
                else {
                    // ����ʧ��ʱ���Ƴ�DFS̽�����
                    explored[playerPos.first][playerPos.second] = false;
                    visitedInDFS[playerPos.first][playerPos.second] = false;
                }
            }
        }

        // ����������
        if (playerPos == exitPos) {
            cout << "DFS̽���з��ֳ���λ��" << endl;
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
            cout << "������Ұ����Դ���ж����������" << endl;
            newResourceFound = true;
            return;
        }

        // ̽������δ���ʵ�����λ��
        bool foundUnvisited = false;
        vector<pair<int, int>> shuffled = directions;
        random_shuffle(shuffled.begin(), shuffled.end());

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
                }
            }
        }

        // === �޸ģ����û�з�����·�������ȿ��ǻ��� ===
        if (!foundUnvisited) {
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
                        cout << "�ް�ȫ·��������ѡ�����λ�� (" << nx << ", " << ny << ")" << endl;
                    }
                }
            }

            // ���û���ҵ����أ��ٿ�������
            if (!foundUnvisited) {
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
                            cout << "�ް�ȫ·����ѡ������λ�� (" << nx << ", " << ny << ")" << endl;
                        }
                    }
                }
            }
        }

        // �����߼�
        if (!foundUnvisited && !s.empty()) {
            playerPos = s.top();
            steps++;
            fullPath.push_back(playerPos);
            cout << "���ݵ�λ��: (" << playerPos.first << ", " << playerPos.second << ")" << endl;
        }
    }

    //// ��ʱ����
    //if (steps > 1000) {
    //    cout << "DFS̽����ʱ��ǿ���˳�" << endl;
    //}
}