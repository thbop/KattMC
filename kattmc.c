#include "stdio.h"
#include "stdbool.h"
#include "pthread.h"

#include "tlog.h"
#include "gptsock.h"
#include "clienthandler.h"


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

void Run() {
    bool running = true;
    while (running) {
        GPTSOCK_listen(serverSock, 1);
        sock_t clientSock = GPTSOCK_accept(serverSock);
        pthread_t ptid;
        pthread_create(&ptid, NULL, &clientHandler, &clientSock);
        pthread_detach(ptid);

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