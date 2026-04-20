#pragma once

#include <ints.h>

// typedef struct Arena Arena;

// typedef struct handle handle;

// extern Arena arena_g;

// null arena_init();
null arena_free();

i8  arena_get(i32 i);
null arena_set(i32 i, i8 val);
null arena_push(i8 val);

null print_row(i32 row);
null print_arena();
