#pragma once

#include <ints.h>
#include <defs.h>

/**
 * three fields
 *  id  = allocation id
 *  token = 64 bit random secret issued at alloc time
 *  generation slot reuse counter
 * 
 * Forging causes _resolve() to silently return NULL
 *  
//  */

typedef struct Handle {
    s64 id;
    u64 token;
    s64 gen;
} Handle;

#define HANDLE_INVALID ((Handle){-1, 0, 0})
#define HANDLE_IS_VALID(h) ((h).id >= 0)

null arena_init(s64 capacity);
null arena_free();

Handle arena_alloc(s64 size);
null   arena_dealloc(Handle h);

null arena_write(Handle h, const i8* src);
null arena_read (Handle h,       i8* dst);

null print_arena();