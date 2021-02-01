
#include "my_headers.h"

using namespace std;

namespace my_namespace {
    json_field& json_field::operator[] (size_t i) {
        if (!isarray)
            throw exception("элемент не является массивом");
        if (i < fields.size())
            return *(fields.begin() + i);
        throw exception("элемент не найден");
    }
    json_field& json_field::operator[] (string& name) {
        if (isarray)
            throw exception("элемент является массивом");
        for (auto it = fields.begin(); it != fields.end(); it++)
            if ((*it).name == name)
                return *it;
        throw exception("элемент не найден");
    };
    json_field::json_field() {
        isarray = false;
    };
    json_field::json_field(string& source) {
        removewhitespace(source);
        int length = source.length();
        if (length > 2) {
            char first = *source.begin();
            if (first == '{') {
                isarray = false;
                string fieldname, fieldvalue, block;
                json_field field;
                int namestart, nameend, blockstart, blockend;
                for (auto it = source.begin() + 1; it != source.end(); it++) {
                    namestart = it - source.begin();
                    nameend = source.find('"', namestart + 2);
                    fieldname = source.substr(namestart + 1, nameend - namestart - 1);
                    blockstart = nameend + 2;
                    first = *(source.begin() + blockstart);
                    if (first == '{' || first == '[') {
                        blockend = findclosechar(source, blockstart + 1, first == '{');
                        block = source.substr(blockstart, blockend - blockstart + 1);
                        field = json_field(block);
                        field.name = fieldname;
                        fields.push_back(field);
                        it = source.begin() + blockend + 1;
                    }
                    else {
                        if (first == '"') {
                            blockend = source.find('"', blockstart + 1);
                            fieldvalue = source.substr(blockstart + 1, blockend - blockstart - 1);
                            it = source.begin() + blockend + 1;
                        }
                        else {
                            blockend = source.find(',', blockstart);
                            if (blockend == -1)
                                blockend = length - 1;
                            fieldvalue = source.substr(blockstart, blockend - blockstart);
                            it = source.begin() + blockend;
                        }
                        field = json_field();
                        field.name = fieldname;
                        field.value = fieldvalue;
                        fields.push_back(field);
                    }
                }
            }
            else {
                isarray = true;
                string fieldvalue, block;
                json_field field;
                int blockstart, blockend;
                for (auto it = source.begin() + 1; it != source.end(); it++) {
                    blockstart = it - source.begin();
                    first = *it;
                    if (first == '{' || first == '[') {
                        blockend = findclosechar(source, blockstart + 1, first == '{');
                        block = source.substr(blockstart, blockend - blockstart + 1);
                        field = json_field(block);
                        fields.push_back(field);
                        it = source.begin() + blockend + 1;
                    }
                    else {
                        if (first == '"') {
                            blockend = source.find('"', blockstart + 1);
                            fieldvalue = source.substr(blockstart + 1, blockend - blockstart - 1);
                            it = source.begin() + blockend + 1;
                        }
                        else {
                            blockend = source.find(',', blockstart);
                            if (blockend == -1)
                                blockend = length - 1;
                            fieldvalue = source.substr(blockstart, blockend - blockstart);
                            it = source.begin() + blockend;
                        }
                        field = json_field();
                        field.value = fieldvalue;
                        fields.push_back(field);
                    }
                }
            }
        }
    }
    json_field::~json_field() {}
    void json_field::removewhitespace(string& source) {
        for (auto i = source.begin(); i != source.end(); i++) {
            if (*i == ' ' || *i == '\r' || *i == '\n') {
                source.erase(i);
                i--;
            }
        }
    };
    int json_field::findclosechar(string& source, int start, bool figure) {
        int count = 0;
        char f1, f2;
        if (figure) {
            f1 = '{';
            f2 = '}';
        }
        else {
            f1 = '[';
            f2 = ']';
        }
        for (auto i = source.begin() + start; i != source.end(); i++) {
            if (*i == f1)
                count++;
            else if (*i == f2)
                count--;
            if (count == -1)
                return i - source.begin();
        }
        return -1;
    }
}