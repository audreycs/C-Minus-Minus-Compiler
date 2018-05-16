%{
	#include "lex.yy.c"
	#include "Tree.h"
	extern void myerror(char*, int);
	int Syn_ERRTYPE = 0;
	int HaveTree = 1;
	TreeNode* TreeRoot = NULL;
%}

%union {
	struct TreeNode *tnode;
}

%locations  /*I don't know how this line works*/

%token <tnode> INT
%token <tnode> FLOAT


%token <tnode> IF ELSE WHILE ASSIGNOP DOT TYPE STRUCT RETURN RELOP
%token <tnode> LP RP LB RB LC RC SEMI COMMA
%token <tnode> PLUS MINUS STAR DIV AND OR NOT
%token <tnode> ID


%type <tnode> Program ExtDefList ExtDef ExtDecList 
%type <tnode> Specifier StructSpecifier OptTag Tag 
%type <tnode> VarDec FunDec VarList ParamDec
%type <tnode> CompSt StmtList Stmt 
%type <tnode> DefList Def DecList Dec 
%type <tnode> Exp Args 


%left ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT NEG
%nonassoc LOWER_THAN_RB
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%nonassoc LOWER_THAN_RC
%nonassoc RC

%nonassoc LOWER_THAN_SEMI
%nonassoc SEMI

%%

/*High-level Definitions*/
/*1*/
Program : ExtDefList { $$=GenerateTreeNode(S_Program,"Program",$1->lineno,1,$1); TreeRoot = $$;}
;
/*2 3*/
ExtDefList : /*empty*/ { $$=GenerateTreeNode(S_ExtDefList,"ExtDefList",yylineno,-1,NULL);}
		| ExtDef ExtDefList { $$=GenerateTreeNode(S_ExtDefList,"ExtDefList",$1->lineno,2,$1,$2);}
;
/*4 5 6*/
ExtDef : Specifier ExtDecList SEMI { $$=GenerateTreeNode(S_ExtDef,"ExtDef",$1->lineno,3,$1,$2,$3);}
		| Specifier SEMI { $$=GenerateTreeNode(S_ExtDef,"ExtDef",$1->lineno,2,$1,$2);}
		| Specifier FunDec CompSt {	$$=GenerateTreeNode(S_ExtDef,"ExtDef",$1->lineno,3,$1,$2,$3);}
		| Specifier FunDec error SEMI { myerror("Syntax error", $4->lineno); HaveTree = 0;}
		| Specifier ExtDecList error SEMI { myerror("Syntax error", $4->lineno); HaveTree = 0;}
		| Specifier error SEMI { myerror("Syntax error sd", $3->lineno); HaveTree = 0;}
		| error SEMI { myerror("Syntax error", $2->lineno); HaveTree = 0;}
;
/*7 8*/
ExtDecList : VarDec { $$=GenerateTreeNode(S_ExtDecList,"ExtDecList",$1->lineno,1,$1);}
		| VarDec COMMA ExtDecList { $$=GenerateTreeNode(S_ExtDecList,"ExtDecList",$1->lineno,3,$1,$2,$3); }
;


/*Specifier*/
/*9 10*/
Specifier : TYPE {$$ = GenerateTreeNode(S_Specifier, "Specifier", $1->lineno, 1, $1);}
		| StructSpecifier {$$ = GenerateTreeNode(S_Specifier, "Specifier", $1->lineno, 1, $1);}
;
/*11 12*/
StructSpecifier: STRUCT OptTag LC DefList RC{$$=GenerateTreeNode(S_ExtDef,"ExtDef",$1->lineno,5,$1,$2,$3,$4,$5); }
		| STRUCT OptTag LC error RC { myerror("Syntax error before \"}\"", $5->lineno); HaveTree = 0; }
		| STRUCT Tag {$$=GenerateTreeNode(S_ExtDef,"ExtDef",$1->lineno,2,$1,$2);}
