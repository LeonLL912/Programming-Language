// interpreter.c by Leon Liang

#include <stdio.h>
#include <string.h>
#include "object.h"
#include "talloc.h"
#include "linkedlist.h"
#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"

// Helper function
// Deal with errors
Object *evaluationError(){
    printf("Evaluation error\n");
    texit(1);
    return NULL;
}

// Helper function
// Look up a symbol in the given frame
// Note: the cdr of a binding is the value object
Object *evalSymbol(Object *tree, Frame *frame) {
    Symbol *symbol = (Symbol *)tree;
    while (frame != NULL) {
        Object *current = frame->bindings;
        while (current->type != NULL_TYPE) {
            Object *binding = car(current);
            if (strcmp(((Symbol *)car(binding))->value, symbol->value) == 0) {
                return cdr(binding);
            }
            current = cdr(current);
        }
        frame = frame->parent;
    }
    return evaluationError();
}

// Helper function
// Evaluate an if expression
Object *evalIf(Object *tree, Frame *frame) {
    Object *conditionCons = cdr(tree);
    if (conditionCons->type != CONS_TYPE) {
        return evaluationError(); // Too few arguments
    }
    Object *thenCons = cdr(cdr(tree));
    if (thenCons->type != CONS_TYPE) {
        return evaluationError(); // Too few arguments
    }
    Object *elseCons = cdr(cdr(cdr(tree)));
    if (elseCons->type == CONS_TYPE && cdr(elseCons)->type == CONS_TYPE){
        return evaluationError(); // Too many arguments
    }
    
    Object *condResult = eval(car(conditionCons), frame);
    if (condResult->type != BOOL_TYPE || ((Boolean *)condResult)->value == 1) {
        return eval(car(thenCons), frame);
    } else {
        if (elseCons->type == CONS_TYPE) {
            return eval(car(elseCons), frame);
        } else {
            Object *unspecified = talloc(sizeof(Object));
            unspecified->type = UNSPECIFIED_TYPE;
            return unspecified;
        }
    }
    return evaluationError();
}

// Helper function
// Evaluate a let expression
Object *evalLet(Object *tree, Frame *frame) {
    Frame *letFrame = talloc(sizeof(Frame));
    letFrame->parent = frame;
    letFrame->bindings = makeNull();
    
    Object *pairRootRoot = cdr(tree);
    if (pairRootRoot->type != CONS_TYPE){
        return evaluationError(); // No var val pairs after let
    }
    Object *pairRoot = car(cdr(tree));
    if (pairRoot->type != CONS_TYPE && pairRoot->type != NULL_TYPE) {
        return evaluationError(); // Bindings should be a list
    }
    Object *body = cdr(cdr(tree));

    // Process each variable-value pair
    while (pairRoot->type == CONS_TYPE) {
        Object *pair = car(pairRoot);
        if (pair->type != CONS_TYPE || cdr(pair)->type != CONS_TYPE) {
            return evaluationError(); // Missing var or val
        }

        Object *var = car(pair);
        Object *val = car(cdr(pair));
        if (var->type != SYMBOL_TYPE) {
            return evaluationError();
        }

        // Look for an existing binding in the current frame
        Object *current = letFrame->bindings;
        while (current->type != NULL_TYPE) {
            Object *existingBinding = car(current);
            if (strcmp(((Symbol *)car(existingBinding))->value, ((Symbol *)var)->value) == 0) {
                return evaluationError(); // Duplicate variable found
            }
            current = cdr(current);
        }

        Object *value = eval(val, frame);
        letFrame->bindings = cons(cons(var, value), letFrame->bindings);

        pairRoot = cdr(pairRoot);
    }

    if (pairRoot->type != NULL_TYPE) {
        return evaluationError();
    }

    // Evaluate each expression in the body, returning the last result
    Object *result = NULL;
    if (body->type == NULL_TYPE) {
        // No body expressions, return unspecified
        result = talloc(sizeof(Object));
        result->type = UNSPECIFIED_TYPE;
    } 
    else {
        while (body->type != NULL_TYPE) {
            result = eval(car(body), letFrame);
            body = cdr(body);
        }
    }
    return result;
}

// Helper function
// Evaluate a quote expression
Object *evalQuote(Object *tree) {
    Object *list = cdr(tree);
    if (list->type == NULL_TYPE || cdr(list)->type != NULL_TYPE) {
        return evaluationError();
    }
    return car(list);
}

