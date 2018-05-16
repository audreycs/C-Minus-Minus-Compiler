#include "SymbolTable.h"

void init_symboltable()
{
	int i = 0;
	for (; i < HASH_SIZE; i++)
		SymbolTable[i] = NULL;
}

unsigned int hash_pjw(char *name)
{
	unsigned int val = 0, i;
	for(; *name; ++name)
	{
		val = (val<<2) + (*name);
		if(i = val & ~0x3fff)
			val = (val ^ (i >> 12)) & 0x3fff;
	}
	return val % HASH_SIZE;
}


int SymbolTableCheck(char* varname)
{
	unsigned int pos = hash_pjw(varname);
	if(SymbolTable[pos] == NULL) // varname is not defined
		return 0;
	else // varname is defined
		return 1;
}


void SymbolTableInsert(char* varname, Type* type)
{
	unsigned int pos = hash_pjw(varname);
	HashNode* hashnode = (HashNode*)malloc(sizeof(HashNode));
	char* name = (char*)malloc(sizeof(varname));
	strcpy(name,varname);
	hashnode->symbol_name = name;
	hashnode->symbol_type = type;
	SymbolTable[pos] = hashnode;
}


Type* SymbolTableGetType(char* varname)
{
	unsigned int pos = hash_pjw(varname);
	return SymbolTable[pos]->symbol_type;
}
