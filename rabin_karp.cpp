#include"match.hpp"
#include<forward_list>
#include<iomanip>
#include "rabin_karp.hpp"
#include <cstdint>
#include<cstdlib>
#include<vector>
#include<iostream>
#include<map>
#include<utility>

using namespace std;

const int TABLE_SIZE=200003; // Hash table size
class rabin_karp {
    public:

    bool check_match(const string& text, const string& pattern, int text_index) 
    {
        if(text.size() -text_index < pattern.size()) return false;
        for(int i=pattern.size()-1;i>=0;i--) 
            if(pattern[i] != text[text_index+i]) return false;
        return true;
    }
};

template <typename T>
int64_t rabin_karp_matcher<T>::pattern_hash(string pattern, int hash_length) const {
    T hash_function(hash_length);
    int64_t current_hash=hash_function.initial_hash;
    for(int i=0;i<hash_length;i++) current_hash =  hash_function.rolling_hash(current_hash,pattern[i],-1);
    return current_hash;
}

template <typename T>
void rabin_karp_matcher<T>::match(
        const string& text,
        const vector<string> orig_patterns,
        match_vector& out) const
{
    rabin_karp rk;

    int hash_sizes[]={1,5,10,20,30,40,50,60,70,80,90,100}; // Fixed pattern lengths

    vector<vector<int> > sect_patterns(12); 

    // Divide patterns into groups by pattern length (according to hash_sizes)
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

    // For each pattern lenght group:
    for(int size=0;size<sect_patterns.size();size++) {
        
        vector<forward_list<pair<int64_t,int> > > hash_table(TABLE_SIZE);
        vector<int>& patterns = sect_patterns[size];
        if(patterns.size()==0) continue;
        int hash_length=1<<20; 
        
        // Calculate the minimum pattern length for this pattern group
        for(int i=0;i<patterns.size();i++) hash_length=min(hash_length,(int)orig_patterns[patterns[i]].size());
        cout<<"Hash Length: "<<hash_length<<endl;

        // Calculate hashes
        for(int i=0;i<patterns.size();i++) {
            T hash_function(hash_length);
            
            int64_t hash = pattern_hash(orig_patterns[patterns[i]],hash_length);
            hash_table[hash%TABLE_SIZE].push_front(make_pair(hash,patterns[i]));

        }

        T hash_function(hash_length);
        int64_t current_hash=hash_function.initial_hash;
        
        // Calculate the rolling hash for each text position and check for matches
        for(int i=0;i<text.size();i++) {
            int next_character=(unsigned char)text[i];
            int prev_character=-1;
            if(i >= hash_length) prev_character = (unsigned char) text[i-hash_length];
            current_hash = hash_function.rolling_hash(current_hash,next_character,prev_character);
            if(i>=hash_length-1) {
                forward_list<pair<int64_t,int> >& plist = hash_table[current_hash%TABLE_SIZE];
                bool found=false;
                // For each pattern for which [hash % TABLE_SIZE] equals [current_hash % TABLE_SIZE]:
                for(auto it = plist.begin(); it!=plist.end();it++) {
                    
                    int64_t hash = it->first;
                    // If [hash] != [current_hash], this is a false alarm
                    if(hash!=current_hash) {
                        continue;
                    }

                    // If [hash] == [current_hash], try to match the pattern
                    int w = it->second;
                    if(rk.check_match(text,orig_patterns[w],i-hash_length+1)) {
                        out.push_back(::match(i-hash_length+1,w));
                    } 
                }
            }
        }   
    }
}

#include "rabin_karp_hash.hpp"
template class rabin_karp_matcher<rabin_karp_hash>;
template class rabin_karp_matcher<adler_hash>;

