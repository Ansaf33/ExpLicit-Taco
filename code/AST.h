#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "symbol_table/Gsymbol.h"

struct TreeNode{

  int val;
  char* string;

  int op;
  int type;
  char* varname;

  struct TreeNode* left;
  struct TreeNode* middle;
  struct TreeNode* right;

  struct Gsymbol* Gsymbol;
  
  char* varid;
  char* code;
};

bool typeSatisfied(struct TreeNode* root);

int getVariableId(void);

int getUsedTemps();

struct TreeNode* createNumNode(int val);

struct TreeNode* createOpNode(int type,int op,struct TreeNode* left,struct TreeNode* right);

struct TreeNode* createStringNode(char* string);

struct TreeNode* createIdNode(char* varname);

struct TreeNode* createIfNode(struct TreeNode* middle, struct TreeNode* left,struct TreeNode* right);

struct TreeNode* createWhileNode(int op,struct TreeNode* left, struct TreeNode* right);

void Inorder(struct TreeNode* root);

void boolean_expression_tac(struct TreeNode* root);

void arithmetic_expression_tac(struct TreeNode* root);

void assignment_tac(struct TreeNode* root);

void write_tac(struct TreeNode* root);

void read_tac(struct TreeNode* root);

#endif


