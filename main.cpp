#include "BossFightStrategy.h"
#include "GameObjects.h"
#include "GreedyResourcePicker.h"
#include "MazeGenerator.h"
#include "PuzzleSolver.h"
#include "ResourcePathPlanner.h"
#include "Utils.h"
using namespace std;

int main() {
    srand(time(0));

    int n;
    cout << "�����Թ��ߴ� (n��n, ��С7): ";
    cin >> n;
    if (n < 7) {
        cout << "�ߴ粻��С��7\n";
        return 1;
    }

    // ���ø���Ԫ������
    int goldCount = max(1, n / 4)+rand()%3;
    int trapCount = max(1, n / 5)+ rand() % 3;
    int lockerCount = max(1, n / 6)+ rand() % 2;
    int bossCount = 1;

    // �����Թ�
    pair<int, int> startPos, exitPos;
    cout << "�������ʼ�����꣨x>=1,y>=1����ʽ��";
    cin >> startPos.first >> startPos.second;
    cout << "��������ֹ�����꣨x<=" << n - 2 << ",y<=" << n - 2 << "����ʽ��";
    cin >> exitPos.first >> exitPos.second;

    auto maze = MazeGenerator::generateMaze(n, goldCount, trapCount, lockerCount, bossCount, startPos, exitPos);

    // ��ӡ�Թ�
    cout << "\n���ɵ��Թ����£�\n";
    MazeGenerator::printMaze(maze);

    // ·���滮�������˱�������ʹ��startPos��exitPos��
    auto result = ResourcePathPlanner::findOptimalPath(maze, startPos, exitPos);



    /*
    std::pair<int, int> startPos = {14, 12}; // S ��λ��
    std::pair<int, int> exitPos = {0, 5};    // E ��λ��

    std::vector<std::vector<MazeCell>> maze1 = {
    {{'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'E'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}},
    {{'#'}, {'G'}, {'T'}, {' '}, {' '}, {' '}, {' '}, {' '}, {'#'}, {' '}, {' '}, {' '}, {' '}, {' '}, {'#'}},
    {{'#'}, {'#'}, {'#'}, {' '}, {'#'}, {' '}, {'#'}, {'#'}, {'#'}, {' '}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}},
    {{'#'}, {'T'}, {'G'}, {' '}, {'#'}, {' '}, {' '}, {' '}, {'#'}, {' '}, {' '}, {' '}, {'T'}, {'G'}, {'#'}},
    {{'#'}, {'#'}, {'#'}, {' '}, {'#'}, {' '}, {'#'}, {'#'}, {'#'}, {' '}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}},
    {{'#'}, {'G'}, {' '}, {' '}, {'#'}, {' '}, {'#'}, {' '}, {' '}, {' '}, {'#'}, {' '}, {' '}, {' '}, {'#'}},
    {{'#'}, {'T'}, {'#'}, {'#'}, {'#'}, {'L'}, {'#'}, {'T'}, {'#'}, {' '}, {'#'}, {'G'}, {'#'}, {'#'}, {'#'}},
    {{'#'}, {' '}, {' '}, {' '}, {'#'}, {' '}, {' '}, {' '}, {'#'}, {' '}, {' '}, {' '}, {' '}, {' '}, {'#'}},
    {{'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'T'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}},
    {{'#'}, {' '}, {' '}, {' '}, {'#'}, {' '}, {' '}, {' '}, {'#'}, {' '}, {'#'}, {'G'}, {'T'}, {'T'}, {'#'}},
    {{'#'}, {'#'}, {'#'}, {'G'}, {'#'}, {' '}, {'#'}, {'#'}, {'#'}, {' '}, {'#'}, {'#'}, {'#'}, {'G'}, {'#'}},
    {{'#'}, {' '}, {' '}, {' '}, {' '}, {' '}, {' '}, {' '}, {' '}, {' '}, {'#'}, {' '}, {' '}, {'T'}, {'#'}},
    {{'#'}, {' '}, {'#'}, {'#'}, {'#'}, {' '}, {'#'}, {'#'}, {'#'}, {' '}, {'#'}, {' '}, {'#'}, {'#'}, {'#'}},
    {{'#'}, {' '}, {' '}, {'T'}, {'#'}, {' '}, {'G'}, {' '}, {'#'}, {' '}, {' '}, {'B'}, {' '}, {' '}, {'#'}},
    {{'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'#'}, {'S'}, {'#'}, {'#'}}
};
    
    */





    // ������
    if (result.success) {
        cout << "\n�ҵ�����·�����ܼ�ֵ: " << result.totalValue << endl;

        // ���ӻ���ʾ
        auto markedMaze = ResourcePathPlanner::markPath(maze, result.path);
        cout << "\n·�����ͼ (* ��ʾ·��):\n";
        MazeGenerator::printMaze(markedMaze);

        // ���·������
        cout << "\n·��������:\n";
        for (auto& p : result.path) {
            cout << "(" << p.first << "," << p.second << ") ";
        }
        cout << endl;
    }
    else {
        cout << "\nδ�ҵ���Ч·����" << endl;
    }
    return 0;  // �Ƴ����ظ���return���
}