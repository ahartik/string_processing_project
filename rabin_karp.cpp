#include"match.hpp"
#include "rabin_karp.hpp"
#include <cstdint>
#include<cstdlib>
#include<vector>
#include<iostream>
using namespace std;
const uint64_t HASH_MOD=257;
const uint64_t TABLE_SIZE=10007;
class rabin_karp {
    public:
    bool check_match(const string& text, const string& pattern, int text_index) 
    {
        if(text.size() -text_index < pattern.size()) return false;
        for(int i=pattern.size()-1;i>=0;i--) 
            if(pattern[i] != text[text_index+i]) return false;
        return true;
    }
    uint64_t pattern_hash(const string& pattern, int hash_length) {
        uint64_t hash=0;
        for(int i=0;i<hash_length;i++) {
            hash=hash*HASH_MOD;
            hash=hash+pattern[i];
        }
        return hash;
    }
};



void rabin_karp_matcher::match(
        const string& text,
        const vector<string> patterns,
        match_vector& out) const
{
                rabin_karp rk;
            vector<vector<int> > hash_table(TABLE_SIZE);
            int hash_length=5;
            for(int i=0;i<patterns.size();i++) hash_length=min(hash_length,(int)patterns[i].size());
            for(int i=0;i<patterns.size();i++) hash_table[rk.pattern_hash(patterns[i],hash_length)%TABLE_SIZE].push_back(i);
            cout<<"hash length: "<<hash_length<<endl;
            uint64_t current_hash=0;
            uint64_t over=1;
            
            for(int i=0;i<hash_length;i++) over*=HASH_MOD;
            
            for(int i=0;i<text.size();i++) {
                current_hash*=HASH_MOD;
                current_hash+=text[i];
                current_hash%=over;
                
                vector<int>& plist = hash_table[current_hash%TABLE_SIZE];
                for(auto it = plist.begin(); it!=plist.end();it++) {
                    if(rk.check_match(text,patterns[*it],i-hash_length+1)) {
                        int a=*it;
                        out.push_back(::match(i-hash_length+1,a));
                    }
                }
            }   


        }

