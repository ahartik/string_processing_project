#include"matcher_list.hpp"
#include<cstdlib>
#include<string>
#include<iostream>
#include "util.hpp"
#include<set>
#include<time.h>


using namespace std;

string text;

void fixed_len(const char* file)
{
    srand(0);
    int count = 5000;
    int parts = 10;
    int step = 3;
    int minlen = 6;
    int maxlen = 100;
    vector<double> times[3];
    for (int i=0;i<3;i++)
    {
        times[i].resize(maxlen+1);
    }

    while(parts-->0)
    {
        cout << "parts = "<<parts<<"\n";
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
        for (int len = minlen; len <= maxlen; len+=step)
        {
            cout << "len = "<<len<<"\n";
            for (int i = 0; i < matcher_list_size; i++)
            {
                match_vector matches;
                double t =  timef();
                matcher_list[i]->match(text, patterns, matches);
                t = timef()-t;
               //cout << matches.size() << " matches\n";
                times[i][len]+=t;
                //fprintf(fout, "%8.8lf ", t);
            }
            // add one character to each pattern
            for (int i = 0; i < count; i++)
            {
                patterns[i].push_back(text[cursor[i]++]);
            }
        }
    }
    FILE* fout = fopen(file, "w");
    for (int j = minlen; j <= maxlen; j+=step)
    {
        fprintf(fout, "%8i ", j);
        for (int i = 0; i < matcher_list_size; i++)
        {
            fprintf(fout, "%8.8lf ", times[i][j]);
        }
        fprintf(fout, "\n");
    }
    fclose(fout);
}

int minlen = 10;
int maxlen = 1000;
string rand_pattern(int len)
{
    return text.substr(rand()%(text.size()-len),len);
}
string rand_pattern()
{
    int len = minlen + rand()%(maxlen-minlen);
    return rand_pattern(len);
}

void variable_len(const char* file)
{
    FILE* fout = fopen(file, "w");
    srand(0);

    int minlen = 100000;
    int maxlen = 3000000;
    int step = 100;;
    vector<int> lens;
    vector<string> patterns;
    int total = 0;
    while(total < minlen)
    {
        patterns.push_back(rand_pattern());
        total+=patterns.back().size();
    }

    while(total <= maxlen)
    {
        cout << "total = "<<total << " / "<<maxlen <<"\n";
        fprintf(fout, "%8i ", total);
        for (int i = 0; i < matcher_list_size; i++)
        {
            double t = timef();
            match_vector matches;
            matcher_list[i]->match(text, patterns, matches);
            // cout << matches.size() << " matches\n";
            t = timef()-t;
            fprintf(fout, "%8.8lf ", t);
        }
        fprintf(fout, "\n");
        for (int i = 0; i < step; i++)
        {
            patterns.push_back(rand_pattern());
            total+=patterns.back().size();
        }
        // add one character to each pattern
    }
    fclose(fout);
}

void growing_text(const char* file)
{
    FILE* fout = fopen(file, "w");
    srand(0);

    int minlen = 1000;
    int maxlen = 100000;
    int pattern_len  = 100000;
    int step = 1000;
    vector<int> lens;
    vector<string> patterns;
    int total = 0;
    int cl = 3;
    while(total < pattern_len)
    {
        patterns.push_back(rand_pattern());
        total+=patterns.back().size();
    }

    string txt=text.substr(0, minlen);;

    while(txt.size() <= maxlen)
    {
        cout << "len = "<<txt.size()<< " / "<<maxlen <<"\n";
        fprintf(fout, "%8i ", txt.size());
        for (int i = 0; i < matcher_list_size; i++)
        {
            double t = timef();
            const int p = 40;
            for (int j = 0 ; j < p; j++)
            {
                match_vector matches;
                matcher_list[i]->match(txt, patterns, matches);
            }
            // cout << matches.size() << " matches\n";
            t = timef()-t;
            t/=p;
            fprintf(fout, "%8.8lf ", t);
        }
        fprintf(fout, "\n");
        // add to txt
        txt+=text.substr(txt.size(),step);
    }
    fclose(fout);
}

void growing_text_fixed(const char* file)
{
    FILE* fout = fopen(file, "w");
    srand(0);

    int minlen = 1000;
    int maxlen = 1000000;
    int pattern_len  = 100000;
    int step = 10000;
    vector<int> lens;
    vector<string> patterns;
    int total = 0;
    int cl = 30;
    while(total < pattern_len)
    {
        patterns.push_back(rand_pattern(cl));
        total+=patterns.back().size();
    }

    string txt=text.substr(0, minlen);;

    while(txt.size() <= maxlen)
    {
        cout << "len = "<<txt.size()<< " / "<<maxlen <<"\n";
        fprintf(fout, "%8i ", txt.size());
        for (int i = 0; i < matcher_list_size; i++)
        {
            double t = timef();
            const int p = 10;
            for (int j = 0 ; j < p; j++)
            {
                match_vector matches;
                matcher_list[i]->match(txt, patterns, matches);
            }
            // cout << matches.size() << " matches\n";
            t = timef()-t;
            t/=p;
            fprintf(fout, "%8.8lf ", t);
        }
        fprintf(fout, "\n");
        // add to txt
        txt+=text.substr(txt.size(),step);
    }
    fclose(fout);
}

int main(int argc, char** argv)
{
    text = read_text("input/musketeers.txt");
    //fixed_len("data/fixed_len.dat");

    //variable_len("data/var_len.dat");
    growing_text("data/text_len.dat");
    //growing_text_fixed("data/text_len_fixed.dat");

    system("gnuplot plots.gp");
}
