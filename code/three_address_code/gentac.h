#ifndef TACGEN_H
#define TACGEN_H

#include <stdio.h>
#include <stdlib.h>
#include "../AST.h"

void copyDeclarations(FILE* ip,FILE* op);

void getTac(FILE* f,struct TreeNode* root);

char* getValue(struct TreeNode* root);

void arithmetic_expression_print_tac(FILE* f,struct TreeNode* root);

void boolean_expression_evaluate_print_tac(FILE* f,struct TreeNode* root);

void boolean_expression_compare_print_tac(FILE* f,struct TreeNode* root);

void assignment_print_tac(FILE* f,struct TreeNode* root);

void if_print_tac(FILE* f,struct TreeNode* root);

void sl_print_tac(FILE* f,struct TreeNode* root);

void write_print_tac(FILE* f,struct TreeNode* root);

void read_print_tac(FILE* f,struct TreeNode* root);

void while_print_tac(FILE* f,struct TreeNode* root);

#endif
