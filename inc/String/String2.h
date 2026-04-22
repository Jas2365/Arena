#pragma once 

#include <ints.h>
#include <defs.h>

#include <string.h>
#include <stdlib.h>

typedef struct string {
    ip8 data;
    s64 len;
} string;

string invalid_string = {
    .data = nullptr,
    .len = 0
};

string make_string(cip8 data /* string view from the compiler and copy to the heap*/) {
    string res;
    if(data) {
        i32 len = strlen(data);
        res.data = (ip8)malloc(len);
        res.len = len;
        memmove(res.data, data, len);
    } else {
        res = invalid_string;
    }
    return res;
}

string concat(const string* a, const string* b) {
    string concat;
    if(a->data && b->data) {
        concat.data = (ip8)malloc(a->len + b->len);
        concat.len = a->len + b->len;
        memmove(concat.data, a->data, a->len);
        memmove(concat.data + a->len, b->data, b->len);
    } else {
        concat = invalid_string;
    }
    return concat;

}

#define fmt_str "%.*s"
#define arg_str(str) (str).len, (str).data
