#include "stdio.h"
#include "stdbool.h"

#include "tlog.h"
#include "gptsock.h"
#include "notchtypes.h"
#include "packet.h"
#include "packettypes.h"


sock_t serverSock;

bool Init() {
    bool ok = true;
    ok = TLogInit();
    TLog("SERVER: Starting!\n");
    GPTSOCK_Init();

    serverSock = GPTSOCK_socket();
    GPTSOCK_bind(serverSock, 25565);


    return ok;
}

void Update() {

}

void Run() {
    GPTSOCK_listen(serverSock, 0);
    sock_t clientSock = GPTSOCK_accept(serverSock);
    if ( clientSock != INVALID_SOCKET ) {
        char buffer[GPTSOCK_BUFFER_SIZE] = {0};

        int len = GPTSOCK_recv(clientSock, buffer, GPTSOCK_BUFFER_SIZE);
        // GPTSOCK_print(buffer, len);
        PacketList *clientHandshake = PacketDecode(buffer, len);
        char username[17];
        wcstombs(username, clientHandshake->tail->value, 16);
        TLog("SERVER: Handshake from \"%s\"!\n", username);

        PacketListFree(clientHandshake);

        size_t handshakeLen;
        char *handshake = PacketTypeNewHandshake(L"-", &handshakeLen);
        GPTSOCK_send(clientSock, handshake, handshakeLen);
        free(handshake);

        memset(buffer, 0, GPTSOCK_BUFFER_SIZE);
        len = GPTSOCK_recv(clientSock, buffer, GPTSOCK_BUFFER_SIZE);
        GPTSOCK_print(buffer, len);

        size_t loginLen;
        char *login = PacketTypeNewLogin(1, 0, 0, &loginLen);
        GPTSOCK_send(clientSock, login, loginLen);
        free(login);

        size_t kickLen;
        char *kick = PacketTypeNewKick(L"§5Thou hast been kicked!", &kickLen);
        GPTSOCK_send(clientSock, kick, kickLen);
        free(kick);
        

        GPTSOCK_close(clientSock);
    }
    GPTSOCK_close(serverSock);
}


void Close() {
    TLogClose();
}


int main() {
    if ( !Init() )
        return 1;
    Run();
    Close();

    return 0;
}