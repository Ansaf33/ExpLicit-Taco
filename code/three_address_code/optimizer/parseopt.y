%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../../symbol_table/Gsymbol.h"
#include "../../symbol_table/varList.h"
#include "../gentac.h"
#include "../instruction_set.h"
#include "../map_implementation/map.h"
#include "../set_implementation/set.h"


SimpleSet set;
HashMap* hashmap = NULL;
struct TreeNode* root;
extern FILE* yyin;
FILE* f;
FILE* o;


int yylex(void);
void yyerror(char* s);
bool init_hashmap(void);
char* combine_three(char* a,char* b,char* c);
bool isId(char* str);
void remove_lhs_id(char* str);
void add_rhs_id(char* str);


%}

%union{
  struct TreeNode* node;
  char* string;
  int integer;
  struct list* list;
}

%type<string>RELOP ARITHOP IDENTIFIER
%type<list>VARLIST
%type<integer> TYPE
%token BEG END
%token READ WRITE
%token NUM ID PLUS MINUS MUL DIV EQUALS 
%token DECL ENDDECL INT STR
%token IF GOTO
%left EQ NEQ
%left GT GTE LT LTE
%left ADD SUB
%left MUL DIV


%%

PROGRAM :
        DECLARATIONS P

DECLARATIONS :
             DECL DL ENDDECL{
                // get the integer from the file and add those temps
                FILE* content = fopen("../../file.txt","r");
                int num = 0;
                fscanf(content,"%d",&num);
                storeTempsInGSymbolTable(num);

               // print the global symbol table
                getAll();
              }
             |
             DECL ENDDECL
             ;

DL :
        DL D
        |
        D
        ;

D :
     TYPE VARLIST ';' {
      addAllSymbols($2,$1,1);
     }
     ;

TYPE :
     INT {
     $$ = $<integer>1;
     }
     |
     STR {
     $$ = $<integer>1;
     }
     ;

VARLIST :
        VARLIST ',' ID {
          $$ = addVariable($1,$<string>3);
        }
        |
        ID {
          $$ = addVariable(NULL,$<string>1);
        }
        ;

P :
  BEG SL END ';' {
  }
  |
  BEG END ';' {
  printf("Valid program.\n");
  }
  ;

SL :
   SL S
   |
   S
   ;

S :
  ID EQUALS IDENTIFIER ARITHOP IDENTIFIER ';' {
    printf("Here\n");
    // combine the rhs expression and get the value
    char* combined = combine_three($<string>3,$<string>4,$<string>5);
    char* value = get(hashmap,combined);

    // if value is not in hashmap
    if( value == NULL ){
      insert(hashmap,combined,$<string>1);
      fprintf(o,"%s = %s %s %s;\n",$<string>1,$<string>3,$<string>4,$<string>5);  
    }

    // if value is in hashmap
    else{
      // if one of them is id and they are not present in set
      if( isId($<string>3) && set_contains(&set,$<string>3) == SET_FALSE || isId($<string>5) && set_contains(&set,$<string>5) == SET_FALSE   ){
        fprintf(o,"%s = %s %s %s;\n",$<string>1,$<string>3,$<string>4,$<string>5);  
      }
      // any other condition, optimization possible
      else{ 
        fprintf(o,"%s = %s;\n",$<string>1,value);
      }
    } 

    // add rhs id to set
    add_rhs_id($<string>3);
    add_rhs_id($<string>5);
    // remove lhs id from set since we modified
    remove_lhs_id($<string>1);
    printf("Done\n");


  }
  |  
  ID EQUALS NUM ';' { 
    fprintf(o,"%s = %s;\n",$<string>1,$<string>3);

    // remove lhs id since we modified
    remove_lhs_id($<string>1);


  }
  |
  ID EQUALS ID ';' {
    fprintf(o,"%s = %s;\n",$<string>1,$<string>3);

    // remove lhs id from set since we modified
    remove_lhs_id($<string>1);
  

  }
  |
  WRITE '(' ID ')' ';' {
    fprintf(o,"write(%s);\n",$<string>3);

  }
  |
  READ '(' ID ')' ';' {
    fprintf(o,"read(%s);\n",$<string>3);

  }
  |
  IF '(' ID RELOP ID ')' GOTO ID {
    fprintf(o,"if ( %s %s %s ) goto %s\n",$<string>3,$<string>4,$<string>5,$<string>8);

  }
  |
  GOTO ID {
    fprintf(o,"goto %s\n",$<string>2);

  }
  |
  ID ':' {
    fprintf(o,"%s:\n",$<string>1);

  }

  ;

RELOP :
       LT {
        $$ = $<string>1;
      }
      |
      LTE {
        $$ = $<string>1;
      }
      |
      GT {
        $$ = $<string>1;  
      }
      |
      GTE {
        $$ = $<string>1;  
      }
      |
      NEQ {
        $$ = $<string>1;
      }
      |
       EQ {
        $$ = $<string>1;      
      }
      ;

ARITHOP : 
        ADD {
          $$ = $<string>1;
        }
        |
        SUB {
          $$ = $<string>1;
        }
        |
        MUL {
          $$ = $<string>1;
        }
        |
        DIV {
          $$ = $<string>1;
        }
        ;

IDENTIFIER : 
          ID {
          $$ = $<string>1;
          }
          |
          NUM {
          $$ = $<string>1;
          }


%%


int main(int argc, char* argv[]){
// ---------------------------------- init hashmap and set
  bool initialized = init_hashmap();
  set_init_alt(&set,20,NULL);

  // -------------------------------- optimized code
  o = fopen(argv[2],"w"); 
  printf("Opened optimized\n");
// --------------------------------- parsing input file
  f = fopen(argv[1],"r");
  printf("Opened input tac file\n");
  yyin = f;
  // ------------------------------- copy declarations
  copyDeclarations(f,o);
  fclose(f);

  f = fopen(argv[1],"r");

  yyparse();
  fprintf(o,"end;\n");

  return 0;
}


void yyerror(char* s){
  printf("ERROR:%s\n",s);
}

// initialize hashmap

bool init_hashmap(){
  hashmap = createHashMap();
  return true;
}

// combine operands and operator into one string


char* combine_three(char* op1,char* op2,char* op3){
  char* fin = (char*)malloc(sizeof(char)*100);
  strcat(fin,op1);
  strcat(fin,op2);
  strcat(fin,op3);
  return fin;
}

// check if it is identifier

bool isId(char* str){
  // number
  if(str[0] >= 48 && str[0] <= 57 ){
    return false;
  }
  // temporary variables (unnecessary because not used again)
  if( str[0] == 't' ){
    return false;
  }
  return true;
}

// remove left hand side id from set

void remove_lhs_id(char* str){
    // remove lhs id from set since we modified
    if( isId(str) && set_contains(&set,str) == SET_TRUE ) {
    set_remove(&set,str);
    }
}


// add right hand side id to set

void add_rhs_id(char* str){
    if( isId(str) ) {
    set_add(&set,str);
    }
  
}
