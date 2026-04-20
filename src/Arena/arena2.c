// #include <Arena/arena2.h>

// #include <ints.h>
// #include <defs.h>

// #include <stdio.h>
// #include <string.h>

// #include <windows.h>
// #include <bcrypt.h>

// // DEFS

// #define MAX_ENTRIES 128
// #define ENTRY_FREE (-1LL)

// // DEFS

// // ENTRY

// typedef struct Entry {
//     s64 id;
//     u64 token;
//     u64 offset;
//     s64 size;
//     s64 gen;
//     u64 checksum;
// } Entry;

// // ENTRY

// // ARENA 
// struct Arena {
//     // Storage region: [Guard | Storage | Guard]
    
//     i8* storage_region;
//     i8* storage;
//     s64 storage_region_size;
//     s64 capacity;
//     s64 used;
    
//     // Storage region: [Guard | Table | Guard]
//     i8*     table_region;
//     Entry*  table;
//     s64     table_region_size;
    
//     s64 page_size;
//     s64 next_id;
//     u64 session_key; 
// };


// struct Arena G;

// // ARENA 

// // RANDOM

// static u64 _rand64(void) {
//     u64 v = 0;
//     BCryptGenRandom(NULL, (PUCHAR)&v, sizeof(v), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
//     return v;
// }

// // RANDOM

// // SYSTEM_INFO

// static s64 _get_page_size(void) {
//     SYSTEM_INFO sysInfo;
//     GetSystemInfo(&sysInfo);
//     return (s64)sysInfo.dwPageSize;
// }

// // SYSTEM_INFO


// // HASH
// //  keyed hash (MurmurHash3 finaliser mix, session-keyed)

// static u64 _mix(u64 v, u64 key) {
//     v ^= key;
//     v ^= v >> 33;
//     v *= 0xff51afd7ed558ccdULL;
//     v ^= v >> 33;
//     v *= 0xc4ceb9fe1a85ec53ULL;
//     v ^= v >> 33;
//     return v;
// }

// static u64 _entry_checksum(const Entry* e) {
//     u64 k = G.session_key;
//     return _mix((u64)e->id,      k   )
//          ^ _mix(     e->token,   k +1)
//          ^ _mix((u64)e->offset,  k +2)
//          ^ _mix((u64)e->size,    k +3)
//          ^ _mix((u64)e->gen,     k +4);
// }

// // HASH

// // VIRTUALPROTECT HELPER

// static null _lock_storage(void) {
//     DWORD old_protect;
//     VirtualProtect(G.storage, (SIZE_T)G.capacity, PAGE_NOACCESS, &old_protect);
// }

// static null _unlock_storage(void) {
//     DWORD old_protect;
//     VirtualProtect(G.storage, (SIZE_T)G.capacity, PAGE_READWRITE, &old_protect);
// }

// static null _lock_table(void) {
//     DWORD old_protect;
//     VirtualProtect(G.table, (SIZE_T)(MAX_ENTRIES * sizeof(Entry)), PAGE_NOACCESS, &old_protect);
// }

// static null _unlock_table(void) {
//     DWORD old_protect;
//     VirtualProtect(G.table, (SIZE_T)(MAX_ENTRIES * sizeof(Entry)), PAGE_READWRITE, &old_protect);
// }

// // VIRTUALPROTECT HELPER

// // PROTECTED REGION ALLOCATOR

// static i8* _alloc_guarded(s64 data_size, s64 ps, s64* total_out) {
//     s64 total = ps + data_size + ps;
//     // MEM_RESERVE | MEM_COMMIT is the windows equv of MAP_ANONYMOUS | MAP_PRIVATE
//     i8* r = (i8*) VirtualAlloc(NULL, (SIZE_T)total, MEM_RESERVE | MEM_COMMIT, PAGE_NOACCESS);
//     if(r == NULL) return NULL;
//     *total_out = total;
//     return r;
// }

// // PROTECTED REGION ALLOCATOR


// // ARENA INIT

// null arena_init(s64 capacity) {
//     G.page_size     = _get_page_size();
//     G.session_key   = _rand64();
//     G.next_id       = 0;

//     s64 ps = G.page_size;
//     // page align both regions
//     s64 stor_sz = ((capacity + ps -1) / ps) * ps;
//     s64 table_sz = (((s64)(MAX_ENTRIES * sizeof(Entry)) + ps -1) / ps) * ps;

//     // STORAGE
//     G.storage_region = _alloc_guarded(stor_sz, ps, &G.storage_region_size);
//     G.storage = G.storage_region + ps;
//     G.capacity = stor_sz;
//     G.used = 0;

//     // TABLE
//     G.table_region = _alloc_guarded(table_sz, ps, &G.table_region_size);
//     G.table = (Entry*)(G.table_region + ps);

//     DWORD old_protect;

