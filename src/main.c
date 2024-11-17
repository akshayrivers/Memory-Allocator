#include <stdio.h>
#include "main.h"
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <pthread/pthread.h>

List_Slabs track[HASH_TABLE_SIZE] = {0};
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;// for thread safety 

// Now we need to define functions which can intitialise and do the required jobs
Slab* create_slab(size_t obejct_size,size_t num_objects){
    Slab* slab =(Slab*)malloc(sizeof(Slab));
     if (!slab) {
        perror("Failed to allocate memory for Slab structure");
        exit(EXIT_FAILURE);
    }
    slab->obejct_size = obejct_size;// we defined a pointer here because we are are accessing the Slab* ptr so use should also acces its members with pointers and not  with . 
    slab->num_objects = num_objects;// filling up the struct 

  if (posix_memalign(&slab->start, sizeof(void*), obejct_size * num_objects) != 0) {
        perror("Failed to allocate aligned memory for slab objects");
        free(slab);
        exit(EXIT_FAILURE);
    }

    //Inttialising the free list 
    slab->free_list= (FreeBlock*)slab->start;// alocated the start into the slab or you can say binded 
    FreeBlock* current = slab->free_list;// i think you got this slab current is space in free list 
    // initialising blocks of memory inside the slab 
    for(size_t i=1;i< num_objects; i++){
        current->next = (FreeBlock*)((char*)slab->start+i*obejct_size);// you might ask why char- well find it out yourself try reading pointer arithmatic for hints
        current = current->next;
    }
    current->next=NULL; // end of our free list of one slab 
    slab->next = NULL; 
    return slab;
}   

// allocating memory from inside the slab 
void* allocate_from_slab(Slab* slab) {
    pthread_mutex_lock(&mutex);
    assert(slab != NULL);

    // Try to allocate from the current slab
    while (slab != NULL) {
        if (slab->free_list != NULL) {
            // Memory is available, allocate it
            void* block = slab->free_list;
            slab->free_list = slab->free_list->next;
            pthread_mutex_unlock(&mutex);
            return block;
        }

        // Move to the next slab if the current one is full
        slab = slab->next;
    }

    // If no slabs have available memory, create a new slab
    Slab* new_slab = create_slab(slab->obejct_size, slab->num_objects);

    // Link the new slab to the previous ones
    slab->next = new_slab;

    // Allocate memory from the newly created slab
    void* block = new_slab->free_list;
    new_slab->free_list = new_slab->free_list->next;
    
    pthread_mutex_unlock(&mutex);
    return block;
}



// deallocating memory back to the slab
void deallocate_to_slab(Slab* slab, void* block) {
    pthread_mutex_lock(&mutex);
    assert(block != NULL); 
    //Before : free_list -> [Block A] -> [Block B] -> NULL

    FreeBlock* new_block = (FreeBlock*)block;

    new_block->next = slab->free_list;
    //After : new_block -> [Block X] -> [Block A] -> [Block B] -> NULL
    slab->free_list = new_block;

    //finally: free_list -> [Block X] -> [Block A] -> [Block B] -> NULL
    pthread_mutex_unlock(&mutex);

}

// Cleanup the whole slab and remove it from track
void destroy_slab(Slab* slab) {
    pthread_mutex_lock(&mutex);
    if (slab == NULL) {
        pthread_mutex_unlock(&mutex);
        return;
    }

    int index = slab->obejct_size % HASH_TABLE_SIZE;

    // Debugging: print the current slab and the tracked slab
    printf("Destroying slab with size %zu at index %d\n", slab->obejct_size, index);
    printf("Current tracked slab: %p\n", track[index].slab);

    if (track[index].slab == slab) {
        // Set the track entry to NULL
        track[index].slab = NULL;
        track[index].obejct_size = 0;
        printf("Slab removed from track at index %d\n", index);
    }

    free(slab->start);
    free(slab);

    pthread_mutex_unlock(&mutex);
}




Slab* get_slab_for_size(size_t obejct_size) {
    int index = obejct_size % HASH_TABLE_SIZE;
    if (track[index].obejct_size == obejct_size && track[index].slab != NULL) {
        return track[index].slab;
    }

    // If no matching slab, create a new one
    Slab* new_slab = create_slab(obejct_size, 128);
    track[index].obejct_size = obejct_size;
    track[index].slab = new_slab;

    return new_slab;
}
void* allocate(size_t object_size) {
    Slab* slab = get_slab_for_size(object_size);
    return allocate_from_slab(slab);
}

void deallocate(size_t object_size, void* block) {
    Slab* slab = get_slab_for_size(object_size);
    deallocate_to_slab(slab, block);
}

// Free all slabs in the track array
void free_all() {
    pthread_mutex_lock(&mutex);
    printf("Starting free_all...\n");

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        Slab* slab = track[i].slab;
        if (slab != NULL) {
            printf("Destroying slab at index %d, object size: %zu\n", i, track[i].obejct_size);
            free(slab); // Call to clean up the slab
            track[i].slab = NULL;  // Ensure slab is removed from track
            track[i].obejct_size = 0;  // Reset the object_size
        } else {
            printf("No slab to destroy at index %d\n", i);
        }
    }

    pthread_mutex_unlock(&mutex);
    printf("free_all completed.\n");
}

