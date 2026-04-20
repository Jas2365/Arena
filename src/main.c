#include <ints.h>
#include <defs.h>

#include <stdio.h>
#include <Arena/arena3.h>
#include <String/String.h>

i32 main(null) { 
    
    _Arena_Init_;

    string a = set_string("hello", 5);
    string b = set_string(" ", 1);
    string c = set_string("world", 5);
    string d = set_string(" ", 1);
    
    string e = set_string("\n", 1);
    string f = set_string("\0 2323", 6);

    string_free(c);

    // string e = set_string("names", 5);

    // Arena_Print();

    print_block();

    // string_audit_log();
    
    _Arena_Free_;


    return 0;
}