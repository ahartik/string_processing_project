#include"match.hpp"
#include<iostream>
#include<boost/foreach.hpp>
#include<stdint.h>
#include<tr1/unordered_set>
#include"aho_corasick.hpp"
#include<queue>
#include<cassert>
#include<tr1/array>

typedef std::tr1::array<int, 256> iarr256;
using namespace std;

class ac_machine
{
    vector<iarr256> m_child;

    int n() const
    {
        return fail.size();
    }
    public:
    vector<tr1::unordered_set<int> > patterns;
    vector<int> fail;
    int& child(int node, char c)
    {
        return m_child[node][(uint8_t)c];
    }
    int add_node()
    {
        m_child.push_back(iarr256());
        for (int i = 0; i < 256; i++)
            m_child.back()[i] = -1;
        fail.push_back(1);
        patterns.push_back(tr1::unordered_set<int>());
        return n()-1;
    }
    
    ac_machine(const vector<string>& pats)
    {
        int fallback = add_node();
        int root = add_node();
        fail[root] = fallback;

        for (int i = 0; i < 256; i++)
            child(fallback, i) = root;
        for (int i = 0; i < pats.size(); i++)
        {
            int v = root;
            int j = 0;
            const string& pat = pats[i];
            int x;
            while ((x = child(v, pat[j])) != -1)
            {
                v = x;
                j++;
            }
            while (j < pat.size())
            {
                int u = add_node();
                child(v,pat[j]) = u;
                cout << "child("<<v<<","<<pat[j]<<") = "<<u<<"\n";
                v = u;
                j++;
            }
            patterns[v].insert(i);
        }
        // Trie is ready.
        // Need to solve fail function
        queue<int> q;
        q.push(root);
        fail[root] = fallback;
        while (q.size() != 0)
        {
            int u = q.front();
            q.pop();
            for (int c = 0; c < 256; c++)
            {
                if (child(u, c) != -1)
                {
                    int v = child(u, c);
                    int w = fail[u];
                    while (child(w,c) == -1) w = fail[w];
                    w = child(w,c);
                    fail[v] = w;
                    patterns[v].insert(patterns[w].begin(), patterns[w].end());
                    q.push(v);
                }
            }
        }
        cout<<"n = "<<n()<<"\n";
    }
    int root()const
    {
        return 1;
    }
};  

void aho_corasick_matcher::match(
        const std::string& text,
        const std::vector<std::string> patterns,
        match_vector& out) const
{
    ac_machine machine(patterns);
    int v = machine.root();
    for (size_t i = 0; i < text.size(); i++)
    {
        char c = text[i];
        while (machine.child(v,c) == -1)
            v = machine.fail[v];
        v = machine.child(v,c);
        //cout <<" v = "<<v<<"\n";
        BOOST_FOREACH(int p, machine.patterns[v])
        {
            out.push_back(::match(i-patterns[p].size()+1, p));
        }
    }
}
