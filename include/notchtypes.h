#ifndef NOTCHTYPES_H
#define NOTCHTYPES_H

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"

enum {
    NOTCHTYPE_BYTE,
    NOTCHTYPE_SHORT,
    NOTCHTYPE_INT,
    NOTCHTYPE_LONG,
    NOTCHTYPE_FLOAT,
    NOTCHTYPE_DOUBLE,
    NOTCHTYPE_BOOL,
    NOTCHTYPE_STRING8,
    NOTCHTYPE_STRING16,
};


// Everything is little endian
typedef int8_t  nbyte;
typedef int16_t nshort;
typedef int32_t nint;
typedef int64_t nlong;
typedef float   nfloat;
typedef double  ndouble;
typedef bool    nbool;
typedef struct nstring8 {
    nshort len;
    char *buffer;
} nstring8;

typedef struct nstring16 {
    nshort len;
    short *buffer;
} nstring16;

void _NotchTypeShortSwap( nshort *v ) {
    *v = ( *v >> 8 ) | ( *v << 8 );
}

void _NotchTypeIntSwap( nint *v ) {
    *v = ( *v >> 24 ) |
         ( ( *v & 0x00FF0000 ) >> 8 ) | 
         ( ( *v & 0x0000FF00 ) << 8 ) |
         ( *v << 24 );
}

void _NotchTypeLongSwap( nlong *v ) {
    *v = ( *v >> 56 ) |
         ( ( *v & 0x00FF000000000000 ) >> 40 ) |
         ( ( *v & 0x0000FF0000000000 ) >> 24 ) |
         ( ( *v & 0x000000FF00000000 ) >> 8 )  |
         ( ( *v & 0x00000000FF000000 ) << 8 )  |
         ( ( *v & 0x0000000000FF0000 ) << 24 ) |
         ( ( *v & 0x000000000000FF00 ) << 40 ) |
         ( *v << 56 );
}

nstring8 _NotchTypeToString8( char *str ) {
    nstring8 str8;
    str8.len = strlen(str);
    _NotchTypeShortSwap(&str8.len);

    str8.buffer = (char*)malloc(str8.len); // Do not include terminator

    memcpy(str8.buffer, str, str8.len);

    return str8;
}

char *_NotchTypeFromString8( nstring8 str8 ) {
    if ( str8.len != 0 && str8.buffer != NULL ) {
        short len = str8.len;
        _NotchTypeShortSwap(&len);
        char *str = (char*)malloc(len);
        memcpy(str, str8.buffer, len );
        
        return str;
    }
    return NULL;
}


void toNotch( int type, void *value ) {
    switch (type) {
        case NOTCHTYPE_SHORT:
            _NotchTypeShortSwap((nshort*)value);
            break;
        case NOTCHTYPE_INT:
            _NotchTypeIntSwap((nint*)value);
            break;
        case NOTCHTYPE_LONG:
            _NotchTypeLongSwap((nlong*)value);
            break;
    }
}

void fromNotch( int type, void *value ) {
    switch (type) {
        case NOTCHTYPE_SHORT:
            _NotchTypeShortSwap((nshort*)value);
            break;
        case NOTCHTYPE_INT:
            _NotchTypeIntSwap((nint*)value);
            break;
        case NOTCHTYPE_LONG:
            _NotchTypeLongSwap((nlong*)value);
            break;
    }
}

#endif