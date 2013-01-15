#include"aho_corasick.hpp"
#include<iostream>
using namespace std;


int main()
{
    // simple sanity check
    //
    string ps[] = 
    {
        "she",
        "he",
        "his",
        "hers"
    };
    vector<string> patterns(ps, ps+4);
    string text = "They were hers, not his. He took them from her by mistake. she was very upset";
    aho_corasick_matcher m;
    match_vector result;
    m.match(text, patterns, result);
    cout<<text<<"\n";
    for(int i = 0; i< result.size();  i++)
    {
        int r = result[i].pattern;
        int pos = result[i].start;
        cout << patterns[r]<<" at "<<pos<<"\n";
    }
}
