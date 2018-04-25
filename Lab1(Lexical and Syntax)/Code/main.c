#include <stdio.h>
#include <stdlib.h>
#include "Tree.h"

extern FILE* f;
extern int yylineno;
extern void yyrestart(FILE* );
extern int yyparse(void);
extern int HaveTree;
extern TreeNode* TreeRoot;

int main(int argc, char** argv)
{
	if(argc <= 1)
		return 1;
	
	FILE* f = fopen(argv[1], "r");
	if(!f)
	{
		perror(argv[1]);
		return 1;
	}
	
	yylineno = 1;

	yyrestart(f);

	yyparse();

	if(HaveTree == 1)
		DisplayTree(TreeRoot, 0);
	
	fclose(f);
	return 0;
}
