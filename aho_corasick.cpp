#include"match.hpp"
#include<list>
#include<forward_list>
#include<algorithm>
#include"bitset.hpp"
#include<utility>
#include<iostream>
#include<stdint.h>
#include<unordered_set>
#include<unordered_map>
#include"aho_corasick.hpp"
#include<queue>
#include<cassert>
#include<array>

using namespace std;

/**
 * Info about Aho-Corasick node.
 */
struct ac_node_info
{
    // Fail function
    uint32_t fail;
    // In compact node next_c is the character with the only edge.
    char next_c : 8;
    // First match of this node.
    unsigned match : 23;
    // Must be last bit.
    // Set to one if this node is compact.
    // This means there there is edge with character next_c to the
    // node at next index if next_c!=0.
    bool compact : 1;
};

static const uint32_t NO_MATCH = (1<<23)-1;
static const uint32_t MAX_NUM_PATTERNS = NO_MATCH-1;

// This is not strictly required, but it's good to know that we are not using
// accidentally more memory than needed.
static_assert(sizeof(ac_node_info)==8, "ac_node_info must be 8 bytes in size");

/**
 * Data structure for trie nodes with multiple children.
 * 
 * Data structure contains bitset of 256 bits and a vector of child nodes.
 * Each char having a child node has bit set to one in the bitset. To find the
 * corresponding edge we count the number of ones preceding given bit. This
 * tells us which edge we must take from the edge vector.
 */
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

    // Functions for iterating all characters.
    // Used in tree construction.
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
 * to distinguish compact and non-compact nodes (compact field in ac_node_info struct).
 * 
 */
class ac_node
{
    union
    {
        // Compact node representation.
        // If single.compact==0 the node info is found from many.arr->info
        ac_node_info single;
        // Array node representation.
        struct 
        {
            trie_array* arr;
        } many;
    };

    bool empty() const
    {
        return is_compact() && single.next_c == 0;
    }
    public:
    // Initially node has no children
    ac_node()
    {
        single.fail = 0;
        single.next_c = 0;
        single.match = NO_MATCH;
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
            return single.next_c != 0 ? 1 : 0;
        }
        return many.arr->edge_count();
    }

    void add(int thiz, char c, int n)
    {
        // Null character always creates array.
        // This way we get correct behaviour
        // as node is considered empty if next_c == 0.
        if (empty() && n == thiz + 1 && c != 0)
        {
            single.next_c = c;
            return;
        }
        else if (is_compact() && single.next_c != 0)
        {
            char oc = single.next_c;
            assert(oc != c);
            int oto = thiz + 1;
            single.compact = false;
            ac_node_info info = single;
            many.arr = new trie_array();
            many.arr->add(oc, oto);
            many.arr->info = info;
        }
        else if(is_compact())
        {
            single.compact = false;
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
            if(c==single.next_c)
                return thiz+1;

            return -1;
        }
        return many.arr->get(c);
    }
    char first_char()
    {
        if (empty())return 0;
        if (is_compact())
            return single.next_c;
        return many.arr->first_char();
    }
    char next_char(char last_char)const
    {
        if (is_compact()) return 0;
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

/**
 * Implementation of construction algorithm.
 *
 */
class ac_machine
{
    vector<ac_node> nodes;
    public:
    // Linked list of match dependencies.
    // If node n matches pattern i it also matches pattern next_match[i].
    // Zero is used as special value to mark end of matches.
    vector<uint32_t> next_match;
    void set_child(int node, char c, int next)
    {
        return nodes[node].add(node, c, next);
    }
    int get_child(int node, char c)
    {
        return nodes[node].get(node, c);
    }
    int add_node()
    {
        nodes.push_back(ac_node());
        return nodes.size()-1;
    }
    int fail(int n) const
    {
        return nodes[n].info().fail;
    }
    
    ac_machine(const vector<string>& pats)
    {
        int fallback = add_node();
        int root = add_node();
        next_match.resize(pats.size());
        
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

            int old_match = nodes[v].info().match;
            int new_match = i;
            nodes[v].info().match = new_match;
            next_match[new_match] = old_match;
        }

        // Trie is ready.
        // Need to solve fail function
        // and match links.
        queue<int> q;
        q.push(root);
        nodes[fallback].info().fail = fallback;
        nodes[root].info().fail = fallback;
        while (q.size() != 0)
        {
            int u = q.front();
            q.pop();
            char c=nodes[u].first_char();

            for(int j=0;j<nodes[u].count();j++,
                c=nodes[u].next_char(c))
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
                // We only visit each node once
                assert(nodes[v].info().fail == 0);
                nodes[v].info().fail = w;

                // Fix matchlists:
                // Searches for the end of links to add
                // matches from the fail-node w.
                int first = nodes[v].info().match; 
                int mv = first;
                int mw = nodes[w].info().match;
                if (mv == NO_MATCH)
                {
                    nodes[v].info().match = mw;
                }
                else
                {
                    while(next_match[mv]!=NO_MATCH)
                        mv = next_match[mv];
                    next_match[mv] = mw;
                }
                
                q.push(v);
            };
        }
        cout << "Machine ready\n";
        cout << nodes.size() << " Nodes\n";
        int uncompact = 0;
        int accept = 0;
        for (size_t i = 0; i < nodes.size(); i++)
        {
            if (matches(i))
                accept++;
            if (!nodes[i].is_compact())
                uncompact++;
        }
        cout << uncompact << " Uncompact\n";
        cout << accept << " accepting\n";
        cout << pats.size() << " patterns\n";
    }

    bool matches(int v) const
    {
        return nodes[v].info().match!=0;
    }
    int match(int v) const
    {
        return nodes[v].info().match;
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

        uint32_t y = machine.match(x);
        int mc = 0;
        while (y!=NO_MATCH)
        {
            int p = y;
            out.push_back(::match(i-patterns[p].size()+1, p));
            y = machine.next_match[y];
            mc++;
        }
    }
}
