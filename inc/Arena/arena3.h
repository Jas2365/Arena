#pragma once

#include <ints.h>
#include <defs.h>

typedef struct Table_Entry {
    i32 handle;
    p64 offset;
    i32 length;

} Table_Entry;

#define arena_size 128
#define MAX_TABLE_ENTRIES 200
typedef struct Arena_Table {
    
    p64 base_ptr;
    Table_Entry entry[MAX_TABLE_ENTRIES];
    i32 current;
    s64 capacity;
    s64 used;

} Arena_Table;

extern Arena_Table T;



null Arena_Init();
null Arena_Free();

#define _Arena_Init_ Arena_Init()
#define _Arena_Free_ Arena_Free()

null Arena_Print();


null Arena_Grow();
Table_Entry Arena_Alloc(i32 len);

null print_block();