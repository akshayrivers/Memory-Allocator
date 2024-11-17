#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "main.h"

// Helper function to print a test result
void print_test_result(int test_number, int passed) {
    if (passed) {
        printf("Test %d PASSED\n", test_number);
    } else {
        printf("Test %d FAILED\n", test_number);
    }
}

// Test 1: Create a slab and ensure it has the correct properties
void test_create_slab() {
    size_t obejct_size = 64;
    size_t num_obejcts = 128;

    Slab* slab = create_slab(obejct_size, num_obejcts);

    // Ensure the slab is not NULL
    assert(slab != NULL);

    // Check if the start pointer is properly aligned (i.e., not NULL)
    assert(slab->start != NULL);

    // Check if the free list is properly initialized
    assert(slab->free_list != NULL);

    printf("Slab creation test passed.\n");
}

// Test 2: Allocate memory from a slab
void test_allocate_from_slab() {
    size_t obejct_size = 64;
    size_t num_obejcts = 128;

    Slab* slab = create_slab(obejct_size, num_obejcts);
    void* allocated_block = allocate_from_slab(slab);

    // Ensure memory is allocated successfully
    assert(allocated_block != NULL);

    // Check that the free list is updated
    assert(slab->free_list != NULL);

    printf("Allocate from slab test passed.\n");
}

// Test 3: Deallocate memory back to the slab
void test_deallocate_to_slab() {
    size_t obejct_size = 64;
    size_t num_obejcts = 128;

    Slab* slab = create_slab(obejct_size, num_obejcts);
    void* allocated_block = allocate_from_slab(slab);

    // Deallocate the memory
    deallocate_to_slab(slab, allocated_block);

    // Ensure the free list is updated and the block is at the front
    assert(slab->free_list == allocated_block);

    printf("Deallocate to slab test passed.\n");
}

// Test 4: Allocate and deallocate multiple blocks
void test_allocate_and_deallocate() {
    size_t obejct_size = 64;
    size_t num_obejcts = 128;

    Slab* slab = create_slab(obejct_size, num_obejcts);

    // Allocate multiple blocks
    void* block1 = allocate_from_slab(slab);
    void* block2 = allocate_from_slab(slab);
    void* block3 = allocate_from_slab(slab);

    // Ensure the blocks are allocated successfully
    assert(block1 != NULL);
    assert(block2 != NULL);
    assert(block3 != NULL);

    // Deallocate the blocks and check if they are returned to the free list
    deallocate_to_slab(slab, block1);
    deallocate_to_slab(slab, block2);
    deallocate_to_slab(slab, block3);

    // Ensure the free list is updated correctly
    assert(slab->free_list == block3);

    printf("Allocate and deallocate multiple blocks test passed.\n");
}

// Test 5: Allocate memory for different obejct sizes using the global allocator
void test_allocate_for_different_sizes() {
    size_t obejct_size_1 = 64;
    size_t obejct_size_2 = 128;
    size_t obejct_size_3 = 256;

    // Allocate memory for different obejct sizes
    void* block1 = allocate(obejct_size_1);
    void* block2 = allocate(obejct_size_2);
    void* block3 = allocate(obejct_size_3);

    // Ensure all blocks are allocated
    assert(block1 != NULL);
    assert(block2 != NULL);
    assert(block3 != NULL);

    // Deallocate the blocks
    deallocate(obejct_size_1, block1);
    deallocate(obejct_size_2, block2);
    deallocate(obejct_size_3, block3);

    printf("Allocate for different sizes test passed.\n");
}

// Test 6: Destroy the slab and ensure memory is freed
void test_destroy_slab() {
    // Creating a slab for testing
    size_t obejct_size = 32;
    size_t num_obejcts = 10;
    Slab* slab = create_slab(obejct_size, num_obejcts);
    
    // Adding the slab to the track array (simulating allocation)
    int index = obejct_size % HASH_TABLE_SIZE;
    track[index].slab = slab;
    track[index].obejct_size = obejct_size;

    // Ensure the slab is in the track array before destruction
    assert(track[index].slab != NULL);
    assert(track[index].obejct_size == obejct_size);

    // Call destroy_slab to clean up the slab
    destroy_slab(slab);

    // Ensure the slab is removed from the track array after destruction
    assert(track[index].slab == NULL);
    assert(track[index].obejct_size == 0);

    printf("destroy_slab test passed.\n");
}


void test_get_slab_for_size_create_new() {
    size_t object_size = 64;

    // Initially, track[object_size % HASH_TABLE_SIZE] should be empty
    int index = object_size % HASH_TABLE_SIZE;
    assert(track[index].slab == NULL);

    // Call get_slab_for_size, which should create a new slab
    Slab* slab = get_slab_for_size(object_size);

    // Ensure the returned slab is not NULL
    assert(slab != NULL);

    // Ensure the slab is stored at the correct index in the track array
    assert(track[index].slab == slab);
    assert(track[index].obejct_size == object_size);

    printf("test_get_slab_for_size_create_new passed.\n");
}
void test_get_slab_for_size_existing() {
    size_t object_size = 64;

    // First call to get_slab_for_size will create a new slab
    Slab* slab1 = get_slab_for_size(object_size);

    // Ensure the slab is created and stored at the correct index
    int index = object_size % HASH_TABLE_SIZE;
    assert(track[index].slab == slab1);
    assert(track[index].obejct_size == object_size);

    // Second call to get_slab_for_size should return the same slab
    Slab* slab2 = get_slab_for_size(object_size);
    assert(slab1 == slab2);  // Ensure the same slab is returned

    printf("test_get_slab_for_size_existing passed.\n");
}
void test_get_slab_for_size_collision() {
    size_t object_size_1 = 64;
    size_t object_size_2 = 128;
    
    // First, create a slab for object_size_1
    Slab* slab1 = get_slab_for_size(object_size_1);
    int index_1 = object_size_1 % HASH_TABLE_SIZE;
    assert(track[index_1].slab == slab1);
    assert(track[index_1].obejct_size == object_size_1);

    // Now, create a slab for object_size_2
    Slab* slab2 = get_slab_for_size(object_size_2);
    int index_2 = object_size_2 % HASH_TABLE_SIZE;
    assert(track[index_2].slab == slab2);
    assert(track[index_2].obejct_size == object_size_2);

    // Check that slabs for different object sizes are stored at different indices
    if (index_1 == index_2) {
        printf("Collision detected: object_size_1 = %zu, object_size_2 = %zu\n", object_size_1, object_size_2);
    }

    // Check that the slabs are different
    assert(slab1 != slab2);

    printf("test_get_slab_for_size_collision passed.\n");
}
// Test function for free_all
void test_free_all(){
    get_slab_for_size(8);
    get_slab_for_size(128);
    get_slab_for_size(248);
    get_slab_for_size(600);
    get_slab_for_size(900);
    free_all();
}
int main() {
    // Run tests
    test_create_slab();
    test_allocate_from_slab();
    test_deallocate_to_slab();
    test_allocate_and_deallocate();
    test_allocate_for_different_sizes();
    test_destroy_slab();
    test_get_slab_for_size_create_new();
    test_get_slab_for_size_existing();

    test_get_slab_for_size_collision();
    test_free_all();

    return 0;
}
