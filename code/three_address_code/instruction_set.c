#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../symbol_table/Gsymbol.h"
#include "../reghandling.h"
#include "instruction_set.h"

// ------------------------------------------------------------------------------------------------------- ARITHMETIC EXPRESSIONS

void arithmetic_TAC(FILE* f,struct Gsymbol* dest,char* src1,char* op,char* src2){
  struct Gsymbol* g1 = lookUp(src1);
  struct Gsymbol* g2 = lookUp(src2);
  if( dest == NULL ){
    printf("Destination entry not found.\n");
    exit(1);
  }
  if( g1 == NULL && !isnum(src1) ){
    printf("Source entry 1 not found\n");
    exit(1);
  }
  if( g2 == NULL && !isnum(src2) ){
    printf("Source entry 2 not found\n");
    exit(1);
  }


  int lreg = getReg();
  int rreg = getReg();
  if( g1 ){
    fprintf(f,"MOV R%d, [%d]\n",lreg,g1->address);
  }
  else{
    fprintf(f,"MOV R%d, %d\n",lreg,atoi(src1));
  }
  if( g2 ){
    fprintf(f,"MOV R%d, [%d]\n",rreg,g2->address);
  }
  else{
    fprintf(f,"MOV R%d, %d\n",rreg,atoi(src2));
  }

  fprintf(f,"%s R%d, R%d\n",op,lreg,rreg);

  int adReg = getReg();
  fprintf(f,"MOV R%d, %d\n",adReg,dest->address);
  fprintf(f,"MOV [R%d], R%d\n",adReg,lreg);

  freeReg();
  freeReg();
  freeReg();
  
}
// ------------------------------------------------------------------------------------------------------- ID = NUM STATEMENT

void id_equals_num_TAC(FILE* f,struct Gsymbol* dest,int num){
  if( dest == NULL ){
    printf("Destination entry not found\n");
    exit(1);
  }

  int a = getReg();
  fprintf(f,"MOV R%d, %d\n",a,dest->address);
  fprintf(f,"MOV [R%d], %d\n",a,num);
  freeReg();
}

// ------------------------------------------------------------------------------------------------------- ID = ID STATEMENT


void id_equals_id_TAC(FILE* f,struct Gsymbol* dest,char* src){
  struct Gsymbol* g = lookUp(src);

  if( dest == NULL ){
    printf("Destination entry not found\n");
    exit(1);
  }
  if( src == NULL && !isnum(src) ){
    printf("Source entry not found\n");
    exit(1);
  }

  int a = getReg();
  if(g){
  fprintf(f,"MOV R%d, [%d]\n",a,g->address);
  }
  else{
  fprintf(f,"MOV R%d, %d\n",a,atoi(src));
  }
  fprintf(f,"MOV [%d], R%d\n",dest->address,a);
  
  freeReg();
}

// ------------------------------------------------------------------------------------------------------- WRITE STATEMENT


void write_id_TAC(FILE* f,struct Gsymbol* content){

  if( content == NULL ){
    printf("Identifier does not exist\n");
    exit(1);
  }

  // stack pointer
  fprintf(f,"MOV SP, %d\n",4500);

  // pushing "write"
  int r1 = getReg();
  fprintf(f,"MOV R%d, \"write\"\n",r1);
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing -2 -> argument 1
  r1 = getReg();
  fprintf(f,"MOV R%d, -2\n",r1);
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing R -> argument 2 (data)
  r1 = getReg();
  fprintf(f,"MOV R%d, [%d]\n",r1,content->address);
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing empty register -> argument 3
  r1 = getReg();
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing register for return value
  r1 = getReg();
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // CALL
  fprintf(f,"CALL 0\n");

  // POPPING REGISTERS
  r1 = getReg();
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  freeReg();

}

// ------------------------------------------------------------------------------------------------------- READ STATEMENT

void read_id_TAC(FILE* f,struct Gsymbol* content){

  if( content == NULL ){
    printf("Identifier does not exist\n");
    exit(1);
  }

  // STACK POINTER
  fprintf(f,"MOV SP, %d\n",4500);

  // pushing "read"
  int r1 = getReg();
  fprintf(f,"MOV R%d, \"read\"\n",r1);
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing -1 -> argument 1
  r1 = getReg();
  fprintf(f,"MOV R%d, -1\n",r1);
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing buffer -> argument 2
  r1 = getReg();
  fprintf(f,"MOV R%d, %d\n",r1,content->address);
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing empty register -> argument 3
  r1 = getReg();
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // pushing register for return value
  r1 = getReg();
  fprintf(f,"PUSH R%d\n",r1);
  freeReg();

  // CALL
  fprintf(f,"CALL 0\n");

  // POPPING REGISTERS
  r1 = getReg();
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  fprintf(f,"POP R%d\n",r1);
  freeReg();

}

// -------------------------------------------------------------------------------------------- JUMP STATEMENT

void jump_TAC(FILE* f,char* label){
  fprintf(f,"JMP %s\n",label);
}

// -------------------------------------------------------------------------------------------- LABEL CALL

void label_TAC(FILE* f,char* label){
  fprintf(f,"%s:\n",label);
}

// ------------------------------------------------------------------------------------------------ IF STATEMENT

void if_TAC(FILE* f,char* id1,char* relop,char* id2,char* thenLabel){

  struct Gsymbol* g1 = lookUp(id1);
  struct Gsymbol* g2 = lookUp(id2);

  if( !g1 && !isnum(id1) ){
    printf("LHS of boolean expression invalid.\n");
    exit(1);
  }
  if( !g2 && !isnum(id2) ){
    printf("RHS of boolean expression invalid.\n");
    exit(1);
  }
  // evaluate boolean expression
  int u = getReg();
  int v = getReg();

  // first entry
  if( g1 ){
    fprintf(f,"MOV R%d, [%d]\n",u,g1->address);
  }
  else{
    fprintf(f,"MOV R%d, %d\n",u,atoi(id1));
  }

  // second entry
  if( g2 ){
    fprintf(f,"MOV R%d, [%d]\n",v,g2->address);
  }
  else{
    fprintf(f,"MOV R%d, %d\n",v,atoi(id2));
  }

  // result stored in u
  fprintf(f,"%s R%d, R%d\n",relop,u,v);

  fprintf(f,"JNZ R%d, %s\n",u,thenLabel);

 }



bool isnum(char* str){
  return str[0] >= 48 && str[0] <= 57;
}
