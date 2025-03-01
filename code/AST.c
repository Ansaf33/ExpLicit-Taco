
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "AST.h"
#include "operators/optrans.h"
#include "symbol_table/Gsymbol.h"
#include "three_address_code/gentac.h"
#include "map_implementation/map.h"


int varId = 1;
HashMap* hashmap = NULL;
// ------------------------------------------------------------------------------------------------------------ GETTING VARIABLE IDENTIFIER

int getVariableId(){
  return varId++;
}

// -------------------------------------------- INITIALIZE HASHMAP

bool init_hashmap(){
  hashmap = createHashMap();
  return true;
}


// ------------------------------------------------------------------------------------------------------------ NUMBER OF TEMPS ( TO STORE IN GST )

int getUsedTemps(){
  return varId-1;
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

  // ---------------------- CHECK HASHMAP
  char con[20];
  sprintf(con,"%d",val);
  struct TreeNode* cached_val = get(hashmap,con);
  if( cached_val ){
    return cached_val;
  }

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

  // ------------------- THREE ADDRESS CODE
  char* varid = (char*)malloc(sizeof(char)*10);
  int id = getVariableId();
  sprintf(varid,"t%d",id);
  temp->varid = varid;
      
  temp->code = (char*)malloc(sizeof(char)*256);
  sprintf(  
    temp->code,
    "%s = %d;\n",
    getValue(temp),
    val 
  ); 

  // --------------------- HASHMAP
  temp->content = (char*)malloc(sizeof(char)*100);
  strcpy(temp->content,con);
  insert(hashmap,temp->content,temp);



  return temp;
}

// ---------------------------------------------------------------------------------------------------------------- CREATE NODE FOR OPERATORS


struct TreeNode* createOpNode(int type,int op,struct TreeNode* left,struct TreeNode* right){

 
  // --------------- HANDLES MAPPING
  char* content = (char*)malloc(sizeof(char)*100);

  if( op <= 10 && op != 4 ){

    if( left && right ){
      strcat(content,left->content);
      strcat(content,map(op));
      strcat(content,right->content);

    }

    struct TreeNode* cached_val = get(hashmap,content);
    if( cached_val ){
      return cached_val;
    }

  }


  // -------------- CREATES THE NODE

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

  // ---------------- DEALS WITH TAC GENERATION

  switch(op){
    case 0:
    case 1:
    case 2:
    case 3:
      arithmetic_expression_tac(temp);
      break;
      case 4:
      assignment_tac(temp);
      break;
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10: 
      boolean_expression_tac(temp);
      break;
    case 11:
      read_tac(temp);
      break;
    case 12:
      write_tac(temp);
      break;
      
  }

  // -------------- INSERTS CONTENT IN HASHMAP

  temp->content = (char*)malloc(sizeof(char)*100);
  strcpy(temp->content,content);

  if( op <= 10 && op != 4 ){
    insert(hashmap,temp->content,temp);
  }

  
  return temp;
}

// --------------------------------------------------------------------------------------------------------------- CREATE NODE FOR STRINGS


struct TreeNode* createStringNode(char* string){

  // --------------- CHECKS HASHMAP
  struct TreeNode* cached_val = get(hashmap,string);
  if( cached_val ){
    return cached_val;
  }

  // ------------------ CREATES NODE
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

  // -------------- PLACES STRING IN HASHMAP

  temp->content = (char*)malloc(sizeof(char)*100);
  strcpy(temp->content,string);
  insert(hashmap,temp->content,temp);

  return temp;
}



// --------------------------------------------------------------------------------------------------------------- CREATE NODE FOR IDs

struct TreeNode* createIdNode(char* varname){


  // --------------- CHECKS HASHMAP
  struct TreeNode* cached_val = get(hashmap,varname);
  if( cached_val ){
    return cached_val;
  }


  // ---------------- CREATES NODE
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

  // ---------------- PLACED INSIDE HASHMAP

  temp->content = (char*)malloc(sizeof(char)*100);
  strcpy(temp->content,varname);
  insert(hashmap,temp->content,temp);



  return temp;

}

// ------------------------------------------------------------------------------------------------------- CREATE NODE FOR IF STATEMENTS

struct TreeNode* createIfNode(struct TreeNode* middle,struct TreeNode* left,struct TreeNode* right){

  // -------------------- CHECKS HASHMAP

