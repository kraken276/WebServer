#undef UNICODE

#include <sstream>
#include <fstream>
#include <vector>
#include <ws2tcpip.h>
#include "my_headers.h"

using namespace std;
//
namespace my_namespace {
    void replace(string& source, string replaced, string replacing) {
        int index = 0;
        while (index != -1) {
            index = source.find(replaced);
            if (index != -1)
                source.replace(index, replaced.size(), replacing);
        }
    }
    void split(vector<string>& lines, string& source, string delimiter) {
        size_t size = source.size(),
            dsize = delimiter.size(),
            start = 0, 
            end = 0;
        while (end < size) {
            end = source.find(delimiter, start);
            if (end == -1) {
                end = size;
            }
            lines.push_back(source.substr(start, end - start));
            start = end + dsize;
        }
    };
    string rootpath;
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
    void sendfile(my_request& request, SOCKET& socket) {
        string responsebody, response;
        ifstream s;
        string temp, path, filepath = request.path;
        replace(filepath, "/", "\\");
        path = getrootpath() + filepath;
        s.open(path, ifstream::binary);
        if (s.is_open()) {
            long c = 0;
            long current = 0;
            while (!s.eof()) {
                response.clear();
                responsebody.clear();
                while (current < 500000) {
                    if (s.eof())
                        break;
                    s >> temp;
                    current += temp.size();
                    responsebody += temp;
                }
                c += current;
                current = 0;
                response += "HTTP/1.1 200 OK\r\n";
                response += "Version: HTTP/1.1\r\n";
                response += "Content-Type: text/html; charset=utf-8\r\n";
                response += "Content-Length: ";
                response += responsebody.size();
                response += "\r\n\r\n";
                response += responsebody;
                send(socket, response.c_str(), response.size(), 0);
            }
        }
    }
    void getresponse(my_request& request, SOCKET& socket) {
        string response;
        switch (getrequestpurpose(request)) {
        case 1:
            sendfile(request, socket);
            break;
        default:
            response = getnotfoundpage();
            send(socket, response.c_str(), response.size(), 0);
            break;
        }
    }
    void processrequest(my_request& request, SOCKET& socket) {
        getresponse(request, socket);
    }
}