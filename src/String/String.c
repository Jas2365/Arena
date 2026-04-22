#include <String/String.h>
#include <Arena/arena3.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/**
 * 
 * canary_before -> detect buffer underrun
 * canary_after  -> detect buffer overrun
 * created_time  -> audit trail
 */

struct string_meta {

    p64 offset;
    s64 length;
    i32 generation;
    u32 canary_before;
    u32 canary_after;
    b8  active;
    u64 created_time;

};


#define MAX_STRINGS MAX_TABLE_ENTRIES
#define STRING_MAX_LEN (1LL << 20) // 1MB limit
#define CANARY_MAGIC 0x23652365
#define STRING_INVALID ((string){-1, -1, 0})

typedef struct String_State {
    string_meta string_table[MAX_STRINGS];
    s64 string_count;
    u32 secret_key; // init at runtime
    
} String_State;

String_State SS;

// _Initialization_

null String_Init() {
    // should change this rng
    SS.string_count = 0;
    SS.secret_key   = 0;
    srand(time(nullptr) ^ (p64)&SS.string_table);
    SS.secret_key = ((u32)rand() << 16 ) | rand();

    // Init all slots as invalid
    for(s64 i = 0; i < MAX_STRINGS; i++) {
        SS.string_table[i].active = false;
        SS.string_table[i].generation = 0;
        SS.string_table[i].canary_before = CANARY_MAGIC;
        SS.string_table[i].canary_after  = CANARY_MAGIC;
    }
}

// _Cryptographic_Checksum_

static u32 compute_checksum(i32 index, i32 generation) {
    u32 hash = SS.secret_key;
    hash ^= (u32)index * 0x9E377989;
    hash ^= (u32)generation * 0x517cc1b7;
    hash = ( hash ^ (hash >> 16)) * 0x85ebca6b;
    hash = ( hash ^ (hash >> 13)) * 0xc28b2ae35;
    return hash ^ (hash >> 16);
}

// _Constant_Time_Comparison_

static b8 secure_equals(u32 a, u32 b) {
    u32 diff = a ^ b;
    return (1 & ((diff -1) >> 31)) == 1;
}

// _Arena_Integrity_Check_

static b8 check_arena_integrity() {
    if(!T.base_ptr)                         { printf("00\n"); return 0; }
    if(!T.capacity)                         { printf("02\n"); return 0; }
    if(T.used < 0 || T.used > T.capacity)   { printf("01\n"); return 0; }

    for(s64 i = 0; i<SS.string_count; i++) {
        string_meta* meta = &SS.string_table[i];
        if(meta->active) {
            if(meta->canary_before != CANARY_MAGIC) return 0;
            if(meta->canary_after  != CANARY_MAGIC) return 0;
        }
    }
    return 1;
}

// _Handle_Validation_

static b8 validate_handle(string s, string_meta** out_meta) {
    if(s.index < 0 || s.index >= MAX_STRINGS) return 0;
    if(s.index >= SS.string_count) return 0;
    if(s.generation <= 0) return 0;

    string_meta* meta = &SS.string_table[s.index];

    u32 expected = compute_checksum(s.index, s.generation);
    if(!secure_equals(s.checksum, expected)) return 0;

    if(meta->generation != s.generation) return 0;
    if(!meta->active) return 0;
    if(meta->canary_before != CANARY_MAGIC) return 0;
    if(meta->canary_after  != CANARY_MAGIC) return 0;

    if(meta->offset < 0 || meta->length < 0) return 0;
    if(meta->length > STRING_MAX_LEN) return 0;
    if(meta->offset + T.base_ptr > T.capacity - meta->length) return 0;

    if(!check_arena_integrity()) return 0;

    if(out_meta) *out_meta = meta;

    return 1;
}

// _String_Creation_

string set_string(cip8 str, s64 len) {
    if(!str)                            { printf("01\n"); return STRING_INVALID; }
    if(len < 0 || len > STRING_MAX_LEN) { printf("02\n"); return STRING_INVALID; }
    if(!check_arena_integrity())        { printf("03\n"); return STRING_INVALID; }
    if(SS.string_count >= MAX_STRINGS)  { printf("04\n"); return STRING_INVALID; }
    if(len > T.capacity - T.used)       { printf("05\n"); return STRING_INVALID; }

    Table_Entry te = Arena_Alloc(len);

    i32 index = te.handle;

    SS.string_count++;
    string_meta* meta = &SS.string_table[index];
    meta->offset = te.offset;
    meta->length = te.length;
    meta->generation++;
    meta->active = 1;
    meta->created_time = (u64)time(nullptr);
    meta->canary_before = CANARY_MAGIC;
    meta->canary_after  = CANARY_MAGIC;

    memcpy((up8)T.base_ptr + te.offset, str, te.length);

    string handle;
    handle.index = index;
    handle.generation = meta->generation;
    handle.checksum = compute_checksum(index, meta->generation);

    return handle;
}

// _String_Access_

cip8 get_string(string s, sp64 out_len) {
    string_meta* meta;
    if(!validate_handle(s, &meta)) {
        if(out_len) *out_len = 0;
        return nullptr;
    }
    if(out_len) *out_len = meta->length;
    return (cip8)((up8)T.base_ptr + meta->offset);
}

cip8 get_stringc(string s) {
    string_meta* meta;
    if(!validate_handle(s, &meta)) {
        return nullptr;
    }
    return (cip8)((up8)T.base_ptr + meta->offset);
}


s64 string_length(string s) {
    string_meta* meta;
    if(!validate_handle(s, &meta)) return -1;
    return meta->length;
}

b8 string_equals(string a, string b) {
    string_meta* meta_a, * meta_b;
    if(!validate_handle(a, &meta_a)) return 0;
    if(!validate_handle(b, &meta_b)) return 0;

    if(meta_a->length != meta_b->length) return 0;

    cip8 data_a = (cip8)((up8) T.base_ptr + meta_a->offset);
    cip8 data_b = (cip8)((up8) T.base_ptr + meta_b->offset);

    u8 diff = 0;
    for(s64 i = 0; i < meta_a->length; i++) {
        diff |= data_a[i] ^ data_b[i];
    }
    return diff == 0;
}

// _Secure_Deallocation_

null string_free(string s) {
    string_meta* meta;
    if(!validate_handle(s, &meta)) return;

    if(meta->length > 0) {
        volatile up8 data = (volatile up8)T.base_ptr + meta->offset;
        for(s64 i = 0; i< meta->length; i++) {
            data[i] = 0xff;
        }
    }

    meta->active = 0;
    meta->generation++;
    meta->offset = -1;
    meta->length = -1;

}

// _Audit_Log_

null string_audit_log() {
    printf("=== STRING AUDIT LOG ==="endl);
    printf("Total strings allocated: %lld"endl, SS.string_count);
    printf("Arena used: %lld / %lld bytes"endl, T.used, T.capacity);

    for(s64 i = 0; i < SS.string_count; i++) {
        string_meta* meta = &SS.string_table[i];
        printf("String[%lld]: ", i);
        if(meta->active) {
            printf("ACTIVE gen=%d offset=%lld len=%lld created=%llu"endl,
            meta->generation, meta->offset, meta->length, meta->created_time);
        } else {
            printf("FREED gen=%d"endl, meta->generation);
        }
    }
}