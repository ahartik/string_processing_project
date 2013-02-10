#pragma once
#include <stdint.h>
#include <cstring>
#include <cstddef>
#include <ostream>
#include <utility>


template<size_t Size>
class bitset
{
    static const size_t WordCount = (Size+63)/64;
    uint64_t words[WordCount];

    static int popcnt(uint64_t x)
    {
        return __builtin_popcountll(x);
    }
    static size_t bitrank(uint64_t x, size_t bit)
    {
        uint64_t mask = ((1ULL<<bit)-1);
        return popcnt(x & mask);
    }
    public:
    bitset()
    {
        memset(words, 0 , sizeof(words));
    }
    
    class reference
    {
        uint64_t* word;
        int offset;
        public:
        reference(uint64_t* w, int o):
            word(w),
            offset(o)
        {}
        reference& 
        operator=(bool b)
        {
            if (b)
            {
                (*word)|=1ULL<<offset;
            }
            else
            {
                (*word) &= ~(1ULL<<offset);
            }
            return *this;
        }
        operator bool() const
        {
            return ((*word)>>offset)&1;
        }
    };
    size_t rank(size_t bit) const
    {
        size_t w = bit / 64;
        size_t o = bit % 64;
        size_t ret = 0;
        for (size_t i = 0; i < w; i++)
            ret += popcnt(words[i]);
        ret += bitrank(words[w], o);
        return ret;
    }
    reference operator[](size_t x)
    {
        return reference(&words[x/64], x % 64);
    }
    bool operator[](size_t x) const 
    {
        return (words[x/64]>>(x%64))&1;
    }
    // Return next set bit from position i
    // if i'th bit is set, return i
    size_t next_set(size_t i)const
    {
        size_t w = i / 64;
        size_t o = i % 64;
        uint64_t mask = ~((1ULL<<o)-1);
        for (; w < WordCount; w++)
        {
            uint64_t m = words[w]&mask;
            mask = -1ULL;
            if (m)
            {
                int f = ffsll(m);
                return w*64+f-1;
            }
        }
        return Size;
    }
};


template<size_t Size>
std::ostream& operator<<(std::ostream& out, const bitset<Size>& b)
{
    for (size_t i = 0; i < Size; i++)
        out << b[i];
    return out;
}
