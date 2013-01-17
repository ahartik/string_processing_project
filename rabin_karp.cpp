#include"match.hpp"
#include "rabin_karp.hpp"
#include <cstdint>
#include<cstdlib>
#include "gmp.h"
#include "gmpxx.h"
#include<vector>
#include<iostream>
using namespace std;
typedef mpz_class bint;
const int HASH_MOD=257;
const int TABLE_SIZE=10007;
class rabin_karp {
    public:
    bool check_match(const string& text, const string& pattern, int text_index) 
    {
        if(text.size() -text_index < pattern.size()) return false;
        for(int i=pattern.size()-1;i>=0;i--) 
            if(pattern[i] != text[text_index+i]) return false;
        return true;
    }
    bint pattern_hash(const string& pattern, int hash_length) {
        bint hash=0;
        for(int i=0;i<hash_length;i++) {
            hash=hash*HASH_MOD;
            hash=hash+(unsigned char)pattern[i];
        }
        return hash;
    }
};

int toint(mpz_class a) {
    return mpz_get_si(a.get_mpz_t());
}

void rabin_karp_matcher::match(
        const string& text,
        const vector<string> orig_patterns,
        match_vector& out) const
{
    rabin_karp rk;
    int hash_sizes[]={1,5,10};
    vector<vector<int> > sect_patterns(3);
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
        vector<vector<int> > hash_table(TABLE_SIZE);
        vector<int>& patterns = sect_patterns[size];
        if(patterns.size()==0) continue;
        int hash_length=1<<20;
        for(int i=0;i<patterns.size();i++) hash_length=min(hash_length,(int)orig_patterns[patterns[i]].size());
        cout<<"Hash length: "<<hash_length<<endl;
        for(int i=0;i<patterns.size();i++) hash_table[toint(rk.pattern_hash(orig_patterns[patterns[i]],hash_length)%TABLE_SIZE)].push_back(patterns[i]);
        bint current_hash=0;
        bint over=1;

        for(int i=0;i<hash_length;i++) over*=HASH_MOD;

        for(int i=0;i<text.size();i++) {
            current_hash*=HASH_MOD;
            current_hash+=(unsigned char)text[i];
            current_hash%=over;
 /*           if(i<text.size()-1000) {
                if(text[i+1]=='A' && text[i+2]=='r' && text[i+3]=='t' && text[i+4]=='a') cout<<"fuc "<<text[i]<<", "<<text[i-1]<<", "<<(int)((unsigned char)text[i])<<endl;
            }
*/
            vector<int>& plist = hash_table[toint(current_hash%TABLE_SIZE)];
            for(auto it = plist.begin(); it!=plist.end();it++) {

                if(rk.check_match(text,orig_patterns[*it],i-hash_length+1)) {
                    int a=*it;
                    out.push_back(::match(i-hash_length+1,a));
                }
            }
        }   
    }


    }

