#include <String/String.h>
#include <Arena/arena3.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

string_meta string_table[MAX_STRINGS];
s64 string_count = 0;
u32 secret_key = 0; // init at runtime

// _Initialization_

null String_Init() {
    // should change this rng
    srand(time(nullptr) ^ (p64)&string_table);
    secret_key = ((u32)rand() << 16 ) | rand();

    // Init all slots as invalid
    for(s64 i = 0; i < MAX_STRINGS; i++) {
        string_table[i].active = false;
        string_table[i].generation = 0;
        string_table[i].canary_before = CANARY_MAGIC;
        string_table[i].canary_after  = CANARY_MAGIC;
    }
}

// _Cryptographic_Checksum_

static u32 compute_checksum(i32 index, i32 generation) {
    u32 hash = secret_key;
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
    if(!G.storage) return 0;
    if(G.used < 0 || G.used > G.capacity) return 0;
    if(G.capacity != arena_size) return 0;

    for(s64 i = 0; i<string_count; i++) {
        string_meta* meta = &string_table[i];
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
    if(s.index >= string_count) return 0;
    if(s.generation <= 0) return 0;

    string_meta* meta = &string_table[s.index];

    u32 expected = compute_checksum(s.index, s.generation);
    if(!secure_equals(s.checksum, expected)) return 0;

    if(meta->generation != s.generation) return 0;
    if(!meta->active) return 0;
    if(meta->canary_before != CANARY_MAGIC) return 0;
    if(meta->canary_after  != CANARY_MAGIC) return 0;

    if(meta->offset < 0 || meta->length < 0) return 0;
    if(meta->length > STRING_MAX_LEN) return 0;
    if(meta->offset > G.capacity - meta->length) return 0;

    if(!check_arena_integrity()) return 0;

    if(out_meta) *out_meta = meta;
    return 1;
}

// _String_Creation_

string set_string(cip8 str, s64 len) {
    if(!str) return STRING_INVALID;
    if(len < 0 || len > STRING_MAX_LEN) return STRING_INVALID;
    if(!check_arena_integrity()) return STRING_INVALID;
    if(string_count >= MAX_STRINGS) return STRING_INVALID;
    if(len > G.capacity - G.used) return STRING_INVALID;

    i32 index = string_count++;
    string_meta* meta = &string_table[index];
    meta->offset = G.used;
    meta->length = len;
    meta->generation++;
    meta->active = 1;
    meta->created_time = (u64)time(nullptr);
    meta->canary_before = CANARY_MAGIC;
    meta->canary_after  = CANARY_MAGIC;

    if(len > 0) {
        memcpy((up8)G.storage + G.used, str, len);
    }
    G.used += len;

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
    return (cip8)((up8)G.storage + meta->offset);
}

cip8 get_stringc(string s) {
    string_meta* meta;
    if(!validate_handle(s, &meta)) {
        return nullptr;
    }
    return (cip8)((up8)G.storage + meta->offset);
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

    cip8 data_a = (cip8)((up8)G.storage + meta_a->offset);
    cip8 data_b = (cip8)((up8)G.storage + meta_b->offset);

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
        volatile up8 data = (volatile up8)G.storage + meta->offset;
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
    printf("Total strings allocated: %lld"endl, string_count);
    printf("Arena used: %lld / %lld bytes"endl, G.used, G.capacity);

    for(s64 i = 0; i < string_count; i++) {
        string_meta* meta = &string_table[i];
        printf("String[%lld]: ", i);
        if(meta->active) {
            printf("ACTIVE gen=%d offset=%lld len=%lld created=%llu"endl,
            meta->generation, meta->offset, meta->length, meta->created_time);
        } else {
            printf("FREED gen=%d"endl, meta->generation);
        }
    }
}