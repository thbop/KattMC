#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "constants.h"
#include "gptsock.h"
#include "client.h"
#include "kmcapi.h"


Client *g_clients;

void ClientHandlerInit() {
    g_clients = NULL;
}

Client *NewClient( sock_t socket ) {
    Client *client = (Client*)malloc(sizeof(Client));

    *client = (Client){
        .sock     = socket,
        .state    = CLIENTSTATE_CONNECTED,
        .entityID = 0,
        .username = {0},
        .prev     = NULL,
        .next     = NULL,
    };

    if ( g_clients == NULL )
        g_clients = client;
    else {
        g_clients->prev = client;
        client->next    = g_clients;
        g_clients       = client;
    }

    return client;
}

void RemoveClient( Client *removed ) {
    if ( removed == NULL ) return;

    Client *prev = removed->prev;
    Client *next = removed->next;

    if ( prev != NULL ) prev->next = next;
    if ( next != NULL ) next->prev = prev;

    GPTSockClose(removed->sock);
    free(removed);
}

void ClientHandlerUnload() {
    Client *client = g_clients;
    Client *n;
    while ( client != NULL ) {
        n = client->next;
        free(client);
        client = n;
    }
}

PacketList *_ClientHandlerRecv( Client *client, char *buffer ) {
    int size = GPTSockRecv(client->sock, buffer, GPTSOCK_BUFFER_SIZE);
    if ( size > 0 ) {
        PacketList *list = PacketDecode(buffer, size);
        return list;
    }

    client->state = CLIENTSTATE_DISCONNECTED;
    return NULL;
}


void *ClientHandlerMain(void *args) {
    Client *client = NewClient( *(sock_t*)args );

    char buffer[GPTSOCK_BUFFER_SIZE];

    while ( client->state != CLIENTSTATE_DISCONNECTED ) {
        PacketList *plist = _ClientHandlerRecv(client, buffer);
        if ( plist != NULL ) {
            nbyte ptype = *(nbyte*)plist->head->value;
            switch (client->state) {
                case CLIENTSTATE_CONNECTED:
                    if ( ptype == PACKETTYPE_HANDSHAKE ) {      // If received a handshake, proceed to login
                        client->state = CLIENTSTATE_HANDSHAKE;
                        KMCHandshake(client);
                    }
                    break;
                case CLIENTSTATE_HANDSHAKE:
                    if ( ptype == PACKETTYPE_LOGIN ) {         // If received a login, proceed to in game (maybe)
                        client->state = CLIENTSTATE_IN_GAME;
                        
                        PacketItem *usernameItem = PL_At(plist, 2);
                        if ( usernameItem == NULL )
                            KMCKick(client, L"Invalid login packet!");
                        else {
                            wcstombs(client->username, usernameItem->value, MAX_USERNAME_LEN+1);
                            KMCLogin(client, 0, 0);
                            TLog("SERVER: Player %s joined the game!\n", client->username); // Probably send this as a chat message
                        }
                    }
                    break;
            }

            // KMCKick(client, L"§5Skill issue.");
        
            PacketListFree(plist);
        }
    }


    RemoveClient(client);
    TLog("SERVER: Client disconnect!\n");
    pthread_exit(NULL);
}

#endif