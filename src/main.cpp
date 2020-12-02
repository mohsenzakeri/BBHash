#include <string>

#include "BBHash.hpp"

int main() {
    std::vector<uint64_t> int_keys;
    for (uint64_t i = 0; i < 1000; i++)
        int_keys.push_back(i);
    bb_hash<uint64_t> int_hash(&int_keys, 1000, 10);

    std::vector<std::string> str_keys;
    auto count = 1000000;
    for (uint64_t i = 0; i < count; i++)
	str_keys.push_back(std::to_string(i));
    bb_hash<std::string> str_hash(&str_keys, count, 1.0, 10000);
    
}
