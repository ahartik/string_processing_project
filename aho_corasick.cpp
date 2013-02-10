#include"match.hpp"

#include<algorithm>
#include"bitset.hpp"
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

using namespace std;

class edge
{
    public:
    int to;
    char c;
    edge():
        to(0),
        c(0)
    {
        
    }
    edge(char x,int z):
        to(z),
        c(x)
    {}
    bool operator<(const edge& e)const
    {
        return c < e.c;
    }
};


#include <boost/iterator/iterator_facade.hpp>


class trie_array
{
public:
    class iterator;
    friend class iterator;
private:
    vector<int> edges;
    bitset<256> skip;
public:
    trie_array() 
    {
    }
    char first_char()
    {
        return char(skip.next_set(0));
    }
    size_t edge_count() const
    {
        return edges.size();
    }
    char next_char(char last_char)const
    {
        uint8_t x = last_char;
        x++;
        x = skip.next_set(x);
        return x;
    }
    bool has_next(char c)const
    {
        uint8_t x = c;
        x++;
        return skip.next_set(x)!=256;
    }

    int get(char c)
    {
        uint8_t uc = c;
        if (skip[uc] == 0)
            return -1;
        return edges[skip.rank(uc)];
    }
    void add(char c, int to)
    {
        uint8_t b = c;
        size_t r = skip.rank(b);
        if(skip[b])
        {
            edges[skip.rank(b)]=to;
            return;
        }
        skip[b] = 1;
        edges.insert(edges.begin() + r, to);
    }
};
class ac_machine
{
    vector<trie_array> m_child;
    int n() const
    {
        return fail.size();
    }
    public:
    vector<vector<int> > patterns;
    vector<int> fail;
    void set_child(int node, char c, int next)
    {
        return m_child[node].add(c, next);
    }
    int get_child(int node, char c)
    {
        return m_child[node].get(c);
    }
    bool has_child(int node, char c)
    {
        return get_child(node, c) != -1;
    }
    int add_node()
    {
        m_child.push_back(trie_array());
        //for (int i = 0; i < 256; i++)
            //m_child.back()[i] = -1;
        fail.push_back(1);
        patterns.push_back(vector<int>());
        return n()-1;
    }
    
    ac_machine(const vector<string>& pats)
    {
        int fallback = add_node();
        int root = add_node();
        fail[root] = fallback;

        for (size_t i = 0; i < pats.size(); i++)
        {
            int v = root;
            size_t j = 0;
            const string& pat = pats[i];
            int x;
            while ((x = get_child(v,pat[j]))!=-1)
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
            patterns[v].push_back(i);
        }

        // for (int i = 0; i< m_child.size(); i++)
            // m_child[i].sort();
            
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
            char c=m_child[u].first_char();
            for(size_t j=0;j<m_child[u].edge_count();j++,
                c=m_child[u].next_char(c))
            {
                int v = m_child[u].get(c);
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
                patterns[v].insert(patterns[v].end(), patterns[w].begin(), patterns[w].end());

                std::sort(patterns[v].begin(), patterns[v].end());
                auto last = std::unique(patterns[v].begin(), patterns[v].end());
                patterns[v].erase(last, patterns[v].end());
                q.push(v);
            };
        }
        cout << "Machine ready\n";
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
        int x;
        while (v && ((x=machine.get_child(v,c))==-1))
            v = machine.fail[v];

        if (v == 0)
            v = machine.root();
        else
            v = x;
        BOOST_FOREACH(int p, machine.patterns[v])
        {
            out.push_back(::match(i-patterns[p].size()+1, p));
        }
    }
}
