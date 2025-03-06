#ifndef PACKETTYPES_H
#define PACKETTYPES_H

// Handle packet construction

#include "stdint.h"
#include "packet.h"

enum {
    PACKETTYPE_KEEPALIVE = 0x00,
    PACKETTYPE_LOGIN     = 0x01,
    PACKETTYPE_HANDSHAKE = 0x02,
    PACKETTYPE_CHATMSG   = 0x03,

    PACKETTYPE_KICK      = 0xFF,
};

char *PacketTypeNewLogin( int entityID, int64_t mapSeed, char dimension, size_t *bufferSize ) {
    PacketList list = NewPacketList();
    PLA_Byte(list, PACKETTYPE_LOGIN);
    PLA_Int(list, entityID);
    PLA_String16(list, L"");
    PLA_Long(list, mapSeed);
    PLA_Byte(list, dimension);

    char *buffer = PacketListEncode(&list, bufferSize);
    PacketListFree(&list);
    return buffer;
}

char *PacketTypeNewHandshake( wchar *hash, size_t *bufferSize ) {
    PacketList list = NewPacketList();
    PLA_Byte(list, PACKETTYPE_HANDSHAKE);
    PLA_String16(list, hash);

    char *buffer = PacketListEncode(&list, bufferSize);
    PacketListFree(&list);
    return buffer;
}


char *PacketTypeNewKick( wchar *reason, size_t *bufferSize ) {
    PacketList list = NewPacketList();
    PLA_Byte(list, PACKETTYPE_KICK);
    PLA_String16(list, reason);

    char *buffer = PacketListEncode(&list, bufferSize);
    PacketListFree(&list);
    return buffer;
}

#endif