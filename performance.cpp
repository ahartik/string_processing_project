#include"matcher_list.hpp"
#include"timef.h"
#include<cstdlib>
#include<string>
#include<iostream>
#include "util.hpp"
#include<set>
using namespace std;

string random_string(int len)
{
    string ret;
    for (int i = 0; i < len; i++)
    {
        int r = rand()%10;
        if (r)ret.push_back('A');
        else ret.push_back('B');
    }
    return ret;
}

const int N = 200000;
const int M = 100;

string text = random_string(N);
vector<string> patterns;

void matcher_time(const multiple_matcher* m)
{
    cout << "=== "<<m->name()<<" ===\n";
    double t = timef();
    match_vector matches;
    m->match(text, patterns, matches);
    t = timef() - t;
    cout<<"\n= Results =\n";
    cout << matches.size()<<" matches\n";
    cout << t << " seconds\n";
    cout<<"------------------------------\n\n";
}

void print_diff(match_vector& a, match_vector& b) {
    set<match> aset,bset;
    for(auto i = a.begin();i!=a.end();i++) aset.insert(*i);
    for(auto i = b.begin();i!=b.end();i++) bset.insert(*i);
    for(auto i = a.begin(); i != a.end();i++) {
        if(bset.count(*i)==0) cout<<"A has "<<(*i).start<<": "<<patterns[(*i).pattern]<<endl;
        
    }
    for(auto i = b.begin();i!=b.end();i++) {
        if(aset.count(*i)==0) cout<<"B has "<<(*i).start<<": "<<patterns[(*i).pattern]<<endl;
    }
}

int main(int argc, char** argv)
{
   // srand(time(0));
    if(argc==3) {
        cout<<"Text: "<<argv[1]<<"\n";
        cout<<"Patterns: "<<argv[2]<<"\n";

        text = read_text(argv[1]).substr(0,1000000);
        
        vector<string> words = read_words(argv[2]);
        words.resize(30000);
        int i=0;
        string current="";
        while(i<words.size()) {
            if(current=="") current=words[i++];
            else if(i%3==0) {
                patterns.push_back(current);
                current="";
            } else {
                current+=" "+words[i++];
            }
        }
      /*  match_vector a,b;
        matcher_list[0]->match(text, patterns, a);
        matcher_list[1]->match(text, patterns, b);
        print_diff(a,b);*/
    } else {

        for (int j = 0; j < M; j++)
        {
            patterns.push_back(random_string(10 + (j * N) / (M * 20)));
        }
        patterns[0]="AAAAAAAAAA";
        for(int i=0;i<10;i++) text[i]='A';
    }
    cout << "Generated\n";

    for (int i = 0; i < matcher_list_size; i++)
    {
        matcher_time(matcher_list[i]);
    }
}
