#include<iostream>
#include<cstdlib>
using namespace std;


int main(int argc, char** argv) {
    srand(time(0));
    int pattern_length=5;
    int alphabet_size=40;
    int pattern_count=100;


    if(argc>=2) pattern_length=atoi(argv[1]);
    if(argc>=3) pattern_count=atoi(argv[2]);
    if(argc>=4) alphabet_size=atoi(argv[3]);
    char low='!';
    for(int j=0;j<pattern_count;j++) {
        for(int i=0;i<pattern_length;i++) {
            cout<<(char)(( rand() % alphabet_size) + low);
        }
        cout<<endl;
    }

}
