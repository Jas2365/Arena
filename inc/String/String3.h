#pragma once 

#include <ints.h>
#include <defs.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <Arena/arena3.h>

typedef struct string {
    p64 data;
    s64 len;
} string;

string invalid_string = {
    .data = -1,
    .len  = -1
};

string make_string(cip8 data /* string view from the compiler and copy to the heap*/) {
    string res;
    if(data) {
        i32 len = strlen(data);
        Table_Entry t = Arena_Alloc(len);
        res.data = t.offset;
        res.len = t.length;
        memcpy((up8)T.base_ptr + t.offset, data, t.length);

    } else {
        res = invalid_string;
    }
    return res;
}

null print_string(string a) {
    printf("str: %.*s"endl , a.len, a.data + T.base_ptr);
}

string concat(const string* a, const string* b) {
    string concat;
    if(a->len && b->len) {
        i32 len = a->len + b->len;
        Table_Entry t =Arena_Alloc(len);
        concat.data = t.offset;
        concat.len = len;

        memmove((null*)(concat.data + T.base_ptr),(null*) (a->data + T.base_ptr), a->len);
        memmove((null*)(concat.data + T.base_ptr + a->len),(null*) (b->data + T.base_ptr), b->len);
    } else {
        concat = invalid_string;
    }
    return concat;

}

#define fmt_str "%.*s"
#define arg_str(str) (str).len, ((str).data)
