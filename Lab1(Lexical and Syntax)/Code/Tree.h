
#ifndef _TREE_H_
#define _TREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

typedef enum { lex_unit = 1, syn_unit } UnitType;

typedef enum { S_empty = 1, S_Program, S_ExtDefList, S_ExtDef, S_ExtDecList,
			   S_Specifier, S_StructSpecifier, S_FunDec, S_CompSt, S_VarDec, S_VarList,
			   S_OptTag, S_Def, S_DefList, S_Tag, S_Dec, S_DecList, S_ParamDec,
			   S_StmtList, S_Stmt, S_Exp, S_Args } Syn_Unit_Cont;

typedef enum { L_ID = 1, L_TYPE, L_INT, L_FLOAT, L_ELSE } Lex_Unit_Cont;

/* TreeNode Structure*/
typedef struct TreeNode{
	UnitType u_type;
	Syn_Unit_Cont s_cont;
	Lex_Unit_Cont l_cont;
	int childnum;
	struct TreeNode **child;
	union{
		char* syn_name;
		char* id_name;
		char* type_name;
		int int_value;
		float float_value;
		char* else_name;
	};
	int lineno;
}TreeNode;


TreeNode* CreateTreeNode(Lex_Unit_Cont l_cont, char* value, int lineno);

TreeNode* GenerateTreeNode(Syn_Unit_Cont s_cont, char* syn_name, 
		                    int lineno, int childnum, ...);

void PrintBlank(int deepth);

void DisplayTree(TreeNode* Head, int curdeepth); 

int FreeTree(TreeNode* head);

void shownode(TreeNode* node);


#endif