;
/*13 14*/
OptTag : /*empty*/ {$$=GenerateTreeNode(S_OptTag,"OptTag",yylineno,-1,NULL);}
		| ID { $$=GenerateTreeNode(S_OptTag,"OptTag",$1->lineno,1,$1); }
;
/*15*/
Tag : ID { $$=GenerateTreeNode(S_Tag,"Tag",$1->lineno,1,$1); }
;


/*Declarators*/
/*16 17 18*/
VarDec : ID { $$=GenerateTreeNode(S_VarDec,"VarDec",$1->lineno,1,$1);}
		| VarDec LB INT RB {$$=GenerateTreeNode(S_VarDec,"VarDec",$1->lineno,4,$1,$2,$3,$4);}
		| VarDec LB INT error INT RB %prec LOWER_THAN_RB { myerror("Syntax error near \"]\"", $3->lineno); HaveTree = 0;}
		| VarDec LB error RB  { myerror("Syntax error before \"]\"", $2->lineno); HaveTree = 0;}
;
/*19 20*/
FunDec : ID LP VarList RP {$$=GenerateTreeNode(S_FunDec,"FunDec",$1->lineno,4,$1,$2,$3,$4);}
		| ID LP RP { $$=GenerateTreeNode(S_FunDec,"FunDec",$1->lineno,3,$1,$2,$3);}
;
/*21 22*/
VarList : ParamDec COMMA VarList {$$=GenerateTreeNode(S_VarList,"VarList",$1->lineno,3,$1,$2,$3);}
		| ParamDec { $$=GenerateTreeNode(S_VarList,"VarList",$1->lineno,1,$1);}
;
/*23*/
ParamDec : Specifier VarDec {$$=GenerateTreeNode(S_ParamDec,"ParamDec",$1->lineno,2,$1,$2);}
;


/*Statements*/
/*24*/
CompSt : LC DefList StmtList RC {$$=GenerateTreeNode(S_CompSt,"CompSt",$1->lineno,4,$1,$2,$3,$4);}
    	| error DefList StmtList RC { myerror("Syntax error",$2->lineno); HaveTree = 0;}
		| LC DefList StmtList  { myerror("Missing \"}\"",yylineno); HaveTree = 0;}
;
/*25 26*/
StmtList : /*empty*/ { $$=GenerateTreeNode(S_StmtList,"StmtList",yylineno,-1,NULL);} 
		| Stmt StmtList { $$=GenerateTreeNode(S_StmtList,"StmtList",$1->lineno,2,$1,$2);}
;
/*27 28 29 30 31 32 33*/
Stmt : Exp SEMI { $$=GenerateTreeNode(S_Stmt,"Stmt",$1->lineno,2,$1,$2);}
		| Exp error ELSE { myerror("Syntax error", $1->lineno); HaveTree = 0;}
		| Exp error SEMI { myerror("Syntax error", $3->lineno); HaveTree = 0;}
		| Exp error COMMA { myerror("Syntax error before \",\"", $3->lineno); HaveTree = 0;}
		| CompSt {$$=GenerateTreeNode(S_Stmt,"Stmt",$1->lineno,1,$1); }
		| RETURN Exp SEMI  { $$=GenerateTreeNode(S_Stmt,"Stmt",$1->lineno,3,$1,$2,$3);} 
		| RETURN Exp error { myerror("syntax error", yylineno); HaveTree = 0;}
		| RETURN error SEMI { myerror("Syntax error", $3->lineno); HaveTree = 0;}
		| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE{$$=GenerateTreeNode(S_Stmt,"Stmt",$1->lineno,5,$1,$2,$3,$4,$5);}
		| IF LP Exp RP Stmt ELSE Stmt { $$=GenerateTreeNode(S_Stmt,"Stmt",$1->lineno,7,$1,$2,$3,$4,$5,$6,$7);}
		| WHILE LP Exp RP Stmt { $$=GenerateTreeNode(S_Stmt,"Stmt",$1->lineno,5,$1,$2,$3,$4,$5);}
		| WHILE LP error RP Stmt { myerror("Syntax error before \")\"",$2->lineno); HaveTree = 0;}
		| error TYPE Exp SEMI { myerror("Syntax error", $2->lineno); HaveTree = 0;}
		| IF LP error RP Stmt %prec LOWER_THAN_ELSE { Syn_ERRTYPE = 0; myerror("Syntax error before \")\"", $4->lineno); HaveTree = 0;}
		| IF LP error RP Stmt ELSE Stmt { Syn_ERRTYPE = 0; myerror("Syntax error before \")\"", $4->lineno); HaveTree = 0;}
