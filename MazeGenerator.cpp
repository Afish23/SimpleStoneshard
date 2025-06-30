#include "MazeGenerator.h"

using json = nlohmann::json;

const int MAX_PATH_WIDTH = 1; // 设为1则强制单宽度路径
vector<vector<MazeCell>> MazeGenerator::generateMaze(int size,
    int goldCount,
    int trapCount,
    int lockerCount,
    int bossCount,
    pair<int, int>& startPos,
    pair<int, int>& exitPos) {
    vector<vector<MazeCell>> maze;

    while (true) {
        maze.assign(size, vector<MazeCell>(size, MazeCell()));

        // 设置外围墙
        for (int i = 0; i < size; i++) {
            maze[0][i].type = maze[size - 1][i].type =
                maze[i][0].type = maze[i][size - 1].type = WALL;
        }

        // 分治生成迷宫
        divide(maze, 1, 1, size - 2, size - 2);

        // 随机选择起点和终点
        auto paths = getAllPaths(maze);
        if (paths.size() < 2) continue;
        random_shuffle(paths.begin(), paths.end());

        startPos = startPos.first == 0 ? paths[0] : startPos;
        exitPos = exitPos.first == 0 ? paths[1] : exitPos;
        maze[startPos.first][startPos.second].type = START;
        maze[exitPos.first][exitPos.second].type = EXIT;

        if (isConnected(maze, startPos, exitPos)) break;
    }

    // 放置各种元素
    set<pair<int, int>> forbidden = { startPos, exitPos };
    placeRandomElements(maze, GOLD, goldCount, forbidden);
    placeRandomElements(maze, TRAP, trapCount, forbidden);
    placeRandomElements(maze, LOCKER, lockerCount, forbidden);
    placeRandomElements(maze, BOSS, bossCount, forbidden);

    return maze;
}

void MazeGenerator::divide(vector<vector<MazeCell>>& maze, int x1, int y1, int x2, int y2) {
    if (x2 - x1 < 2 || y2 - y1 < 2) return;

    // 使用现代随机数生成器
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // 确定分割方向（水平或垂直）
    bool horizontal;
    if (x2 - x1 > y2 - y1) {
        horizontal = true;
    }
    else if (y2 - y1 > x2 - x1) {
        horizontal = false;
    }
    else {
        std::uniform_int_distribution<> dist(0, 1);
        horizontal = (dist(gen) == 0);
    }

    if (horizontal) {
        // 确保分割线在奇数位置（防止宽路径形成）
        std::uniform_int_distribution<> dist_row(x1 + 1, x2 - 1);
        int split_row = dist_row(gen);
        if ((split_row - x1) % 2 == 0) split_row = (split_row < x2 - 1) ? split_row + 1 : split_row - 1;

        // 确保门位置在偶数位置
        std::uniform_int_distribution<> dist_col(y1, y2);
        int door_col = dist_col(gen);
        if ((door_col - y1) % 2 != 0) door_col = (door_col < y2) ? door_col + 1 : door_col - 1;

        // 创建分割墙（留出门位置）
        for (int col = y1; col <= y2; col++) {
            if (col == door_col) continue;
            maze[split_row][col].type = WALL;
        }

        // 递归处理子区域
        divide(maze, x1, y1, split_row - 1, y2);
        divide(maze, split_row + 1, y1, x2, y2);
    }
    else {
        // 确保分割线在奇数位置（防止宽路径形成）
        std::uniform_int_distribution<> dist_col(y1 + 1, y2 - 1);
        int split_col = dist_col(gen);
        if ((split_col - y1) % 2 == 0) split_col = (split_col < y2 - 1) ? split_col + 1 : split_col - 1;

        // 确保门位置在偶数位置
        std::uniform_int_distribution<> dist_row(x1, x2);
        int door_row = dist_row(gen);
        if ((door_row - x1) % 2 != 0) door_row = (door_row < x2) ? door_row + 1 : door_row - 1;

        // 创建分割墙（留出门位置）
        for (int row = x1; row <= x2; row++) {
            if (row == door_row) continue;
            maze[row][split_col].type = WALL;
        }

        // 递归处理子区域
        divide(maze, x1, y1, x2, split_col - 1);
        divide(maze, x1, split_col + 1, x2, y2);
    }
}

bool MazeGenerator::isConnected(const vector<vector<MazeCell>>& maze,
    pair<int, int> start,
    pair<int, int> exit) {
    int n = maze.size();
    vector<vector<bool>> vis(n, vector<bool>(n, false));
    queue<pair<int, int>> q;
    q.push(start);
    vis[start.first][start.second] = true;
    int dx[] = { -1,1,0,0 }, dy[] = { 0,0,-1,1 };
    while (!q.empty()) {
        auto temp = q.front(); q.pop();
        int x = temp.first, y = temp.second;
        if (x == exit.first && y == exit.second) return true;
        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx >= 0 && nx < n && ny >= 0 && ny < n && !vis[nx][ny] && maze[nx][ny].type != WALL) {
                vis[nx][ny] = true; q.push({ nx,ny });
            }
        }
    }
    return false;
}

vector<pair<int, int>> MazeGenerator::getAllPaths(const vector<vector<MazeCell>>& maze) {
    vector<pair<int, int>> res;
    int n = maze.size();
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (maze[i][j].type == PATH)
                res.push_back({ i,j });
    return res;
}

void MazeGenerator::placeRandomElements(vector<vector<MazeCell>>& maze,
    char elem,
    int count,
    const set<pair<int, int>>& forbidden) {
    vector<pair<int, int>> paths = getAllPaths(maze);
    vector<pair<int, int>> candidates;
    for (const auto& pos : paths)
        if (forbidden.count(pos) == 0)
            candidates.push_back(pos);
    random_shuffle(candidates.begin(), candidates.end());
    for (int i = 0; i < min((int)candidates.size(), count); i++) {
        maze[candidates[i].first][candidates[i].second].type = elem;
    }
}
void MazeGenerator::writeMazeToJson(const vector<vector<MazeCell>>& maze, const string& filename) {
    int n = maze.size();
    int m = maze[0].size();
    std::ofstream fout(filename);
    fout << "{\n  \"maze\": [\n";
    for (int i = 0; i < n; ++i) {
        fout << "    [";
        for (int j2 = 0; j2 < m; ++j2) {
            fout << "\"" << maze[i][j2].type << "\"";
            if (j2 != m - 1) fout << ",";
        }
        fout << "]";
        if (i != n - 1) fout << ",";
        fout << "\n";
    }
    fout << "  ]\n}\n";
    fout.close();
}
void MazeGenerator::printMaze(const vector<vector<MazeCell>>& maze) {
    int n = maze.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            cout << maze[i][j].type << " ";
        cout << endl;
    }    
}



