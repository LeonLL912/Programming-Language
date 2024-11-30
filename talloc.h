


#include <stdlib.h>
#include "object.h"

#ifndef _TALLOC
#define _TALLOC

// Input size: The number of bytes to allocate from the heap.
// Return: A pointer to heap-allocated memory of size bytes. NULL upon failure.
// A replacement for the built-in C function malloc. This function tracks the 
// allocated heap memory in a data structure, such that tfree can free it later.
void *talloc(size_t size);

// Frees all heap memory previously talloced (as well as any memory needed to 
// administer that memory).
void tfree();

// Input status: A C error code. Zero if no error, non-zero if error.
// A replacement for the built-in C function exit. Calls tfree and then exit.
void texit(int status);

#endif

