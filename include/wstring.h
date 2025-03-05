#ifndef WSTRING_H
#define WSTRING_H

#include "stdarg.h"

#ifdef _WIN32
#include "wchar.h"
#endif


typedef unsigned short wchar;

size_t wstrlen( const wchar *str ) {
#ifdef _WIN32
    return wcslen(str);
#elif linux
    size_t size = 0;
    while ( *str ) {
        str++;
        size++;
    }
    return size;
#endif
}

void wprint( wchar *str ) {
    while ( *str ) {
        printf("%lc", *str);
        str++;
    }
}




#endif