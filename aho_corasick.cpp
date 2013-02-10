#include"match.hpp"
#include<list>
#include<forward_list>
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

class ac_node
{
    union
    {
        struct 
        {
            int32_t c;
            uint32_t to;
        } single;
        struct {
            char pad[sizeof(single)-sizeof(trie_array*)];
            trie_array* arr;
        } many;
    };

    int compact_to() const
    {
        return single.to>>1;
    }
    char compact_c() const
    {
        return single.c;
    }
    bool empty() const
    {
        return many.arr == NULL;
    }
    public:
    ac_node():
        single{0,0}
    {
    }
    ac_node(const ac_node& c)
    {
        if(c.is_compact() || c.empty())
            this->single = c.single;
        else this->many.arr = new trie_array(*(c.many.arr));
    }

    bool is_compact() const
    {
        return bool(single.to&1);
    }

    int count() const
    {
        if (is_compact()) {
            return 1;
        }
        if (many.arr==NULL)
            return 0;
        return many.arr->edge_count();
    }
    void add(char c, int n)
    {
        if (empty())
        {
            single.c = c;
            single.to = n<<1;
            single.to |= 1;
            return;
        }
        if (is_compact())
        {
            char oc = compact_c();
            int oto = compact_to();
            many.arr = new trie_array();
            many.arr->add(oc, oto);
        }
        many.arr->add(c,n);
    }
    int get(char c)
    {
        if (empty()) return -1;
        if(is_compact())
        {
            if(c==compact_c())
                return compact_to();
            return -1;
        }
        return many.arr->get(c);
    }
    char first_char()
    {
        if (empty())return 0;
        if (is_compact())
            return compact_c();
        return many.arr->first_char();
    }
    char next_char(char last_char)const
    {
        if (empty() || is_compact()) return 0;
        return many.arr->next_char(last_char);
    }
    ~ac_node()
    {
        if(!is_compact())
            delete many.arr;
    }

    friend void swap(ac_node& a, ac_node& b)
    {
        ac_node t;
        memcpy(&t, &a, sizeof(ac_node));
        memcpy(&a, &b, sizeof(ac_node));
        memcpy(&b, &t, sizeof(ac_node));
    }
};
class ac_machine
{
    vector<ac_node> m_child;
    
    int n() const
    {
        return fail.size();
    }
    public:
    vector<forward_list<int>> patterns;
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
        m_child.push_back(ac_node());
        fail.push_back(1);
        patterns.push_back(forward_list<int>());
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
            patterns[v].push_front(i);
        }
        for (size_t i = 0; i < m_child.size();i++)
            patterns[i].sort();

        cout << m_child.size()<<" Nodes\n";
        int uncompact = 0;
        for (int i = 0; i < m_child.size(); i++)
            if(!m_child[i].is_compact())
                uncompact++;
        cout << uncompact << " Uncompact\n";

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
            for(int j=0;j<m_child[u].count();j++,
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
                patterns[v].merge(forward_list<int>(patterns[v]));
                patterns[v].unique();
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
