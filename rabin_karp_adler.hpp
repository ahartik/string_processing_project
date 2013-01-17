#ifndef RABIN_KARP_ADLER_H
#define RABIN_KARP_ADLER_H
#include"match.hpp"

class rabin_karp_adler_matcher : public multiple_matcher
{
public:
    
    void match(const std::string& text,
               const std::vector<std::string> patterns,
               match_vector& out) const;
    const char* name() const
    {
        return "Rabin-Karp-Adler";
    }
    rabin_karp_adler_matcher(){}
    ~rabin_karp_adler_matcher() {}
};
#endif
