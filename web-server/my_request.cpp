
#include "my_headers.h"

using namespace std;

namespace my_namespace {
    my_request::my_request(string& source) {
        string mainblock, body;
        size_t size = source.size();
        size_t split = source.find("\r\n\r\n");
        if (split != -1) {
            mainblock = source.substr(0, split);
            body = source.substr(split + 4, size - split - 4);
        }
        else {
            mainblock = source;
        }
        setmainprops(mainblock);
        if (contenttype == "application/json")
            json_body = json_field(body);
    }
    my_request::my_request() {}
    void my_request::setmainprops(string& mainblock) {
        vector<string> lines, props;
        split(lines, mainblock, "\r\n");
        string first = lines[0];
        split(props, first, " ");
        method = props[0];
        path = props[1];
        protocol = props[2];
        for (auto it = lines.begin() + 1; it != lines.end(); it++) {
            props.clear();
            split(props, *it, ": ");
            if (props[0] == "Content-Type")
                contenttype = props[1];
            headers.insert(pair<string, string>(props[0], props[1]));
        }
    }
}