  char* content = (char*)malloc(sizeof(char)*100);
  strcat(content,middle->left->content);
  strcat(content,map(middle->op));
  strcat(content,middle->right->content);

  struct TreeNode* cached_val = get(hashmap,content);

  // ------------------- CREATES NODE

  struct TreeNode* temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));
  temp->val = -1;
  temp->string = NULL;
  temp->op = 14;
  temp->type = -1;
  temp->varname = NULL;
  temp->left = left;
  temp->middle = (cached_val)?cached_val:middle;
  temp->right = right;
  temp->Gsymbol = NULL;

  // CHECK IF SATISFIABLE

  if( middle ){ 
    if(!typeSatisfied(temp)){
      printf("If Condition : Type not matching.\n");
      exit(1);
    }
        
  }

  // ------------------------- INSERTS EXPRESSION INSIDE CACHE

  if( !cached_val ){
    insert(hashmap,temp->content,temp);
  }





  

  return temp;

}

// ------------------------------------------------------------------------------------------------------- CREATE NODE FOR WHILE STATEMENTS

struct TreeNode* createWhileNode(int op,struct TreeNode* left,struct TreeNode* right){

  // -------------------- CHECKS HASHMAP

  char* content = (char*)malloc(sizeof(char)*100);
  strcat(content,left->left->content);
  strcat(content,map(left->op));
  strcat(content,left->right->content);

  struct TreeNode* cached_val = get(hashmap,content);

  // ------------------- CREATES NODE


  struct TreeNode* temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));  
  temp->val = -1;
  temp->string = NULL;
  temp->op = op;
  temp->type = -1;
  temp->varname = NULL;
  temp->left = cached_val?cached_val:left;
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

  if( !cached_val ){
    insert(hashmap,temp->content,temp);
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
    printf("[ %d ] ------ [ %p ]\n",root->val,root);
  }
  // IT IS A STRING
  if(root->string != NULL ){
    printf("[ %s ] ----- [ %p ]\n",root->string,root);
  }
  // IT IS AN OPERATOR
  else if(root->op != -1 ){
    printf(" [ %s ] ------ [ %p ]\n",map(root->op),root);
  }
  // IT IS A VARIABLE
  else if( root->varname != NULL ){
    printf(" [ %s ] ------- [ %p ]",root->varname,root);
  }

  Inorder(root->middle);
  Inorder(root->right);
}

// --------------------------------------------------------------------------------------- CODE FOR ARITHMETIC EXPRESSIONS

void arithmetic_expression_tac(struct TreeNode* temp){

  char* varid = (char*)malloc(sizeof(char)*10);
  int id = getVariableId();
  sprintf(varid,"t%d",id);
  temp->varid = varid;
      
  temp->code = (char*)malloc(sizeof(char)*256);
  sprintf(  
    temp->code,
    "%s = %s %s %s;\n",
    getValue(temp),
    getValue(temp->left),
    map(temp->op),
    getValue(temp->right)
  ); 

}

// --------------------------------------------------------------------------------------- CODE FOR ASSIGNMENTS

void assignment_tac(struct TreeNode* temp){
  temp->code = (char*)malloc(sizeof(char)*256);
  sprintf(
    temp->code,
    "%s = %s;\n",
    getValue(temp->left),
    getValue(temp->right)
  );  
}

// --------------------------------------------------------------------------------------- CODE FOR WRITE STATEMENT


void write_tac(struct TreeNode* temp){
      temp->code = (char*)malloc(sizeof(char)*256);
      sprintf(
        temp->code,
        "write(%s);\n",
        getValue(temp->left)
      );
}

// --------------------------------------------------------------------------------------- CODE FOR BOOLEAN EXPRESSIONS


void boolean_expression_tac(struct TreeNode* temp){
      temp->code = (char*)malloc(sizeof(char)*256);
       sprintf(
        temp->code,
        "%s %s %s",
        getValue(temp->left),
        map(temp->op),
        getValue(temp->right)
      );
}

// --------------------------------------------------------------------------------------- CODE FOR READ STATEMENT

void read_tac(struct TreeNode* temp){
  temp->code = (char*)malloc(sizeof(char)*256);
  sprintf(
    temp->code,
    "read(%s);\n",
    getValue(temp->left)
  );
}

// -------------------------------------------------------------------------------------- CODE FOR WHILE STATEMENT

