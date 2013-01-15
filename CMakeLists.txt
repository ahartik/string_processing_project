set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
project (multiple_pattern_match)
add_executable(test test.cpp aho_corasick.cpp)
