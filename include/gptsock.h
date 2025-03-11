#ifndef GPTSockH
#define GPTSockH

// A socket library built from boilerplate code provided by ChatGPT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define GPTSOCK_BUFFER_SIZE 1024

#ifdef _WIN32
    #include <winsock2.h>
    typedef SOCKET sock_t;
    #define CLOSESOCKET closesocket
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int sock_t;
    #define CLOSESOCKET close
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR   -1
#endif

void GPTSockInit() {
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

void GPTSockClean() {
#ifdef _WIN32
    WSACleanup();
#endif
}

sock_t NewGPTSocket() {
    sock_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if ( sock == INVALID_SOCKET ) {
        TLog("GPTSOCK ERROR: Failed to create socket!\n");
        GPTSockClean();
    }
    return sock;
}

int GPTSockBind(sock_t sock, int port) {
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    int result = bind(sock, (struct sockaddr*)&server, sizeof(server));
    if (result == SOCKET_ERROR) {
        TLog("GPTSOCK ERROR: Failed to bind socket to port %d!\n", port);
        GPTSockClean();
    }

    return result;
}

int GPTSockListen(sock_t sock, int backlog) {
    return listen(sock, backlog);
}

sock_t GPTSockAccept(sock_t sock) {
    struct sockaddr_in client;
    int clientSize = sizeof(client);

    sock_t clientSock = accept(sock, (struct sockaddr*)&client, &clientSize);
    if ( sock == INVALID_SOCKET ) {
        TLog("GPTSOCK ERROR: Failed to accept client!\n");
        GPTSockClean();
    }

    return clientSock;
}

int GPTSockConnect(sock_t sock, const char *ip, int port) {
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);
    return connect(sock, (struct sockaddr*)&server, sizeof(server));
}

int GPTSockSend(sock_t sock, const char *data, int len) {
    return send(sock, data, len, 0);
}


int GPTSockRecv(sock_t sock, char *buffer, int size) {
    return recv(sock, buffer, size, 0);
}

void GPTSockClose(sock_t sock) {
    CLOSESOCKET(sock);
}

void GPTSockPrint( char *buffer, int len ) {
    for ( int i = 0; i < len; i++ ) {
        if ( isprint(buffer[i]) ) putchar(buffer[i]);
        else                      printf("\\x%02X", buffer[i]);
    }
    putchar('\n');
}

#endif

