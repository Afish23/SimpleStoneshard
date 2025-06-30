#include "PuzzleSolver.h"

// 你的SHA256和PasswordLock类直接粘贴到这里
// ---- SHA256 和 PasswordLock BEGIN ----
class SHA256 {
private:
    const static uint32_t K[64];
    std::array<uint32_t, 8> H = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    inline uint32_t rightRotate(uint32_t value, unsigned int count) {
        return (value >> count) | (value << (32 - count));
    }
    void processBlock(const uint8_t* block) {
        uint32_t W[64];
        for (int t = 0; t < 16; t++) {
            W[t] = (block[t * 4] << 24) | (block[t * 4 + 1] << 16) |
                (block[t * 4 + 2] << 8) | (block[t * 4 + 3]);
        }
        for (int t = 16; t < 64; t++) {
            uint32_t s0 = rightRotate(W[t - 15], 7) ^ rightRotate(W[t - 15], 18) ^ (W[t - 15] >> 3);
            uint32_t s1 = rightRotate(W[t - 2], 17) ^ rightRotate(W[t - 2], 19) ^ (W[t - 2] >> 10);
            W[t] = W[t - 16] + s0 + W[t - 7] + s1;
        }
        uint32_t a = H[0], b = H[1], c = H[2], d = H[3];
        uint32_t e = H[4], f = H[5], g = H[6], h = H[7];
        for (int t = 0; t < 64; t++) {
            uint32_t S1 = rightRotate(e, 6) ^ rightRotate(e, 11) ^ rightRotate(e, 25);
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t temp1 = h + S1 + ch + K[t] + W[t];
            uint32_t S0 = rightRotate(a, 2) ^ rightRotate(a, 13) ^ rightRotate(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;
            h = g; g = f; f = e; e = d + temp1;
            d = c; c = b; b = a; a = temp1 + temp2;
        }
        H[0] += a; H[1] += b; H[2] += c; H[3] += d;
        H[4] += e; H[5] += f; H[6] += g; H[7] += h;
    }
public:
    std::vector<uint8_t> compute(const std::vector<uint8_t>& message) {
        uint64_t originalBitLength = message.size() * 8;
        uint64_t paddedLength = message.size() + 1 + 8;
        paddedLength = (paddedLength + 63) & ~63;
        std::vector<uint8_t> paddedMessage(paddedLength, 0);
        std::copy(message.begin(), message.end(), paddedMessage.begin());
        paddedMessage[message.size()] = 0x80;
        for (int i = 0; i < 8; i++) {
            paddedMessage[paddedLength - 8 + i] = (originalBitLength >> ((7 - i) * 8)) & 0xFF;
        }
        for (size_t i = 0; i < paddedLength; i += 64) {
            processBlock(&paddedMessage[i]);
        }
        std::vector<uint8_t> hash(32);
        for (int i = 0; i < 8; i++) {
            hash[i * 4] = (H[i] >> 24) & 0xFF;
            hash[i * 4 + 1] = (H[i] >> 16) & 0xFF;
            hash[i * 4 + 2] = (H[i] >> 8) & 0xFF;
            hash[i * 4 + 3] = H[i] & 0xFF;
        }
        return hash;
    }
    void reset() {
        H = {
            0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
            0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
        };
    }
};
const uint32_t SHA256::K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

class PasswordLock {
private:
    std::vector<unsigned char> salt = {
        0xb2, 0x53, 0x22, 0x65, 0x7d, 0xdf, 0xb0, 0xfe,
        0x9c, 0xde, 0xde, 0xfe, 0xf3, 0x1d, 0xdc, 0x3e
    };
    std::string bytesToHex(const std::vector<uint8_t>& bytes) {
        std::stringstream ss;
        for (const auto& byte : bytes) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
        return ss.str();
    }
public:
    std::string hashPassword(const std::string& password) {
        std::vector<unsigned char> passwordBytes(password.begin(), password.end());
        std::vector<unsigned char> combined;
        combined.insert(combined.end(), salt.begin(), salt.end());
        combined.insert(combined.end(), passwordBytes.begin(), passwordBytes.end());
        SHA256 sha256;
        std::vector<uint8_t> hashBytes = sha256.compute(combined);
        return bytesToHex(hashBytes);
    }
    bool verifyPassword(const std::string& inputPassword, const std::string& storedHash) {
        return hashPassword(inputPassword) == storedHash;
    }
};
// ---- SHA256 和 PasswordLock END ----

// 判断素数
bool is_prime(int num) {
    if (num < 2) return false;
    for (int i = 2; i * i <= num; ++i)
        if (num % i == 0) return false;
    return true;
}

std::vector<std::vector<int>> generate_candidates(const std::vector<std::vector<int>>& clues) {
    int fix[3] = { -1, -1, -1 };    // 指定位数字
    int even_odd[3] = { -2, -2, -2 }; // -2未知，0偶，1奇
    bool unique_prime = false;
    for (const auto& clue : clues) {
        if (clue.size() == 2) {
            if (clue[0] == -1 && clue[1] == -1) unique_prime = true;
            else if (clue[1] == 0) even_odd[clue[0] - 1] = 0;
            else if (clue[1] == 1) even_odd[clue[0] - 1] = 1;
        }
        else if (clue.size() == 3) {
            for (int i = 0; i < 3; ++i)
                if (clue[i] != -1) fix[i] = clue[i];
        }
    }
    std::vector<std::vector<int>> result;
    for (int d1 = 0; d1 <= 9; ++d1) {
        if (fix[0] != -1 && d1 != fix[0]) continue;
        if (even_odd[0] != -2 && (d1 % 2 != even_odd[0])) continue;
        for (int d2 = 0; d2 <= 9; ++d2) {
            if (fix[1] != -1 && d2 != fix[1]) continue;
            if (even_odd[1] != -2 && (d2 % 2 != even_odd[1])) continue;
            for (int d3 = 0; d3 <= 9; ++d3) {
                if (fix[2] != -1 && d3 != fix[2]) continue;
                if (even_odd[2] != -2 && (d3 % 2 != even_odd[2])) continue;
                std::vector<int> cand = { d1, d2, d3 };
                if (unique_prime) {
                    if (!is_prime(d1) || !is_prime(d2) || !is_prime(d3)) continue;
                    std::set<int> s{ d1, d2, d3 };
                    if (s.size() != 3) continue;
                }
                result.push_back(cand);
            }
        }
    }
    return result;
}

PasswordResult solve_password(const std::vector<std::vector<int>>& clues, const std::string& L) {
    auto candidates = generate_candidates(clues);
    std::cout << "候选密码数量: " << candidates.size() << std::endl;
    int tries = 0;
    std::string found_pwd;
    std::vector<int> found_digits;
    PasswordLock lock;
    std::cout << "\n[DEBUG] 候选密码列表：" << std::endl;
    for (const auto& pwd : candidates) {
        std::cout << pwd[0] << pwd[1] << pwd[2] << std::endl;
    }
    for (const auto& pwd : candidates) {
        std::ostringstream oss;
        oss << pwd[0] << pwd[1] << pwd[2];
        ++tries;

        std::string current_pwd = oss.str();
        std::string current_hash = lock.hashPassword(current_pwd);

        // 调试输出：打印当前尝试的密码和哈希值
        //std::cout << "\n[DEBUG] 尝试密码 #" << tries << ": " << current_pwd
        //    << ", 计算哈希: " << current_hash << std::endl;
        //std::cout << "目标哈希 L: " << L << std::endl;  // 同时输出目标哈希值


        if (lock.hashPassword(oss.str()) == L) {
            found_pwd = oss.str();
            found_digits = pwd;
            break;
        }
    }
    return PasswordResult{ found_pwd, tries, found_digits };
}