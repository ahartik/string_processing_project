#include<vector>
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
    vector<string> words=read_words(filename);
    string s="";
    for(int i=0;i<words.size();i++) s+=(i>0 ? " " : "") + words[i];
    return s;
}
