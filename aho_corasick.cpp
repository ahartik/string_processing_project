#include"match.hpp"
#include<algorithm>
#include"bitset.hpp"
#include<utility>
#include<iostream>
#include<stdint.h>
#include"aho_corasick.hpp"
#include<queue>
#include<cassert>
#include"util.hpp"

/**
 * Implementation of Aho-Corasick multiple pattern matching algorithm.
 *
 *
 * About portability:
 * This code has only been tested on linux, x86_64 and gcc, but *should*
 * also work on 32bit x86 platforms.
 *
 * This most probably won't work on big-endian platforms.
 */

using namespace std;


/**
 * Info about Aho-Corasick node.
 *
 * Compact nodes can at most have one transition and it can only be
 * to the next node in a node array.
 *
 */
struct ac_node_info
{
    // Fail function
    uint32_t fail;
    // In compact node next_c is the character of the transition
    // Empty nodes are marked with next_c == 0.
    char next_c;
    // First match of this node.
    unsigned match : 23;
    // Must be the last bit of the last byte.
    // This is the case on current gcc,
    // but not necessarily on other compilers.
    //
    // Is this node compact?
    bool compact : 1;
};

// This is not strictly required, but it's good to know that we are not using
// more memory than we want to by accident.
static_assert(sizeof(ac_node_info)==8, "ac_node_info must be 8 bytes in size");

static const uint32_t NO_MATCH = (1<<23)-1;
static const uint32_t MAX_NUM_PATTERNS = NO_MATCH-1;

/**
 * Data structure for aho-corasick nodes with multiple transitions.
 *
 * trie_array contains bitset of 256 bits and a vector of transition nodes.
 * Each char having a transition has bit set to one in the bitset.
 *
 * To find the corresponding transition we count the number of ones preceding
 * given bit. This tells us the index of the transition in the vector trans.
 */
class trie_array
{
private:
    vector<int> trans;
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
    size_t count() const
    {
        return trans.size();
    }

    // Functions for iterating all characters.
    // Used in tree construction.
    char next_char(char last_char) const
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
        return trans[skip.rank(uc)];
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
        // Possibly time-consuming, at most O(alphabet size)
        trans.insert(trans.begin() + r, to);
    }
};

/**
 * Optimized class representing node in Aho-Corasick machine.
 * This is either compact node with one or zero transitions
 * or pointer to a trie_array structure.
 *
 * This code depends on the fact that 64bit cpu has at most 63-bit virtual addressing.
 * At the time all intel and amd processors have at most 48-bit virtual addressing space.
 * This last bit will be used to distinguish compact and non-compact nodes
 * (compact field in ac_node_info struct).
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
    // Initially node has no transitions
    ac_node()
    {
        single.fail = 0;
        single.next_c = 0;
        single.match = 0;
        // Check our assumptions about bit-field alignment
        assert((intptr_t(many.arr) & ((1ULL<<63ULL)-1))==0);
        single.match = NO_MATCH;
        single.compact = 1;
    }
    ac_node(ac_node&& c)
    {
        memcpy(this, &c, sizeof(ac_node));
        memset(&c, 0, sizeof(ac_node));
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
        return many.arr->count();
    }
    /**
     * thiz parameter in these functions is the index of this
     * node.
     */
    void add(int thiz, char c, int n)
    {
        // If c==0 we have to create trie_array because we mark empty nodes
        // with next_c = 0
        if (empty() && n == thiz + 1 && c != 0)
        {
            single.next_c = c;
            return;
        }
        else if (is_compact() && single.next_c != 0)
        {
            // Create trie_array
            // and move existing transition there.
            char oc = single.next_c;
            int oto = thiz + 1;
            single.compact = false;
            ac_node_info info = single;
            many.arr = new trie_array();
            many.arr->add(oc, oto);
            many.arr->info = info;
        }
        else if(is_compact())
        {
            // Create trie_array.
            single.compact = false;
            ac_node_info info = single;
            many.arr = new trie_array();
            many.arr->info = info;
        }
        many.arr->add(c, n);
        assert(!is_compact());
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
    char next_char(char last_char) const
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
    ~ac_node()
    {
        if(!is_compact())
            delete many.arr;
    }
};

/**
 * Implementation of construction algorithm.
 */
