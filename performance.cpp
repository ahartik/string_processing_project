#include"matcher_list.hpp"
#include"timef.h"
#include<cstdlib>
#include<string>
#include<iostream>
#include "util.hpp"
#include<set>
using namespace std;
vector<string> patterns;
string text;

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

int main(int argc, char** argv)
{
    int matcher_test=-1;
    if(argc>=3) {
        cout<<"Text: "<<argv[1]<<"\n";
        cout<<"Patterns: "<<argv[2]<<"\n";

        text = read_text(argv[1]);
        patterns = read_words(argv[2]);

        cout<<text.length()<<endl;
        cout<<patterns.size()<<endl;
        if(argc==4) {
            matcher_test=atoi(argv[3]);
            cout << "testing only algorithm '" <<
                   matcher_list[matcher_test]->name()<<"'\n";
        }
    } else {
        cerr << "Usage: "<<argv[0]<<" text patterns [algorithm id]\n";
        cerr << "Available algorithms:\n";
        for (int i = 0; i < matcher_list_size; i++)
        {
            cerr<<i<<" - "<<matcher_list[i]->name()<<"\n";
        }
        return 1;
    }

    cout << "Generated\n";

    for (int i = 0; i < matcher_list_size; i++)
    {
        if(matcher_test != -1 && matcher_test!=i) continue;
        matcher_time(matcher_list[i]);
    }
}
