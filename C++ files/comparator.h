#include <string.h>

#ifndef ComparatorHeader_h
#define ComparatorHeader_h

using namespace std;

//function object
struct cmpStr {

    bool operator()(char* a, char* b) const {
        return strcmp(a, b) < 0;
    }

};

#endif
