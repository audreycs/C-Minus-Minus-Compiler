#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include "TypeNode.h"
#include "Tree.h"
#include "SymbolTable.h"

void Program(TreeNode*);
void ExtDefList(TreeNode*);
void ExtDef(TreeNode*);
void ExtDecList(Type*,TreeNode*);
Type* Specifier(TreeNode*);
Type* StructSpecifier(TreeNode*);
char* OptTag(TreeNode*);
char* Tag(TreeNode*);
char* VarDec(Type*,TreeNode*);
void FunDec(Type*,TreeNode*);
FieldList* VarList(TreeNode*);
FieldList* ParamDec(TreeNode*);
void CompSt(Type*,TreeNode*);
void StmtList(Type*,TreeNode*);
void Stmt(Type*,TreeNode*);
FieldList* DefList(TreeNode*);
FieldList* Def(TreeNode*);
FieldList* DecList(Type*,TreeNode*);
char* Dec(Type*,TreeNode*);
Type* Exp(TreeNode*);
FieldList* Args(TreeNode*);

int TypeEqual(Type*, Type*);
int FieldEqual(FieldList*, FieldList*);


int TypeEqual(Type* type1, Type* type2);
int FieldEqual(FieldList* list1, FieldList* list2);
int FuncParamEqual(FieldList* list1, FieldList* list2);
Type* FieldCheck(FieldList*, char*);

#endif
