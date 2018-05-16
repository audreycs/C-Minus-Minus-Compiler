/*
 *
 */


%{
	#include "syntax.tab.h"
	#include "Tree.h"

	int yycolumn = 1;
	#define YY_USER_ACTION \
	yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn; \
	yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng; \
	
	#define PYYT printf("");
	#define YYDEBUG 1
//	yydebug = 1;

	extern int HaveTree;
%}

%option yylineno

digit     [0-9]
INT       ([1-9]{digit}*)|0
FLOAT     {INT}{DOT}{digit}+

letter_   [a-zA-Z_]
ID        {letter_}({letter_}|{digit})*

SEMI      ;
COMMA     ,
ASSIGNOP  =
RELOP     >|<|>=|<=|==|!=
PLUS      \+
MINUS     -
STAR      \*
DIV       \/
AND       &&
OR        \|\|
DOT       \.
NOT       !
TYPE      int|float
LP        \(
RP        \)
LB        \[
RB        \]
LC        \{
RC        \}
DELIM     [ \t\r]+
NEXTLINE  [\n]
STRUCT    struct
RETURN    return
IF        if
ELSE      else
WHILE     while

NoteForm1 [/][/][^\n]*
Form1     \/\*
Form2     \*\/
NoteForm2 {Form1}(([^\*\n]+(\/))|[^\*\/]|(\/\*)|(\*))*{Form2}
NOTE      {NoteForm1}|{NoteForm2}
ERRORID   {digit}+{ID}


%%

{INT}		{ PYYT
			yylval.tnode = CreateTreeNode(L_INT, yytext, yylineno);
			return INT;
			}
{FLOAT}     { PYYT
			yylval.tnode = CreateTreeNode(L_FLOAT, yytext, yylineno);
	        return FLOAT;
			}
{SEMI}      { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "SEMI" , yylineno);
			return SEMI;
			}
{COMMA}     { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "COMMA", yylineno);
			return COMMA;
			}
{ASSIGNOP}  { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "ASSIGNOP", yylineno);
			return ASSIGNOP;
			}
{RELOP}     { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "RELOP", yylineno);
			return RELOP;
			}
{PLUS}      { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "PLUS", yylineno);
			return PLUS; 
			}
{MINUS}     { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "MINUS", yylineno);
			return MINUS; 
			}
{STAR}      { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "STAR", yylineno);
			return STAR;
			}
{DIV}       { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "DIV", yylineno);
			return DIV; }
{AND}       { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "AND", yylineno);
			return AND;
			}
{OR}        {
			yylval.tnode = CreateTreeNode(L_ELSE, "OR", yylineno);
			return OR;
			}
{DOT}       {
			yylval.tnode = CreateTreeNode(L_ELSE, "DOT", yylineno);
			return DOT;
			}
{NOT}       {
			yylval.tnode = CreateTreeNode(L_ELSE, "NOT", yylineno);
			return NOT;
			}
{TYPE}      { PYYT
			yylval.tnode = CreateTreeNode(L_TYPE, yytext, yylineno);
			return TYPE;
			}
{LP}        { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "LP", yylineno);
			return LP;
			}
{RP}        { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "RP", yylineno);
			return RP;
			}
{LB}        { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "LB", yylineno);
			return LB;
			}
{RB}        { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "RB", yylineno);
			return RB;
			}
{LC}        { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "LC", yylineno);
			return LC;
			}
{RC}        { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "RC", yylineno);
			return RC;
			}
{DELIM}     { PYYT
			}

{NEXTLINE}  { PYYT
				yycolumn = 1;
			}

{STRUCT}    { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "STRUCT", yylineno);
			return STRUCT;
			}
{RETURN}    { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "RETURN", yylineno);
			return RETURN;
			}
{IF}        { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "IF", yylineno);
			return IF;
			}
{ELSE}      { PYYT
			yylval.tnode = CreateTreeNode(L_ELSE, "ELSE", yylineno);
			return ELSE;
			}
{WHILE}     {
			yylval.tnode = CreateTreeNode(L_ELSE, "WHILE", yylineno);
			return WHILE;
			}
{ID}        { PYYT
			yylval.tnode = CreateTreeNode(L_ID, yytext, yylineno);
//			printf("%s\n",yytext);
			return ID;
			}

{NOTE}      {;}

{ERRORID}   { PYYT
			yylval.tnode = CreateTreeNode(L_ID, "ERRORID", yylineno);
			printf("Error type A at Line %d: error id \"%s\".\n", yylineno, yytext);
			HaveTree = 0;
			return ID;}

.           { 
			printf("Error type A at Line %d: Mysterious character \"%s\".\n", yylineno, yytext);
			HaveTree = 0;
			}

%%

