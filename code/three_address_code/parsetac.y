%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../symbol_table/Gsymbol.h"
#include "../symbol_table/varList.h"
#include "gentac.h"
#include "instruction_set.h"
#include "map_implementation/map.h"
#include "set_implementation/set.h"


SimpleSet set;
HashMap* hashmap = NULL;
struct TreeNode* root;
extern FILE* yyin;
FILE* f;
FILE* xsm;

int yylex(void);
void yyerror(char* s);


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
                FILE* content = fopen("../file.txt","r");
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
    arithmetic_TAC(xsm,lookUp($<string>1),$<string>3,$<string>4,$<string>5);
  }
  |  
  ID EQUALS NUM ';' { 
    id_equals_num_TAC(xsm,lookUp($<string>1),atoi($<string>3));
  }
  |
  ID EQUALS ID ';' {
  
    id_equals_id_TAC(xsm,lookUp($<string>1),$<string>3);
  }
  |
  WRITE '(' ID ')' ';' {
   write_id_TAC(xsm,lookUp($<string>3));
  }
  |
  READ '(' ID ')' ';' {
    read_id_TAC(xsm,lookUp($<string>3));
  }
  |
  IF '(' ID RELOP ID ')' GOTO ID {
    if_TAC(xsm,lookUp($<string>3),$<string>4,lookUp($<string>5),$<string>8); 
  }
  |
  GOTO ID {
    jump_TAC(xsm,$<string>2);
  }
  |
  ID ':' {
    label_TAC(xsm,$<string>1);
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

// -------------------------------- initial assembly code
  xsm = fopen(argv[2],"w");
  fprintf(xsm,"%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",0,2056,0,0,0,0,0,0);
  fprintf(xsm,"BRKP\n");
// --------------------------------- parsing input file
  f = fopen(argv[1],"r");
  yyin = f;
  yyparse();
  fprintf(xsm,"INT 10\n");

  return 0;
}

void yyerror(char* s){
  printf("ERROR:%s\n",s);
}



