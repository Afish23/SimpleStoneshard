#ifndef MAZE_GENERATOR_H  // 防止头文件重复包含的宏保护
#define MAZE_GENERATOR_H

// 项目自定义头文件（需确保路径正确）
#include "BossFightStrategy.h"
#include "GameObjects.h"
#include "GreedyResourcePicker.h"
#include "PuzzleSolver.h"
#include "ResourcePathPlanner.h"
#include "Utils.h"

// 标准库头文件
#include <vector>
#include <queue>
#include <utility>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <set>
#include <iostream>

// 迷宫元素常量定义
const char WALL = '#';
const char PATH = ' ';
const char START = 'S';
const char EXIT = 'E';
const char GOLD = 'G';
const char TRAP = 'T';
const char LOCKER = 'L';
const char BOSS = 'B';

// 迷宫单元格结构体
struct MazeCell {
    char type;
    MazeCell() : type(PATH) {}  // 构造函数默认初始化为通路
};

// 函数原型声明（仅写“函数头”，无实现）
void divide(std::vector<std::vector<MazeCell>>& maze, int x1, int y1, int x2, int y2);
bool is_connected(const std::vector<std::vector<MazeCell>>& maze, std::pair<int, int> start, std::pair<int, int> exit);
std::vector<std::pair<int, int>> get_all_paths(const std::vector<std::vector<MazeCell>>& maze);
void place_random_elements(std::vector<std::vector<MazeCell>>& maze, char elem, int count, const std::set<std::pair<int, int>>& forbidden);
void print_maze(const std::vector<std::vector<MazeCell>>& maze);

#endif  // MAZE_GENERATOR_H