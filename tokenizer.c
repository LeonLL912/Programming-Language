// tokenizer.c by Leon Liang

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "object.h"
#include "talloc.h"
#include "linkedlist.h"
#include "tokenizer.h"

// Helper function
// Return: A newly allocated Object of INT_TYPE.
Integer *makeIntToken(){
    Integer *intObj = (Integer *)talloc(sizeof(Integer));
    intObj->type = INT_TYPE;
    return (Integer *)intObj;
}

// Helper function
// Return: A newly allocated Object of DOUBLE_TYPE.
Double *makeDoubleToken(){
    Double *doubleObj = (Double *)talloc(sizeof(Double));
    doubleObj->type = DOUBLE_TYPE;
    return (Double *)doubleObj;
}

// Helper function
// Return: A newly allocated Object of STR_TYPE.
String *makeStringToken(){
    String *strObj = (String *)talloc(sizeof(String));
    strObj->type = STR_TYPE;
    return (String *)strObj;
}

// Helper function
// Return: A newly allocated Object of BOOL_TYPE.
Boolean *makeBooleanToken(){
    Boolean *boolObj = (Boolean *)talloc(sizeof(Boolean));
    boolObj->type = BOOL_TYPE;
    return (Boolean *)boolObj;
}

// Helper function
// Return: A newly allocated Object of SYMBOL_TYPE.
Symbol *makeSymbolToken(){
    Symbol *symbolObj = (Symbol *)talloc(sizeof(Symbol));
    symbolObj->type = SYMBOL_TYPE;
    return (Symbol *)symbolObj;
}

// Helper function
// Return: A newly allocated Object of OPEN_TYPE.
Object *makeOpenToken(){
    Object *openObj = (Object *)talloc(sizeof(Object));
    openObj->type = OPEN_TYPE;
    return (Object *)openObj;
}

// Helper function
// Return: A newly allocated Object of CLOSE_TYPE.
Object *makeCloseToken(){
    Object *closeObj = (Object *)talloc(sizeof(Object));
    closeObj->type = CLOSE_TYPE;
    return (Object *)closeObj;
}

// Helper function
// Return: A newly allocated Object of CLOSEBRACE_TYPE.
Object *makeCloseBraceToken(){
    Object *closeBraceObj = (Object *)talloc(sizeof(Object));
    closeBraceObj->type = CLOSEBRACE_TYPE;
    return (Object *)closeBraceObj;
}

// Helper function
// Return: A newly allocated Object of CONS_TYPE.
ConsCell *makeConsCellToken(){
    ConsCell *conObj = (ConsCell *)talloc(sizeof(ConsCell));
    conObj->type = CONS_TYPE;
    return (ConsCell *)conObj;
}



