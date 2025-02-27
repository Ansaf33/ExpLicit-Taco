
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "AST.h"
#include "operators/optrans.h"
#include "symbol_table/Gsymbol.h"



int varId = 1;
// ------------------------------------------------------------------------------------------------------------ GETTING VARIABLE IDENTIFIER

int getVariableId(){
  return varId++;
}

// ------------------------------------------------------------------------------------------------------------- STORE TEMPS IN SYMBOL TABLE

void storeTempsInGSymbolTable(){
  char* v = (char*)malloc(sizeof(char)*3); 
  for(int i=1;i<varId;i++){
    sprintf(v,"t%d",i);
    addGsymbol(v,0,1);
  }
  free(v);
}

// --------------------------------------------------------------------------------------------------------------- CHECK IF TYPE IS SAME

bool typeSatisfied(struct TreeNode* root){
    
    // if arithmetic operator, left and right should be integers
    if(root->op >=0 && root->op <= 3){
      return root->left->type == 0 && root->right->type == 0;
    }
    // if assigns, left should be an identifier (integer) and right should be integer
    else if( root->op == 4 ){
       return root->left->type == 0 && root->right->type == 0 || root->left->type == 2 && root->right->type == 2;
    }
    // if logical operators, left and right type should be the type for expressions (integer)
    else if( root->op >= 5 && root->op <= 10 ){
          return root->left->type == 0 && root->right->type == 0;
   }
    // if READ statement, left type should be integer/string
    else if( root->op == 11 ){
        return root->left->type == 0 || root->left->type == 2;
   }
   // if WRITE statement, left type should be integer/string
    else if( root->op == 12 ){
        return root->left->type == 0 || root->left->type == 2;
   }
    // if IF statement, condition type should be boolean
    else if( root->op == 14 ){
        return root->middle->type == 1;
    }
    // if WHILE statement, condition type should be boolean
    else if( root->op == 15 ){
        return root->left->type == 1;
    }
    // if REPEAT statement, condition type should be boolean
    else if( root->op == 18 ){
        return root->left->type == 1;
    }
    // if DOWHILE statement, condition type should be boolean
    else if( root->op == 19 ){
        return root->left->type == 1;
    }

   return true;

}

// ------------------------------------------------------------------------------------------------------------------ CREATE NODE FOR NUMBERS

struct TreeNode* createNumNode(int val){


  struct TreeNode* temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));
  temp->val = val;
  temp->string = NULL;
  temp->op = -1;
  temp->type = 0;
  temp->varname = NULL;
  temp->left = NULL;
  temp->right = NULL;
  temp->middle = NULL;
  temp->Gsymbol = NULL;



  return temp;
}

// ---------------------------------------------------------------------------------------------------------------- CREATE NODE FOR OPERATORS


struct TreeNode* createOpNode(int type,int op,struct TreeNode* left,struct TreeNode* right){



  struct TreeNode* temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));
  temp->val = -1;
  temp->string = NULL;
  temp->op = op;
  temp->type = type;
  temp->varname = "\0";
  temp->left = left;
  temp->right = right;
  temp->middle = NULL;
  temp->Gsymbol = NULL;

  if( left  ){ 
    if(!typeSatisfied(temp)){
      printf("Operator Condition : Type not matching.\n");
      exit(1);
    }
  }

  // three address code for arithmetic and boolean operators
  if( op <= 10 && op != 4 ){
    char* varid = (char*)malloc(sizeof(char)*10);
    int id = getVariableId();
    sprintf(varid,"t%d",id);
    temp->varid = varid;
    
    temp->code = (char*)malloc(sizeof(char)*256);


    // convert the numbers to string for compatibility
    char leftnum[10];
    char rightnum[10];
    sprintf(leftnum,"%d",temp->left->val);
    sprintf(rightnum,"%d",temp->right->val);
    

    sprintf(  
            temp->code,
            "%s = %s %s %s\n",
            temp->varid,
            temp->left->varid ? temp->left->varid : temp->left->varname ? temp->left->varname : leftnum,
            map(op),
            temp->right->varid ? temp->right->varid : temp->right->varname ? temp->right->varname : rightnum
            );
  }

  // three address code for assignment operator
  if( op == 4 ){
    temp->code = (char*)malloc(sizeof(char)*256);
    sprintf(
      temp->code,
      "%s = %s\n",
      temp->left->varname,
      temp->right->varid ? temp->right->varid : temp->right->varname
    );

  }

  
  return temp;
}

