#include <chrono>

#include "BBHash.hpp"

int main() {
    for (uint64_t i = 0; i < 3; i++) {
        uint64_t n = 10000*std::pow(10,i+1);
	double gamma = 1;
	std::vector<std::string> keys;
	for (uint64_t j = 0; j < n; j++) {
            keys.push_back(std::to_string(std::rand()));
	}
	bb_hash<std::string> bb_test(&keys, n, gamma);
	auto t1 = std::chrono::high_resolution_clock::now();
        for (uint64_t j = 0; j < n; j++) {
	    auto res = bb_test.query(keys[j]);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

	uint64_t alien_count = 0;
	for (size_t j = 0; j < 1000; j++) {
	    uint64_t res = bb_test.query(std::to_string(std::rand()));
	    if (res < n)
		alien_count++;
	}

	std::cerr<< "BBHash with n: " << n << " and gamma: " << gamma << " time:" << duration << " num_bits: " <<
		bb_test.num_bits() << " reg_map_count:" << bb_test.reg_hash_count() << " alien_count:" << alien_count <<"\n";


	std::unordered_map<std::string, uint64_t> umap_test;
        for (uint64_t j = 0; j < n; j++) {
            umap_test[keys[j]] = j;
	}
        t1 = std::chrono::high_resolution_clock::now();
        for (uint64_t j = 0; j < n; j++) {
	    auto res = umap_test[keys[j]];
	}
	t2 = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
	std::cerr<< "Unorderedmap with n: " << n << " time:" << duration << "\n";

	//test with different gamma values
	for (size_t k = 2; k <= 10; k+=2) {
            gamma = k;	
      	    bb_hash<std::string> bb_test2(&keys, n, gamma);
	    t1 = std::chrono::high_resolution_clock::now();
            for (uint64_t j = 0; j < n; j++) {
	        auto res = bb_test2.query(keys[j]);
	    }
	    t2 = std::chrono::high_resolution_clock::now();
	    duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
            alien_count = 0;
	    for (size_t j = 0; j < 1000; j++) {
	        uint64_t res = bb_test.query(std::to_string(std::rand()));
	        if (res < n)
		    alien_count++;
	    }
            std::cerr<< "BBHash with n: " << n << " and gamma: " << gamma << " time:" << duration << " num_bits: " <<
		bb_test.num_bits() << " reg_map_count:" << bb_test.reg_hash_count() << " alien_count:" << alien_count <<"\n";
	}
    }
}
