#pragma once
#include "BossFightStrategy.h"
#include "GameObjects.h"
#include "GreedyResourcePicker.h"
#include "MazeGenerator.h"
#include "PuzzleSolver.h"
#include <vector>
#include <utility>
#include <climits>
using namespace std;

class ResourcePathPlanner {
public:
    struct DPState {
        int value = INT_MIN;
        int keys = 0;
        bool hasWeapon = true;
        vector<vector<bool>> collected;
    };

    struct Result {
        vector<pair<int, int>> path;
        int totalValue;
        bool success;
    };

    static Result findOptimalPath(
        const vector<vector<MazeCell>>& maze,
        pair<int, int> start,
        pair<int, int> exit);

    static vector<vector<char>> markPath(
        const vector<vector<MazeCell>>& originalMaze,
        const vector<pair<int, int>>& path);

private:
    static int getValue(char cellType);
    static bool isValidPosition(const vector<vector<MazeCell>>& maze,
        int x, int y);
    static void printDPState(const vector<vector<DPState>>& dp, int k);
    
    static constexpr int dx[4] = { -1, 1, 0, 0 };
    static constexpr int dy[4] = { 0, 0, -1, 1 };
};