#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../AST.h"
//#include "../parser.y"
#include "../operators.optrans.h"
#include "../symbol_table/Gsymbol.h"

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
    if( strstr(line,"enddecl") ){
      fputs(line,op);
      copying = 0;
    }
    // copy contents
    if( copying ){
      fputs(line,op);
    }

  }

  fputs("begin",op);

  fclose(ip);
  fclose(op);

}

