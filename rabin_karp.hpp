#ifndef RABIN_KARP_H
#define RABIN_KARP_H
#include"match.hpp"
template <typename T>
class rabin_karp_matcher : public multiple_matcher
{

    int64_t pattern_hash(std::string pattern, int hash_length) const;

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
