#pragma once

#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <cstdlib>
#include <queue>
#include <cmath>
#include <stack>
#include <functional>
#include"MazeGenerator.h"
#include"PuzzleSolver.h"
#include"BossFightStrategy.h"
#include"FightBossVisual.h"
#include"GameObjects.h"
#include"json.hpp"
using namespace std;


vector<pair<pair<int, int>, char>> getVisibleResources(
    const vector<vector<MazeCell>>& maze,
    const pair<int, int>& playerPos,
    int visionRadius);

bool tryUnlockLocker(vector<vector<MazeCell>>& maze,
    const pair<int, int>& pos,
    int& totalScore);

vector<pair<int, int>> findPathDFS(
    const vector<vector<MazeCell>>& maze,
    const pair<int, int>& start,
    const pair<int, int>& end,
    bool avoidTraps = true);

bool moveToPosition(
    vector<vector<MazeCell>>& maze,
    pair<int, int>& playerPos,
    const pair<int, int>& target,
    int& totalScore,
    int& steps,
    unordered_set<string>& visited,
    vector<pair<int, int>>& fullPath,
    vector<pair<int, int>>& bossSteps  // 添加这个参数
);

void greedyResourceCollection(
    std::vector<std::vector<MazeCell>>& maze,
    std::pair<int, int> startPos,
    std::pair<int, int> exitPos,
    std::vector<std::pair<int, int>>& fullPath,
    std::vector<std::pair<int, int>>& bossSteps,
    int& totalScore  // 添加分数引用参数
);
void finalizeBossFight(
    vector<vector<MazeCell>>& maze,
    pair<int, int> bossPos,
    int& totalScore,
    int turnsUsed);
int getResourceValue(char cellType);
int manhattanDistance(const pair<int, int>& a, const pair<int, int>& b);
bool isPassable(char c);
void printMazeWithPath(const vector<vector<MazeCell>>& maze, const vector<pair<int, int>>& fullPath);
void exploreDFS(
    vector<vector<MazeCell>>& maze,
    pair<int, int>& playerPos,
    const pair<int, int>& exitPos,
    int& totalScore,
    int& steps,
    unordered_set<string>& visited,
    vector<pair<int, int>>& fullPath,
    bool& newResourceFound);

bool triggerBossFight(
    vector<vector<MazeCell>>& maze,
    pair<int, int>& playerPos,
    int& totalScore,
    bool delayScoreUpdate);