#ifndef __BB_HASH__
#define __BB_HASH__

#include <vector>
#include <unordered_map>

#include "RankSupport.hpp"
#include "BitVector.hpp"

#define BIT_VEC_LEVEL_DEF 3

template<typename key_type>
class bb_hash {
public:
    bb_hash() { }
    bb_hash(uint64_t n, double g, size_t b = BIT_VEC_LEVEL_DEF) :
	    size(n), gamma(g), bit_vec_count(b) { }
    bb_hash(std::vector<key_type>* keys, uint64_t n, double g, size_t b = BIT_VEC_LEVEL_DEF);
    uint64_t query(key_type key);
    uint64_t num_bits();
    uint64_t reg_hash_count() { return reg_hash.size(); }
    void save(char* output_dir);
    void load(char* index_dir);
private:
    std::vector<rank_support*> A;
    std::vector<uint64_t> hash_mods;
    uint64_t size;
    double gamma;
    size_t bit_vec_count;
    std::unordered_map<key_type, uint64_t> reg_hash;
};

template class bb_hash<std::string>;
#endif
