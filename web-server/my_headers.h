#pragma once

#include <ws2tcpip.h>
#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "27015"

using namespace std;

namespace my_namespace {
    class my_request;
    class my_webthread;
    template<class _source>
    class my_pointer {
    public:
        _source* objpointer;
        my_pointer(_source* objpointer) {
            this->objpointer = objpointer;
        }
        ~my_pointer() {
            delete objpointer;
        }
    };
    template<class _source>
    class my_arraypointer {
    public:
        _source* objpointer;
        my_arraypointer(_source* objpointer) {
            this->objpointer = objpointer;
        }
        ~my_arraypointer() {
            delete [] objpointer;
        }
    };
    class my_webserver {
    public:
        mutex freethreadsmutex,
            queuethreadsmutex;
        SOCKET ListenSocket;
        list<my_webthread*> threads;
        int freethreads;
        addrinfo* resultaddr;
        bool error;
        my_webserver(const char* port, int threadscount);
        void listenwithfunction(void func(my_request&, SOCKET&));
        ~my_webserver();
    private:
        void getaddressname(SOCKET& socket, char* address, int addresslen);
    };
    class json_field {
    public:
        string name, value;
        vector<json_field> fields;
        bool isarray;
        json_field& operator[] (size_t i);
        json_field& operator[] (string& name);
        json_field();
        json_field(string& source);
        ~json_field();
    private:
        void removewhitespace(string& source);
        int findclosechar(string& source, int start, bool figure);
    };
    class my_request {
    public:
        string method, path, protocol, contenttype;
        map<string, string> headers;
        json_field json_body;
        my_request(string& source);
        my_request();
    private:
        void setmainprops(string& mainblock);
    };
    class my_webthread {
    public:
        SOCKET socket;
        my_webserver* server;
        my_request request;
        void (*func) (my_request& request, SOCKET& socket);
        my_webthread();
        ~my_webthread();
        void start();
        void trystart();
        void checkthreadsqueue();
    };
    void split(vector<string>& lines, string& source, string delimiter);
    void processrequest(my_request& request, SOCKET& socket);
}