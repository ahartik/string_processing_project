#include"match.hpp"
#include<iomanip>
#include "rabin_karp_adler.hpp"
#include <cstdint>
#include<cstdlib>
#include<vector>
#include<iostream>
#include<map>
#include<utility>
using namespace std;
const int HASH_MOD=65521;
const int TABLE_SIZE=200003;



class rabin_karp_adler {
    public:
        bool check_match(const string& text, const string& pattern, int text_index) 
        {
            if(text.size() -text_index < pattern.size()) return false;
            for(int i=pattern.size()-1;i>=0;i--) 
                if(pattern[i] != text[text_index+i]) return false;
            return true;
        }
        uint32_t pattern_hash(const string& pattern, int hash_length) {
            uint32_t hash_a=1;
            uint32_t hash_b=0;
            for(int i=0;i<hash_length;i++) {
                hash_a+=(unsigned char)pattern[i];
                hash_b+=hash_a;
                hash_a%=HASH_MOD;
                hash_b%=HASH_MOD;
            }
            return (hash_b<<16) | hash_a;
        }
};

void rabin_karp_adler_matcher::match(
        const string& text,
        const vector<string> orig_patterns,
        match_vector& out) const
{
    rabin_karp_adler rk;
    int hash_sizes[]={1,10};
    vector<vector<int> > sect_patterns(2);
    for(int i=0;i<orig_patterns.size();i++) {
        bool t=false;
        for(int j=1;j<sect_patterns.size();j++) {
            if(orig_patterns[i].size()<hash_sizes[j]) {
                sect_patterns[j-1].push_back(i);
                t=true; 
                break;
            }
        }
        if(!t) sect_patterns[sect_patterns.size()-1].push_back(i);
    }
    for(int size=0;size<sect_patterns.size();size++) {
        vector<vector<pair<uint32_t,int> > > hash_table(TABLE_SIZE);
        vector<int>& patterns = sect_patterns[size];
        if(patterns.size()==0) continue;
        int hash_length=1<<20; 
        for(int i=0;i<patterns.size();i++) hash_length=min(hash_length,(int)orig_patterns[patterns[i]].size());
        cout<<"Hash Length: "<<hash_length<<endl;
        for(int i=0;i<patterns.size();i++) {
            uint32_t hash = rk.pattern_hash(orig_patterns[patterns[i]],hash_length);
            hash_table[hash%TABLE_SIZE].push_back(make_pair(hash,patterns[i]));

        }
        uint32_t current_a=1;
        uint32_t current_b=0;

        for(int i=0;i<text.size();i++) {
            if(i<hash_length) {
                current_a+=(unsigned char)text[i];
                current_b+=current_a;
                current_a %= HASH_MOD;
                current_b %= HASH_MOD;
            } else {
                int rem=(unsigned char)text[i-hash_length];
                int add=(unsigned char)text[i];
                current_b=current_b+current_a-(hash_length+1)*rem-1+add;
                current_a+=add-rem;
                current_b%=HASH_MOD;
                current_a%=HASH_MOD;

            }
            if(i>=hash_length-1) {
                uint32_t current_hash = (current_b << 16) | current_a;
                vector<pair<uint32_t,int> >& plist = hash_table[current_hash%TABLE_SIZE];
                bool found=false;
                for(auto it = plist.begin(); it!=plist.end();it++) {
                    uint32_t hash = it->first;
                    if(hash!=current_hash) {
                        continue;

                    }
                    int w = it->second;
                    if(rk.check_match(text,orig_patterns[w],i-hash_length+1)) {
                        out.push_back(::match(i-hash_length+1,w));
                    }
                }
            }
        }   
}


}

