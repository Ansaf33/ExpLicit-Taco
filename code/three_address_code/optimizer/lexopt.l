%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
#include "../../symbol_table/Gsymbol.h"

%}




%%

[0-9]+ {
  yylval.string = (char*)malloc(sizeof(char)*16);
  strcpy(yylval.string,yytext);
  return NUM;
}

"decl" {
  return DECL;
}

"enddecl" {
  return ENDDECL;
}

"int" {
  yylval.integer = 0;
  return INT;
}

"str" {
  yylval.integer = 2;
  return STR;
}

"begin" {
  return BEG;
}

"end" {
  return END;
}

"read" {
  return READ;
}

"write" {
  return WRITE;
}

"if" {
  return IF;
}

"goto" {
  return GOTO;
}

[<] {
  yylval.string = (char*)malloc(sizeof(char)*3);
  strcpy(yylval.string,"<");
  return LT;
}

[>] {
  yylval.string = (char*)malloc(sizeof(char)*3);
  strcpy(yylval.string,">");
  return GT;
}

"<=" {
  yylval.string = (char*)malloc(sizeof(char)*3);
  strcpy(yylval.string,"<=");
  return LTE;
}

">=" {
  yylval.string = (char*)malloc(sizeof(char)*3);
  strcpy(yylval.string,">="); 
  return GTE;
}

"==" {
  yylval.string = (char*)malloc(sizeof(char)*3);
  strcpy(yylval.string,"==");
  return EQ;
}

"!=" {
  yylval.string = (char*)malloc(sizeof(char)*3);
  strcpy(yylval.string,"!=");
  return NEQ;
}

[+] {
  yylval.string = (char*)malloc(sizeof(char)*3);
  strcpy(yylval.string,"+");
  return ADD;
}

[-] {
  yylval.string = (char*)malloc(sizeof(char)*3);
  strcpy(yylval.string,"-"); 
  return MINUS;
}

[*] {
  yylval.string = (char*)malloc(sizeof(char)*3);
  strcpy(yylval.string,"*");
  return MUL;
}

[/] {
  yylval.string = (char*)malloc(sizeof(char)*3);
  strcpy(yylval.string,"/");
  return DIV;
}

[=] {
  return EQUALS;
}

[a-zA-Z][a-zA-Z0-9]* {
  yylval.string = (char*)malloc(sizeof(char)*100);
  strcpy(yylval.string,yytext);
   return ID;
}

[ \t] ;


[(] {
  return '(';
}
[)] {
  return ')';
}
[;] {
  return ';';
}
. {
  return yytext[0];
}


%%


int yywrap(){
  return 1;
}
