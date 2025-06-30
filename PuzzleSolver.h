#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <string>
#include <array>
#include <cstring>

// �ж�һ�������Ƿ�Ϊ����
bool is_prime(int num);

// ����������������C����λ�������ѡ
std::vector<std::vector<int>> generate_candidates(const std::vector<std::vector<int>>& clues);

// ����������߼�
struct PasswordResult {
    std::string password; // ��ȷ����
    int tries;            // ���Դ���
    std::vector<int> password_digits; // ��ȷ�������λ����
};

// �����к�ѡ������ܱȶԣ��ҵ���ȷ���룬���س��Դ�������ȷ����
PasswordResult solve_password(const std::vector<std::vector<int>>& clues, const std::string& L);