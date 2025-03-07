#ifndef PACKET_H
#define PACKET_H

// A library to encode/decode packets

#include "gptsock.h"
#include "notchtypes.h"
#include "tlog.h"



typedef struct PacketItem {
    int type;
    void *value;
    void *prev, *next;
} PacketItem;

typedef struct PacketList {
    PacketItem *head, *tail;
    bool notchMode;
} PacketList;

#define PacketListForEach(list, it) \
    for ( PacketItem *it = list->head; it != NULL; it = it->next )


PacketList *NewPacketList(bool notchMode) {
    PacketList *list = (PacketList*)malloc(sizeof(PacketList));
    *list = (PacketList){
        .head      = NULL,
        .tail      = NULL,
        .notchMode = notchMode,
    };
    return list;
}

PacketItem *PacketListAppend( PacketList *list, int type, void *value ) {
    PacketItem *item = (PacketItem*)malloc(sizeof(PacketItem));
    item->type  = type;
    item->value = value;
    item->prev  =
    item->next  = NULL;

    if ( list->head == NULL && list->tail == NULL ) {
        list->head = 
        list->tail = item;
    }
    else {
        item->prev = list->tail;
        list->tail->next = item;
        list->tail = item;
    }
}

// Packet List Append Shorthands:
// Notch Appends
#define PLA_NByte(list, value) \
    PacketListAppend( list, NOTCHTYPE_BYTE, toNotch(NOTCHTYPE_BYTE, value, 0.0, NULL) )

#define PLA_NShort(list, value) \
    PacketListAppend( list, NOTCHTYPE_SHORT, toNotch(NOTCHTYPE_SHORT, value, 0.0, NULL) )

#define PLA_NInt(list, value) \
    PacketListAppend( list, NOTCHTYPE_INT, toNotch(NOTCHTYPE_INT, value, 0.0, NULL) )

#define PLA_NLong(list, value) \
    PacketListAppend( list, NOTCHTYPE_LONG, toNotch(NOTCHTYPE_LONG, value, 0.0, NULL) )

#define PLA_NFloat(list, value) \
    PacketListAppend( list, NOTCHTYPE_FLOAT, toNotch(NOTCHTYPE_FLOAT, 0, value, NULL) )

#define PLA_NDouble(list, value) \
    PacketListAppend( list, NOTCHTYPE_DOUBLE, toNotch(NOTCHTYPE_DOUBLE, 0, value, NULL) )

#define PLA_NBool(list, value) \
    PacketListAppend( list, NOTCHTYPE_BOOL, toNotch(NOTCHTYPE_BOOL, value, 0.0, NULL) )

#define PLA_NString8(list, value) \
    PacketListAppend( list, NOTCHTYPE_STRING8, toNotch(NOTCHTYPE_STRING8, 0, 0.0, value) )

#define PLA_NString16(list, value) \
    PacketListAppend( list, NOTCHTYPE_STRING16, toNotch(NOTCHTYPE_STRING16, 0, 0.0, value) )

// Regular appends (which is not confusing at all)
#define PLA_RByte(list, value) \
    PacketListAppend( list, REG_BYTE, regAlloc(REG_BYTE, value, 0.0, NULL) )

#define PLA_RShort(list, value) \
    PacketListAppend( list, REG_SHORT, regAlloc(REG_SHORT, value, 0.0, NULL) )

#define PLA_RInt(list, value) \
    PacketListAppend( list, REG_INT, regAlloc(REG_INT, value, 0.0, NULL) )

#define PLA_RLong(list, value) \
    PacketListAppend( list, REG_LONG, regAlloc(REG_LONG, value, 0.0, NULL) )

#define PLA_RFloat(list, value) \
    PacketListAppend( list, REG_FLOAT, regAlloc(REG_FLOAT, 0, value, NULL) )

#define PLA_RDouble(list, value) \
    PacketListAppend( list, REG_DOUBLE, regAlloc(REG_DOUBLE, 0, value, NULL) )

#define PLA_RBool(list, value) \
    PacketListAppend( list, REG_BOOL, regAlloc(REG_BOOL, value, 0.0, NULL) )

// Probably never use these tbh
#define PLA_RString(list, value) \
    PacketListAppend( list, REG_STRING, regAlloc(REG_STRING, 0, 0.0, value) )

#define PLA_RWString(list, value) \
    PacketListAppend( list, REG_WSTRING, regAlloc(REG_WSTRING, 0, 0.0, value) )


PacketItem *PLA_StringFromBuffer( PacketList* list, char *buffer ) {
    nstring8 *nstr = NotchTypeStrFromBuffer(NOTCHTYPE_STRING8, buffer);
    PacketItem *item = PacketListAppend( list, REG_STRING, fromNotch(NOTCHTYPE_STRING8, nstr) );
    free(nstr);
    return item;
}

PacketItem *PLA_WStringFromBuffer( PacketList* list, char *buffer ) {
    nstring16 *nstr = NotchTypeStrFromBuffer(NOTCHTYPE_STRING16, buffer);
    PacketItem *item = PacketListAppend( list, REG_WSTRING, fromNotch(NOTCHTYPE_STRING16, nstr) );
    free(nstr);
    return item;
}


void PacketListFree( PacketList *list ) {
    if ( list == NULL ) return;

    PacketItem *item = list->head;
    PacketItem *n;
    while ( item != NULL ) {
        n = item->next;
        if ( list->notchMode ) NotchTypeFree(item->type, item->value);
        else                   free(item->value);

        free(item);
        item = n;
    }
    free(list);
}


// Calculates the number in bytes of an encoded packet
size_t PacketSize( PacketList *list ) {
    size_t size = 0;
    PacketListForEach(list, item)
        size += NotchTypeSizeOf(item->type, item->value);
    
    return size;
}

size_t _PacketItemSet( char *buffer, PacketItem *item ) {
    size_t itemSize = NotchTypeSizeOf(item->type, item->value);

    switch (item->type) {
        case NOTCHTYPE_STRING8: {
            nstring8 *nstr = item->value;
            memcpy( buffer, &nstr->len, sizeof(nshort) );
            memcpy( buffer+sizeof(nshort), nstr->buffer, itemSize-sizeof(nshort) );
            return itemSize;
        } case NOTCHTYPE_STRING16: {
            nstring16 *nstr = item->value;
            memcpy( buffer, &nstr->len, sizeof(nshort) );
            memcpy( buffer+sizeof(nshort), nstr->buffer, itemSize-sizeof(nshort) );
            return itemSize;
        }
    }
    
    memcpy( buffer, item->value, itemSize );
    return itemSize;
}

char *PacketListEncode( PacketList *list, size_t *bufferSize ) {
    *bufferSize = PacketSize(list);
    if ( *bufferSize > GPTSOCK_BUFFER_SIZE )
        // If this happens, make packet size dynamic
        TLog("WARNING: Packet buffer size of %lld exceeds packet size of %d!", *bufferSize, GPTSOCK_BUFFER_SIZE);
    
    char *buffer = (char*)malloc( *bufferSize );
    size_t x = 0;
    PacketListForEach(list, item) {
        size_t itemSize = _PacketItemSet( buffer+x, item );
        x += itemSize;
    }

    return buffer;
}

// Might move to packettypes.h
// PacketList *PacketDecode( char *buffer ) {}



#endif