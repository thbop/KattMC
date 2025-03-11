#ifndef CLIENT_H
#define CLIENT_H

#include "constants.h"

enum {
    CLIENTSTATE_CONNECTED,
    CLIENTSTATE_DISCONNECTED,
    CLIENTSTATE_HANDSHAKE,
    CLIENTSTATE_IN_GAME,
};

typedef struct Client {
    sock_t sock;
    int state;
    int entityID;
    // Entity *entity;
    char username[MAX_USERNAME_LEN+1];

    void *prev, *next;
} Client;

#endif