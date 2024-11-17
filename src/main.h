#ifndef MAIN_H
#define MAIN_H

#include <stddef.h>
#include <pthread.h>

// Macros
#define HASH_TABLE_SIZE 32
#define Pool_Size 1024 * 1024 // 1 MB

// Struct definitions
// we are using this as a linked list to acces our own free list 
typedef struct FreeBlock {
    struct FreeBlock* next;
} FreeBlock;

typedef struct Slab {
    size_t obejct_size;
    size_t num_objects;
    void* start;
    FreeBlock* free_list;
    struct Slab* next;
} Slab;

typedef struct list {
    size_t obejct_size;
    Slab* slab;
} List_Slabs;

// Function declarations
Slab* create_slab(size_t object_size, size_t num_objects);
void* allocate_from_slab(Slab* slab);
void deallocate_to_slab(Slab* slab, void* block);
void destroy_slab(Slab* slab);
Slab* get_slab_for_size(size_t object_size);
void* allocate(size_t object_size);
void deallocate(size_t object_size, void* block);
void free_all();
extern List_Slabs track[HASH_TABLE_SIZE];

#endif // MAIN_H