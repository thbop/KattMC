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
    GPTSockInit();

    serverSock = NewGPTSocket();
    GPTSockBind(serverSock, 25565);

    ClientHandlerInit();


    return ok;
}

void Run() {
    bool running = true;
    while (running) {
        GPTSockListen(serverSock, 1);
        sock_t clientSock = GPTSockAccept(serverSock);
        TLog("SERVER: New connection!\n");
        pthread_t ptid;
        pthread_create(&ptid, NULL, &ClientHandlerMain, &clientSock);
        pthread_detach(ptid);

    }
    GPTSockClose(serverSock);
}


void Close() {
    ClientHandlerUnload();
    GPTSockClean();
    TLogClose();
}


int main() {
    if ( !Init() )
        return 1;
    Run();
    Close();

    return 0;
}