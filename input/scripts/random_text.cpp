#include<iostream>
#include<cstdlib>
using namespace std;


int main(int argc, char** argv) {
    srand(time(0));
    int alphabet_size=40;
    int text_length=100;
    if(argc>=2) text_length=atoi(argv[1]);
    if(argc>=3) alphabet_size=atoi(argv[2]);
    char low='A';
    if (alphabet_size + low > 'Z')
        low = '!';
    if(alphabet_size > 90) low=1;
    for(int i=0;i<text_length;i++) cout<<(char)(( rand() % alphabet_size) + low);

}
