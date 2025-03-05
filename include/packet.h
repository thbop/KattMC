#ifndef PACKET_H
#define PACKET_H

// A library to encode/decode packets

#include "notchtypes.h"

typedef struct PacketItem {
    int type;
    void *value;
    void *prev, *next;
} PacketItem;

typedef struct PacketList {
    PacketItem *head, *tail;
} PacketList;

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

void PacketListFree( PacketList *list ) {}

// Calculates the number in bytes of an encoded packet
size_t PacketSize( PacketList *list ) {}

char *PacketEncode( PacketList *list ) {}

PacketList *PacketDecode( char *buffer ) {}



#endif