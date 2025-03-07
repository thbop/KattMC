#ifndef WSTRING_H
#define WSTRING_H

// This library might not have purpose

#include "wchar.h"



typedef unsigned short wchar;

size_t wstrlen( const wchar *str ) {
// #ifdef _WIN32
    return wcslen(str);
// #elif linux
//     size_t size = 0;
//     while ( *str ) {
//         str++;
//         size++;
//     }
//     return size;
// #endif
}

void wprint( wchar *str ) {
    while ( *str ) {
        printf("%lc", *str);
        str++;
    }
}




#endif