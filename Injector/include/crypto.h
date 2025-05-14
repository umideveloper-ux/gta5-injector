#pragma once
#include <vector>
#include <string>

class Crypto {
public:
    static void Encrypt(std::vector<char>& data, const std::string& key);
    static void Decrypt(std::vector<char>& data, const std::string& key);
};
