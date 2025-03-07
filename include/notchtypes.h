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

// Regular counterparts
enum {
    REG_BYTE,
    REG_SHORT,
    REG_INT,
    REG_LONG,
    REG_FLOAT,
    REG_DOUBLE,
    REG_BOOL,
    REG_STRING,
    REG_WSTRING,
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
    nstring8 nstr;
    short len = strlen(str);
    nstr.len = len;
    _NotchTypeShortSwap(&nstr.len);

    nstr.buffer = (char*)malloc(len); // Do not include terminator

    memcpy(nstr.buffer, str, len);

    return nstr;
}

char *_NotchTypeFromString8( nstring8 *nstr ) {
    if ( nstr->len != 0 && nstr->buffer != NULL ) {
        short len = nstr->len;
        _NotchTypeShortSwap(&len);
        char *str = (char*)malloc(len+1);
        memcpy(str, nstr->buffer, len );
        str[len] = 0;
        
        return str;
    }
    return NULL;
}

nstring16 _NotchTypeToString16( wchar *str ) {
    nstring16 nstr;

    short len = wstrlen(str);
    nstr.len = len;
    _NotchTypeShortSwap(&nstr.len);

    nstr.buffer = (nshort*)malloc(nstr.len * sizeof(nshort)); // Do not include terminator

    // Slow manual copying... though it correctly translates to big endian
    for ( short i = 0; i < len; i++ ) {
        nshort c = str[i];
        _NotchTypeShortSwap(&c);
        nstr.buffer[i] = c;
    }

    return nstr;
}

wchar *_NotchTypeFromString16( nstring16 *nstr ) {
    if ( nstr->len != 0 && nstr->buffer != NULL ) {
        short len = nstr->len;
        _NotchTypeShortSwap(&len);

        wchar *str = (wchar*)malloc((len + 1) * sizeof(wchar));
        
        // Slow manual copying... but it correctly converts to little endian
        for ( short i = 0; i < len; i++ ) {
            short c = nstr->buffer[i];
            _NotchTypeShortSwap(&c);
            str[i] = c;
        }
        str[len] = 0;
        
        return str;
    }
    return NULL;
}

void *regAlloc( int type, int64_t integer, double number, void *string ) {
    switch (type) {
        case REG_BYTE: {
            nbyte *v = (nbyte*)malloc(sizeof(nbyte));
            *v = integer;
            return v;
        }
        case REG_SHORT: {
            nshort *v = (nshort*)malloc(sizeof(nshort));
            *v = integer;
            return v;
        }
        case REG_INT: {
            nint *v = (nint*)malloc(sizeof(nint));
            *v = integer;
            return v;
        }
        case REG_LONG: {
            nlong *v = (nlong*)malloc(sizeof(nlong));
            *v = integer;
            return v;
        }
        case REG_FLOAT: {
            nfloat *v = (nfloat*)malloc(sizeof(nfloat));
            *v = number;
            return v;
        }
        case REG_DOUBLE: {
            ndouble *v = (ndouble*)malloc(sizeof(ndouble));
            *v = number;
            return v;
        }
        case REG_BOOL: {
            nbool *v = (nbool*)malloc(sizeof(nbool));
            *v = integer;
            return v;
        }
        case REG_STRING: {
            size_t strSize = strlen(string)+1;
            char *str = (char*)malloc(strSize);
            memcpy(str, string, strSize);
            return str;
        }
        case REG_WSTRING: {
            size_t strSize = (strlen(string)+1)*sizeof(short);
            wchar *str = (wchar*)malloc(strSize);
            memcpy(str, string, strSize);
            return str;
        }
    }
    return NULL;
}


