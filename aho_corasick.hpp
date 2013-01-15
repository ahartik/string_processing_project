#pragma once
#include"match.hpp"

class aho_corasick_matcher : public multiple_matcher
{
public:
    
    void match(const std::string& text,
               const std::vector<std::string> patterns,
               match_vector& out) const;
    const char* name() const
    {
        return "Aho-Corasick";
    }
};
