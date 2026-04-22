#include <ints.h>
#include <defs.h>

#include <stdio.h>
#include <Arena/arena3.h>
// #include <String/String.h>
#include <String/String3.h>



i32 main(i32 argc, ip8 argv[]) { 
    _Arena_Init_;

    string b = make_string("a");
    string a = make_string("hello");
    string c = make_string("world");

    string d = concat(&a, &c);

    printf("d : offset %p" endl, d.data);
    printf("d : length %d" endl, d.len);
    print_string(d);

    // Table_Entry k = Arena_Alloc(2);

    // printf("k : %p"endl, T.base_ptr);
    // printf("k : %p"endl, k.offset);
    // printf("k : %d"endl, k.length);

    // ip8 ll = (ip8)(k.offset + T.base_ptr) ;
    // *ll = 'a';
    // *(ll+1) = 'a';

    // Arena_Grow();

    // string a = set_string("hello", 5);
    // string b = set_string("world", 5);

    // string_free(a);

    // printf("%.*s"endl,k.length, T.base_ptr + k.offset);
    
    // // string b = set_string(" ", 1);
    // string c = set_string("world\\", 6);
    // // string d = set_string(" ", 1);
    
    // string e = set_string("\n", 1);
    // string f = set_string("2323", 4);

    // string_free(c);

    // Arena_Print();

    print_block();

    // string_audit_log();
    
    // printf("str: %.*s"endl, 8, get_stringc(a));
    // printf("str: %.*s"endl,arg_str(a));
    // printf("str: %.*s"endl,arg_str(b));
    // printf("str: %.*s"endl,arg_str(c));
    // printf("str: %.*s"endl,arg_str(d));
    // printf("str: %.*s"endl,arg_str(e));
    // printf("str: %.*s"endl,arg_str(f));

    _Arena_Free_;




    return 0;
}

/**
 * 

    // if(argc < 2) {
    //     return 0;
    // }

    // string strs[argc];

    // for(i32 i = 0; i < argc; i++) {
    //     strs[i] = make_string(argv[i]);
    // }

    // for(i32 i = 0; i < argc; i++) {
    //     printf("str: "fmt_str endl, arg_str(strs[i]));
    // } endline;


 */