

#include <stdbool.h>
#include "object.h"

#ifndef _LINKEDLIST
#define _LINKEDLIST

// Return: A newly allocated Object of NULL_TYPE.
Object *makeNull();

// Input newCar: An instance of Object or one of its subclasses.
// Input newCdr: An instance of Object or one of its subclasses.
// Return: A newly allocated ConsCell object with that car and cdr.
Object *cons(Object *newCar, Object *newCdr);

// Input list: A ConsCell that is the head of a list.
// Prints the entire list in a human-readable way.
void display(Object *list);

// Input list: A ConsCell that is the head of a list.
// Return: A ConsCell that is the head of a list. The new list is the reverse of 
// the given list. The content of the list (the cars) is not duplicated but 
// rather shared between the old and new lists.
Object *reverse(Object *list);

// Input list: A ConsCell.
// Return: The car of that ConsCell.
// This is a convenience function to slightly accelerate taking cars of objects 
// known to be cons cells.
Object *car(Object *list);

// Input list: A ConsCell.
// Return: The cdr of that ConsCell.
// This is a convenience function to slightly accelerate taking cars of objects 
// known to be cons cells.
Object *cdr(Object *list);

// Input value: Any object.
// Return: A Boolean indicating whether that object is of NULL_TYPE.
bool isNull(Object *value);

// Input value: A ConsCell that is the head of a list.
// Return: The length of the list. For example, the Scheme list () has length 0, 
// and the Scheme list (7) has length 1.
int length(Object *value);

#endif


