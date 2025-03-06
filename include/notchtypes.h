#ifndef NOTCHTYPES_H
#define NOTCHTYPES_H

// A library to work with Minecraft protocol types
// Assumes the host is little endian

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"
#include "wstring.h"



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
    nshort *buffer;
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
    short len = strlen(str);
    str8.len = len;
    _NotchTypeShortSwap(&str8.len);

    str8.buffer = (char*)malloc(len); // Do not include terminator

    memcpy(str8.buffer, str, len);

    return str8;
}

char *_NotchTypeFromString8( nstring8 str8 ) {
    if ( str8.len != 0 && str8.buffer != NULL ) {
        short len = str8.len;
        _NotchTypeShortSwap(&len);
        char *str = (char*)malloc(len+1);
        memcpy(str, str8.buffer, len );
        str[len] = 0;
        
        return str;
    }
    return NULL;
}

nstring16 _NotchTypeToString16( wchar *str ) {
    nstring16 str16;

    short len = wstrlen(str);
    str16.len = len;
    _NotchTypeShortSwap(&str16.len);

    str16.buffer = (nshort*)malloc(str16.len * sizeof(nshort)); // Do not include terminator

    // Slow manual copying... though it correctly translates to big endian
    for ( short i = 0; i < len; i++ ) {
        nshort c = str[i];
        _NotchTypeShortSwap(&c);
        str16.buffer[i] = c;
    }

    return str16;
}

wchar *_NotchTypeFromString16( nstring16 str16 ) {
    if ( str16.len != 0 && str16.buffer != NULL ) {
        short len = str16.len;
        _NotchTypeShortSwap(&len);

        wchar *str = (wchar*)malloc((len + 1) * sizeof(wchar));
        
        // Slow manual copying... but it correctly converts to little endian
        for ( short i = 0; i < len; i++ ) {
            short c = str16.buffer[i];
            _NotchTypeShortSwap(&c);
            str[i] = c;
        }
        str[len] = 0;
        
        return str;
    }
    return NULL;
}


void *toNotch( int type, void *value ) {
    switch (type) {
        case NOTCHTYPE_BYTE: {
            nbyte *v = (nbyte*)malloc(sizeof(nbyte));
            *v = *(nbyte*)value;
            return v;
        }
        case NOTCHTYPE_SHORT: {
            nshort *v = (nshort*)malloc(sizeof(nshort));
            *v = *(nshort*)value;
            _NotchTypeShortSwap(v);
            return v;
        }
        case NOTCHTYPE_INT: {
            nint *v = (nint*)malloc(sizeof(nint));
            *v = *(nint*)value;
            _NotchTypeIntSwap(v);
            return v;
        }
        case NOTCHTYPE_LONG: {
            nlong *v = (nlong*)malloc(sizeof(nlong));
            *v = *(nlong*)value;
            _NotchTypeLongSwap(v);
            return v;
        }
        case NOTCHTYPE_BOOL: {
            nbool *v = (nbool*)malloc(sizeof(nbool));
            *v = *(nbool*)value;
            return v;
        }
        case NOTCHTYPE_STRING8: {
            nstring8 *str = (nstring8*)malloc(sizeof(nstring8));
            *str = _NotchTypeToString8((char*)value);
            return str;
        }
        case NOTCHTYPE_STRING16: {
            nstring16 *str = (nstring16*)malloc(sizeof(nstring16));
            *str = _NotchTypeToString16((wchar*)value);
            return str;
        }
    }
    return NULL;
}

void *fromNotch( int type, void *value ) {
    switch (type) {
        case NOTCHTYPE_BYTE: 
            return value;
        case NOTCHTYPE_SHORT: {
            nshort *v = *(nshort*)value;
            _NotchTypeShortSwap(v);
            return v;
        }
        case NOTCHTYPE_INT: {
            nint *v = *(nint*)value;
            _NotchTypeIntSwap(v);
            return v;
        }
        case NOTCHTYPE_LONG: {
            nlong *v = *(nlong*)value;
            _NotchTypeLongSwap(v);
            return v;
        }
        case NOTCHTYPE_BOOL: 
            return value;

        case NOTCHTYPE_STRING8:
            return _NotchTypeFromString8(*(nstring8*)value);
            
        case NOTCHTYPE_STRING16:
            return _NotchTypeFromString16(*(nstring16*)value);
    }
    return NULL;
}

void NotchTypePrintStr( int type, void *nstr ) {
    switch (type) {
        case NOTCHTYPE_STRING8: {
            char *str = _NotchTypeFromString8(*(nstring8*)nstr);
            printf("String8(\"%s\")", str);
            free(str);
            break;
        } case NOTCHTYPE_STRING16: {
            wchar *str = _NotchTypeFromString16(*(nstring16*)nstr);
            printf("String16(\"");
            wprint(str);
            printf("\")");
            free(str);

            break;
        }
    }
}

void NotchTypeFreeStr( int type, void *nstr ) {
    switch (type) {
        case NOTCHTYPE_STRING8: {
            char *buffer = ((nstring8*)nstr)->buffer;
            if ( buffer != NULL )
                free( buffer );
            break;
        } case NOTCHTYPE_STRING16: {
            nshort *buffer = ((nstring16*)nstr)->buffer;
            if ( buffer != NULL )
                free( buffer );
            break;
        }
    }
    free(nstr);
}

#endif