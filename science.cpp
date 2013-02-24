#include"matcher_list.hpp"
#include<cstdlib>
#include<string>
#include<iostream>
#include "util.hpp"
#include<set>
#include<time.h>


static double timef() {
    return clock() / (double)CLOCKS_PER_SEC;
}
using namespace std;

string text;

void fixed_len(FILE* fout)
{
    text = read_text("input/musketeers.txt");
    srand(0);
    int count = 10000;
    int minlen = 6;
    int maxlen = 100;
    vector<int> lens;
    vector<string> patterns(count);
    vector<int> cursor(count);
    for (int i = 0; i < count; i++)
    {
        cursor[i] = rand()%(text.size()-maxlen-1);
        for (int j = 0; j < minlen; j++)
        {
            patterns[i].push_back(text[cursor[i]++]);
        }
    }
    for (int len = minlen; len <= maxlen; len++)
    {
        fprintf(fout, "%8i ", len);
        for (int i = 0; i < matcher_list_size; i++)
        {
            double t = timef();
            match_vector matches;
            matcher_list[i]->match(text, patterns, matches);
            t = timef()-t;
            fprintf(fout, "%8.8lf ", t);
        }
        fprintf(fout, "\n");
        // add one character to each pattern
        for (int i = 0; i < count; i++)
        {
            patterns[i].push_back(text[cursor[i]++]);
        }
    }
}

int main()
{
    FILE* o = fopen("data/fixed_len.dat","w");
    fixed_len(o);
    fclose(o);
    system("gnuplot plots.gp");
}
