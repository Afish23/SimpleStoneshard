#include "ResourcePathPlanner.h"

ResourcePathPlanner::Result ResourcePathPlanner::findOptimalPath(
    const vector<vector<MazeCell>>& maze,
    pair<int, int> start,
    pair<int, int> exit) {

    Result result;
    int n = maze.size();
    if (n == 0) return result;

    // ��ʼ��DP���·����¼
    vector<vector<DPState>> dp(n, vector<DPState>(n, {
    -10000,       // value
    0,             // keys
    true,          // hasWeapon
    vector<vector<bool>>(n, vector<bool>(n, false))  // collected
        }));
    vector<vector<pair<int, int>>> prev(n, vector<pair<int, int>>(n, { -1,-1 }));

    // ����ʼ��
    dp[start.first][start.second] = {
        0,  // value
        0,  // keys
        true,  // hasWeapon
        vector<vector<bool>>(n, vector<bool>(n, false))  // collected
    };
    prev[exit.first][exit.second] = { -1, -1 };  // ��ʼ��Ϊ��ǰ��
    // ǿ���յ��ֵ
    const int EXIT_BASE_VALUE = 10000;
    dp[exit.first][exit.second].value = EXIT_BASE_VALUE;

    // Bellman-Ford�㷨����
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
                    // ��ȿ�����ǰ״̬
                    DPState newState = dp[i][j];

                    // ����ͬ��Ԫ������
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

                    // ״̬��������
                    if (newState.value > dp[ni][nj].value&&(ni!=start.first&&nj!=start.second)) {
                        dp[ni][nj] = newState;
                        prev[ni][nj] = { i, j };
                        updated = true;
                    }
                    //cout << "\n--- ���� (" << ni << "," << nj << ") ---\n";
                    //cout << "����: (" << i << "," << j << ")\n";
                    //cout << "��ֵ: " << newState.value << "\n";

                    //// ��ӡ����DP����
                    //cout << "��ǰDP����:\n";
                    //for (int x = 0; x < n; x++) {
                    //    for (int y = 0; y < n; y++) {
                    //        if (maze[x][y].type == WALL) {
                    //            cout << "#####\t"; // ǽ��������
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

    // ����յ��Ƿ�ɴ�
    if (dp[exit.first][exit.second].value == INT_MIN) {
        cerr << "�����յ㲻�ɴ�" << endl;
        return result;
    }

    // �ڻ���·��ǰ��ӵ������
    cout << "\n===== ���� prev ���� =====" << endl;
    // ����յ��Ƿ�����Чǰ��
    if (prev[exit.first][exit.second] == pair<int, int>(-1, -1)) {
        cout << "�����յ� (" << exit.first << "," << exit.second
            << ") û����Ч��ǰ���ڵ㣡" << endl;
    }
    else {
        cout << "�յ� (" << exit.first << "," << exit.second
            << ") ��ǰ���� (" << prev[exit.first][exit.second].first
            << "," << prev[exit.first][exit.second].second << ")" << endl;
    }

    // ��ӡ���з�(-1,-1)��ǰ����ϵ
    cout << "\n������Ч��ǰ����ϵ��" << endl;
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
    const int maxSteps = n * n * 2;  // ��ֹ����ѭ��

    while (current != start) {
        // ���ѭ������
        if (stepCount++ > maxSteps) {
            cerr << "���棺���������ݲ���" << endl;
            validPath = false;
            break;
        }

        // ����Ƿ��γɻ�·
        if (visited.count(current)) {
            cerr << "���棺��⵽·����· ("
                << current.first << "," << current.second << ")" << endl;
            validPath = false;
            break;
        }

        // �����Ч����һ����
        if (prev[current.first][current.second] == pair<int, int>(-1, -1)) {
            cerr << "���棺������Ч����һ�� ("
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

        // ����·���ܼ�ֵ����ȥ���ڵĻ�׼ֵ��
        int totalValue = dp[exit.first][exit.second].value - EXIT_BASE_VALUE;

        // ��֤·��������
        for (size_t i = 1; i < path.size(); ++i) {
            int dx = abs(path[i].first - path[i - 1].first);
            int dy = abs(path[i].second - path[i - 1].second);
            if (dx + dy != 1) {
                cerr << "���棺·�������� between ("
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

    // ���·����Ч�����Է��ز���·���������ã�
    if (!result.success && !path.empty()) {
        cerr << "���ز���·���������ڵ��ԣ�" << endl;
        reverse(path.begin(), path.end());
        result = { path, dp[exit.first][exit.second].value - EXIT_BASE_VALUE, false };
    }

    return result;
}

int ResourcePathPlanner::getValue(char cellType) {
    switch (cellType) {
    case TRAP:  return -3;
    case PATH:  return -1;  // ���������·��
    case START: return 0;
    case EXIT:  return 0;
    default:    return 0;
    }
}

// ��ȷ��ʵ�ַ�ʽ
bool ResourcePathPlanner::isValidPosition(
    const vector<vector<MazeCell>>& maze,
    int x,
    int y)
{
    //cout << 1 << endl;
    // ����Թ��Ƿ�Ϊ��
    if (maze.empty() || maze[0].empty()) return false;

    // �ϸ�������
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

    // ����ԭʼ�Թ�
    for (size_t i = 0; i < originalMaze.size(); i++) {
        for (size_t j = 0; j < originalMaze[i].size(); j++) {
            markedMaze[i][j] = originalMaze[i][j].type;
        }
    }

    // ��ǿ·�����
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
            if (cell.value == INT_MIN) cout << "[-��]";
            else cout << "[" << cell.value
                << "|K" << cell.keys
                << "|W" << cell.hasWeapon << "]";
        }
        cout << endl;
    }
}