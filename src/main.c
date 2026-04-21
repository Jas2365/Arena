#include <ints.h>
#include <defs.h>

#include <stdio.h>
// #include <Arena/arena3.h>
// #include <String/String.h>
#include <String/String2.h>



i32 main(null) { 
    
    string a = make_string("hello1234");
    string b = make_string("hello");

    printf("str  : a %.*s"endl, a.len, a.data);
    printf("addr : a %p"endl, a.data);
    
    printf("str  : b %.*s"endl, b.len, b.data);
    printf("addr : b %p"endl, b.data);

    for(i32 i = 0; i < 60; i++) {
        printf("%p | %c | %x "endl, (a.data + i), *(a.data + i), *((up8)(a.data + i)));
    }
    
    printf(" str a : " fmt_str " one" endl, arg_str(a));
    printf("len : %d "endl, a.len);

    return 0;
}

/**
 * 
    _Arena_Init_;

    string a = set_string("hello", 5);
    // string b = set_string(" ", 1);
    string c = set_string("world", 5);
    // string d = set_string(" ", 1);
    
    string e = set_string("\n", 1);
    string f = set_string("2323", 4);

    // string_free(c);

    printf("str: "endl);
    printf("a chk: %d "endl, a.checksum);


    // Arena_Print();

    print_block();

    string_audit_log();
    
    printf("str: %.*s"endl, 8, get_stringc(a));
    // printf("str: %.*s"endl,arg_str(a));
    // printf("str: %.*s"endl,arg_str(b));
    // printf("str: %.*s"endl,arg_str(c));
    // printf("str: %.*s"endl,arg_str(d));
    // printf("str: %.*s"endl,arg_str(e));
    // printf("str: %.*s"endl,arg_str(f));

    _Arena_Free_;

 */