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

string make_string(cip8 data) {
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

#define fmt_str "%.*s"
#define arg_str(str) (str).len, (str).data
