#undef UNICODE

#include <sstream>
#include <fstream>
#include <ws2tcpip.h>
#include "my_headers.h"

using namespace std;

namespace my_namespace {
    string rootpath;
    void replace(string& source, string replaced, string replacing) {
        int index = 0;
        while (index != -1) {
            index = source.find(replaced);
            if (index != -1)
                source.replace(index, replaced.size(), replacing);
        }
    }
    void split(vector<string>& lines, string& source, string delimiter) {
        int size = source.size(),
            dsize = delimiter.size();
        int start = 0, end = 0;
        while (end < size) {
            end = source.find(delimiter, start);
            if (end == -1) {
                end = size;
            }
            lines.push_back(source.substr(start, end - start));
            start = end + dsize;
        }
    };
    string getrootpath() {
        if (rootpath == "") {
            char myPath[_MAX_PATH + 1];
            GetModuleFileName(NULL, myPath, _MAX_PATH);
            string ppath(myPath);
            int last = ppath.find_last_of('\\');
            rootpath = ppath.substr(0, last);
        }
        return rootpath;
    }
    bool isfilepath(string& path) {
        int a = path.find_last_of('/');
        int b = path.find_last_of('.');
        return b > a;
    }
    int getrequestpurpose(my_request& request) {
        if (isfilepath(request.path))
            return 1;
        return -1;
    }
    string getnotfoundpage() {
        stringstream response_body, response;
        response_body << "<head>"
            << "<link href = \"https://cdn.jsdelivr.net/npm/bootstrap@5.0.0-beta1/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-giJF6kkoqNQ00vy+HMDP7azOuL0xtbfIcaT9wjKHr8RbDVddVHyTfAAsrekwKmP1\" crossorigin=\"anonymous\">"
            << "<script src = \"https://unpkg.com/react@17/umd/react.development.js\" crossorigin></script>"
            << "<script src = \"https://unpkg.com/react-dom@17/umd/react-dom.development.js\" crossorigin></script>"
            << "<script src = \"src/myscript.js\" crossorigin></script>"
            << "<title>Page not found</title>\n"
            << "</head>"
            << "<body>"
            << "<h1>Page not found</h1>\n"
            << "</body>";

        response << "HTTP/1.1 404 Not Found\r\n"
            << "Version: HTTP/1.1\r\n"
            << "Content-Type: text/html; charset=utf-8\r\n"
            << "Content-Length: " << response_body.str().length()
            << "\r\n\r\n"
            << response_body.str();
        return response.str();
    }
    string getfileresponse(my_request& request) {
        stringstream response_body, response;
        ifstream s;
        string temp, path, filepath = request.path;
        replace(filepath, "/", "\\");
        path = getrootpath() + filepath;
        s.open(path);
        if (s.is_open()) {
            while (!s.eof()) {
                s >> temp;
                response_body << temp;
            }
        }
        response << "HTTP/1.1 200 OK\r\n"
            << "Version: HTTP/1.1\r\n"
            << "Content-Type: text/html; charset=utf-8\r\n"
            << "Content-Length: " << response_body.str().length()
            << "\r\n\r\n"
            << response_body.str();
        return response.str();
    }
    string getresponse(my_request& request) {
        string response;
        switch (getrequestpurpose(request)) {
        case 1:
            response = getfileresponse(request);
            break;
        default:
            response = getnotfoundpage();
            break;
        }
        return response;
    }
    void requestthread(SOCKET* socket) {
        char recvbuf[DEFAULT_BUFLEN];
        int recvbuflen = DEFAULT_BUFLEN;
        int result = recv(*socket, recvbuf, recvbuflen, 0);
        if (result > 0) {
            string request = string(recvbuf, result);
            my_request a = my_request(request);
            string response = getresponse(a);
            const char* b = response.c_str();
            int responselen = strlen(b);
            result = send(*socket, b, responselen, 0);
            printf("Bytes sent: %d\n", result);
        }
        shutdown(*socket, SD_SEND);
        closesocket(*socket);
        delete socket;
    }
    void getaddressname(SOCKET& socket, char* address, int addresslen) {
        sockaddr_in name;
        socklen_t namelen = sizeof(name);
        getpeername(socket, (sockaddr*)&name, &namelen);
        inet_ntop(AF_INET, &name.sin_addr, address, addresslen);
    }
}