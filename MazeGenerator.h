#ifndef MAZE_GENERATOR_H  // ��ֹͷ�ļ��ظ������ĺ걣��
#define MAZE_GENERATOR_H

// ��Ŀ�Զ���ͷ�ļ�����ȷ��·����ȷ��
#include "BossFightStrategy.h"
#include "GameObjects.h"
#include "GreedyResourcePicker.h"
#include "PuzzleSolver.h"
#include "ResourcePathPlanner.h"
#include "Utils.h"

// ��׼��ͷ�ļ�
#include <vector>
#include <queue>
#include <utility>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <set>
#include <iostream>

// �Թ�Ԫ�س�������
const char WALL = '#';
const char PATH = ' ';
const char START = 'S';
const char EXIT = 'E';
const char GOLD = 'G';
const char TRAP = 'T';
const char LOCKER = 'L';
const char BOSS = 'B';

// �Թ���Ԫ��ṹ��
struct MazeCell {
    char type;
    MazeCell() : type(PATH) {}  // ���캯��Ĭ�ϳ�ʼ��Ϊͨ·
};

// ����ԭ����������д������ͷ������ʵ�֣�
void divide(std::vector<std::vector<MazeCell>>& maze, int x1, int y1, int x2, int y2);
bool is_connected(const std::vector<std::vector<MazeCell>>& maze, std::pair<int, int> start, std::pair<int, int> exit);
std::vector<std::pair<int, int>> get_all_paths(const std::vector<std::vector<MazeCell>>& maze);
void place_random_elements(std::vector<std::vector<MazeCell>>& maze, char elem, int count, const std::set<std::pair<int, int>>& forbidden);
void print_maze(const std::vector<std::vector<MazeCell>>& maze);

#endif  // MAZE_GENERATOR_H