// Helper function
// Evaluate a define expression
Object *evalDefine(Object *tree, Frame *frame) {
    // Check if there are exactly two arguments
    if (cdr(tree)->type != CONS_TYPE || cdr(cdr(tree))->type != CONS_TYPE || cdr(cdr(cdr(tree)))->type != NULL_TYPE) {
        return evaluationError();
    }

    // Check if the first argument is a symbol
    Object *symbol = car(cdr(tree));
    if (symbol->type != SYMBOL_TYPE) {
        return evaluationError(); 
    }

    // Check if the symbol already exists in the current frame
    Object *current = frame->bindings;
    while (current->type == CONS_TYPE) {
        Object *binding = car(current);
        if (strcmp(((Symbol *)car(binding))->value, ((Symbol *)symbol)->value) == 0) {
            return evaluationError();
        }
        current = cdr(current);
    }

    Object *valueExpr = car(cdr(cdr(tree)));
    Object *value = eval(valueExpr, frame);
    Object *binding = cons(symbol, value);
    frame->bindings = cons(binding, frame->bindings);

    // Return an object of VOID_TYPE as the result of define
    Object *voidResult = talloc(sizeof(Object));
    voidResult->type = VOID_TYPE;
    return voidResult;
}

// Helper function
// Evaluate a lambda expression
Object *evalLambda(Object *tree, Frame *frame) {
    Closure *closure = talloc(sizeof(Closure));
    closure->type = CLOSURE_TYPE;
    closure->frame = frame;

    if (cdr(tree)->type != CONS_TYPE){
        return evaluationError();
    }

    // Handle the list of parameters
    Object *paramList = car(cdr(tree));
    if (paramList->type != CONS_TYPE && paramList->type != NULL_TYPE) {
        return evaluationError();
    }
    
    Object *currentParam = paramList;
    while (currentParam->type == CONS_TYPE) {
        Object *param = car(currentParam);
        if (param->type != SYMBOL_TYPE) {
            return evaluationError(); // Each parameter must be a symbol
        }
        currentParam = cdr(currentParam);
    }
    if (currentParam->type != NULL_TYPE) {
        return evaluationError();
    }

    // Handle duplicate parameters
    currentParam = paramList;
    while (currentParam->type == CONS_TYPE) {
        Object *check = cdr(currentParam);
        while (check->type == CONS_TYPE) {
            if (strcmp(((Symbol *)car(currentParam))->value, ((Symbol *)car(check))->value) == 0) {
                return evaluationError(); // Duplicate parameter found
            }
            check = cdr(check);
        }
        currentParam = cdr(currentParam);
    }

    // Handle the body expressions
    Object *bodyList = cdr(cdr(tree));
    if (bodyList->type == NULL_TYPE){
        return evaluationError(); // Missing body expression
    }

    closure->paramNames = paramList;
    closure->functionCode = bodyList;

    return (Object *)closure;
}

// Helper function
// Apply a closure to arguments
Object *apply(Object *function, Object *args) {
    Closure *closure = (Closure *)function;
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->parent = closure->frame;
    newFrame->bindings = makeNull();

    // Adding var val pairs to the binding of the new frame
    Object *paramList = closure->paramNames;
    while (paramList->type == CONS_TYPE && args->type == CONS_TYPE) {
        Object *param = car(paramList);
        Object *argValue = car(args);

        newFrame->bindings = cons(cons(param, argValue), newFrame->bindings);
        paramList = cdr(paramList);
        args = cdr(args);
    }

    if (paramList->type != NULL_TYPE || args->type != NULL_TYPE) {
        return evaluationError();
    }

    Object *body = closure->functionCode;
    Object *result = NULL;
    if (body->type == NULL_TYPE) {
        result = talloc(sizeof(Object));
        result->type = UNSPECIFIED_TYPE;
    } else {
        while (body->type != NULL_TYPE) {
            result = eval(car(body), newFrame);
            body = cdr(body);
        }
    }

    return result;
}

// Helper function
// Apply a primitive to arguments
Object *applyPrimitives(Object *function, Object *args) {
    Primitive *primitive = (Primitive *)function;
    Object *(*primFunc)(Object *);
    primFunc = primitive->pf;

    return primFunc(args);
}

// Helper function
// Handle null? primitive
Object *primitiveNull(Object *args) {
    if (args->type != CONS_TYPE || cdr(args)->type != NULL_TYPE) {
        return evaluationError(); // Argument count is not 1
    }
    Object *arg = car(args);
    Boolean *result = talloc(sizeof(Boolean));
    result->type = BOOL_TYPE;
    if (arg->type == NULL_TYPE){
        result->value = 1;
    }
    else {
        result->value = 0;
    }
    return (Object *)result;
}

