#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../AST.h"
#include "gentac.h"
#include "../operators/optrans.h"

int blockLabel = 0;

int getBlockLabel(){
  return blockLabel++;
}

// --------------------------------------------------------------------- COPY DECLARATIONS FROM INPUT FILE

void copyDeclarations(FILE* ip,FILE* op){

  int copying = 0;
  char line[256];

  while(fgets(line,sizeof(line),ip)){

    // check for DECL
    if( strstr(line,"decl") ){
      copying = 1;
    }
    // check for ENDDECL
    if( strstr(line,"enddecl\n") ){
      fputs(line,op);
      copying = 0;
    }
    // copy contents
    if( copying ){
      fputs(line,op);
    }

  }

  fputs("\nbegin\n",op);

  fclose(ip);


}
// --------------------------- THREE ADDRESS CODE GEN

void getTac(FILE* f,struct TreeNode* root){
  if( root == NULL ){
    return;
  }

  switch(root->op){
    case 4:
      assignment_print_tac(f,root);
      break;
    case 11:
      read_print_tac(f,root);
      break;
    case 12:
      write_print_tac(f,root);
      break;
    case 13:
      sl_print_tac(f,root);
      break;
    case 14:
      if_print_tac(f,root);
      break;
    case 15:
      while_print_tac(f,root);
      break;

    
  }


 }

// --------------------------------------------------------------------------- GETTING FINAL VALUE

char* getValue(struct TreeNode* root){
  char * num = (char*)malloc(sizeof(char)*10);
  sprintf(num,"%d",root->val);
  return root->varid?root->varid:root->varname?root->varname:num;
}

// ---------------------------------------------------------------- THREE ADDRESS CODE FOR ARITHMETIC EXPRESSIONS

void arithmetic_expression_print_tac(FILE* f,struct TreeNode* root){
  if( root == NULL ){
    return;
  }
  arithmetic_expression_print_tac(f,root->left);
  arithmetic_expression_print_tac(f,root->right);

  if( root->code ) fprintf(f,"%s",root->code);
}

// ----------------------------------------------------------------- THREE ADDRESS CODE FOR BOOLEAN EXPRESSIONS ( EXPRESSION RESULT STORE IN TEMP )

void boolean_expression_evaluate_print_tac(FILE* f,struct TreeNode* root){
      arithmetic_expression_print_tac(f,root->left);
      arithmetic_expression_print_tac(f,root->right);   
}

// --------------------------------------------------------------- THREE ADDRESS CODE FOR BOOLEAN EXPRESSIONS ( COMPARISON )

void boolean_expression_compare_print_tac(FILE* f,struct TreeNode* root){
    fprintf(f,"%s",root->code);
}

// ------------------------------------------------------------------ THREE ADDRESS CODE FOR ASSIGNMENT OPERATIONS

void assignment_print_tac(FILE* f,struct TreeNode* root){
      arithmetic_expression_print_tac(f,root->right);
      fprintf(f,"%s",root->code);
}

// ------------------------------------------------------------------- THREE ADDRESS CODE FOR IF STATEMENTS

void if_print_tac(FILE* f,struct TreeNode* root){

    int thenB = getBlockLabel();
    int elseB = getBlockLabel();
    int endB = getBlockLabel();

    boolean_expression_evaluate_print_tac(f,root->middle);
    fprintf(f,"if ( ");
    boolean_expression_compare_print_tac(f,root->middle);
    fprintf(f," ) goto L%d\n",thenB);
    fprintf(f,"goto L%d\n",elseB);
    fprintf(f,"L%d:\n",thenB);
    getTac(f,root->left);
    fprintf(f,"goto L%d\n",endB);


    if( root->right ){
      fprintf(f,"L%d:\n",elseB);
      getTac(f,root->right);
    }

    fprintf(f,"L%d:\n",endB);
}

// ------------------------------------------------------------------- THREE ADDRESS CODE FOR STATEMENT LISTS

void sl_print_tac(FILE* f,struct TreeNode* root){
      getTac(f,root->left);
      getTac(f,root->right);  
}

// ------------------------------------------------------------------------ THREE ADDRESS CODES FOR WRITE STATEMENTS

void write_print_tac(FILE* f,struct TreeNode* root){
    arithmetic_expression_print_tac(f,root->left);
    fprintf(f,"write(%s);\n",getValue(root->left));
}

// ------------------------------------------------------------------------- THREE ADDRESS CODE FOR READ STATEMENT

void read_print_tac(FILE* f,struct TreeNode* root){
  fprintf(f,"read(%s);\n",getValue(root->left));
}

// ----------------------------------------------------------------------- THREE ADDRESS CODE FOR WHILE STATEMENT

void while_print_tac(FILE* f,struct TreeNode* root){
  int condB = getBlockLabel();
  int bodyB = getBlockLabel();
  int nextB = getBlockLabel();

  boolean_expression_evaluate_print_tac(f,root->left);  
  fprintf(f,"L%d:\n",condB);
  fprintf(f,"if ( ");
  boolean_expression_compare_print_tac(f,root->left);
  fprintf(f," ) goto L%d\n",bodyB);
  fprintf(f,"goto L%d\n",nextB);

  fprintf(f,"L%d:\n",bodyB);
  getTac(f,root->right);
  fprintf(f,"goto L%d\n",condB);

  fprintf(f,"L%d:\n",nextB);
  

  

}
