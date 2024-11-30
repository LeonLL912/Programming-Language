


#include "object.h"

#ifndef _TOKENIZER
#define _TOKENIZER

// Return: A cons cell that is the head of a list. The list consists of the 
// tokens read from standard input (stdin).
Object *tokenize();

// Input list: A list of tokens, as returned from the tokenize function.
// Prints the tokens, one per line with type annotation, as exemplified in the 
// assignment.
void displayTokens(Object *list);

#endif