void *toNotch( int type, int64_t integer, double number, void *string ) {
    switch (type) {
        case NOTCHTYPE_BYTE: {
            nbyte *v = (nbyte*)malloc(sizeof(nbyte));
            *v = integer;
            return v;
        }
        case NOTCHTYPE_SHORT: {
            nshort *v = (nshort*)malloc(sizeof(nshort));
            *v = integer;
            _NotchTypeShortSwap(v);
            return v;
        }
        case NOTCHTYPE_INT: {
            nint *v = (nint*)malloc(sizeof(nint));
            *v = integer;
            _NotchTypeIntSwap(v);
            return v;
        }
        case NOTCHTYPE_LONG: {
            nlong *v = (nlong*)malloc(sizeof(nlong));
            *v = integer;
            _NotchTypeLongSwap(v);
            return v;
        }
        case NOTCHTYPE_FLOAT: {
            nfloat *v = (nfloat*)malloc(sizeof(nfloat));
            *v = number;
            return v;
        }
        case NOTCHTYPE_DOUBLE: {
            ndouble *v = (ndouble*)malloc(sizeof(ndouble));
            *v = number;
            return v;
        }
        case NOTCHTYPE_BOOL: {
            nbool *v = (nbool*)malloc(sizeof(nbool));
            *v = integer;
            return v;
        }
        case NOTCHTYPE_STRING8: {
            nstring8 *str = (nstring8*)malloc(sizeof(nstring8));
            *str = _NotchTypeToString8((char*)string);
            return str;
        }
        case NOTCHTYPE_STRING16: {
            nstring16 *str = (nstring16*)malloc(sizeof(nstring16));
            *str = _NotchTypeToString16((wchar*)string);
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
            short *v = (short*)malloc(sizeof(short));
            *v = *(short*)value;
            _NotchTypeShortSwap(v);
            return v;
        }
        case NOTCHTYPE_INT: {
            int *v = (int*)malloc(sizeof(int));
            *v = *(int*)value;
            _NotchTypeIntSwap(v);
            return v;
        }
        case NOTCHTYPE_LONG: {
            int64_t *v = (int64_t*)malloc(sizeof(int64_t));
            *v = *(int64_t*)value;
            _NotchTypeLongSwap(v);
            return v;
        }
        case NOTCHTYPE_BOOL: 
        case NOTCHTYPE_FLOAT:
        case NOTCHTYPE_DOUBLE:
            return value;

        case NOTCHTYPE_STRING8:
            return _NotchTypeFromString8(value);

        case NOTCHTYPE_STRING16:
            return _NotchTypeFromString16(value);
    }
    return NULL;
}

// DO NOT use NotchTypeFree on these strings!!!
// Use free instead
void *NotchTypeStrFromBuffer( int type, void *buffer ) {
    nshort len = *(nshort*)buffer;
    switch (type) {
        case NOTCHTYPE_STRING8: {
            nstring8 *nstr = (nstring8*)malloc(sizeof(nstring8));
            nstr->len = len;
            nstr->buffer = (char*)buffer + 2;
            return nstr;
        }
        case NOTCHTYPE_STRING16: {
            nstring16 *nstr = (nstring16*)malloc(sizeof(nstring16));
            nstr->len = len;
            nstr->buffer = (nshort*)buffer + 1;
            return nstr;
        }
    }
    return NULL;
}

void NotchTypePrintStr( int type, void *nstr ) {
    switch (type) {
        case NOTCHTYPE_STRING8: {
            char *str = _NotchTypeFromString8(nstr);
            printf("String8(\"%s\")", str);
            free(str);
            break;
        } case NOTCHTYPE_STRING16: {
            wchar *str = _NotchTypeFromString16(nstr);
            printf("String16(\"");
            wprint(str);
            printf("\")");
            free(str);

            break;
        }
    }
}

size_t NotchTypeSizeOf( int type, void *value ) {
    switch (type) {
        case NOTCHTYPE_BYTE:     return sizeof(nbyte);
        case NOTCHTYPE_SHORT:    return sizeof(nshort);
        case NOTCHTYPE_INT:      return sizeof(nint);
        case NOTCHTYPE_LONG:     return sizeof(nlong);
        case NOTCHTYPE_BOOL:     return sizeof(nbool);
        case NOTCHTYPE_FLOAT:    return sizeof(nfloat);
        case NOTCHTYPE_DOUBLE:   return sizeof(ndouble);
        case NOTCHTYPE_STRING8: {
            short len = ((nstring8*)value)->len;
            _NotchTypeShortSwap(&len);
            return len + sizeof(nshort);
        }
        case NOTCHTYPE_STRING16: {
            short len = ((nstring16*)value)->len;
            _NotchTypeShortSwap(&len);
            return (len + 1) * sizeof(nshort);
        }
    }
    return 0;
}

void NotchTypeFree( int type, void *value ) {
    if ( value == NULL ) return;
    switch (type) {
        case NOTCHTYPE_STRING8: {
            char *buffer = ((nstring8*)value)->buffer;
            if ( buffer != NULL )
                free( buffer );
            break;
        } case NOTCHTYPE_STRING16: {
            nshort *buffer = ((nstring16*)value)->buffer;
            if ( buffer != NULL )
                free( buffer );
            break;
        }
    }
    free(value);
}

#endif