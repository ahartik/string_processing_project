#include<vector>
#include<sstream>
#include<iostream>
#include<fstream>
using namespace std;


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
