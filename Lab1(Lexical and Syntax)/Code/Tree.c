#include "Tree.h"
#define SHOW printf("here\n");

TreeNode* CreateTreeNode(Lex_Unit_Cont l_cont, char* value, int lineno)
{
	TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
	node->u_type = lex_unit;
	node->s_cont = 0;
	node->l_cont = l_cont;
	node->childnum = 0;
	node->child = NULL;
	node->lineno = lineno;
	switch(node->l_cont)
	{
		case L_ID: {node->id_name = (char*)malloc(strlen(value));
					strcpy(node->id_name, value);
					break; }
		case L_TYPE: {node->type_name = (char*)malloc(strlen(value));
					  strcpy(node->type_name, value);
					  break;}
		case L_INT: { node->int_value = atoi(value); break;}
		case L_FLOAT: { node->float_value = atof(value); break;}
		case L_ELSE: { node->else_name = (char*)malloc(strlen(value));
					   strcpy(node->else_name, value);
					   break;}
	}


	return node;
}


TreeNode* GenerateTreeNode(Syn_Unit_Cont s_cont, char* syn_name, 
							int lineno, int childnum, ...)
{
	TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
	
	node->u_type = syn_unit; 
	node->s_cont = s_cont;
	node->l_cont = 0;
	node->childnum = childnum; 
	node->lineno = lineno;
	va_list args;
	va_start(args, childnum);
	node->child = (TreeNode **)malloc(sizeof(TreeNode*)*abs(childnum));
	int count = 0; 
	for(; count < abs(childnum); count++)
	{
		TreeNode* temp = va_arg(args, TreeNode*);
		node->child[count] = temp;
		
	}
	node->syn_name = syn_name;

	return node; 
}

void DisplayTree(TreeNode* Head, int curdeepth)
{
	if(Head->childnum < 0)
		return;
	PrintBlank(curdeepth);
	if(Head->u_type == syn_unit)
		printf("%s (%d)\n", Head->syn_name, Head->lineno);
	else
	{
		if(Head->l_cont == L_ID)
			printf("ID: %s\n", Head->id_name);
		else if(Head->l_cont == L_TYPE)
			printf("TYPE: %s\n", Head->type_name);
		else if(Head->l_cont == L_INT)
			printf("INT: %d\n", Head->int_value); 
		else if(Head->l_cont == L_FLOAT)
			printf("FLOAT: %lf\n",Head->float_value);
		else
			printf("%s\n",Head->else_name);
	}

	if(Head->childnum > 0 && Head->child[0] != NULL)
	{
		int i;
		for(i=0; i < Head->childnum ; i++)
		{
			DisplayTree(Head->child[i], curdeepth+1);
		}
	}
}

void PrintBlank(int deepth)
{
	int temp = 0;
	for(; temp < deepth; temp++)
		printf("  ");
}


void shownode(TreeNode* node)
{
	printf("%d\n", node->u_type);
	printf("%d\n", node->s_cont);
	printf("%d\n", node->l_cont);
	printf("%d\n", node->lineno);
}