struct ac_machine
{
    // Vector of all nodes of this machine.
    // Compact nodes will be placed in consecutively so
    // we don't need to store their transition destinations.
    vector<ac_node> nodes;
    // Linked list of match dependencies.
    // If node n matches pattern i it also matches pattern next_match[i].
    // Zero is used as special value to mark end of matches.
    vector<uint32_t> next_match;

    // index of root node
    int root;
    // index of fallback node
    // fail(root) = fallback
    int fallback;
    void set_trans(int node, char c, int next)
    {
        return nodes[node].add(node, c, next);
    }
    int get_trans(int node, char c)
    {
        return nodes[node].get(node, c);
    }
    // Creates new index for empty node
    int add_node()
    {
        nodes.push_back(ac_node());
        return nodes.size()-1;
    }
    uint32_t& fail(int n)
    {
        return nodes[n].info().fail;
    }
    ac_machine(const vector<string>& pats)
    {
        fallback = add_node();
        root = add_node();

        next_match.resize(pats.size());
        assert(pats.size() <= MAX_NUM_PATTERNS);
        // This is the maximum number of nodes we will need
        int total = 2;
        for (size_t i = 0; i < pats.size(); i++)
            total += pats[i].size();

        nodes.reserve(total);

        for (size_t i = 0; i < pats.size(); i++)
        {
            int v = root;
            size_t j = 0;
            const string& pat = pats[i];
            int x;
            while ((x = get_trans(v, pat[j]))!=-1)
            {
                v = x;
                j++;
            }
            while (j < pat.size())
            {
                int u = add_node();
                set_trans(v, pat[j], u);
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
        fail(fallback) = fallback;
        fail(root) = fallback;
        while (q.size() != 0)
        {
            int u = q.front();
            q.pop();
            char c=nodes[u].first_char();

            for(int j=0;j<nodes[u].count();j++,
                c=nodes[u].next_char(c))
            {
                int v = get_trans(u,c);
                int w = fail(u);
                int x = 1;
                while ((x=get_trans(w,c))==-1)
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
                assert(fail(v) == 0);
                fail(v) = w;

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
#if 0

        cout << "Machine ready\n";
        cout << nodes.size() << " nodes\n";
        int uncompact = 0;
        for (size_t i = 0; i < nodes.size(); i++)
        {
            if (!nodes[i].is_compact())
                uncompact++;
        }
        cout << uncompact << " large nodes\n";
#endif
    }

    int match(int v)
    {
        return nodes[v].info().match;
    }
};

void aho_corasick_matcher::match(
        const std::string& text,
        const std::vector<std::string>& patterns,
        match_vector& out) const
{
    double t = timef();
    ac_machine machine(patterns);

    pp_time = timef()-t;
    t = timef();

    int v = machine.root;
    for (size_t i = 0; i < text.size(); i++)
    {
        char c = text[i];
        int x;
        while (v!=machine.fallback &&
               ((x=machine.get_trans(v,c))==-1))
            v = machine.fail(v);

        // From fallback node we always get back to the root node
        if (v == machine.fallback)
            v = machine.root;
        else
            v = x;
        x = v;

        // Iterate the matchlist
        uint32_t y = machine.match(x);
        while (y!=NO_MATCH)
        {
            int p = y;
            out.push_back(::match(i-patterns[p].size()+1, p));
            y = machine.next_match[y];
        }
    }
    match_time = timef() - t;
}
void aho_corasick_matcher::print_dot(const std::vector<std::string>& patterns)
{
    ac_machine machine(patterns);
    cout << "digraph {\n";
    vector<string> names(machine.nodes.size());
    for (int i = 1; i < machine.nodes.size(); i++)
    {
        ac_node& n = machine.nodes[i];
        char c = n.first_char();
        for (int j=0;j<n.count();j++)
        {
            int t = n.get(i, c);
            names[t] = names[i];
            names[t].push_back(c);
            cout << "n"<<i<<" -> "<<"n"<<t<<" [label=\""<<c<<"\"]\n";
            c = n.next_char(c);
        }
        
        cout << "n"<<i<<" -> "<<"n"<<machine.fail(i)<<" [style=dashed, constraint=false]\n";
    }
    names[0] = "_|_";
    for (int i = 0; i < machine.nodes.size(); i++)
    {
        cout << "n"<<i<<" [label=\""<<names[i]<<"\"]\n";
    }
    cout << "n0 -> n1 [label=\"*\",constraint=false]\n";
    cout << "}\n";
}
