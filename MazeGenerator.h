#pragma once
#include <vector>
#include <queue>
#include <utility>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <set>
#include <iostream>
using namespace std;

// 定义迷宫单元格类型
const char WALL = '#';
const char PATH = ' ';
const char START = 'S';
const char EXIT = 'E';
const char GOLD = 'G';
const char TRAP = 'T';
const char LOCKER = 'L';
const char BOSS = 'B';

struct MazeCell {
    char type;
    MazeCell() : type(PATH) {}  // 构造函数默认初始化为通路
};

class MazeGenerator {
public:
    // 生成迷宫主函数
    static vector<vector<MazeCell>> generateMaze(int size, 
                                              int goldCount, 
                                              int trapCount, 
                                              int lockerCount, 
                                              int bossCount,
                                              pair<int, int>& startPos,
                                              pair<int, int>& exitPos);
    
    // 打印迷宫
    static void printMaze(const vector<vector<MazeCell>>& maze);
    static void printMaze(const vector<vector<char>>& maze);
    //static void printMaze1(const std::vector<std::vector<char>>& maze);
    // 分治法生成迷宫
    static void divide(vector<vector<MazeCell>>& maze, int x1, int y1, int x2, int y2);

    // 检查连通性
    static bool isConnected(const vector<vector<MazeCell>>& maze,
        pair<int, int> start,
        pair<int, int> exit);

    // 获取所有通路
    static vector<pair<int, int>> getAllPaths(const vector<vector<MazeCell>>& maze);

    // 随机放置元素
    static void placeRandomElements(vector<vector<MazeCell>>& maze,
        char elem,
        int count,
        const set<pair<int, int>>& forbidden);
};