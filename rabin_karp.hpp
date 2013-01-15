#ifndef RABIN_KARP_H
#define RABIN_KARP_H
#include"match.hpp"

class rabin_karp_matcher : public multiple_matcher
{
public:
    
    void match(const std::string& text,
               const std::vector<std::string> patterns,
               match_vector& out) const;
    const char* name() const
    {
        return "Rabin-Karp";
    }
    rabin_karp_matcher(){}
    ~rabin_karp_matcher() {}
};
#endif
