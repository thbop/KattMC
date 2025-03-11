#ifndef KMCAPI_H
#define KMCAPI_H

#include "wstring.h"
#include "gptsock.h"
#include "notchtypes.h"
#include "packet.h"
#include "packettypes.h"
#include "client.h"

void KMCHandshake(Client* client) {
    // Construct packet buffer
    size_t bufferSize;
    char *buffer = PacketTypeNewHandshake(L"-", &bufferSize);

    // Send packet and the free buffer
    GPTSockSend(client->sock, buffer, bufferSize);
    free(buffer);
}

void KMCLogin(Client* client, int64_t mapSeed, nbyte dimension) {
    // Construct packet buffer
    size_t bufferSize;
    char *buffer = PacketTypeNewLogin(client->entityID, mapSeed, dimension, &bufferSize);

    // Send packet and the free buffer
    GPTSockSend(client->sock, buffer, bufferSize);
    free(buffer);
}


void KMCKick(Client* client, wchar *reason) {
    // Construct packet buffer
    size_t bufferSize;
    char *buffer = PacketTypeNewKick(reason, &bufferSize);

    // Send packet and the free buffer
    GPTSockSend(client->sock, buffer, bufferSize);
    free(buffer);
    client->state = CLIENTSTATE_DISCONNECTED;

    // Log event
    TLog("SERVER: Kicked %s!\n", client->username);
}

#endif