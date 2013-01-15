#pragma once

#include<vector>
#include"match.hpp"
#include"aho_corasick.hpp"

static aho_corasick_matcher ac_match;

multiple_matcher* matcher_list[] = 
{
    &ac_match
};

const int matcher_list_size = sizeof(matcher_list) /
                              sizeof(matcher_list[0]);

