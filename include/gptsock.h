#ifndef GPTSOCK_H
#define GPTSOCK_H

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

void GPTSOCK_Init() {
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

void GPTSOCK_Clean() {
#ifdef _WIN32
    WSACleanup();
#endif
}

sock_t GPTSOCK_socket() {
    sock_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if ( sock == INVALID_SOCKET ) {
        TLog("GPTSOCK ERROR: Failed to create socket!\n");
        GPTSOCK_Clean();
    }
    return sock;
}

int GPTSOCK_bind(sock_t sock, int port) {
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    int result = bind(sock, (struct sockaddr*)&server, sizeof(server));
    if (result == SOCKET_ERROR) {
        TLog("GPTSOCK ERROR: Failed to bind socket to port %d!\n", port);
        GPTSOCK_Clean();
    }

    return result;
}

int GPTSOCK_listen(sock_t sock, int backlog) {
    return listen(sock, backlog);
}

sock_t GPTSOCK_accept(sock_t sock) {
    struct sockaddr_in client;
    int client_size = sizeof(client);

    sock_t clientSock = accept(sock, (struct sockaddr*)&client, &client_size);
    if ( sock == INVALID_SOCKET ) {
        TLog("GPTSOCK ERROR: Failed to accept client!\n");
        GPTSOCK_Clean();
    }

    return clientSock;
}

int GPTSOCK_connect(sock_t sock, const char *ip, int port) {
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);
    return connect(sock, (struct sockaddr*)&server, sizeof(server));
}

int GPTSOCK_send(sock_t sock, const char *data, int len) {
    return send(sock, data, len, 0);
}


int GPTSOCK_recv(sock_t sock, char *buffer, int size) {
    return recv(sock, buffer, size, 0);
}

void GPTSOCK_close(sock_t sock) {
    CLOSESOCKET(sock);
}

void GPTSOCK_print( char *buffer, int len ) {
    for ( int i = 0; i < len; i++ ) {
        if ( isprint(buffer[i]) ) putchar(buffer[i]);
        else                      printf("\\x%02X", buffer[i]);
    }
    putchar('\n');
}

#endif

// int main() {
//     init_sockets();
    
//     // Server example
//     sock_t server_sock = create_socket();
//     if (bind_socket(server_sock, 25565) == 0) {
//         listen_socket(server_sock, 5);
//         printf("Server listening on port 25565...\n");
        
//         sock_t client_sock = accept_connection(server_sock);
//         char buffer[1024] = {0};
//         printf("Accepted!\n");
//         if (client_sock != -1) {
//             int r = receive_data(client_sock, buffer, 1024);
//             for ( int i = 0; i < r; i++ ) {
//                 putchar(buffer[i]);
//             }
//             close_socket(client_sock);
//         }
//         close_socket(server_sock);
//     }



//     return 0;
// }
