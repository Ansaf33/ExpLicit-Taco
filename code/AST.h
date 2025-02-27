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

void storeTempsInGSymbolTable(void);

int getVariableId(void);

struct TreeNode* createNumNode(int val);

struct TreeNode* createOpNode(int type,int op,struct TreeNode* left,struct TreeNode* right);

struct TreeNode* createStringNode(char* string);

struct TreeNode* createIdNode(char* varname);

struct TreeNode* createIfNode(struct TreeNode* middle, struct TreeNode* left,struct TreeNode* right);

struct TreeNode* createWhileNode(int op,struct TreeNode* left, struct TreeNode* right);

void Inorder(struct TreeNode* root);

void printTac(struct TreeNode* root);

char* getValue(struct TreeNode* root);

#endif


