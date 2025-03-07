#ifndef KMCAPI_H
#define KMCAPI_H

#include "wstring.h"
#include "gptsock.h"
#include "notchtypes.h"
#include "packet.h"
#include "packettypes.h"


// TODO: Pass in the client instead of client socket
void KMC_kick(sock_t client, wchar *reason) {
    // Construct kick packet buffer
    size_t bufferSize;
    char *buffer = PacketTypeNewKick(reason, &bufferSize);

    // Send packet and the free buffer
    GPTSOCK_send(client, buffer, bufferSize);
    free(buffer);

    // Log event
}

#endif