;


/*Local Definitions*/
/*34 35*/
DefList : /*empty*/ { $$=GenerateTreeNode(S_DefList,"DefList", yylineno,-1,NULL);}
		| Def DefList { $$=GenerateTreeNode(S_DefList,"DefList",$1->lineno,2,$1,$2);}
;
/*36*/
Def : Specifier DecList SEMI { $$=GenerateTreeNode(S_Def,"Def",$1->lineno,3,$1,$2,$3);}
/*	| Specifier error SEMI  { myerror("syntax error", $3->lineno); HaveTree = 0;} 
	| Specifier DecList error %prec LOWER_THAN_SEMI {Syn_ERRTYPE = 0; myerror("9", $2->lineno); HaveTree = 0; } */
;
/*37 38*/
DecList : Dec { $$=GenerateTreeNode(S_DecList,"DecList",$1->lineno,1,$1);}
		| Dec COMMA DecList { $$=GenerateTreeNode(S_DecList,"DecList",$1->lineno,3,$1,$2,$3);}
;
/*39 40*/
Dec : VarDec { $$=GenerateTreeNode(S_Dec,"Dec",$1->lineno,1,$1);}
	| VarDec ASSIGNOP Exp { $$=GenerateTreeNode(S_Dec,"Dec",$1->lineno,3,$1,$2,$3);}
;


/*Expressions*/
/*41+*/
Exp : Exp ASSIGNOP Exp { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,3,$1,$2,$3);} 
	| Exp AND Exp { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,3,$1,$2,$3);}
	| Exp OR Exp { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,3,$1,$2,$3);}
	| Exp RELOP Exp { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,3,$1,$2,$3);}
	| Exp PLUS Exp { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,3,$1,$2,$3);}
	| Exp MINUS Exp { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,3,$1,$2,$3);}
	| Exp STAR Exp { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,3,$1,$2,$3);}
	| Exp DIV Exp { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,3,$1,$2,$3);}
	| LP Exp RP { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,3,$1,$2,$3);}
	| MINUS Exp %prec NEG { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,2,$1,$2);}
	| NOT Exp { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,2,$1,$2);}
	| ID LP Args RP { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,4,$1,$2,$3,$4);}
	| ID LP RP { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,3,$1,$2,$3);}
	| Exp LB Exp RB { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,4,$1,$2,$3,$4);}
	| Exp DOT ID { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,3,$1,$2,$3);}
	| ID        { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,1,$1); }
	| INT       { $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,1,$1); }
	| FLOAT		{ $$=GenerateTreeNode(S_Exp,"Exp",$1->lineno,1,$1); }
	| Exp LB error RB    { myerror("syntax error before \"]\"", $4->lineno); HaveTree = 0;}
	| error DIV { myerror("syntax error", $2->lineno); HaveTree = 0;} 
	| DOT error INT  { myerror("syntax error", $1->lineno); HaveTree = 0;}  
	| ID LP Args error RP { myerror("syntax error before \"}\"", $5->lineno); HaveTree = 0;}
;
/*60*/
Args : Exp COMMA Args { $$=GenerateTreeNode(S_Args,"Args", $1->lineno,3,$1,$2,$3);}
	| Exp { $$=GenerateTreeNode(S_Args,"Args",$1->lineno,1,$1);}
;


%%

yyerror(char* msg){
}

void myerror(char *msg, int errline){
	printf("Error type B at Line %d: %s.\n", errline, msg);
}

