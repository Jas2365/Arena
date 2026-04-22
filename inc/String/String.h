#pragma once

#include <ints.h>

typedef struct string {

    i32 index;
    i32 generation;
    u32 checksum;

} string;

typedef struct string_meta string_meta;

// fns

null String_Init();

// create

string set_string(cip8 str, s64 len);

// access
cip8 get_string(string s, sp64 out_len);
cip8 get_stringc(string s);


b8 string_equals(string a, string b);
s64 string_length(string s);

#define arg_str(sv) string_length(sv), get_stringc(sv)

// free
null string_free(string s);

// logs
null string_audit_log();
