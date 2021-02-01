#pragma once
#include <iostream>
#include <vector>
#include <winsock2.h>
#include <map>

using namespace std;

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "27015"

namespace my_namespace {
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
    private:
        void setmainprops(string& mainblock);
    };

    void replace(string& source, string replaced, string replacing);
    void split(vector<string>& lines, string& source, string delimiter);
    string getrootpath();
    bool isfilepath(string& path);
    int getrequestpurpose(my_request& request);
    string getnotfoundpage();
    string getfileresponse(my_request& request);
    string getresponse(my_request& request);
    void requestthread(SOCKET* socket);
    string getaddressname(SOCKET& socket);
}