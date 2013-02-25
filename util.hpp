#include<vector>
#include<sstream>
#include<time.h>
#include<iostream>
#include<fstream>
using namespace std;

namespace {

    vector<string> read_words(const char* filename) {
        ifstream fin(filename);
        vector<string> words;
        while(!fin.eof()) {
            string a;
            fin>>a;
            if(!fin.eof()) words.push_back(a);

        }
        return words;
    }

    string read_text(const char* filename) {
        std::ifstream t(filename);
        std::stringstream buffer;
        buffer << t.rdbuf();
        return buffer.str();
    }
    double timef() {
        return clock() / (double)CLOCKS_PER_SEC;
    }
}
