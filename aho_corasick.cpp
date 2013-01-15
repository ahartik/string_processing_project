#include"match.hpp"
#include<bitset>
#include<utility>
#include<iostream>
#include<boost/foreach.hpp>
#include<stdint.h>
#include<tr1/unordered_set>
#include<tr1/unordered_map>
#include"aho_corasick.hpp"
#include<queue>
#include<cassert>
#include<tr1/array>

typedef std::tr1::array<int, 256> iarr256;
using namespace std;

namespace std
{
    namespace tr1
    {
        template<>
            struct hash<std::pair<int,char> >
            {
                size_t operator()(const std::pair<int, char>& c) const
                {
                    return c.first*256 + c.second;
                }
            };
    }
}

class ac_machine
{
    tr1::unordered_map<pair<int,char>, int> m_child;
    vector<bitset<256> > m_has_child;
    int n() const
    {
        return fail.size();
    }
    public:
    vector<tr1::unordered_set<int> > patterns;
    vector<int> fail;
    int& child(int node, char c)
    {
        m_has_child[node][(uint8_t)c]=1;
        return m_child[std::make_pair(node,c)];
    }
    bool has_child(int node, char c)
    {
        return m_has_child[node][(uint8_t)c];
        //return m_child.count(std::make_pair(node, c));
    }
    int add_node()
    {
        //m_child.push_back(iarr256());
        //for (int i = 0; i < 256; i++)
            //m_child.back()[i] = -1;
        m_has_child.push_back(bitset<256>());
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
            while (has_child(v,pat[j]))
            {
                v = child(v, pat[j]);
                j++;
            }
            while (j < pat.size())
            {
                int u = add_node();
                child(v,pat[j]) = u;
                v = u;
                j++;
            }
            patterns[v].insert(i);
        }
        cout << "Trie ready\n";
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
                if (has_child(u,c))
                {
                    int v = child(u, c);
                    int w = fail[u];
                    while (!has_child(w,c)) w = fail[w];
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
        while (!machine.has_child(v,c))
            v = machine.fail[v];
        v = machine.child(v,c);
        BOOST_FOREACH(int p, machine.patterns[v])
        {
            out.push_back(::match(i-patterns[p].size()+1, p));
        }
    }
}
