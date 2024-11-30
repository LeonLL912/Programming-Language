// linkedlist.c by Leon Liang

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "object.h"
#include "linkedlist.h"
#include "talloc.h"
#include <stdio.h>

// Return: A newly allocated Object of NULL_TYPE.
Object *makeNull(){
    Object *nullObj = (Object *)talloc(sizeof(Object));
    nullObj->type = NULL_TYPE;
    return (Object *)nullObj;
}

// Return: A newly allocated Object of INT_TYPE.
Integer *makeInt(){
    Integer *intObj = (Integer *)talloc(sizeof(Integer));
    intObj->type = INT_TYPE;
    return (Integer *)intObj;
}

// Return: A newly allocated Object of DOUBLE_TYPE.
Double *makeDouble(){
    Double *doubleObj = (Double *)talloc(sizeof(Double));
    doubleObj->type = DOUBLE_TYPE;
    return (Double *)doubleObj;
}

// Return: A newly allocated Object of STR_TYPE.
String *makeString(){
    String *strObj = (String *)talloc(sizeof(String));
    strObj->type = STR_TYPE;
    return (String *)strObj;
}

// Return: A newly allocated Object of CONS_TYPE.
ConsCell *makeConsCell(){
    ConsCell *conObj = (ConsCell *)talloc(sizeof(ConsCell));
    conObj->type = CONS_TYPE;
    return (ConsCell *)conObj;
}

// Input newCar: An instance of Object or one of its subclasses.
// Input newCdr: An instance of Object or one of its subclasses.
// Return: A newly allocated ConsCell object with that car and cdr.
Object *cons(Object *newCar, Object *newCdr){
    ConsCell *newObj = (ConsCell *)talloc(sizeof(ConsCell));
    newObj->type = CONS_TYPE;
    newObj->car = newCar;
    newObj->cdr = newCdr;
    return (Object *)newObj;
}

// Input list: A ConsCell that is the head of a list.
// Prints the entire list in a human-readable way.
void display(Object *list) {
    if (list->type == NULL_TYPE) {
        printf("()");
        return;
    }
    
    printf("(");
    while (list->type != NULL_TYPE) {
        assert(list->type == CONS_TYPE);

        objectType type = car(list)->type;
        if (type == INT_TYPE) {
            printf("%d", ((Integer *)car(list))->value);
        } else if (type == DOUBLE_TYPE) {
            printf("%f", ((Double *)car(list))->value);
        } else if (type == STR_TYPE) {
            printf("\"%s\"", ((String *)car(list))->value);
        } else if (type == CONS_TYPE) {
            display(car(list));
        }

        list = cdr(list);
        if (list->type != NULL_TYPE) {
            printf(" ");
        }
    }
    printf(")");
}

// Input list: A ConsCell that is the head of a list.
// Return: A ConsCell that is the head of a list. The new list is the reverse of 
// the given list. All content within the list is duplicated; there is no shared 
// memory whatsoever between the given list and the new one.
Object *reverse(Object *list) {
    Object *rlist = makeNull();
    while (list->type != NULL_TYPE) {
        rlist = cons(car(list),rlist);
        list = cdr(list);
    }
    return rlist;
}

// Input list: A ConsCell.
// Return: The car of that ConsCell.
// This is a convenience function to slightly accelerate taking cars of objects 
// known to be cons cells.
Object *car(Object *list){
    assert (list->type == CONS_TYPE);
    return ((ConsCell *)list)->car;
}

// Input list: A ConsCell.
// Return: The cdr of that ConsCell.
// This is a convenience function to slightly accelerate taking cars of objects 
// known to be cons cells.
Object *cdr(Object *list){
    assert (list->type == CONS_TYPE);
    return ((ConsCell *)list)->cdr;
}

// Input list: Any object.
// Return: A Boolean indicating whether that object is of NULL_TYPE.
bool isNull(Object *value){
    return (value->type == NULL_TYPE);
}

// Input value: A ConsCell that is the head of a list.
// Return: The length of the list. For example, the Scheme list () has length 0, 
// and the Scheme list (7) has length 1.
int length(Object *value){
    int count = 0;
    while (value->type != NULL_TYPE) {
        assert (CONS_TYPE == value->type);
        assert (value != NULL);
        count++;
        value = cdr(value);
    }
    return count;
}
