#include"match.hpp"
#include<iomanip>
#include "rabin_karp.hpp"
#include <cstdint>
#include<cstdlib>
#include<vector>
#include<iostream>
#include<map>
#include<utility>
using namespace std;
const int64_t HASH_MOD=1LL<<31;
const int64_t HASH_BASE=1103515245;
const int TABLE_SIZE=200003;



class rabin_karp {
    public:
    bool check_match(const string& text, const string& pattern, int text_index) 
    {
        if(text.size() -text_index < pattern.size()) return false;
        for(int i=pattern.size()-1;i>=0;i--) 
            if(pattern[i] != text[text_index+i]) return false;
        return true;
    }
    int64_t pattern_hash(const string& pattern, int hash_length) {
        int64_t hash=0;
        for(int i=0;i<hash_length;i++) {
            hash = (hash*HASH_BASE)%HASH_MOD;
            hash=(hash+((unsigned char)pattern[i]))%HASH_MOD;
        }
        while(hash<0) hash+=HASH_MOD;
        return hash;
    }
};

void rabin_karp_matcher::match(
        const string& text,
        const vector<string> orig_patterns,
        match_vector& out) const
{
    rabin_karp rk;
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
        int mismatch=0;
        int modmismatch=0;
        int hashmismatch=0;
        int suffixmismatch=0;
        int correct=0;
        int hashcorrect=0;
        vector<vector<pair<int64_t,int> > > hash_table(TABLE_SIZE);
        vector<int>& patterns = sect_patterns[size];
        if(patterns.size()==0) continue;
        int hash_length=1<<20; 
        for(int i=0;i<patterns.size();i++) hash_length=min(hash_length,(int)orig_patterns[patterns[i]].size());
        cout<<"Hash Length: "<<hash_length<<endl;
        for(int i=0;i<patterns.size();i++) {
            int64_t hash = rk.pattern_hash(orig_patterns[patterns[i]],hash_length);
            hash_table[hash%TABLE_SIZE].push_back(make_pair(hash,patterns[i]));

        }
        int64_t current_hash=0;
        int64_t over=1;
        for(int i=0;i<hash_length-1;i++) over=(over*HASH_BASE)%HASH_MOD;
        for(int i=0;i<text.size();i++) {
            if(i<hash_length) {
                current_hash=(current_hash*HASH_BASE)%HASH_MOD;
                current_hash=(current_hash+((unsigned char)text[i]))%HASH_MOD;
            } else {
                int rem=(unsigned char)text[i-hash_length];
                int add=(unsigned char)text[i];
                current_hash=(current_hash - ((rem*over)%HASH_MOD))%HASH_MOD;
                while(current_hash<0) current_hash += HASH_MOD;
                current_hash=(current_hash * HASH_BASE)%HASH_MOD;
                current_hash=(current_hash + add)%HASH_MOD;
            }
            if(i>=hash_length-1) {
                vector<pair<int64_t,int> >& plist = hash_table[current_hash%TABLE_SIZE];
                bool found=false;
                for(auto it = plist.begin(); it!=plist.end();it++) {
                    int64_t hash = it->first;
                    if(hash!=current_hash) {
                        modmismatch++;
                        continue;

                    }
                    int w = it->second;
                    if(rk.check_match(text,orig_patterns[w],i-hash_length+1)) {
                        out.push_back(::match(i-hash_length+1,w));
                        correct++;
                    } else {
                        bool t=true;
                        for(int j=0;j<hash_length;j++)  {
                            if(text[j+i-hash_length+1]!=orig_patterns[w][j]) t=false;
                        }
                        if(t) suffixmismatch++;
                        else hashmismatch++;
                        
                    }
                }
            }
        }   
        double total=0.01*(suffixmismatch+hashmismatch+modmismatch+correct);
        cout.precision(1);
        cout<<"Correct: "<<fixed<<correct/total<<"%\n";
        cout<<"Mod mismatch: "<<fixed<<modmismatch/total<<"%\n";
        cout<<"Hash mismatch: "<<fixed<<hashmismatch/total<<"%\n";
        cout<<"Suffix mismatch: "<<fixed<<suffixmismatch/total<<"%\n";
        cout<<endl;
        cout.precision(4);
    }


}

