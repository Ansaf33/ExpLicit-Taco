#ifndef INSTRUCTION_SET_H
#define INSTRUCTION_SET_H


#include <stdio.h>
#include <stdbool.h>
#include "../symbol_table/Gsymbol.h"
#include "../reghandling.h"
#include <stdlib.h>

int getReg(void);

void freeReg(void);

void arithmetic_TAC(FILE* f,struct Gsymbol* dest,char* src1,char* op,char* src2);

void write_id_TAC(FILE* f,struct Gsymbol* content);

void read_id_TAC(FILE* f,struct Gsymbol* content);

void id_equals_num_TAC(FILE* f,struct Gsymbol* dest,int num);

void id_equals_id_TAC(FILE* f,struct Gsymbol* dest,char* src);

void if_TAC(FILE* f,struct Gsymbol* id1,char* relop,struct Gsymbol* id2,char* thenLabel);

void jump_TAC(FILE* f,char* label);

void label_TAC(FILE* f,char* label);

bool isnum(char* str);

#endif
