#ifndef rkh_hpp
#define rkh_hpp
#include <stdint.h>
#include<string>

#include<iostream>
using namespace std;
const int64_t HASH_MOD=1LL<<31; // Modulo used in rolling hash calculation
const int64_t HASH_BASE=1103515245; // Number base used in rolling hash calculation

/*

*/
class rabin_karp_hash {
    public:
        int64_t over;
        static const int64_t initial_hash = 0;
        rabin_karp_hash(int hash_length) {
            over=1;
            for(int i=0;i<hash_length-1;i++) over=(over*HASH_BASE)%HASH_MOD;
        }

        int64_t rolling_hash(int64_t current_hash, int next_character, int prev_character) const {
            if(prev_character != -1) {
                current_hash=(current_hash - ((prev_character*over)%HASH_MOD))%HASH_MOD;
                while(current_hash<0) current_hash += HASH_MOD;
            }
            current_hash=(current_hash * HASH_BASE)%HASH_MOD;
            current_hash=(current_hash + next_character)%HASH_MOD;
            return current_hash;
        }

};
class adler_hash {
int hash_length;
    public:
        static const int64_t initial_hash = 1;
        static const int mod = 65521;
        adler_hash(int hash_length) {
            this->hash_length=hash_length;
        }

        int64_t rolling_hash(int64_t current_hash, int next_character, int prev_character) const {
            int64_t A = current_hash & 65535;
            int64_t B = (current_hash >> 16) & 65535;
            if(prev_character != -1) {
                B =  B + A -(hash_length+1)*prev_character+next_character-1;
                A = A + next_character - prev_character;
            } else {
                A += next_character;
                B += A;
            }
            A %= mod;
            B %= mod;
            while (B<0) B+=mod;
            return (B<<16) | A;
        }

};
#endif
