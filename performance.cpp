#include"matcher_list.hpp"
#include"timef.h"
#include<cstdlib>
#include<string>
#include<iostream>
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

int main()
{
    for (int j = 0; j < M; j++)
    {
        patterns.push_back(random_string(10 + (j * N) / (M * 20)));
    }
    cout << "Generated\n";

    for (int i = 0; i < matcher_list_size; i++)
    {
        matcher_time(matcher_list[i]);
    }
}
