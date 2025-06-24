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

// �����Թ���Ԫ������
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
    MazeCell() : type(PATH) {}  // ���캯��Ĭ�ϳ�ʼ��Ϊͨ·
};

class MazeGenerator {
public:
    // �����Թ�������
    static vector<vector<MazeCell>> generateMaze(int size, 
                                              int goldCount, 
                                              int trapCount, 
                                              int lockerCount, 
                                              int bossCount,
                                              pair<int, int>& startPos,
                                              pair<int, int>& exitPos);
    
    // ��ӡ�Թ�
    static void printMaze(const vector<vector<MazeCell>>& maze);
    static void printMaze(const vector<vector<char>>& maze);
    //static void printMaze1(const std::vector<std::vector<char>>& maze);
    // ���η������Թ�
    static void divide(vector<vector<MazeCell>>& maze, int x1, int y1, int x2, int y2);

    // �����ͨ��
    static bool isConnected(const vector<vector<MazeCell>>& maze,
        pair<int, int> start,
        pair<int, int> exit);

    // ��ȡ����ͨ·
    static vector<pair<int, int>> getAllPaths(const vector<vector<MazeCell>>& maze);

    // �������Ԫ��
    static void placeRandomElements(vector<vector<MazeCell>>& maze,
        char elem,
        int count,
        const set<pair<int, int>>& forbidden);
};