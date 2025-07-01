#include "GreedyResourcePicker.h"

// ���������պ��������У�
int solveCombinationLock() {
    // ���ݷ��ƽ���λ������
    // ʵ��ʵ�ֻ᷵�س��Դ���
    return 3; // ʾ��ֵ��ʵ��Ӧ���л᷵����ʵ���Դ���
}

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

    // �Զ����ӻ���������ս������
    fightBossVisualAuto(bossHps, skills, result.second);

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

// ���ؽ����������������ƽ⣩
bool tryUnlockLocker(vector<vector<MazeCell>>& maze,
    const pair<int, int>& pos,
    int& totalScore) {
    cout << "���ֻ��أ���ʼ����..." << endl;

    // �ƽ�������
    int attempts = solveCombinationLock();
    int penalty = attempts; // ÿ�γ��Կ�5��

    totalScore -= penalty;

    cout << "�۷�: " << penalty << endl;

    maze[pos.first][pos.second].type = ' '; // �������Ϊͨ·
    return true;
}

// �����������Ѱ�ҳ���·��
vector<pair<int, int>> findPathDFS(
    const vector<vector<MazeCell>>& maze,
    const pair<int, int>& start,
    const pair<int, int>& end) {

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
                !visited[nx][ny] &&
                isPassable(maze[nx][ny].type)) {

                visited[nx][ny] = true;
                parent[nx][ny] = curr;
                s.push({ nx, ny });
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
        cout << "�޷�����Ŀ��λ�� (" << target.first << ", " << target.second << ")" << endl;
        return false;
    }

    // �ؽ�·�����޸�·���ؽ��߼���
    vector<pair<int, int>> path;
    pair<int, int> curr = target;
    while (curr != playerPos) {
        path.push_back(curr);
        curr = parent[curr.first][curr.second];
    }
    reverse(path.begin(), path.end());

    // ��·���ƶ�
    for (const auto& pos : path) {
        playerPos = pos;
        steps++;

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
        else if (cellType == 'L') {
            bool unlocked = tryUnlockLocker(maze, playerPos, totalScore);
            if (unlocked) {
                cout << "�����ѽ�������Ϊͨ·��";
            }
        }
        else if (cellType == 'B') {
            // ����bossս��ʹ��ͨ�ú�����
            triggerBossFight(maze, playerPos, totalScore);
        }

        cout << " ��ǰλ��: (" << playerPos.first << ", " << playerPos.second << ")"
            << " �ܵ÷�: " << totalScore
            << endl;

        // ���Ϊ�ѷ���
        string posKey = to_string(playerPos.first) + "," + to_string(playerPos.second);
        visited.insert(posKey);

        // ÿ���ƶ�������Ұ���Ƿ�������Դ
        auto newResources = getVisibleResources(maze, playerPos, 1);
        if (!newResources.empty()) {
            cout << "��������Դ���жϵ�ǰ·����" << endl;
            return true; // �жϵ�ǰ·��������ֹ��Ϸ
        }
    }

    return true;
}

// ������������������ģʽ��
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

            cout << "���� " << steps + 1 << ": ѡ��Ŀ�� ("
                << targetPos.first << ", " << targetPos.second
                << ") ���� '" << targetType
                << "' �Լ۱�: " << resourceValues[0].second << endl;

            // �ƶ���Ŀ��λ��
            if (!moveToPosition(maze, playerPos, targetPos, totalScore, steps, visited)) {
                cout << "�ƶ�ʧ�ܣ�����Ѱ������·��..." << endl;

                // ����ƶ�ʧ�ܣ�����Ѱ�ҳ���
                auto path = findPathDFS(maze, playerPos, exitPos);
                if (!path.empty()) {
                    cout << "�ҵ����·����ת�����..." << endl;
                    if (!moveToPosition(maze, playerPos, exitPos, totalScore, steps, visited)) {
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

            // ʹ��DFSѰ�ҳ���·��
            vector<pair<int, int>> path = findPathDFS(maze, playerPos, exitPos);

            if (path.empty()) {
                cout << "�޷��ҵ�ͨ�����ڵ�·����" << endl;
                break;
            }

            cout << "�ҵ��������·������" << path.size() << "����" << endl;

            // ��·���ƶ�
            for (const auto& nextPos : path) {
                // �ƶ�����һ��λ��
                playerPos = nextPos;
                steps++;

                // ����Ƿ񵽴����
                if (playerPos == exitPos) {
                    cout << "��ϲ�����ѵ�����ڣ�" << endl;
                    break;
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
                else if (cellType == 'L') {
                    bool unlocked = tryUnlockLocker(maze, playerPos, totalScore);
                    if (unlocked) {
                        cout << "�����ѽ�������Ϊͨ·��";
                    }
                }
                else if (cellType == 'B') {
                    // ����bossս��ʹ��ͨ�ú�����
                    triggerBossFight(maze, playerPos, totalScore);
                }

                cout << " ��ǰλ��: (" << playerPos.first << ", " << playerPos.second << ")"
                    << " �ܵ÷�: " << totalScore
                    << endl;

                // ���Ϊ�ѷ���
                string posKey = to_string(playerPos.first) + "," + to_string(playerPos.second);
                visited.insert(posKey);

                // �����Ұ���Ƿ��������Դ
                auto newResources = getVisibleResources(maze, playerPos, 1);
                if (!newResources.empty()) {
                    cout << "��������Դ���ж����������" << endl;
                    break; // �ж����������������ѭ��
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