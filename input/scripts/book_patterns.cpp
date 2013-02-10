#include<iostream>
#include<cstdlib>
#include "../../util.hpp"
using namespace std;


int main(int argc, char** argv) {
    srand(time(0));
    int pattern_length=5;
    int pattern_count=100;

    string source = read_text(argv[1]);
    if(argc>=3) pattern_length=atoi(argv[2]);
    if(argc>=4) pattern_count=atoi(argv[3]);
    for(int j=0;j<pattern_count;j++) {
        int a = rand() % (source.length() - pattern_length);
        int b = a + pattern_length;
        for(int i=a;i<b;i++) cout<<source[i];
    cout<<endl;

    }   

}
