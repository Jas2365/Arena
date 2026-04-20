#pragma once

#include <ints.h>

typedef struct string {

    i32 index;
    i32 generation;
    u32 checksum;

} string;

/**
 * 
 * canary_before -> detect buffer underrun
 * canary_after  -> detect buffer overrun
 * created_time  -> audit trail
 */

typedef struct string_meta {

    s64 offset;
    s64 length;
    i32 generation;
    u32 canary_before;
    u32 canary_after;
    b8  active;
    u64 created_time;

} string_meta;

#define MAX_STRINGS 1024
#define STRING_MAX_LEN (1LL << 20) // 1MB limit
#define CANARY_MAGIC 0x23652365
#define STRING_INVALID ((string){-1, -1, 0})

extern string_meta string_table[MAX_STRINGS];
extern s64 string_count;
extern u32 secret_key; // init at runtime

// fns

null String_Init();

// create

string set_string(cip8 str, s64 len);

// access
const char* get_string(string s, sp64 out_len);

b8 string_equals(string a, string b);
s64 string_length(string s);

// free
null string_free(string s);

// logs
null string_audit_log();