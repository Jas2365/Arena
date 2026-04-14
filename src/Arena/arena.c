#include <Arena/arena.h>

#include <ints.h>
#include <defs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Entry {
    s64 index;
    p64 offset;
} Entry;

typedef struct Table {
    p64 base_ptr;
    Entry entries[128];                                                     // temporary table size for now, same as memory pool
} Table;

struct Arena {
    Table table;
    i8* storage;
    s64 used;
    s64 capacity;
};

struct handle {

};

Arena arena_g;

null arena_init() {
    arena_g.storage = (i8*)malloc(128);
    arena_g.used = 0;
    arena_g.capacity = 128;
    arena_g.table.base_ptr = (p64)arena_g.storage;
    memset(arena_g.storage, 0, 128);
}

null arena_free() {
    if(arena_g.storage) free(arena_g.storage);
}

i8 arena_get(i32 i) {
    return arena_g.storage[i];
}
null arena_set(i32 i, i8 val) {
    arena_g.storage[i] = val;
}

null arena_push(i8 val) {
    arena_g.storage[arena_g.offset++] = val;
}

null print_row(i32 row) {
    for(i32 i = row; i< row+16; i++ ) {
        printf("%02x ", arena_g.storage[i]);
    }endline;
}

null print_arena() {
    printf("         00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"endl);
    for(i32 i = 0; i< 128; i+=16) {
        printf("%08x ", i);
        print_row(i);
    }
}
