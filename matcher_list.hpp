#pragma once

#include<vector>
#include"match.hpp"
#include"aho_corasick.hpp"
#include"rabin_karp.hpp"
#include "rabin_karp_hash.hpp"

extern template class rabin_karp_matcher<rabin_karp_hash>;

static aho_corasick_matcher ac_match;
static rabin_karp_matcher<rabin_karp_hash> rk_match;
static rabin_karp_matcher<adler_hash> rka_match;

multiple_matcher* matcher_list[] = 
{
    &ac_match,
    &rk_match,
    &rka_match,
};

const int matcher_list_size = sizeof(matcher_list) /
                              sizeof(matcher_list[0]);

