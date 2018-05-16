#ifndef _SYMBOLTABLE_H_
#define _SYMBOLTABLE_H_

#include "TypeNode.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define HASH_SIZE 0x3fff 

typedef struct HashNode_ HashNode;

struct HashNode_
{
	char* symbol_name;
	Type* symbol_type;
};

HashNode* SymbolTable[HASH_SIZE];


void init_symboltable();
unsigned int hash_pjw(char *name);

int SymbolTableCheck(char* varname);
void SymbolTableInsert(char* varname, Type* type);
Type* SymbolTableGetType(char* varname);


#endif
