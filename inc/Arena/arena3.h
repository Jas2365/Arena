#pragma once

#include <ints.h>
#include <defs.h>

typedef struct Arena {
    
    null* storage;
    s64 used;
    s64 capacity;

} Arena;

extern Arena G;

#define arena_size 129

null Arena_Init();
null Arena_Free();

#define _Arena_Init_ Arena_Init()
#define _Arena_Free_ Arena_Free()

null Arena_Print();

null print_block();