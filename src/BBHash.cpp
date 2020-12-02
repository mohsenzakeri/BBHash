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
        bit_vector* A_bit_vec = new bit_vector(static_cast<uint64_t>(left_keys_count * gamma));
        bit_vector* C_bit_vec = new bit_vector(static_cast<uint64_t>(left_keys_count * gamma));
        A.push_back(A_bit_vec);
        C.push_back(C_bit_vec);
	for (uint64_t i = 0; i < size; i++) {
	    if (all_keys.read_bit(i) == 1) continue;

            XXH64_hash_t hash = XXH64(static_cast<void*>(&((*keys)[i])), sizeof((*keys)[i]), seed);
	    uint64_t hash_value = hash % (static_cast<uint64_t>(left_keys_count * gamma));
	    if (hash_value > A.back()->get_length()) std::cerr<< "Warning!\n";
	    if (hash_value > C.back()->get_length()) std::cerr<< "Warning!\n";
	    if (A.back()->read_bit(hash_value) == 0 and C.back()->read_bit(hash_value) == 0) {
		A.back()->set_index(hash_value);
	    } else if (A.back()->read_bit(hash_value) == 1 and C.back()->read_bit(hash_value) == 0) {
	        C.back()->set_index(hash_value);
		A.back()->reset_index(hash_value);
	    }
	}

	uint64_t current_keys_count = left_keys_count;
	for (uint64_t i = 0; i < size; i++) {
	    if (all_keys.read_bit(i) == 1) continue;

            XXH64_hash_t hash = XXH64(static_cast<void*>(&((*keys)[i])), sizeof((*keys)[i]), seed);
	    uint64_t hash_value = hash % (static_cast<uint64_t>(current_keys_count * gamma));
	    if (A.back()->read_bit(hash_value) == 1) {
		all_keys.set_index(i);
		left_keys_count -= 1;
	    }
	}
	std::cerr<<seed << " " << left_keys_count<< "  " << all_keys.num_ones() << "\n";
        
	seed +=1;
    }
}
