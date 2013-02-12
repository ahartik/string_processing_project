set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
add_definitions(-std=c++0x)
project (multiple_pattern_match)
set(CMAKE_BUILD_TYPE Release)
set (MATCHER_SRC rabin_karp.cpp aho_corasick.cpp)
set(CMAKE_CXX_FLAGS "-O2 -Wall -march=native -Wno-sign-compare")
>>>>>>> bbb5606cdfa436685315c6fe96e87390cf1d4c67
add_executable(test test.cpp ${MATCHER_SRC})
add_executable(performance performance.cpp ${MATCHER_SRC})
