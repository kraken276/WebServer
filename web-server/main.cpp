#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <thread>
#include <iostream>
#include "my_headers.h"

using namespace std;
using namespace my_namespace;

#pragma comment (lib, "Ws2_32.lib")

int __cdecl main(int argc, char* argv[])
{
    setlocale(LC_ALL, "rus");

    my_webserver server(DEFAULT_PORT, 2);
    server.listenwithfunction(processrequest);

    return 0;
}