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

int main(int argc, char** argv)
{
    srand(0);
    int matcher_test=-1;
    if(argc>=3) {
        cout<<"Text: "<<argv[1]<<"\n";
        cout<<"Patterns: "<<argv[2]<<"\n";

        text = read_text(argv[1]);

        vector<string> words = read_words(argv[2]);
        for(int i=0;i<words.size();i++) patterns.push_back(words[i]);
        cout<<text.length()<<endl;
        cout<<patterns.size()<<endl;
        if(argc==4) {
        matcher_test=atoi(argv[3]);
        cout<<"testing "<<matcher_test<<endl;
        }
    } else {

        for (int j = 0; j < M; j++)
        {
            patterns.push_back(random_string(10 + (j * N) / (M * 20)));
        }

        int P = 2;

        text = random_string(N/P);
        for (int j = 0; j < M / P; j++)
        {
            int s = rand()%text.size();
            int l = min(text.size()-s, 10 + j * text.size() * P/ (2 * M));
            patterns.push_back(text.substr(s, l));
        }
        for (int j = 0; j < N / P; j++)
            text.push_back('B');


        string lp("BB");
        for (int j = 0; j < M / P - 1; j++)
        {
            lp+="BBBB";
            patterns.push_back(lp);
        }
    }

    cout << "Generated\n";

    for (int i = 0; i < matcher_list_size; i++)
    {
        if(matcher_test != -1 && matcher_test!=i) continue;
        matcher_time(matcher_list[i]);
    }
}
