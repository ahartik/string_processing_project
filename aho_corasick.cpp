#include"match.hpp"
#include<list>
#include<forward_list>
#include<algorithm>
#include"bitset.hpp"
#include<utility>
#include<iostream>
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

struct ac_node_info
{
    uint32_t fail;
    uint16_t pad1;
    char c;
    unsigned pad2 : 4;
    bool has_next : 1;
    bool matches : 1;
    bool contains : 1;
    bool compact : 1;
};

class trie_array
{
private:
    vector<int> edges;
    bitset<256> skip;
public:
    ac_node_info info;
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
        return skip.next_set(x) != 256;
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

/**
 * Optimized class representing node in Aho-Corasick machine.
 * This is either compact node with one or zero children.
 * Or pointer to the trie_array structure.
 *
 * This code depends on the fact that pointers created using operator
 * new will have zeros in their lowest bits. We use the lowest bit
 * to distinguish compact and non-compact nodes.
 * 
 */
class ac_node
{
    union
    {
        ac_node_info single;
        struct {
            char pad[sizeof(single)-sizeof(trie_array*)];
            trie_array* arr;
        } many;
    };

    char compact_c() const
    {
        return single.c;
    }
    bool empty() const
    {
        return is_compact() && !single.has_next;
    }
    public:
    ac_node()
    {
        single.fail = 0;
        single.c = 0;
        single.has_next = 0;
        single.matches = 0;
        single.contains = 0;
        single.compact = 1;
    }
    ac_node(ac_node&& c)
    {
        memcpy(this, &c, sizeof(ac_node));
        memset(&c, 0 ,sizeof(ac_node));
        c.single.compact = 1;
    }
    ac_node(const ac_node& c)
    {
        if(c.is_compact() || c.empty())
            this->single = c.single;
        else this->many.arr = new trie_array(*(c.many.arr));
    }

    bool is_compact() const
    {
        return single.compact;
    }

    int count() const
    {
        if (is_compact()) {
            return single.has_next;
        }
        if (many.arr==NULL)
            return 0;
        return many.arr->edge_count();
    }

    void add(int thiz, char c, int n)
    {
        if (empty() && n == thiz + 1)
        {
            single.c = c;
            single.has_next = 1;
            return;
        }
        else if (is_compact() && single.has_next)
        {
            char oc = compact_c();
            assert(oc != c);
            int oto = thiz + 1;
            ac_node_info info = single;
            many.arr = new trie_array();
            many.arr->add(oc, oto);
            many.arr->info = info;
        }
        else if(is_compact())
        {
            ac_node_info info = single;
            many.arr = new trie_array();
            many.arr->info = info;
        }
        many.arr->add(c, n);
        assert(info().fail==0);
    }
    int get(int thiz, char c)
    {
        if (empty()) return -1;
        if(is_compact())
        {
            if(c==compact_c())
                return thiz+1;

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

    ac_node_info& info()
    {
        if (single.compact)
            return single;
        return many.arr->info;
    }
    const ac_node_info& info() const
    {
        if (single.compact)
            return single;
        return many.arr->info;
    }
    ~ac_node()
    {
        if(!is_compact())
            delete many.arr;
    }
};

class ac_machine
{
    vector<ac_node> m_child;
    
    int n() const
    {
        return m_child.size();
    }
    public:
    vector<pair<int,int>> patterns;
    void set_child(int node, char c, int next)
    {
        return m_child[node].add(node, c, next);
    }
    int get_child(int node, char c)
    {
        return m_child[node].get(node, c);
    }
    bool has_child(int node, char c)
    {
        return get_child(node, c) != -1;
    }
    int add_node()
    {
        m_child.push_back(ac_node());
        return n()-1;
    }
    int fail(int n) const
    {
        return m_child[n].info().fail;
    }
    
    ac_machine(const vector<string>& pats)
    {
        int fallback = add_node();
        int root = add_node();
        
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
            m_child[v].info().matches = 1;
            m_child[v].info().contains = 1;
            patterns.emplace_back(v,i);
        }

        std::sort(patterns.begin(), patterns.end());
        
        // Trie is ready.
        // Need to solve fail function
        queue<int> q;
        q.push(root);
        m_child[fallback].info().fail = fallback;
        m_child[root].info().fail = fallback;
        while (q.size() != 0)
        {
            int u = q.front();
            q.pop();
            char c=m_child[u].first_char();

            for(int j=0;j<m_child[u].count();j++,
                c=m_child[u].next_char(c))
            {
                int v = get_child(u,c);
                int w = fail(u);
                int x = 1;
                while ((x=get_child(w,c))==-1)
                {
                    w = fail(w);
                    if (w == 0)
                    {
                        x = root;
                        break;
                    }
                }
                w = x;
                assert(m_child[v].info().fail == 0);
                m_child[v].info().fail = w;
                m_child[v].info().matches |= m_child[w].info().matches;
                q.push(v);
            };
        }
        cout << "Machine ready\n";
        cout << m_child.size() << " Nodes\n";
        int uncompact = 0;
        int accept = 0;
        for (size_t i = 0; i < m_child.size(); i++)
        {
            if (matches(i))
                accept++;
            if (!m_child[i].is_compact())
                uncompact++;
        }
        cout << uncompact << " Uncompact\n";
        cout << accept << " accepting\n";
    }
    bool contains_match(int v) const
    {
        return m_child[v].info().contains;
    }
    bool matches(int v) const
    {
        return m_child[v].info().matches;
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
            v = machine.fail(v);

        if (v == 0)
            v = machine.root();
        else
            v = x;
        x = v;

        const vector<pair<int,int> > &pv = machine.patterns;
        while (machine.matches(x))
        {
            if (machine.contains_match(x))
            {
                pair<int, int> l(x,0);
                for (auto it = std::lower_bound(pv.begin(), pv.end(), l);
                     it != pv.end() && it->first == x;
                     ++it)
                {
                    int p = it->second;
                    out.push_back(::match(i-patterns[p].size()+1, p));
                }
            }
            x = machine.fail(x);
        }
    }
}
