#ifndef ICG_H
#define ICG_H

#include "ast.h"

// Function to generate intermediate code from AST
void generate_intermediate_code(AST *ast);

// Function to write intermediate code to a file
void write_intermediate_code(AST *ast, const char *filename);

#endif
