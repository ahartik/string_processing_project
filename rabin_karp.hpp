#ifndef RABIN_KARP_H
#define RABIN_KARP_H
#include"match.hpp"
#include<string>
template <typename Hasher>
class rabin_karp_matcher : public multiple_matcher
{

    int64_t pattern_hash(std::string pattern, int hash_length) const;
    bool check_match(const std::string& text, const std::string& pattern, int text_index) const;

public:
    void match(const std::string& text,
               const std::vector<std::string>& patterns,
               match_vector& out) const;
    std::string name() const
    {
        Hasher h(0);
        return std::string("Rabin-Karp (")+h.name()+")";
    }
    rabin_karp_matcher(){}
    ~rabin_karp_matcher() {}
};
#endif
