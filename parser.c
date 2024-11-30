//parser.c by Leon Liang

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "object.h"
#include "talloc.h"
#include "linkedlist.h"
#include "tokenizer.h"
#include "parser.h"

// Input tokens: A linked list of tokens. The output of the tokenize function.
// Return: A linked list that stores the abstract syntax tree (forest, actually) 
// for that token list. If a syntax error is encountered, then an error message 
// is printed and the program cleanly exits.
Object *parse(Object *tokens){
    Object *stack = makeNull();
    Object *currentToken = tokens;
    int numOpen = 0;
    int numClose = 0;

    while (currentToken->type != NULL_TYPE){
        Object *token = car(currentToken);
        currentToken = cdr(currentToken);
        if (token->type == OPEN_TYPE){
            numOpen++;
        }

        if (token->type == CLOSE_TYPE){
            numClose++;
            if (numClose > numOpen){
                printf("Syntax error: too many close parentheses\n");
                texit(1);
            }

            // Start building a list until an open paren
            Object *list = makeNull();
            // Pop tokens off the stack and append them to a list
            while (car(stack)->type != OPEN_TYPE){
                list = cons(car(stack),list);
                stack = cdr(stack);
            }
            stack = cdr(stack);  // Discard the open paren
            stack = cons(list,stack);
        } 
        else if (token->type == CLOSEBRACE_TYPE){
            if (numClose >= numOpen){
                printf("Syntax error: too many close parens\n");
                texit(1);
            }
            if (currentToken->type != NULL_TYPE){
                if (car(currentToken)->type != OPEN_TYPE){
                    printf("Syntax error: wrong close brace usage\n");
                    texit(1);
                }
            }
            for (int i = numClose; i < numOpen; i++) {
                numClose++;
                // Start building a list until an open paren
                Object *list = makeNull();
                // Pop tokens off the stack and append them to a list
                while (car(stack)->type != OPEN_TYPE){
                    list = cons(car(stack),list);
                    stack = cdr(stack);
                }
                stack = cdr(stack);  // Discard the open paren
                stack = cons(list,stack);
            }
        }
        else{
            stack = cons(token, stack);  // Push the token onto the stack if it's not ')' or '}'
        }
    }
    if (numClose < numOpen){
        printf("Syntax error: not enough close parentheses\n");
        texit(1);
    }

    return reverse(stack);
}

// Helper function to print an token
void printObject(Object *obj){
    if (obj->type == INT_TYPE){
        printf("%d", ((Integer *)obj)->value);
    } 
    else if (obj->type == DOUBLE_TYPE){
        printf("%f", ((Double *)obj)->value);
    } 
    else if (obj->type == STR_TYPE){
        printf("\"%s\"", ((String *)obj)->value);
    } 
    else if (obj->type == SYMBOL_TYPE){
        printf("%s", ((Symbol *)obj)->value);
    } 
    else if (obj->type == BOOL_TYPE){
        Boolean *boolToken = (Boolean *)obj;
        if (boolToken->value == 0){
            printf("#f");
        }
        else if (boolToken->value == 1){
            printf("#t");
        }
    } 
    else if (obj->type == CONS_TYPE){
        printTree(obj);
    } 
    else if (obj->type == NULL_TYPE){
        printf("()");
    }
}

// Input tree: An abstract syntax tree (forest). The output of parse.
// Prints the tree in a human-readable format that closely resembles the 
// original Scheme code that led to the abstract syntax tree.
void printTree(Object *tree) {
    Object *current = tree;
    
    while (current->type != NULL_TYPE) {
        Object *item = car(current);

        if (item->type == CONS_TYPE) {
            printf("(");
            printTree(item);
            printf(")");
        } else {
            printObject(item); 
        }

        current = cdr(current);
        
        if (current->type != NULL_TYPE) {
            printf(" ");
        }
    }
}