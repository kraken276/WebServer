#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <thread>
#include "my_headers.h"

using namespace std;

#pragma comment (lib, "Ws2_32.lib")

using namespace my_namespace;

int __cdecl main(int argc, char* argv[])
{
    setlocale(LC_ALL, "rus");
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET* ClientSocket;

    addrinfo* result = NULL;
    struct addrinfo hints;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    printf("listening to port %s...\n", DEFAULT_PORT);

    do {
        ClientSocket = new SOCKET;
        *ClientSocket = accept(ListenSocket, NULL, NULL);
        char address[16];
        getaddressname(*ClientSocket, address, 16);
        printf("new connection from %s: starting new thread\n", address);
        thread requestthread(requestthread, ClientSocket);
        requestthread.detach();
    } while (true);

    closesocket(ListenSocket);

    closesocket(*ClientSocket);
    WSACleanup();

    return 0;
}