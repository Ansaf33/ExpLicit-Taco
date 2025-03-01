%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../symbol_table/Gsymbol.h"
#include "../symbol_table/varList.h"
#include "gentac.h"
#include "instruction_set.h"


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

%type<string>RELOP
%type<list>VARLIST
%type<integer> TYPE
%token BEG END
%token READ WRITE
%token NUM ID PLUS MINUS MUL DIV EQUALS 
%token DECL ENDDECL INT STR
%token IF GOTO
%left EQ NEQ
%left GT GTE LT LTE
%left PLUS MINUS
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
                printf("Number of temporaries : %d\n",num);
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
  ID EQUALS ID PLUS ID ';' {
    arithmetic_TAC(xsm,lookUp($<string>1),lookUp($<string>3),"ADD",lookUp($<string>5));
  }
  |
  ID EQUALS ID MINUS ID ';' {
    arithmetic_TAC(xsm,lookUp($<string>1),lookUp($<string>3),"SUB",lookUp($<string>5));
  }
  |
  ID EQUALS ID MUL ID ';' {
    arithmetic_TAC(xsm,lookUp($<string>1),lookUp($<string>3),"MUL",lookUp($<string>5));
  }
  |
  ID EQUALS ID DIV ID ';' {
    arithmetic_TAC(xsm,lookUp($<string>1),lookUp($<string>3),"DIV",lookUp($<string>5));
  }
  |  
  ID EQUALS NUM ';' { 
    id_equals_num_TAC(xsm,lookUp($<string>1),atoi($<string>3));
  }
  |
  ID EQUALS ID ';' {
    id_equals_id_TAC(xsm,lookUp($<string>1),lookUp($<string>3));
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
  



        



%%


int main(int argc, char* argv[]){

// -------------------------------- INITIAL ASSEMBLY CODE
  xsm = fopen(argv[2],"w");
  fprintf(xsm,"%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",0,2056,0,0,0,0,0,0);
  fprintf(xsm,"BRKP\n");

// --------------------------------- PARSING INPUT FILE
  f = fopen(argv[1],"r");
  yyin = f;

  yyparse();

  fprintf(xsm,"INT 10\n");

  return 0;
}

void yyerror(char* s){
  printf("ERROR:%s\n",s);
}

