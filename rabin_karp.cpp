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

const int64_t HASH_MOD=1LL<<31; // Modulo used in rolling hash calculation
const int64_t HASH_BASE=1103515245; // Number base used in rolling hash calculation
const int TABLE_SIZE=200003; // Hash table size



class rabin_karp {
    public:

    // Check if pattern[0..N) matches text[text_index ... text_index+N)
    bool check_match(const string& text, const string& pattern, int text_index) 
    {
        if(text.size() -text_index < pattern.size()) return false;
        for(int i=pattern.size()-1;i>=0;i--) 
            if(pattern[i] != text[text_index+i]) return false;
        return true;
    }

    // calculate the hash of given pattern up to [hash_length] characters
    int64_t pattern_hash(const string& pattern, int hash_length) {
        int64_t hash=0;
        for(int i=0;i<hash_length;i++) {
            // hash <- hash * base + next_character
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
            int64_t hash = rk.pattern_hash(orig_patterns[patterns[i]],hash_length);
            hash_table[hash%TABLE_SIZE].push_front(make_pair(hash,patterns[i]));

        }

        int64_t current_hash=0;
        
        // Calculate base^hash_length
        int64_t over=1;
        for(int i=0;i<hash_length-1;i++) over=(over*HASH_BASE)%HASH_MOD;
        
        // Calculate the rolling hash for each text position and check for matches
        for(int i=0;i<text.size();i++) {
            int next_character=(unsigned char)text[i];
            if(i<hash_length) { 
                // If the hash does not contain [hash_length] characters: hash <- hash + [add]
                current_hash=(current_hash*HASH_BASE)%HASH_MOD;
                current_hash=(current_hash+next_character)%HASH_MOD;
            } else { 
                // Else: hash <- (hash - previous_character * base^hash_length) * base + new_character
                
                //Character to be removed from current_hash
                int previous_character=(unsigned char)text[i-hash_length];
                //Character to be next_charactered to current_hash

                //Previous_characterove [previous_character] from hash
                current_hash=(current_hash - ((previous_character*over)%HASH_MOD))%HASH_MOD;
                while(current_hash<0) current_hash += HASH_MOD;

                // hash <- hash * base + [next_character]
                current_hash=(current_hash * HASH_BASE)%HASH_MOD;
                current_hash=(current_hash + next_character)%HASH_MOD;
            }
            // Check for matches:
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

