#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define Pool_Size 1024*1024//which is 1 mb

// we are using this as a linked list to acces our own free list 
typedef struct FreeBlock{
    struct FreeBlock* next;
} FreeBlock;


typedef struct Slab{
    size_t obejct_size; //size of each object in this slab 
    size_t num_objects; // number of object in this slab
    void* start; 
    FreeBlock* free_list;// free list of pointers for slots 
    struct Slab* next; // pointer to the next slab 
} Slab;

// Now we need to define functions which can intitialise and do the required jobs
Slab* create_slab(size_t obejct_size,size_t num_objects){
    Slab* slab =(Slab*)malloc(sizeof(Slab));
     if (!slab) {
        perror("Failed to allocate memory for Slab structure");
        exit(EXIT_FAILURE);
    }
    slab->obejct_size = obejct_size;// we defined a pointer here because we are are accessing the Slab* ptr so use should also acces its members with pointers and not  with . 
    slab->num_objects = num_objects;// filling up the struct 

    slab->start = malloc(obejct_size*num_objects);// allocated the memory for the full linked list of slab 
      if (!slab->start) {
        perror("Failed to allocate memory for slab objects");
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
void* allocate_from_slab(Slab* slab){

    if(!slab->free_list){
        return NULL;//Slab is full 
    }
    // we used the space and moved on to the next block now 
    void* block = slab->free_list;
    slab->free_list = slab->free_list->next;

    return block;
}

// deallocating memory back to the slab
void deallocate_to_slab(Slab* slab, void* block) {
    //Before : free_list -> [Block A] -> [Block B] -> NULL

    FreeBlock* new_block = (FreeBlock*)block;

    new_block->next = slab->free_list;
    //After : new_block -> [Block X] -> [Block A] -> [Block B] -> NULL
    slab->free_list = new_block;

    //finally: free_list -> [Block X] -> [Block A] -> [Block B] -> NULL
}

// Cleanup the whole slab ultimately free all the memory it used 
void destroy_slab(Slab* slab) {
    free(slab->start);
    free(slab);
}
int main(){
    Slab* slab= create_slab(64,128);

    void* obj1= allocate_from_slab(slab);
    void* obj2 = allocate_from_slab(slab);
    printf("Allocated memory : %p , %p\n",obj1,obj2);
    deallocate_to_slab(slab, obj1);
    deallocate_to_slab(slab, obj2);
    printf("Allocated memory : %p , %p\n",obj1,obj2);
    destroy_slab(slab);
    printf("Allocated memory : %p , %p\n",obj1,obj2);
    return 0;
}