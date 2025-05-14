#include "include/crypto.h"

void Crypto::Encrypt(std::vector<char>& data, const std::string& key) {
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] ^= key[i % key.size()];
    }
}

void Crypto::Decrypt(std::vector<char>& data, const std::string& key) {
    Encrypt(data, key);
}
