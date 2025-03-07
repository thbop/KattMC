#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "constants.h"
#include "gptsock.h"
#include "kmcapi.h"

typedef struct Client {
    sock_t sock;
    int entityID;
    // Entity *entity;
    char username[MAXUSERNAMELEN+1];

    void *prev, *next;
} Client;


void *clientHandler(void *args) {
    sock_t client = *(sock_t*)args;

    char buffer[GPTSOCK_BUFFER_SIZE];

    int size = GPTSOCK_recv(client, buffer, GPTSOCK_BUFFER_SIZE);
    PacketList *clientHandshake = PacketDecode(buffer, size);

    KMC_kick(client, clientHandshake->tail->value);

    PacketListFree(clientHandshake);


    GPTSOCK_close(client);
    pthread_exit(NULL);
}

#endif