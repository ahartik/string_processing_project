#include"match.hpp"
#include<algorithm>
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

class edge
{
    public:
    int to;
    char c;
    edge(char x,int z):
        to(z),
        c(x)
    {}
    bool operator<(const edge& e)const
    {
        if (c != e.c)
            return c < e.c;
        return to < e.to;
    }
};

class trie_array
{
    public:
    typedef vector<edge> vec_type;
    typedef vec_type::iterator iterator;
    private:
    vec_type vec;

    public:

    int get(char c)
    {
        for (int i = 0 ; i < vec.size(); i++)
            if (vec[i].c == c)
                return vec[i].to;
        return -1;
    }
    int fast_get(char c)
    {
        vec_type::iterator it = std::lower_bound(vec.begin(), vec.end(), edge(c,0));
        if (it==vec.end())return -1;
        if (it->c != c)return -1;
        return it->to;
    }
    void sort()
    {
        std::sort(vec.begin(), vec.end());
    }
    void add(char c,int x)
    {
        vec.push_back(edge(c, x));
    }
    vec_type::iterator begin()
    {
        return vec.begin();
    }
    vec_type::iterator end()
    {
        return vec.end();
    }
};

class ac_machine
{
    vector<trie_array> m_child;
    int n() const
    {
        return fail.size();
    }
    int i_get_child(int node, char c)
    {
        return m_child[node].get(c);
    }
    public:
    vector<tr1::unordered_set<int> > patterns;
    vector<int> fail;
    void set_child(int node, char c, int next)
    {
        return m_child[node].add(c, next);
    }
    int get_child(int node, char c)
    {
        return m_child[node].fast_get(c);
    }
    bool has_child(int node, char c)
    {
        return get_child(node, c) != -1;
    }
    int add_node()
    {
        //m_child.push_back(iarr256());
        m_child.push_back(trie_array());
        //for (int i = 0; i < 256; i++)
            //m_child.back()[i] = -1;
        fail.push_back(1);
        patterns.push_back(tr1::unordered_set<int>());
        return n()-1;
    }
    
    ac_machine(const vector<string>& pats)
    {
        int fallback = add_node();
        int root = add_node();
        fail[root] = fallback;

        for (int i = 0; i < pats.size(); i++)
        {
            int v = root;
            int j = 0;
            const string& pat = pats[i];
            int x;
            while ((x = i_get_child(v,pat[j]))!=-1)
            {
                v = x;
                j++;
            }
            while (j < pat.size())
            {
                int u = add_node();
                set_child(v, pat[j], u);
                v = u;
                j++;
            }
            patterns[v].insert(i);
        }
        for (int i = 0; i< m_child.size(); i++)
            m_child[i].sort();
            
        // Trie is ready.
        // Need to solve fail function
        queue<int> q;
        q.push(root);
        fail[fallback] = fallback;
        fail[root] = fallback;
        while (q.size() != 0)
        {
            int u = q.front();
            q.pop();
            for (trie_array::iterator it = m_child[u].begin();
                 it!=m_child[u].end();
                 ++it)
            {
                char c = it->c;
                int v = it->to;
                {
                    int w = fail[u];
                    int x = 1;
                    while ((x=get_child(w,c))==-1)
                    {
                        w = fail[w];
                        if (w == 0)
                        {
                            x = root;
                            break;
                        }
                    }
                    w = x;
                    fail[v] = w;
                    patterns[v].insert(patterns[w].begin(), patterns[w].end());
                    q.push(v);
                }
            }
        }
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
        while (v && !machine.has_child(v,c))
            v = machine.fail[v];
        v = machine.get_child(v,c);
        if (v == -1)
            v = machine.root();
        BOOST_FOREACH(int p, machine.patterns[v])
        {
            out.push_back(::match(i-patterns[p].size()+1, p));
        }
    }
}
