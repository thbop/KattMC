#ifndef PACKETTYPES_H
#define PACKETTYPES_H

// Handle packet construction

#include "stdint.h"

#include "wstring.h"
#include "external/rcompress.h"

#include "constants.h"
#include "packet.h"

enum {
    PACKETTYPE_KEEPALIVE = 0x00, // Client to server
    PACKETTYPE_LOGIN     = 0x01, // Both
    PACKETTYPE_HANDSHAKE = 0x02, // Both
    PACKETTYPE_CHATMSG   = 0x03, // Both

    PACKETTYPE_KICK      = 0xFF, // Server to client
};

char *PacketTypeNewLogin( int entityID, int64_t mapSeed, char dimension, size_t *bufferSize ) {
    PacketList *list = NewPacketList(true);
    PLA_NByte(list, PACKETTYPE_LOGIN);
    PLA_NInt(list, entityID);
    PLA_NString16(list, L"");
    PLA_NLong(list, mapSeed);
    PLA_NByte(list, dimension);

    char *buffer = PacketListEncode(list, bufferSize);
    PacketListFree(list);
    return buffer;
}

char *PacketTypeNewHandshake( wchar *hash, size_t *bufferSize ) {
    PacketList *list = NewPacketList(true);
    PLA_NByte(list, PACKETTYPE_HANDSHAKE);
    PLA_NString16(list, hash);

    char *buffer = PacketListEncode(list, bufferSize);
    PacketListFree(list);
    return buffer;
}


char *PacketTypeNewKick( wchar *reason, size_t *bufferSize ) {
    PacketList *list = NewPacketList(true);
    PLA_NByte(list, PACKETTYPE_KICK);
    PLA_NString16(list, reason);

    char *buffer = PacketListEncode(list, bufferSize);
    PacketListFree(list);
    return buffer;
}


#define PACKETTYPE_MINLOGINSIZE     16
#define PACKETTYPE_MAXLOGINSIZE     (PACKETTYPE_MINLOGINSIZE+MAXUSERNAMESIZE)
#define PACKETTYPE_MINHANDSHAKESIZE 3
#define PACKETTYPE_MAXHANDSHAKESIZE (PACKETTYPE_MINHANDSHAKESIZE+MAXUSERNAMESIZE)

PacketList *_PacketTypeDecodeLogin( char *buffer, int bufferSize ) {
    if ( bufferSize < PACKETTYPE_MINLOGINSIZE || bufferSize > PACKETTYPE_MAXLOGINSIZE ) {
        TLog("WARNING: Invalid login packet!\n");
        return NULL;
    }

    PacketList *list = NewPacketList(false);
    int x = 0;

    PLA_NByte(list, *buffer);              x += sizeof(nbyte);                            // Packet type
    PLA_NInt(list, *(int*)(buffer+x));     x += sizeof(nint);                             // Protocol version
    PLA_WStringFromBuffer(list, buffer+x); x += wstrlen(list->tail->value)+sizeof(wchar); // Username
    // There are two other meaningless values that I'll ignore                            // Seed
                                                                                          // Dimension

    return list;
}

PacketList *_PacketTypeDecodeHandshake( char *buffer, int bufferSize ) {
    if ( bufferSize < PACKETTYPE_MINHANDSHAKESIZE || bufferSize > PACKETTYPE_MAXHANDSHAKESIZE ) {
        TLog("WARNING: Invalid handshake packet!\n");
        return NULL;
    }

    PacketList *list = NewPacketList(false);
    int x = 0;

    PLA_RByte(list, *buffer); x += sizeof(nbyte); // Packet type
    PLA_WStringFromBuffer(list, buffer+x);        // Username

    return list;
}

PacketList *PacketDecode( char *buffer, int bufferSize ) {
    nbyte type = *buffer; // The type should be the first byte in the buffer
    switch (type) {
        case PACKETTYPE_KEEPALIVE: break;
        case PACKETTYPE_LOGIN:     return _PacketTypeDecodeLogin(buffer, bufferSize);
        case PACKETTYPE_HANDSHAKE: return _PacketTypeDecodeHandshake(buffer, bufferSize);
    }

    return NULL;
}

#endif