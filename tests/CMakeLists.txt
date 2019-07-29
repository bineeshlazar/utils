cmake_minimum_required(VERSION 3.2)

add_executable(test 
test_main.cpp
test_channel.cpp
)

target_link_libraries(test pthread)