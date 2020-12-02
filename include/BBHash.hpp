#ifndef __BB_HASH__
#define __BB_HASH__

#include <vector>
#include <unordered_map>

#include "RankSupport.hpp"
#include "BitVector.hpp"

#define MAX_REG_HASH_DEF 10000

template<typename key_type>
class bb_hash {
public:
    bb_hash(uint64_t n, double g, uint64_t max_reg = MAX_REG_HASH_DEF) : size(n), gamma(g), max_regular_hash_count(max_reg) { }
    bb_hash(std::vector<key_type>* keys, uint64_t n, double g, uint64_t max_reg = MAX_REG_HASH_DEF);
    uint64_t query(key_type key);
private:
    std::vector<rank_support*> A;
    std::vector<uint64_t> hash_mods;
    uint64_t size;
    double gamma;
    uint64_t max_regular_hash_count;
    std::unordered_map<key_type, uint64_t> reg_hash;
};

template class bb_hash<std::string>;
#endif
