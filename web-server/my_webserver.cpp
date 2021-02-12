#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <thread>
#include "my_headers.h"

using namespace std;

#pragma comment (lib, "Ws2_32.lib")

namespace my_namespace {
    my_webserver::my_webserver(const char* port, int threadscount) {
        freethreads = threadscount;
        resultaddr = NULL;
        error = false;
        ListenSocket = INVALID_SOCKET;

        WSADATA wsaData;
        int iResult;
        addrinfo hints;

        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed with error: %d\n", iResult);
            error = true;
            return;
        }

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &resultaddr);
        if (iResult != 0) {
            printf("getaddrinfo failed with error: %d\n", iResult);
            error = true;
            return;
        }

        ListenSocket = socket(resultaddr->ai_family, resultaddr->ai_socktype, resultaddr->ai_protocol);
        if (ListenSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            error = true;
            return;
        }

        iResult = bind(ListenSocket, resultaddr->ai_addr, (int)resultaddr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            printf("bind failed with error: %d\n", WSAGetLastError());
            error = true;
            return;
        }

        iResult = listen(ListenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            printf("listen failed with error: %d\n", WSAGetLastError());
            error = true;
            return;
        }
        printf("listening to port %s...\n", DEFAULT_PORT);
    }
    void my_webserver::listenwithfunction(void func(my_request&, SOCKET&)) {
        if (error)
            throw exception("server not working");
        char recvbuf[DEFAULT_BUFLEN];
        int recvbuflen = DEFAULT_BUFLEN;
        string message;
        int result;
        char address[16];
        my_webthread* thr;
        while (true) {
            thr = new my_webthread;
            thr->socket = accept(ListenSocket, NULL, NULL);
            result = recv(thr->socket, recvbuf, recvbuflen, 0);
            message = string(recvbuf, result);
            thr->request = my_request(message);
            thr->func = func;
            thr->server = this;

            thr->trystart();

            getaddressname(thr->socket, address, 16);
            printf("new connection from %s: starting new thread\n", address);
        }
    }
    my_webserver::~my_webserver() {
        closesocket(ListenSocket);
        WSACleanup();
        if (resultaddr != NULL)
            freeaddrinfo(resultaddr);
        for (auto it = threads.begin(); it != threads.end(); it++)
            delete* it;
    }
    void my_webserver::getaddressname(SOCKET& socket, char* address, int addresslen) {
        sockaddr_in name;
        socklen_t namelen = sizeof(name);
        getpeername(socket, (sockaddr*)&name, &namelen);
        inet_ntop(AF_INET, &name.sin_addr, address, addresslen);
    }
}