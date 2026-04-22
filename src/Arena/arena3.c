#include <Arena/arena3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Arena {
    
    null* storage;
    s64 used;
    s64 capacity;

} Arena;

Arena G;
Arena_Table T;

Table_Entry Arena_Alloc(i32 len) {
    
    if(len > G.capacity - G.used) return (Table_Entry) {};
    if(len <= 0) return (Table_Entry) {};

    i32 idx = T.current;
    T.entry[idx].handle = idx;
    T.entry[idx].offset = G.used;
    T.entry[idx].length = len;

    T.current++;
    T.used += len;
    G.used += len;

    return T.entry[idx];

}

null Arena_Init() {

    G.storage   = malloc(arena_size);
    G.capacity  = arena_size;
    G.used      = 0;
    memset(G.storage, -1, arena_size);
    
    T.base_ptr  = (p64)G.storage;
    T.current   = 0;
    T.capacity  = G.capacity;
    T.used      = G.used;
    memset(T.entry, 0, sizeof(Table_Entry) * MAX_TABLE_ENTRIES);

}

null Arena_Grow() {
    s64 nw_cap = G.capacity * 2;
    G.storage = realloc(G.storage, nw_cap);
    G.capacity = nw_cap;

    T.base_ptr = (p64)G.storage;
    T.capacity = G.capacity;


}

null Arena_Free() {
    if(G.storage) {
        free(G.storage);
        G.storage   = nullptr;
        G.capacity  = 0;
        G.used      = 0;
    }
}




#define sttype(T, byte_offset)  (*(T*)((u8*)(G.storage) + (byte_offset))) 

null print_byte(s64 index) {
    printf("%02x ", sttype(u8, index));
}
null print_byte_char(s64 index) {
    u8 byte = sttype(u8, index);
    
    switch(byte) {
        case '\n': printf("\\n "); break;      
        case '\r': printf("\\r "); break;      // carriage return
        case '\t': printf("\\t "); break;      
        case '\b': printf("\\b "); break;      
        case '\f': printf("\\f "); break;      // form feed
        case '\v': printf("\\v "); break;      // vertical tab
        case '\0': printf("00 "); break;      
        case '\\': printf("\\\\ "); break;     
        default: printf("%02c ", byte); break;
    }
}

i32 print_line(s64 start, s64 width) {
   
    for(s64 i = start; i < start+width; i++) {
        if (i < G.used) {
            print_byte(i);
        } 
        else if(i < G.capacity) {
            printf("%02x ", 0);
        }
        else {
            printf("%02c ", ' ');
        }  
    };

    return 1;
}

i32 print_line_char(s64 start, s64 width) {
   
    for(s64 i = start; i < start+width; i++) {
        if (i < G.used) {
            print_byte_char(i);
        } 
        else if(i < G.capacity) {
            printf("%02x ", 0);
        }
        else {
            printf("%02c ", ' ');
        }
    };

    return 1;
}

null print_block() {
    i32 width = 16;
    
    printf("----------------------------------------------------------------------------------------------------------------"endl);
    printf("|          | 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f | 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f |"endl);
    printf("----------------------------------------------------------------------------------------------------------------"endl);
    for(s64 i = 0; i < G.capacity; i+= width) {
        printf("| %08x | ", i);
        if(!print_line(i, width)) break;
        printf("| ");
        if(!print_line_char(i, width)) break;
        printf("|");
        endline;
    } 
    printf("----------------------------------------------------------------------------------------------------------------"endl);
    endline;
}

null print_row(s64 start, s64 width) {
    for(s64 i = start; i < start + width; i++){
        printf("%02x ", sttype(u8, i));
    } endline;
}

null Arena_Print() {
    i32 width = 16;
    printf("         00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"endl);
    for(s64 i = 0; i< G.capacity; i+= width) {
        printf("%08x ", i);
        print_row(i, width);
    } endline;
}

