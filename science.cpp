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
    int count = 30000;
    int parts = 5;
    int step = 5;
    int minlen = 10;
    int maxlen = 200;
    vector<double> times[3];
    vector<double> m_times[3];
    for (int i=0;i<3;i++)
    {
        times[i].resize(maxlen+1);
        m_times[i].resize(maxlen+1);
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
                m_times[i][len]+=matcher_list[i]->match_time;
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
            fprintf(fout, "%8.8lf %8.8f ", times[i][j], m_times[i][j]);
        }
        fprintf(fout, "\n");
    }
    fclose(fout);
}

int minlen = 10;
int maxlen = 200;
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
    int maxlen = 1000000;
    int step = 200;
    vector<int> lens;
    const int repeat = 15;
    vector<string> patterns[repeat];
    int total = 0;
    while(total < minlen)
    {
        int l = rand_pattern().size();
        for (int i = 0; i < repeat; i++)
            patterns[i].push_back(rand_pattern(l));
        total+=l;
    }

    while(total <= maxlen)
    {
        cout << "total = "<<total << " / "<<maxlen <<"\n";
        fprintf(fout, "%8i ", total);
        for (int i = 0; i < matcher_list_size; i++)
        {
            double t = timef();
            double match_time = 0;
            for (int j = 0;j<repeat;j++)
            {
                match_vector matches;
                matcher_list[i]->match(text, patterns[j], matches);
                match_time += matcher_list[i]->match_time;
            }
            // cout << matches.size() << " matches\n";
            t = timef()-t;
            t/=repeat;
            match_time /= repeat;
            fprintf(fout, "%8.8lf %8.8lf ", t, match_time);
        }
        fprintf(fout, "\n");
        for (int i = 0; i < step; i++)
        {
            int l = rand_pattern().size();
            for (int j = 0; j < repeat; j++)
                patterns[j].push_back(rand_pattern(l));
            total+=l;
        }
        // add one character to each pattern
    }
    fclose(fout);
}

void growing_text(const char* file)
{
    FILE* fout = fopen(file, "w");
    srand(0);

    int minlen = 10000;
    int maxlen = 1000000;
    int pattern_len  = 100000;
    int step = 10000;
    vector<int> lens;
    vector<string> patterns;
    int total = 0;
    while(total < pattern_len)
    {
        patterns.push_back(rand_pattern());
        total+=patterns.back().size();
    }

    cout << "M = "<<total << " P = "<<patterns.size()<<"\n";
    string txt=text.substr(0, minlen);;

    while(txt.size() <= maxlen)
    {
        cout << "len = "<<txt.size()<< " / "<<maxlen <<"\n";
        fprintf(fout, "%8i ", txt.size());
        for (int i = 0; i < matcher_list_size; i++)
        {
            const int p = 10;
            double t = timef();
            double mt = 0 ;
            for (int j = 0 ; j < p; j++)
            {
                match_vector matches;
                matcher_list[i]->match(txt, patterns, matches);
                mt+=matcher_list[i]->match_time;
            }
            // cout << matches.size() << " matches\n";
            t = timef()-t;
            t/=p;
            mt/=p;
            fprintf(fout, "%8.8lf %8.8lf ", t, mt);
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
    int pattern_count  = 10000;
    int step = 10000;
    vector<int> lens;
    vector<string> patterns;
    int total = 0;
    int cl = 20;
    for (int i=0;i<pattern_count;i++)
    {
        patterns.push_back(rand_pattern(cl));
        total+=patterns.back().size();
    }
    cout << "M = "<<total << " P = "<<patterns.size()<<"\n";

    string txt=text.substr(0, minlen);;

    while(txt.size() <= maxlen)
    {
        cout << "len = "<<txt.size()<< " / "<<maxlen <<"\n";
        fprintf(fout, "%8i ", txt.size());
        for (int i = 0; i < matcher_list_size; i++)
        {
            const int p = 10;
            double t = timef();
            double mt = 0 ;
            for (int j = 0 ; j < p; j++)
            {
                match_vector matches;
                matcher_list[i]->match(txt, patterns, matches);
                mt+=matcher_list[i]->match_time;
            }
            // cout << matches.size() << " matches\n";
            t = timef()-t;
            t/=p;
            mt/=p;
            fprintf(fout, "%8.8lf %8.8lf ", t, mt);
        }
        fprintf(fout, "\n");
        // add to txt
        txt+=text.substr(txt.size(),step);
    }
    fclose(fout);
}
void warmup()
{
    int c = 1<<24;
    long long y = 0;
    for (int i = 0 ; i <c; i++)
        y += rand();
    cout << "Warmup: "<< y << "\n";
}

int main(int argc, char** argv)
{
    warmup();
    text = read_text("input/musketeers.txt");
    fixed_len("data/fixed_len.dat");

    //variable_len("data/var_len.dat");
    //growing_text("data/text_len.dat");
    //growing_text_fixed("data/text_len_fixed.dat");

    system("gnuplot plots.gp");
}
