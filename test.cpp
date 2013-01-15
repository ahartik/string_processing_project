#include"aho_corasick.hpp"
#include"matcher_list.hpp"
#include<string>
#include<iostream>
using namespace std;

string match_output(const multiple_matcher* m,
                    const string& text,
                    const vector<string>& patterns)
{
    string result = text;
    match_vector matches;
    m->match(text, patterns, matches);

    for (int i = 0; i < matches.size(); i++)
    {
        int p = matches[i].pattern;
        int s = matches[i].start;
        for (int j = 0; j < patterns[p].size(); j++)
        {
            result[s+j]='A'+p;
        }
    }
    return result;
}

int main()
{
    // simple sanity check
    //
    string ps[] = 
    {
        "she",
        "he",
        "his",
        "her"
    };
    vector<string> patterns(ps, ps+4);
    string text = "They were hers, not his. He took them from her by mistake. she was very upset";
    cout << text <<"\n";

    for (int i = 0; i < matcher_list_size; i++)
    {
        cout << matcher_list[i]->name()<<":\n";
        cout << match_output(matcher_list[i], text, patterns)<<"\n";
    }
}
