#include <Arena/arena3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Arena G;

null Arena_Init() {

    G.storage   = malloc(arena_size);
    G.capacity  = arena_size;
    G.used      = 0;

    memset(G.storage, -1, arena_size);

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
        case '\0': printf("\\0 "); break;      
        case '\\': printf("\\\\ "); break;     
        default: printf("%02c ", byte); break;
    }
}

i32 print_line(s64 start, s64 width) {
   
    for(s64 i = start; i < start+width; i++) {
    
        if (i < G.used) {
            print_byte(i);
        } 
        else if(i > G.capacity) {
            printf("%02c ", ' ');
        }else {
            printf("%02x ", 0);
        }  
    };

    return 1;
}

i32 print_line_char(s64 start, s64 width) {
   
    for(s64 i = start; i < start+width; i++) {
    
        if (i < G.used) {
            print_byte_char(i);
        } 
        else if(i > G.capacity) {
            printf("%02c ", ' ');
        }
        else {
            printf("%02x ", 0);
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

