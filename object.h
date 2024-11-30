


#ifndef _VALUE
#define _VALUE

typedef enum {
    INT_TYPE, DOUBLE_TYPE, STR_TYPE, CONS_TYPE, NULL_TYPE, PTR_TYPE,
    OPEN_TYPE, CLOSE_TYPE, BOOL_TYPE, SYMBOL_TYPE, CLOSEBRACE_TYPE, 
    UNSPECIFIED_TYPE, VOID_TYPE, CLOSURE_TYPE, PRIMITIVE_TYPE
} objectType;

// An Object can have a few types --- any type that requires no extra storage.
typedef struct Object {
    objectType type;
} Object;

// An Integer should have INT_TYPE.
typedef struct Integer {
    objectType type;
    int value;
} Integer;

// A Double should have DOUBLE_TYPE.
typedef struct Double {
    objectType type;
    double value;
} Double;

// A String should have STR_TYPE. Its value member should point to a heap-
// allocated C string. As always, that string should be null-terminated.
typedef struct String {
    objectType type;
    char *value;
} String;

// A Pointer should have PTR_TYPE. I think that this class is never used in the 
// entire interpreter. But let's just leave it here for now.
typedef struct Pointer {
    objectType type;
    void *value;
} Pointer;

// A ConsCell should have CONS_TYPE.
typedef struct ConsCell {
    objectType type;
    Object *car;
    Object *cdr;
} ConsCell;

// A Boolean should have BOOL_TYPE. The value should be 0 for false, 1 for true.
typedef struct Boolean {
    objectType type;
    int value;
} Boolean;

// A Symbol should have SYMBOL_TYPE. Its value member should point to a heap-
// allocated C string. As always, that string should be null-terminated.
typedef struct Symbol {
    objectType type;
    char *value;
} Symbol;

// A frame is a list of bindings and a pointer to a parent frame. The details of 
// how the bindings are stored are up to you. Just be consistent.
struct Frame {
    Object *bindings;
    struct Frame *parent;
};
typedef struct Frame Frame;

// A Closure should have CLOSURE_TYPE.
struct Closure {
    objectType type;
    Object *paramNames;
    Object *functionCode;
    Frame *frame;
};
typedef struct Closure Closure;

// A Primitive should have PRIMITIVE_TYPE.
struct Primitive {
    objectType type;
    Object *(*pf)(Object *);
} ;
typedef struct Primitive Primitive;

#endif


