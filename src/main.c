#include <ints.h>
#include <defs.h>

#include <stdio.h>
#include <Arena/arena.h>

i32 main(null) { 
    
    arena_init();

    arena_push(0x23);
    arena_push(0x23);
    // arena_set(0, 0x23);
    // arena_set(2, 0x23);
    // arena_set(4, 0x23);

    print_arena();


    arena_free();

    return 0;
}