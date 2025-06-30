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

// 判断一个数字是否为素数
bool is_prime(int num);

// 生成所有满足线索C的三位数密码候选
std::vector<std::vector<int>> generate_candidates(const std::vector<std::vector<int>>& clues);

// 密码求解主逻辑
struct PasswordResult {
    std::string password; // 正确密码
    int tries;            // 尝试次数
    std::vector<int> password_digits; // 正确密码的三位数组
};

// 对所有候选密码加密比对，找到正确密码，返回尝试次数和正确密码
PasswordResult solve_password(const std::vector<std::vector<int>>& clues, const std::string& L);