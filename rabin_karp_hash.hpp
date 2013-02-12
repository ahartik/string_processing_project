#ifndef rkh_hpp
#define rkh_hpp
#include <stdint.h>
#include<string>

#include<iostream>
using namespace std;
/*
    Rabin-Karp hash function: hash( T[1..N] ) == T[1] * base^N + T[2] * base^(N-1) + ... + T[N]  MOD modulo
*/
class rabin_karp_hash {
    public:
        static const int64_t base=33; //1103515245; // Number base used in rolling hash calculation
        static const int64_t modulo=1LL<<31; // Modulo used in rolling hash calculation
        int64_t over;
        static const int64_t initial_hash = 0;
        rabin_karp_hash(int hash_length) {
            // Calculate over = base^N for later use
            over=1;
            for(int i=0;i<hash_length-1;i++) over=(over*base)%modulo;
        }
        // Calculates the new hash value using the previous hash, next character and the character to be removed (-1 if the hash isn't complete yet)
        int64_t rolling_hash(int64_t current_hash, int next_character, int prev_character) const {
            if(prev_character != -1) {
                current_hash=(current_hash - ((prev_character*over)%modulo))%modulo;
                while(current_hash<0) current_hash += modulo;
            }
            current_hash=(current_hash * base)%modulo;
            current_hash=(current_hash + next_character)%modulo;
            return current_hash;
        }

        std::string name() const
        {
            return "Modulo";
        }
};
/*
 Adler-32: hash( T[1..N] ) = B*65536 + A, where
                A =  1 + T[1] + T[2] + ... + T[N] MOD 65521
                B = N + N * T[1] + (N-1) * T[2] + ... + (1) * T[N]
 
 */
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

        std::string name() const
        {
            return "Adler";
        }

};
#endif
