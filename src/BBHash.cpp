#include <stdio.h>
#include <string.h>
#include <limits>
#include <sys/stat.h> 

#include "BBHash.hpp"
#include "xxhash.h"

template <typename key_type>
bb_hash<key_type>::bb_hash(std::vector<key_type>* keys, uint64_t n, double g, uint64_t b) : size(n), gamma(g), bit_vec_count(b) {
    if (keys->size() != size) {
        std::cerr<<"number of keys is different from the size\n";
	return;
    }

    bit_vector all_keys(size);  
    uint64_t keys_hashed_count = 0;
    uint64_t left_keys_count = size;
    uint64_t seed = 0;
    for(size_t j = 0; j < bit_vec_count and left_keys_count > 0; j++) {
	hash_mods.push_back(left_keys_count);

        bit_vector* A_bit_vec = new bit_vector(static_cast<uint64_t>(hash_mods.back() * gamma));
        bit_vector* C_bit_vec = new bit_vector(static_cast<uint64_t>(hash_mods.back() * gamma));

	for (uint64_t i = 0; i < size; i++) {
	    if (all_keys.read_bit(i) == 1) continue;

	    XXH64_hash_t hash;
	    auto xx = (*keys)[i];
	    if (typeid(xx) == typeid(std::string)) {
    	        hash = XXH64(reinterpret_cast<void*>(const_cast<char*>(xx.data())), xx.length(), seed);
            } else {
                hash = XXH64(static_cast<void*>(&xx), sizeof(xx), seed);
	    }
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

	    auto xx = (*keys)[i];
	    XXH64_hash_t hash;
	    if (typeid(xx) == typeid(std::string)) {
    	        hash = XXH64(reinterpret_cast<void*>(const_cast<char*>(xx.data())), xx.length(), seed);
	    } else {
                hash = XXH64(static_cast<void*>(&xx), sizeof(xx), seed);
	    }
	    uint64_t hash_value = hash % (static_cast<uint64_t>(hash_mods.back() * gamma));
	    if (A_bit_vec->read_bit(hash_value) == 1) {
		all_keys.set_index(i);
		left_keys_count -= 1;
	    }
	}
	//std::cerr<<seed << " " << left_keys_count<< "  " << all_keys.num_ones() << "\n";
        
	seed++;
    }

    if (left_keys_count > 0) {
        uint64_t count = 0;
        for (uint64_t i = 0; i < size; i++) {
           if (all_keys.read_bit(i)) continue;
           reg_hash[(*keys)[i]] = count;
      	   count += 1;
        }
    }
}

template <typename key_type>
uint64_t bb_hash<key_type>::query(key_type key) {
    uint64_t res = 0;
    for (size_t i = 0; i < A.size(); i++) {
        XXH64_hash_t hash;
        if (typeid(key) == typeid(std::string)) {
            hash = XXH64(reinterpret_cast<void*>(const_cast<char*>(key.data())), key.length(), i);
        } else {
            hash = XXH64(static_cast<void*>(&key), sizeof(key), i);
	}
	uint64_t hash_value = hash % (static_cast<uint64_t>(hash_mods[i] * gamma));
        if (A[i]->get_bit_vec()->read_bit(hash_value) == 1) {
	    return res + A[i]->rank1(hash_value);
	}
        res += A[i]->rank1(static_cast<uint64_t>(hash_mods.back() * gamma) - 1);
    }
    if (reg_hash.find(key) != reg_hash.end())
        return res + reg_hash[key];
    else
	return std::numeric_limits<uint64_t>::max();
}

template <typename key_type>
uint64_t bb_hash<key_type>::num_bits() {
    uint64_t res = 0;
    for (auto a : A)
        res += a->get_bit_vec()->get_length();
    return res;
}

template <typename key_type>
void bb_hash<key_type>::save(char* output_dir) {
    mkdir(output_dir,0777);
    uint64_t index = 0;
    for (auto& a : A) {
	std::string file_name = static_cast<std::string>(output_dir) + "/" + std::to_string(index) + ".rank";
        a->save(file_name);
	index += 1;
    }
    std::cerr<<"A vectors are stored.\n";
    std::string fname = static_cast<std::string>(output_dir) + "/bbhash.bin";
    std::ofstream fout(fname, std::ios::out | std::ios::binary);
    fout.write((char*) &size, sizeof(size));
    fout.write((char*) &gamma, sizeof(gamma));
    fout.write((char*) &bit_vec_count, sizeof(bit_vec_count));
    uint64_t num_bit_vecs = hash_mods.size();
    fout.write((char*) &num_bit_vecs, sizeof(num_bit_vecs));
    for (uint64_t i = 0; i < hash_mods.size(); i++) {
        uint64_t curr_mod = hash_mods[i];
        fout.write((char*) &curr_mod, sizeof(curr_mod));
    }
    uint64_t reg_hash_size = reg_hash.size();
    fout.write((char*) &reg_hash_size, sizeof(reg_hash_size));
    for (auto& kv : reg_hash) {
	key_type key = kv.first;
	uint64_t val = kv.second;
        size_t keylen = key.size();
        fout.write((char*) &keylen, sizeof(keylen));
        fout.write(key.c_str(), keylen); 
        fout.write((char*) &val, sizeof(val));
    }
    std::cerr<<"Rest of the hash function files are stored.\nIndex saved in "<<output_dir<<"\n";
    fout.close();
}



template <typename key_type>
void bb_hash<key_type>::load(char* index_dir) {
    std::string fname = static_cast<std::string>(index_dir) + "/bbhash.bin";
    std::ifstream fin(fname, std::ios::in | std::ios::binary);
    fin.read((char*) &size, sizeof(size));
    fin.read((char*) &gamma, sizeof(gamma));
    fin.read((char*) &bit_vec_count, sizeof(bit_vec_count));
    uint64_t hash_count = 0;
    fin.read((char*) &hash_count, sizeof(hash_count));
    uint64_t curr_mod = 0;
    std::cerr<<"here " << hash_count << "\n";
    for (uint64_t i = 0; i < hash_count; i++) {
        fin.read((char*) &curr_mod, sizeof(curr_mod));
	hash_mods.push_back(curr_mod);
    }
    uint64_t reg_hash_size = 0;
    fin.read((char*) &reg_hash_size, sizeof(reg_hash_size));
    std::cerr<<"here " << reg_hash_size << "\n";
    for (uint64_t i = 0; i < reg_hash_size; i++) {
	std::string key;
        uint64_t val;
	size_t keylen = 0;
	fin.read((char*) &keylen, sizeof(keylen));
	char* key_char = new char[keylen + 1];
	fin.read(key_char, keylen);
	key_char[keylen] = '\0';
	key = key_char;
        fin.read((char*) &val, sizeof(val));
	reg_hash[key] = val;
    }
    fin.close();

    std::cerr<< "the index is being loaded.\n";
    for (uint32_t i = 0; i < hash_count; i++) {
	std::string file_name = static_cast<std::string>(index_dir) + "/" + std::to_string(i) + ".rank";
        rank_support* new_rank = new rank_support();
	new_rank->load(file_name);
	A.push_back(new_rank);
    }
    std::cerr<<"All the bitvectors are loaded.\nIndex loaded successfully!\n";
}
