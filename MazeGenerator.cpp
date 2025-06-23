#include "MazeGenerator.h"  // 包含对应的头文件
using namespace std;       // 可在源文件中使用命名空间（头文件不建议）

// ========== 函数实现 ==========

// 分治法生成唯一通路迷宫
void divide(vector<vector<MazeCell>>& maze, int x1, int y1, int x2, int y2) {
    if (x2 - x1 < 2 || y2 - y1 < 2) return;
    bool horizontal = (rand() % 2 == 0);

    if (horizontal) {
        int split_row = x1 + 1 + rand() % (x2 - x1 - 1);
        int door_col = y1 + rand() % (y2 - y1 + 1);
        for (int col = y1; col <= y2; col++) {
            if (col == door_col) continue;
            maze[split_row][col].type = WALL;
        }
        divide(maze, x1, y1, split_row - 1, y2);
        divide(maze, split_row + 1, y1, x2, y2);
    }
    else {
        int split_col = y1 + 1 + rand() % (y2 - y1 - 1);
        int door_row = x1 + rand() % (x2 - x1 + 1);
        for (int row = x1; row <= x2; row++) {
            if (row == door_row) continue;
            maze[row][split_col].type = WALL;
        }
        divide(maze, x1, y1, x2, split_col - 1);
        divide(maze, x1, split_col + 1, x2, y2);
    }
}

// BFS检查连通性
bool is_connected(const vector<vector<MazeCell>>& maze, pair<int, int> start, pair<int, int> exit) {
    int n = maze.size();
    vector<vector<bool>> vis(n, vector<bool>(n, false));
    queue<pair<int, int>> q;
    q.push(start);
    vis[start.first][start.second] = true;
    int dx[] = { -1, 1, 0, 0 }, dy[] = { 0, 0, -1, 1 };
    while (!q.empty()) {
        auto temp = q.front();
        q.pop();
        int x = temp.first, y = temp.second;
        if (x == exit.first && y == exit.second) return true;
        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx >= 0 && nx < n && ny >= 0 && ny < n && !vis[nx][ny] && maze[nx][ny].type != WALL) {
                vis[nx][ny] = true;
                q.push({ nx, ny });
            }
        }
    }
    return false;
}

// 获得全部通路格
vector<pair<int, int>> get_all_paths(const vector<vector<MazeCell>>& maze) {
    vector<pair<int, int>> res;
    int n = maze.size();
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (maze[i][j].type == PATH)
                res.push_back({ i, j });
    return res;
}

// 随机分配元素（如金币、陷阱等）
void place_random_elements(vector<vector<MazeCell>>& maze, char elem, int count, const set<pair<int, int>>& forbidden) {
    vector<pair<int, int>> paths = get_all_paths(maze);
    vector<pair<int, int>> candidates;
    for (const auto& pos : paths)
        if (forbidden.count(pos) == 0)
            candidates.push_back(pos);
    random_shuffle(candidates.begin(), candidates.end());
    for (int i = 0; i < min((int)candidates.size(), count); i++) {
        maze[candidates[i].first][candidates[i].second].type = elem;
    }
}

// 打印迷宫
void print_maze(const vector<vector<MazeCell>>& maze) {
    int n = maze.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            cout << maze[i][j].type << " ";
        cout << endl;
    }
}
