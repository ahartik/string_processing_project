#ifndef TIMEF_H
#define TIMEF_H
#include<time.h>

static double timef() {
    return clock() / (double)CLOCKS_PER_SEC;
}

#endif

