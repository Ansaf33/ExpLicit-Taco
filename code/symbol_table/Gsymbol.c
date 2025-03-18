#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Gsymbol.h"

struct Gsymbol* head = NULL;
static int address = 4095;


struct Gsymbol* createNode(char* name, int type,int size){
  struct Gsymbol* temp = (struct Gsymbol*)malloc(sizeof(struct Gsymbol));
  temp->name = (char*)malloc(sizeof(char)*100);
  temp->name = strcpy(temp->name,name);
  temp->type = type;
  temp->size = size;
  temp->address = ++address;

  return temp;

}


void addGsymbol(char* name,int type,int size){

  if( !lookUp(name) ){
    struct Gsymbol* temp = createNode(name,type,size);
    temp->next = head;
    head = temp;

  }
  else{
    printf("Variable %s already declared.\n",name);
    exit(1);
  }
  

}

struct Gsymbol* lookUp(char* name){

  struct Gsymbol* current = head;

  while(current != NULL){

    if( strcmp(current->name,name) == 0 ){
      return current;
    }

    current = current->next;


  }

  return NULL;


}


bool checkIfDeclared(char* name){

  struct Gsymbol* current = head;
  while(current!=NULL){
    if( strcmp(current->name,name) == 0 ){
      return true;    
    }
    current = current->next;
  }
  printf("Not declared\n");
  exit(1);
  return false;


}

// PRINTING DETAILS OF ALL GsymbolS IN THE LINKED LIST

void getAll(){
  struct Gsymbol* current = head;
  if( current == NULL ){
    printf("GST is empty.\n");
  }
  while( current != NULL ){
    printf("name : %s | type : %d | size : %d | address : %d\n",current->name,current->type,current->size,current->address);
    current = current->next;
  }

}


void deleteGSymbolTable(){
  struct Gsymbol* cur = head;
  struct Gsymbol* prev = NULL;
  while(cur){
    prev = cur;
    cur = cur->next;
    free(prev);
  }
  head = NULL;
  address = 4095;
}

// ------------------------------------------------------------------------------------------------------------- STORE TEMPS IN SYMBOL TABLE

void storeTempsInGSymbolTable(int id){
  char* v = (char*)malloc(sizeof(char)*3); 
  for(int i=1;i<=id;i++){
    sprintf(v,"t%d",i);
    addGsymbol(v,0,1);
  }
  free(v);
}

struct Gsymbol* getHead(){
  return head;
}