// Helper function
// Handle car primitive
Object *primitiveCar(Object *args) {
    if (args->type != CONS_TYPE || cdr(args)->type != NULL_TYPE) {
        return evaluationError(); // Error if argument count is not 1 or the type of the argument is not cons
    }
    Object *arg = car(args);
    if (arg->type != CONS_TYPE) {
        return evaluationError(); // Error if argument is not a cons cell
    }
    return car(arg);
}

// Helper function
// Handle cdr primitive
Object *primitiveCdr(Object *args) {
    if (args->type != CONS_TYPE || cdr(args)->type != NULL_TYPE) {
        return evaluationError(); // Error if argument count is not 1
    }
    Object *arg = car(args);
    if (arg->type != CONS_TYPE) {
        return evaluationError(); // Error if argument is not a cons cell
    }
    return cdr(arg);
}

// Helper function
// Handle cons primitive
Object *primitiveCons(Object *args) {
    if (args->type != CONS_TYPE || cdr(args)->type != CONS_TYPE || cdr(cdr(args))->type != NULL_TYPE) {
        return evaluationError(); // Argument count is not 2
    }
    Object *first = car(args);
    Object *second = car(cdr(args));
    return cons(first, second);
}

// Helper function
// Handle '+' primitive
Object *primitiveAdd(Object *args) {
    double sum = 0;
    int isdouble = 0; // 0 if the sum should be int, 1 if the sum should be double
    while (args->type == CONS_TYPE) {
        Object *arg = car(args);
        if (arg->type == INT_TYPE) {
            sum += ((Integer *)arg)->value;
        } else if (arg->type == DOUBLE_TYPE) {
            sum += ((Double *)arg)->value;
            isdouble = 1;
        } else {
            return evaluationError();
        }
        args = cdr(args);
    }

    if (isdouble == 0){
        Integer *result = talloc(sizeof(Integer));
        result->type = INT_TYPE;
        int sumInt = sum;
        result->value = sumInt;
        return (Object *)result;
    }
    else if (isdouble == 1){
        Double *result = talloc(sizeof(Double));
        result->type = DOUBLE_TYPE;
        result->value = sum;
        return (Object *)result;
    }
    
    return evaluationError(); // Should never get here
}

// Helper function
// Handle map primitive
Object *primitiveMap(Object *args) {
    if (args->type != CONS_TYPE || cdr(args)->type != CONS_TYPE || cdr(cdr(args))->type != NULL_TYPE) {
        return evaluationError(); // Argument count is not 2
    }
    Object *func = car(args);
    Object *list = car(cdr(args));
    if (list->type != CONS_TYPE && list->type != NULL_TYPE) {
        return evaluationError(); // Second argument is not a list
    }

    Object *result = makeNull();
    while (list->type == CONS_TYPE) {
        Object *element = car(list); 
        Object *argList = cons(element, makeNull()); 

        Object *mappedValue;
        if (func->type == CLOSURE_TYPE) {
            mappedValue = apply(func, argList); 
        } else if (func->type == PRIMITIVE_TYPE) {
            mappedValue = applyPrimitives(func, argList); 
        } else {
            return evaluationError(); // First argument is not a function
        }

        result = cons(mappedValue, result); 
        list = cdr(list); 
    }
    
    result = reverse(result);
    return result;
}

// Input tree: A cons cell representing the root of the abstract syntax tree for 
// a single Scheme expression (not an entire program).
// Input frame: The frame, with respect to which to perform the evaluation.
// Return: The value of the given expression with respect to the given frame.
Object *eval(Object *tree, Frame *frame){
    if (tree->type == INT_TYPE || tree->type == DOUBLE_TYPE || tree->type == STR_TYPE || tree->type == BOOL_TYPE){
        return tree;
    }
    else if (tree->type == SYMBOL_TYPE){
        return  evalSymbol(tree,frame);
    }
    else if (tree->type == CONS_TYPE){
        Object *carCons = car(tree);
        if (carCons->type == SYMBOL_TYPE && strcmp(((Symbol *)carCons)->value, "if") == 0) {
            return evalIf(tree, frame);
        }
        else if (carCons->type == SYMBOL_TYPE && strcmp(((Symbol *)carCons)->value, "let") == 0) {
            return evalLet(tree, frame);
        }
        else if (carCons->type == SYMBOL_TYPE && strcmp(((Symbol *)carCons)->value, "quote") == 0) {
            return evalQuote(tree);
        }
        else if (carCons->type == SYMBOL_TYPE && strcmp(((Symbol *)carCons)->value, "define") == 0) {
            return evalDefine(tree, frame);
        }
        else if (carCons->type == SYMBOL_TYPE && strcmp(((Symbol *)carCons)->value, "lambda") == 0) {
            return evalLambda(tree, frame);
        }
        else {
            // Assume it's a function application
            Object *function = eval(carCons, frame);
            Object *args = cdr(tree);

            Object *evaluatedArgs = makeNull();
            while (args->type == CONS_TYPE) {
                evaluatedArgs = cons(eval(car(args), frame), evaluatedArgs);
                args = cdr(args);
            }
            evaluatedArgs = reverse(evaluatedArgs);

            if (function->type == PRIMITIVE_TYPE) {
                return applyPrimitives(function, evaluatedArgs);
            }
            else if (function->type == CLOSURE_TYPE) {
                return apply(function, evaluatedArgs);
            }
        }
    }
    return evaluationError();
}