// Return: A cons cell that is the head of a list. The list consists of the 
// tokens read from standard input (stdin).
Object *tokenize(){
    int ch;                       // int, not char; see fgetc documentation
    char buffer[300 + 1];         // based on 300-char limit plus terminating \0
    int index = 0;                // where in buffer to place the next char read
    objectType type = NULL_TYPE;  // type of token being built in buffer
    Object *list = makeNull();

    ch = fgetc(stdin);
    while (ch != EOF) {

        // Skip whitespace
        if (isspace(ch)) {
            ch = fgetc(stdin);
        }

        // Handle open parenthesis
        else if (ch == '(') {
            Object *openToken = makeOpenToken();
            list = cons(openToken, list);
            ch = fgetc(stdin);
        }

        // Handle close parenthesis
        else if (ch == ')') {
            Object *closeToken = makeCloseToken();
            list = cons(closeToken, list);
            ch = fgetc(stdin);
        }

        // Handle close brace
        else if (ch == '}') {
            Object *closeBraceToken = makeCloseBraceToken();
            list = cons(closeBraceToken, list);
            ch = fgetc(stdin);
        }

        // Handle boolean
        else if (ch == '#') {
            ch = fgetc(stdin);
            if (ch == 't'){
                Boolean *booleanToken = makeBooleanToken();
                booleanToken->value = 1;
                list = cons((Object *)booleanToken, list);
                ch = fgetc(stdin);
            }
            else if (ch == 'f'){
                Boolean *booleanToken = makeBooleanToken();
                booleanToken->value = 0;
                list = cons((Object *)booleanToken, list);
                ch = fgetc(stdin);
            }
            else {
                printf("Syntax error\n");
                texit(1);
            }
        }

        // Handle integer or double that start with a digit
        else if (isdigit(ch)) {
            // Collect integer or double token
            index = 0;
            buffer[index++] = ch;
            ch = fgetc(stdin);
            type = 0; //0 if integer, 1 if double

            // Read in digits or decimal points
            while (isdigit(ch) || ch == '.') {
                if (ch == '.'){
                    type = 1;
                }
                buffer[index++] = ch;
                ch = fgetc(stdin);
            }
            buffer[index] = '\0';

            
            if (type == 1) {
                // It's a double
                double dvalue = atof(buffer);
                Double *doubleToken = makeDoubleToken();
                doubleToken->value = dvalue;
                list = cons((Object *)doubleToken, list);
            } 
            else if (type == 0){
                // It's an integer
                int ivalue = atoi(buffer);
                Integer *intToken = makeIntToken();
                intToken->value = ivalue;
                list = cons((Object *)intToken, list);
            }
            else {
                printf("Syntax error\n");
                texit(1);
            }
        }

        // Handle double starts with '.'
        else if (ch == '.') {
            // Collect integer or double token
            index = 0;
            buffer[index++] = ch;
            ch = fgetc(stdin);

            if (isdigit(ch) == false) {
                printf("Syntax error\n");
                texit(1);
            }

            // Read in digits or decimal points
            while (isdigit(ch)) {
                buffer[index++] = ch;
                ch = fgetc(stdin);
            }
            buffer[index] = '\0';
            
            double dvalue = atof(buffer);
            Double *doubleToken = makeDoubleToken();
            doubleToken->value = dvalue;
            list = cons((Object *)doubleToken, list);
           
        }

        // Handle integers, doubles, or possible symbol that start with "+" or "-"
        else if (ch == '-' || ch == '+') {
            // Collect integer or double token
            index = 0;
            buffer[index++] = ch;
            ch = fgetc(stdin);
            type = 0; //0 if integer, 1 if double

            if (isspace(ch) || ch == ')' || ch == '}'){
                buffer[index++] = '\0';
                Symbol *symbolToken = makeSymbolToken();
                symbolToken->value = (char *)talloc(strlen(buffer) + 1);
                strcpy(symbolToken->value,buffer);
                list = cons((Object *)symbolToken, list);
            }
            else if (isdigit(ch)){
                // Read in digits or decimal points
                while (isdigit(ch) || ch == '.') {
                    if (ch == '.'){
                        type = 1;
                    }
                    buffer[index++] = ch;
                    ch = fgetc(stdin);
                }
                buffer[index] = '\0';

                
                if (type == 1) {
                    // It's a double
                    double dvalue = atof(buffer);
                    Double *doubleToken = makeDoubleToken();
                    doubleToken->value = dvalue;
                    list = cons((Object *)doubleToken, list);
                } 
                else if (type == 0){
                    // It's an integer
                    int ivalue = atoi(buffer);
                    Integer *intToken = makeIntToken();
                    intToken->value = ivalue;
                    list = cons((Object *)intToken, list);
                }
                else {
                    printf("Syntax error\n");
                    texit(1);
                }
            }
            else{
                printf("Syntax error\n");
                texit(1);
            }

            
        }

        // Handle strings
        else if (ch == '"') {
            index = 0;
            ch = fgetc(stdin); 

            while (ch != '"' && ch != EOF) {
                buffer[index++] = ch;
                ch = fgetc(stdin);
            }
            buffer[index] = '\0'; 

            if (ch == '"') {
                String *strToken = makeStringToken();
                strToken->value = (char *)talloc(strlen(buffer) + 1);
                strcpy(strToken->value,buffer);
                list = cons((Object *)strToken, list);
                ch = fgetc(stdin);
            } else {
                printf("Syntax error: Unterminated string\n");
                texit(1);
            }
        }

        // Handle comments
        else if (ch == ';') {
            // Skip until newline
            while (ch != '\n' && ch != EOF) {
                ch = fgetc(stdin);
            }
        }

        // Handle symbols (can start with letters or special characters)
        else if (isalpha(ch) || strchr("!$%&*/:<=>?~_^", ch)) {
            index = 0;
            buffer[index++] = ch;
            ch = fgetc(stdin);

            while (isalnum(ch)|| strchr("!$%&*/:<=>?~_^+-", ch)) {
                buffer[index++] = ch;
                ch = fgetc(stdin);
            }
            buffer[index] = '\0'; 

            Symbol *symbolToken = makeSymbolToken();
            symbolToken->value = (char *)talloc(strlen(buffer) + 1);
            strcpy(symbolToken->value,buffer);
            list = cons((Object *)symbolToken, list);
        }

        // Handle syntax error
        else {
            printf("Syntax error: Unrecognized character '%c'\n", ch);
            texit(1);
        }
    }

    return reverse(list);
}

// Input list: A list of tokens, as returned from the tokenize function.
// Prints the tokens, one per line with type annotation, as exemplified in the 
// assignment.
void displayTokens(Object *list){
    while (list->type != NULL_TYPE) {
        Object *token = car(list);

        if (token->type == INT_TYPE) {
            Integer *intToken = (Integer *)token;
            printf("%d:integer\n", intToken->value);
        } 
        else if (token->type == DOUBLE_TYPE) {
            Double *doubleToken = (Double *)token;
            printf("%f:double\n", doubleToken->value);
        } 
        else if (token->type == STR_TYPE) {
            String *strToken = (String *)token;
            printf("\"%s\":string\n", strToken->value);
        } 
        else if (token->type == SYMBOL_TYPE) {
            Symbol *symbolToken = (Symbol *)token;
            printf("%s:symbol\n", symbolToken->value);
        } 
        else if (token->type == BOOL_TYPE) {
            Boolean *boolToken = (Boolean *)token;
            if (boolToken->value == 0){
                printf("#f:boolean\n");
            }
            else if (boolToken->value == 1){
                printf("#t:boolean\n");
            }
        } 
        else if (token->type == OPEN_TYPE) {
            printf("(:open\n");
        } 
        else if (token->type == CLOSE_TYPE) {
            printf("):close\n");
        } 
        else if (token->type == CLOSEBRACE_TYPE) {
            printf("}:closebrace\n");
        } 

        list = cdr(list);
    }
    printf("\n");
}