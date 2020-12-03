#include <string>

#include "xxhash.h"

#include "BBHash.hpp"

int main() {
    auto count = 1000000;
    /*std::vector<uint64_t> int_keys;
    for (uint64_t i = 0; i < count; i++)
        int_keys.push_back(i);
    bb_hash<uint64_t> int_hash(&int_keys, count, 1.0);*/

    std::vector<std::string> str_keys;
    for (uint64_t i = 0; i < count; i++) {
    	str_keys.push_back(std::to_string(i));
    }
    bb_hash<std::string> str_hash(&str_keys, count, 1.0);

    auto lookup = str_keys;
    auto h = str_hash;
    for (uint64_t i = 0; i < count; i++) {
        if (h.query(lookup[i]) > count)
            std::cerr<<h.query(lookup[i]) << " Error!\n";
    }
    h.save("BBB");


    bb_hash<std::string> new_h;
    new_h.load("BBB");
    for (uint64_t i = 0; i < count; i++) {
        if (new_h.query(lookup[i]) > count)
            std::cerr<<new_h.query(lookup[i]) << " Error!\n";
    }
}
