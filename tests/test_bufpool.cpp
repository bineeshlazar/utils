#include <algorithm>
#include <random>
#include <array>
#include <vector>
#include <cstring>

#include "catch.hpp"
#include "../bufpool.hpp"


TEST_CASE( "Verify bufpool operation", "[bufpool]" ) {
	const int size = 1024;
	const int iter = 5000;

	//create a array with random data
	std::array<char, size> rand_arr;
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uid(0,9);

	std::generate(rand_arr.begin(), rand_arr.end(), [&]() { return uid(dre); });

	utils::bufpool<char, size> pool;

	std::vector<std::shared_ptr<char>> ptrs;

	for(int i = 0; i < iter ; i++) {
		auto ptr = pool.alloc();
		ptrs.push_back(ptr);
		std::memcpy(ptr.get(), rand_arr.data(), size);
	}

	int ret = 0;
	for (int i = 0; i < iter ; i++){
		ret = std::memcmp(ptrs[i].get(), rand_arr.data(), size);
		REQUIRE(ret == 0);
	}

}
