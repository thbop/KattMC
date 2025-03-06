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
} PacketList;

#define PacketListForEach(list, it) \
    for ( PacketItem *it = list->head; it != NULL; it = it->next )

PacketList NewPacketList() {
    return (PacketList){NULL, NULL};
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

void PacketListFree( PacketList *list ) {
    PacketItem *item = list->head;
    PacketItem *n;
    while ( item != NULL ) {
        n = item->next;
        NotchTypeFree(item->type, item->value);
        free(item);
        item = n;
    }
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

char *PacketEncode( PacketList *list, size_t *bufferSize ) {
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

PacketList *PacketDecode( char *buffer ) {}



#endif