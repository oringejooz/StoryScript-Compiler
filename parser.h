#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

AST *parse();
void parse_statement();
ASTNode *parse_choice();

#endif