cmake_minimum_required(VERSION 2.6)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
add_definitions(-std=c++0x)
project (multiple_pattern_match)
set(CMAKE_BUILD_TYPE Release)
set (MATCHER_SRC rabin_karp.cpp aho_corasick.cpp)
set(CMAKE_CXX_FLAGS "-O3 -Wall -march=native -mtune=native -Wno-sign-compare")
add_executable(test test.cpp ${MATCHER_SRC})
add_executable(performance performance.cpp ${MATCHER_SRC})
add_executable(science science.cpp ${MATCHER_SRC})
