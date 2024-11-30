// talloc.c by Leon Liang

#include <stdlib.h>
#include <assert.h>
#include "talloc.h"

typedef struct MemNode {
    void *ptr;            
    struct MemNode *next;   
} MemNode;

static MemNode *memList = NULL;

// Input size: The number of bytes to allocate from the heap.
// Return: A pointer to heap-allocated memory of size bytes. NULL upon failure.
// A replacement for the built-in C function malloc. This function tracks the 
// allocated heap memory in a data structure, such that tfree can free it later.
void *talloc(size_t size){
    //allocating memory on the heap
    void *newptr = malloc(size);
    assert(newptr != NULL);

    //creating memNode to keep track of memory allocated
    MemNode *memNode = (MemNode *)malloc(sizeof(MemNode));
    assert(memNode != NULL);
    memNode->ptr = newptr;

    //adding the memNode to the memList
    memNode->next = memList;
    memList = memNode;

    //returning the pointer to the allocated space on heap
    return newptr;
}

// Frees all heap memory previously talloced (as well as any memory needed to 
// administer that memory).
void tfree(){
    MemNode *current = memList;
    while (current != NULL) {
        MemNode *nextNode = current->next;
        free(current->ptr);
        free(current);    
        current = nextNode;
    }
    memList = NULL; 
}

// Input status: A C error code. Zero if no error, non-zero if error.
// A replacement for the built-in C function exit. Calls tfree and then exit.
void texit(int status){
    tfree();
    exit(status);
}