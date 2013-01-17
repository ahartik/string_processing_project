#pragma once

#include<vector>
#include"match.hpp"
#include"aho_corasick.hpp"
#include"rabin_karp.hpp"
#include"rabin_karp_adler.hpp"

static aho_corasick_matcher ac_match;
static rabin_karp_matcher rk_match;
static rabin_karp_adler_matcher rka_match;

multiple_matcher* matcher_list[] = 
{
    &ac_match,
    &rk_match,
    &rka_match,
};

const int matcher_list_size = sizeof(matcher_list) /
                              sizeof(matcher_list[0]);

