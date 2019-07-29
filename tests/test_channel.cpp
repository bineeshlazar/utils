/*
 * test_channel.cpp
 *
 *  Created on: 26-Jul-2019
 *      Author: bineesh
 */


#include <future>
#include <string>

#include "catch.hpp"
#include "../channel.hpp"
#include "../threadpool.hpp"

TEST_CASE( "Verify int-channel operation", "[channel] [thpool]" ) {

	utils::threadpool tpool(1);
	utils::channel<int> ch;
	auto ch_ptr = &ch;
	auto itr_count = 10000;

	tpool.post([ch_ptr, itr_count]() -> void {
		for (int i = 0; i < itr_count ; i++) {
			ch_ptr->put(std::move(i));
		}
	});

	for (int i = 0; i < itr_count ; i++){
		int val;
		ch.get(val);
		REQUIRE(val == i);
	}
}

TEST_CASE( "Verify string-channel operation", "[channel] [thpool]" ) {

	utils::threadpool tpool(1);
	utils::channel<std::string> ch;
	auto ch_ptr = &ch;
	auto itr_count = 10000;

	tpool.post([ch_ptr, itr_count]() -> void {
		for (int i = 0; i < itr_count ; i++) {
			ch_ptr->put(std::to_string(i));
		}
	});

	for (int i = 0; i < itr_count ; i++){
		std::string val;
		ch.get(val);
		REQUIRE(val == std::to_string(i));
	}
}
