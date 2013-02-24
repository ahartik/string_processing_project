#include"aho_corasick.hpp"

using namespace std;
int main()
{
    vector<string> ps = 
    {
        "ab",
        "bc",
        "abc",
        "aa",
        "baa"
    };
    aho_corasick_matcher m;
    m.print_dot(ps);

}
