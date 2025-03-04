#include "stdio.h"
#include "stdbool.h"

#include "tlog.h"
#include "gptsock.h"


sock_t serverSock;

bool Init() {
    bool ok = true;
    TLog("SERVER: Starting!\n");
    ok = TLogInit();
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