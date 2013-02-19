#include<iostream>
#include<cstdlib>
using namespace std;


int main(int argc, char** argv) {
    srand(time(0));
    int pattern_length_min=5;
    int pattern_length_max=10;
    int alphabet_size=40;
    int pattern_count=100;


    if(argc>=2) pattern_length_min=atoi(argv[1]);
    if(argc>=3) pattern_length_max=atoi(argv[2]);
    if(argc>=4) pattern_count=atoi(argv[3]);
    if(argc>=5) alphabet_size=atoi(argv[4]);
    char low='A';
    if (alphabet_size + low > 'Z')
        low = '!';
    for(int j=0;j<pattern_count;j++) {
        int len = pattern_length_min;
        if (len != pattern_length_max)
            len+= rand()%(pattern_length_max-len);
        for(int i=0;i<len;i++) {
            cout<<(char)(( rand() % alphabet_size) + low);
        }
        cout<<endl;
    }

}
