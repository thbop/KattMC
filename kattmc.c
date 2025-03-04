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

        char res[5] = { 2, 0, 1, 0, '-' };
        GPTSOCK_send(clientSock, res, 5);

        memset(buffer, 0, GPTSOCK_BUFFER_SIZE);
        len = GPTSOCK_recv(clientSock, buffer, GPTSOCK_BUFFER_SIZE);
        printf("%d\n", len);
        GPTSOCK_print(buffer, len);

        char kick[13] = { 0xFF, 0, 5, 0, 'H', 0, 'e', 0, 'l', 0, 'l', 0, 'o' };
        GPTSOCK_send(clientSock, kick, 13);

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