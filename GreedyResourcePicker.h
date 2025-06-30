#pragma once
//贪心实时资源拾取策略模块
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
    const pair<int, int>& end);

bool moveToPosition(
    vector<vector<MazeCell>>& maze,
    pair<int, int>& playerPos,
    const pair<int, int>& target,
    int& totalScore,
    int& steps,
    unordered_set<string>& visited);

void greedyResourceCollection(
    vector<vector<MazeCell>>& maze,
    pair<int, int> startPos,
    pair<int, int> exitPos,
    int visionRadius);

int getResourceValue(char cellType);
int manhattanDistance(const pair<int, int>& a, const pair<int, int>& b);
bool isPassable(char c);