// --------------------------------------------------------------------------------------------------------------- CREATE NODE FOR STRINGS


struct TreeNode* createStringNode(char* string){
  struct TreeNode* temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));
  temp->val = -1;
  temp->string = (char*)malloc(sizeof(char)*100);
  strcpy(temp->string,string);
  temp->op = -1;
  temp->type = 2;
  temp->varname = NULL;
  temp->left = NULL;
  temp->right = NULL;
  temp->middle = NULL;
  temp->Gsymbol = NULL;

  return temp;
}



// --------------------------------------------------------------------------------------------------------------- CREATE NODE FOR IDs

struct TreeNode* createIdNode(char* varname){

  struct TreeNode* temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));
  temp->Gsymbol = lookUp(varname);

  if(temp->Gsymbol == NULL){
    printf("Cannot declare variables outside declaration scope\n");
    exit(1);
  }
 
  temp->val = -1;
  temp->string = NULL;
  temp->op = -1;
  temp->type = temp->Gsymbol->type;
  temp->varname = (char*)malloc(sizeof(char)*100);
  strcpy(temp->varname,varname);
  temp->left = NULL;
  temp->right = NULL;
  temp->middle = NULL;



  return temp;

}

// ------------------------------------------------------------------------------------------------------- CREATE NODE FOR IF STATEMENTS

struct TreeNode* createIfNode(struct TreeNode* middle,struct TreeNode* left,struct TreeNode* right){

  struct TreeNode* temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));
  temp->val = -1;
  temp->string = NULL;
  temp->op = 14;
  temp->type = -1;
  temp->varname = NULL;
  temp->left = left;
  temp->middle = middle;
  temp->right = right;
  temp->Gsymbol = NULL;

  // CHECK IF SATISFIABLE

  if( middle ){ 
    if(!typeSatisfied(temp)){
      printf("If Condition : Type not matching.\n");
      exit(1);
    }
        
  }


  

  return temp;

}

// ------------------------------------------------------------------------------------------------------- CREATE NODE FOR WHILE STATEMENTS

struct TreeNode* createWhileNode(int op,struct TreeNode* left,struct TreeNode* right){
  struct TreeNode* temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));  
  temp->val = -1;
  temp->string = NULL;
  temp->op = op;
  temp->type = -1;
  temp->varname = NULL;
  temp->left = left;
  temp->right = right;
  temp->middle = NULL;
  temp->Gsymbol = NULL;

  // CHECK IF SATISFIABLE

  if( left && right ){ 
    if(!typeSatisfied(temp)){
      printf("While Condition : Type not matching.\n");
      exit(1);
    }
  }
  

  return temp;


}


void Inorder(struct TreeNode* root){
  if(root == NULL){
    return;
  }
  Inorder(root->left);
  // IT IS A NUMBER
  if(root->val != -1 ){
    printf(" ( %d )",root->val);
  }
  // IT IS A STRING
  if(root->string != NULL ){
    printf(" ( %s )",root->string);
  }
  // IT IS AN OPERATOR
  else if(root->op != -1 ){
    printf(" ( %s )",map(root->op));
  }
  // IT IS A VARIABLE
  else if( root->varname != NULL ){
    printf(" ( %s )",root->varname);
  }

  Inorder(root->middle);
  Inorder(root->right);
}

// --------------------------- THREE ADDRESS CODE GEN

void printTac(struct TreeNode* root){
  if( root == NULL ){
    return;
  }
  printTac(root->left);
  printTac(root->right);

  if( root->op <= 10 && root->code ){
    printf("%s",root->code);
  }

  if( root->op == 12 ){
    printf("write(%s)\n",getValue(root->left));
  }

 }


char* getValue(struct TreeNode* root){
  char num[10];
  sprintf(num,"%d",root->val);
  return root->varid?root->varid:root->varname?root->varname:num;
}
