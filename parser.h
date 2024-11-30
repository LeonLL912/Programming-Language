


#include "object.h"

#ifndef _PARSER
#define _PARSER

// Input tokens: A linked list of tokens. The output of the tokenize function.
// Return: A linked list that stores the abstract syntax tree (forest, actually) 
// for that token list. If a syntax error is encountered, then an error message 
// is printed and the program cleanly exits.
Object *parse(Object *tokens);

// Input tree: An abstract syntax tree (forest). The output of parse.
// Prints the tree in a human-readable format that closely resembles the 
// original Scheme code that led to the abstract syntax tree.
void printTree(Object *tree);

#endif


