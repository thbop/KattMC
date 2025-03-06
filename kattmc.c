#include "stdio.h"
#include "stdbool.h"

#include "tlog.h"
#include "gptsock.h"
#include "notchtypes.h"
#include "packet.h"


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
        GPTSOCK_print(buffer, len);

        char res[5] = { 2, 0, 1, 0, '-' };
        GPTSOCK_send(clientSock, res, 5);

        memset(buffer, 0, GPTSOCK_BUFFER_SIZE);
        GPTSOCK_recv(clientSock, buffer, GPTSOCK_BUFFER_SIZE);
        GPTSOCK_print(buffer, len);

        char login[16] = { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        GPTSOCK_send(clientSock, login, 16);
        

        PacketList list = NewPacketList();
        PacketListAppend( &list, NOTCHTYPE_BYTE, toNotch(NOTCHTYPE_BYTE, 0xFF, 0.0, NULL) );
        PacketListAppend( &list, NOTCHTYPE_STRING16, toNotch(NOTCHTYPE_STRING16, 0, 0.0, L"§dHello, you have been kicked by my server. Take that idiot!") );
        
        size_t kickLen;
        char *kick = PacketEncode(&list, &kickLen);
        GPTSOCK_send(clientSock, kick, kickLen);

        free(kick);
        PacketListFree(&list);

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