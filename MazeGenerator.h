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

// ¶¨ÒåÃÔ¹¬µ¥Ôª¸ñÀàÐÍ
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
    MazeCell() : type(PATH) {}  // ¹¹Ôìº¯ÊýÄ¬ÈÏ³õÊ¼»¯ÎªÍ¨Â·
};

class MazeGenerator {
public:

    // Éú³ÉÃÔ¹¬Ö÷º¯Êý
    static vector<vector<MazeCell>> generateMaze(int size, 
                                              int goldCount, 
                                              int trapCount, 
                                              int lockerCount, 
                                              int bossCount,
                                              pair<int, int>& startPos,
                                              pair<int, int>& exitPos);
    
    // ´òÓ¡ÃÔ¹¬
    static void printMaze(const vector<vector<MazeCell>>& maze);
    // ·ÖÖÎ·¨Éú³ÉÃÔ¹¬
    static void divide(vector<vector<MazeCell>>& maze, int x1, int y1, int x2, int y2);

    // ¼ì²éÁ¬Í¨ÐÔ
    static bool isConnected(const vector<vector<MazeCell>>& maze,
        pair<int, int> start,
        pair<int, int> exit);

    // »ñÈ¡ËùÓÐÍ¨Â·
    static vector<pair<int, int>> getAllPaths(const vector<vector<MazeCell>>& maze);

    // Ëæ»ú·ÅÖÃÔªËØ
    static void placeRandomElements(vector<vector<MazeCell>>& maze,
        char elem,
        int count,
        const set<pair<int, int>>& forbidden);
	void generate(int n);
};