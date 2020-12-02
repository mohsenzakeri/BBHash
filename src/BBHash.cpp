#include <stdio.h>
#include <string.h>

#include "BBHash.hpp"
#include "xxhash.h"

template <typename key_type>
bb_hash<key_type>::bb_hash(std::vector<key_type>* keys, uint64_t n, double g, uint64_t max_reg) : size(n), gamma(g) {
    if (keys->size() != size) {
        std::cerr<<"number of keys is different from the size\n";
	return;
    }

    max_regular_hash_count = max_reg;
    bit_vector all_keys(size);  
    uint64_t keys_hashed_count = 0;
    uint64_t left_keys_count = size;
    uint64_t seed = 0;
    while (left_keys_count > 0) {
	if (left_keys_count < max_regular_hash_count) {
	    uint64_t count = 0;
            for (uint64_t i = 0; i < size; i++) {
                if (all_keys.read_bit(i)) continue;
		reg_hash[(*keys)[i]] = count;
		count += 1;
	    }
	    break;
	}
	hash_mods.push_back(left_keys_count);
        bit_vector* A_bit_vec = new bit_vector(static_cast<uint64_t>(hash_mods.back() * gamma));
        bit_vector* C_bit_vec = new bit_vector(static_cast<uint64_t>(hash_mods.back() * gamma));

	for (uint64_t i = 0; i < size; i++) {
	    if (all_keys.read_bit(i) == 1) continue;

            XXH64_hash_t hash = XXH64(static_cast<void*>(&((*keys)[i])), sizeof((*keys)[i]), seed);
	    uint64_t hash_value = hash % (static_cast<uint64_t>(left_keys_count * gamma));
	    if (hash_value > A_bit_vec->get_length()) std::cerr<< "Warning!\n";
	    if (hash_value > C_bit_vec->get_length()) std::cerr<< "Warning!\n";
	    if (A_bit_vec->read_bit(hash_value) == 0 and C_bit_vec->read_bit(hash_value) == 0) {
		A_bit_vec->set_index(hash_value);
	    } else if (A_bit_vec->read_bit(hash_value) == 1 and C_bit_vec->read_bit(hash_value) == 0) {
	        C_bit_vec->set_index(hash_value);
		A_bit_vec->reset_index(hash_value);
	    }
	}
        delete C_bit_vec;
        rank_support* A_rank = new rank_support(A_bit_vec);
        A.push_back(A_rank);

	uint64_t current_keys_count = left_keys_count;
	for (uint64_t i = 0; i < size; i++) {
	    if (all_keys.read_bit(i) == 1) continue;

            XXH64_hash_t hash = XXH64(static_cast<void*>(&((*keys)[i])), sizeof((*keys)[i]), seed);
	    uint64_t hash_value = hash % (static_cast<uint64_t>(hash_mods.back() * gamma));
	    if (A_bit_vec->read_bit(hash_value) == 1) {
		all_keys.set_index(i);
		left_keys_count -= 1;
	    }
	}
	std::cerr<<seed << " " << left_keys_count<< "  " << all_keys.num_ones() << "\n";
        
	seed++;
    }
}

template <typename key_type>
uint64_t bb_hash<key_type>::query(key_type key) {
    uint64_t res = 0;
    for (size_t i = 0; i < A.size(); i++) {
        XXH64_hash_t hash = XXH64(static_cast<void*>(&key), sizeof(key), i);
	uint64_t hash_value = hash % (static_cast<uint64_t>(hash_mods.back() * gamma));
        if (A[i]->get_bit_vec()->read_bit(hash_value) == 1) {
	    return res + A[i]->rank1(hash_value);
	}
	
    }
}
