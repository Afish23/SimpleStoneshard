#pragma once
#include "MazeGenerator.h"
#include <vector>
#include <utility>
#include <climits>
#include <unordered_map>

class ResourcePathPlanner {
public:
    struct Result {
        std::vector<std::pair<int, int>> path;
        int totalValue = 0;
        bool success = false;
    };

    static Result findOptimalPath(
        const std::vector<std::vector<MazeCell>>& maze,
        std::pair<int, int> start,
        std::pair<int, int> exit);

    static std::vector<std::vector<MazeCell>> markPath(
        const std::vector<std::vector<MazeCell>>& originalMaze,
        const std::vector<std::pair<int, int>>& path);

private:
    static int getValue(char cellType);
};