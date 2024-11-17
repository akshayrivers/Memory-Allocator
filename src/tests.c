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
    size_t object_size = 64;
    size_t num_objects = 128;

    Slab* slab = create_slab(object_size, num_objects);

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
    size_t object_size = 64;
    size_t num_objects = 128;

    Slab* slab = create_slab(object_size, num_objects);
    void* allocated_block = allocate_from_slab(slab);

    // Ensure memory is allocated successfully
    assert(allocated_block != NULL);

    // Check that the free list is updated
    assert(slab->free_list != NULL);

    printf("Allocate from slab test passed.\n");
}

// Test 3: Deallocate memory back to the slab
void test_deallocate_to_slab() {
    size_t object_size = 64;
    size_t num_objects = 128;

    Slab* slab = create_slab(object_size, num_objects);
    void* allocated_block = allocate_from_slab(slab);

    // Deallocate the memory
    deallocate_to_slab(slab, allocated_block);

    // Ensure the free list is updated and the block is at the front
    assert(slab->free_list == allocated_block);

    printf("Deallocate to slab test passed.\n");
}

// Test 4: Allocate and deallocate multiple blocks
void test_allocate_and_deallocate() {
    size_t object_size = 64;
    size_t num_objects = 128;

    Slab* slab = create_slab(object_size, num_objects);

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

// Test 5: Allocate memory for different object sizes using the global allocator
void test_allocate_for_different_sizes() {
    size_t object_size_1 = 64;
    size_t object_size_2 = 128;
    size_t object_size_3 = 256;

    // Allocate memory for different object sizes
    void* block1 = allocate(object_size_1);
    void* block2 = allocate(object_size_2);
    void* block3 = allocate(object_size_3);

    // Ensure all blocks are allocated
    assert(block1 != NULL);
    assert(block2 != NULL);
    assert(block3 != NULL);

    // Deallocate the blocks
    deallocate(object_size_1, block1);
    deallocate(object_size_2, block2);
    deallocate(object_size_3, block3);

    printf("Allocate for different sizes test passed.\n");
}

// Test 6: Destroy the slab and ensure memory is freed
void test_destroy_slab() {
    size_t object_size = 64;
    size_t num_objects = 128;

    // Create a slab
    Slab* slab = create_slab(object_size, num_objects);

    // Destroy the slab and ensure it frees the memory
    destroy_slab(slab);

    // Set slab to NULL after destruction to prevent accessing freed memory
    slab = NULL;

    // Optionally, check if the track array has been updated to reflect the removal
    int index = object_size % HASH_TABLE_SIZE;
    assert(track[index].slab == NULL);  // Ensure the slab is removed from the track
    assert(track[index].obejct_size == 0);  // Ensure the object size is reset

    printf("Destroy slab test passed.\n");
}
void test_free_all() {
    size_t object_size1 = 64;
    size_t object_size2 = 128;
    size_t num_objects = 128;

    // Allocate slabs for different object sizes
    Slab* slab1 = create_slab(object_size1, num_objects);
    Slab* slab2 = create_slab(object_size2, num_objects);

    // Track the slabs manually (mimicking allocation through get_slab_for_size)
    track[object_size1 % HASH_TABLE_SIZE].slab = slab1;
    track[object_size1 % HASH_TABLE_SIZE].obejct_size = object_size1;

    track[object_size2 % HASH_TABLE_SIZE].slab = slab2;
    track[object_size2 % HASH_TABLE_SIZE].obejct_size = object_size2;

    // Ensure that slabs are properly allocated in the track array
    assert(track[object_size1 % HASH_TABLE_SIZE].slab == slab1);
    assert(track[object_size2 % HASH_TABLE_SIZE].slab == slab2);

    // Call free_all to free all slabs
    free_all();

    // Ensure all slabs are removed from the track array
    assert(track[object_size1 % HASH_TABLE_SIZE].slab == NULL);
    assert(track[object_size2 % HASH_TABLE_SIZE].slab == NULL);
    
    // Ensure the object sizes are reset
    assert(track[object_size1 % HASH_TABLE_SIZE].obejct_size == 0);
    assert(track[object_size2 % HASH_TABLE_SIZE].obejct_size == 0);

    printf("free_all test passed.\n");
}


int main() {
    // Run tests
    test_create_slab();
    test_allocate_from_slab();
    test_deallocate_to_slab();
    test_allocate_and_deallocate();
    test_allocate_for_different_sizes();
    //test_destroy_slab();
    //test_free_all();

    return 0;
}