//     // zeroed and lock
//     VirtualProtect(G.table, (SIZE_T)table_sz, PAGE_READWRITE, &old_protect);
//     memset(G.table, 0, (size_t)table_sz);
//     for(i32 i = 0; i < MAX_ENTRIES; i++) 
//         G.table[i].id = ENTRY_FREE;
//     VirtualProtect(G.table, (SIZE_T)table_sz, PAGE_NOACCESS, &old_protect);

//     VirtualProtect(G.storage, (SIZE_T)stor_sz, PAGE_READWRITE, &old_protect);
//     memset(G.storage, 0, (size_t)stor_sz);
//     VirtualProtect(G.storage, (SIZE_T)stor_sz, PAGE_NOACCESS, &old_protect);

// }

// // ARENA INIT

// // ARENA FREE

// null arena_free(void) {

//     if(G.storage_region) VirtualFree(G.storage_region, 0, MEM_RELEASE);
//     if(G.table_region) VirtualFree(G.table_region, 0, MEM_RELEASE);
//     memset(&G, 0, sizeof G);
// }

// // ARENA FREE

// // ARENA ALLOC

// Handle arena_alloc(s64 size) {
//     if(size <= 0 || G.used + size > G.capacity) 
//         return HANDLE_INVALID;
    
//     _unlock_table();

//     Handle h = HANDLE_INVALID;
//     for(i32 i =0; i< MAX_ENTRIES; i++) {

//         if(G.table[i].id != ENTRY_FREE) continue;
        
//         Entry* e = &G.table[i];
//         e->id = G.next_id++;
//         e->token = _rand64();
//         e->offset = G.used;
//         e->size = size;

//         e->checksum = _entry_checksum(e);

//         h.id = e->id;
//         h.token = e->token;
//         h.gen = e->gen;

//         G.used += size;
//         break;

//     }

//     _lock_table();
//     return h;
// }

// // ARENA ALLOC

// // ARENA DEALLOC

// null arena_dealloc(Handle h) {
//     _unlock_table();

//     for(i32 i = 0; i< MAX_ENTRIES; i++) {
//         Entry* e = &G.table[i];
//         if(e->id = ENTRY_FREE || e->id != h.id) continue;

//         if(e->checksum  != _entry_checksum(e))  goto done;
//         if(e->token     != h.token)             goto done;
//         if(e->gen       != h.gen)               goto done;

//         _unlock_storage();
//         memset(G.storage + e->offset, 0, (size_t)e->size);
//         _lock_storage();

//         e->gen++;
//         e->id = ENTRY_FREE;
//         e->token = 0;
//         e->checksum = 0;
//         break;
//     }
    
// done:
//     _lock_table();
// }

// // ARENA DEALLOC

// // ARENA RESOLVE

// static i32 _resolve(Handle h, Entry* out) {
//     i32 ok = 0;
//     _unlock_table();
//     for(i32 i =0; i< MAX_ENTRIES; i++) {
//         Entry* e = &G.table[i];
//         if(e->id == ENTRY_FREE || e->id != h.id) continue;

//         if(e->checksum  != _entry_checksum(e))  break; // tampered
//         if(e->token     != h.token)             break; // forged
//         if(e->gen       != h.gen)               break; // stale
    
//         *out = *e;
//         ok = 1;
//         break;
//     }
//     _lock_table();
//     return ok;
// }

// // ARENA RESOLVE

// // ARENA READWRITE

// null arena_write(Handle h, const i8* src) {
//     Entry e;
//     if(!_resolve(h, &e))    return;

//     _unlock_storage();
//     memcpy(G.storage + e.offset, src, (size_t)e.size);
//     _lock_storage();
// }

// null arena_read(Handle h, i8* dst) {
//     Entry e;
//     if(!_resolve(h, &e))    return;

//     _unlock_storage();
//     memcpy(dst, G.storage + e.offset, (size_t)e.size);
//     _lock_storage();
// }
// // null arena_write(Handle h, s64 off, const i8* src, s64 size) {
// //     Entry e;
// //     if(!_resolve(h, &e))    return;
// //     if(off < 0 || off + size > e.size) return;

// //     _unlock_storage();
// //     memcpy(G.storage + e.offset + off, src, (size_t)size);
// //     _lock_storage();
// // }

// // null arena_read(Handle h, s64 off, i8* dst, s64 size) {
// //     Entry e;
// //     if(!_resolve(h, &e))    return;
// //     if(off < 0 || off + size > e.size) return;

// //     _unlock_storage();
// //     memcpy(dst, G.storage + e.offset + off, (size_t)size);
// //     _lock_storage();
// // }

// // ARENA READWRITE


// null print_row(i32 row) {
//     for(i32 i = row; i< row+16; i++ ) {
//         printf("%02x ", G.storage[i]);
//     }endline;
// }

// null print_arena() {
//     _unlock_storage();
//     printf("         00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f"endl);
//     for(i32 i = 0; i< G.used; i+=16) {
//         printf("%08x ", i);
//         print_row(i);
//     }
//     _lock_storage();
// }