// Helper function to print an object's value
void printObj(Object *obj) {
    if (obj->type == INT_TYPE) {
        printf("%d", ((Integer *)obj)->value);
    } 
    else if (obj->type == DOUBLE_TYPE) {
        printf("%f", ((Double *)obj)->value);
    } 
    else if (obj->type == STR_TYPE) {
        printf("\"%s\"", ((String *)obj)->value);
    } 
    else if (obj->type == SYMBOL_TYPE) {
        printf("%s", ((Symbol *)obj)->value);
    } 
    else if (obj->type == BOOL_TYPE) {
        Boolean *boolToken = (Boolean *)obj;
        if (boolToken->value == 0) {
            printf("#f");
        } else {
            printf("#t");
        }
    } 
    else if (obj->type == CONS_TYPE) {
        printf("(");
        Object *current = obj;
        while (current->type == CONS_TYPE) {
            printObj(car(current));
            current = cdr(current);
            if (current->type == CONS_TYPE) {
                printf(" ");
            }
            else if (current->type != NULL_TYPE) {
                // Handle dotted pair
                printf(" . ");
                printObj(current);
            }
        }
        printf(")");
    } 
    else if (obj->type == NULL_TYPE) {
        printf("()");
    } 
    else if (obj->type == UNSPECIFIED_TYPE) {
        printf("#<unspecified>");
    } 
    else if (obj->type == VOID_TYPE) {
        // Do nothing for VOID_TYPE
    } 
    else if (obj->type == CLOSURE_TYPE) {
        printf("#<procedure>");
    } 
    else {
        printf("#<unknown>");
    }
}

// Helper function
// Add primitives to the global frame
void addBinding(char *str, Primitive *primitive, Frame *frame){
    Symbol *symbol = talloc(sizeof(Symbol));
    symbol->type = SYMBOL_TYPE;
    symbol->value = str;

    Object *binding = cons((Object *)symbol,(Object *)primitive);

    frame->bindings = cons(binding, frame->bindings);
}

// Input tree: A cons cell representing the root of the abstract syntax tree for 
// a Scheme program (which may contain multiple expressions).
// Evaluates the program, printing the result of each expression in it.
void interpret(Object *tree) {
    Frame *globalFrame = talloc(sizeof(Frame));
    globalFrame->parent = NULL; // No parent frame for global scope
    globalFrame->bindings = makeNull();

    // null?
    Primitive *primNull = talloc(sizeof(Primitive));
    primNull->type = PRIMITIVE_TYPE;
    primNull->pf = primitiveNull;
    char *nullString = "null?";
    addBinding(nullString, primNull, globalFrame);

    // car
    Primitive *primCar = talloc(sizeof(Primitive));
    primCar->type = PRIMITIVE_TYPE;
    primCar->pf = primitiveCar;
    char *carString = "car";
    addBinding(carString, primCar, globalFrame);

    // cdr
    Primitive *primCdr = talloc(sizeof(Primitive));
    primCdr->type = PRIMITIVE_TYPE;
    primCdr->pf = primitiveCdr;
    char *cdrString = "cdr";
    addBinding(cdrString, primCdr, globalFrame);

    // cons
    Primitive *primCons = talloc(sizeof(Primitive));
    primCons->type = PRIMITIVE_TYPE;
    primCons->pf = primitiveCons;
    char *consString = "cons";
    addBinding(consString, primCons, globalFrame);

    // +
    Primitive *primAdd = talloc(sizeof(Primitive));
    primAdd->type = PRIMITIVE_TYPE;
    primAdd->pf = primitiveAdd;
    char *addString = "+";
    addBinding(addString, primAdd, globalFrame);

    // map
    Primitive *primMap = talloc(sizeof(Primitive));
    primMap->type = PRIMITIVE_TYPE;
    primMap->pf = primitiveMap;
    char *mapString = "map";
    addBinding(mapString, primMap, globalFrame);

    while (tree->type != NULL_TYPE) {
        Object *result = eval(car(tree), globalFrame);
        printObj(result);
        printf("\n");
        tree = cdr(tree);
    }
}