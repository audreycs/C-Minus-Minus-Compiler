#include "semantic.h" 

// fieldloca : 1 means variable is not a structure field
// fieldloca : 2 means variable is a structure field
// fieldloca : 3 means variable is a array
int fieldloca = 1;

//void ShowField(FieldList*);

void Program(TreeNode* root)
{ 
	// Program -> ExtDefList
	if(root == NULL)
		return;
	ExtDefList(root->child[0]);
}


void ExtDefList(TreeNode* root)
{
	if(root == NULL)
		return;
	// ExtDefList -> empty
	if(root -> childnum == -1)
		return;
	// ExtDefList -> ExtDef ExtDefList
	ExtDef(root->child[0]);
	ExtDefList(root->child[1]);
}


void ExtDef(TreeNode* root)
{
	if(root == NULL)	
		return;
	Type* spe_type = Specifier(root->child[0]); // spe_type maybe is NULL
	if(spe_type == NULL) return;

	// ExtDef -> Specifier SEMI
	if( strcmp(root->child[1]->else_name, "SEMI") == 0 ) 
	{
		return;
	}
	// ExtDef -> Specifier ExtDecList SEMI
	else if( strcmp(root->child[1]->syn_name, "ExtDecList") == 0 ) 
	{
		ExtDecList(spe_type, root->child[1]);
		return;
	}
	// ExtDef -> Specifier FunDec CompSt
	else 
	{
		FunDec(spe_type, root->child[1]);
		CompSt(spe_type, root->child[2]);
		return;
	}
}


void ExtDecList(Type* type, TreeNode* root)
{
	if( root->childnum == 1 ) // ExtDecList -> VarDec
	{
		VarDec(type, root->child[0]);
	}
	else // ExtDecList -> VarDec COMMA ExtDecList
	{
		VarDec(type, root->child[0]);
		ExtDecList(type, root->child[2]);
	}
	return;
}


Type* Specifier(TreeNode* root)
{
	if( root->child[0]->l_cont == L_TYPE ) // Specifier -> TYPE
	{
		Type* type = (Type*)malloc(sizeof(Type));
		type->kind == BASIC;
		if(strcmp(root->child[0]->type_name, "int") == 0)
			type->u.basic = B_INT;
		else
			type->u.basic = B_FLOAT;
		return type;
	}
	else // Specifier -> StructureSpecifier
	{
		Type* type = StructSpecifier(root->child[0]); // type maybe is NULL
		return type;
	}
}


Type* StructSpecifier(TreeNode* root)
{
	// StructureSpecifier -> STRUCT Tag
	if( strcmp(root->child[1]->syn_name, "Tag")==0 )
	{
		char* varname = Tag(root->child[1]); // Tag's ID_name
		int ifdef = SymbolTableCheck(varname);
		if(ifdef == 0) // Struct is not defined
		{
			printf("Error type 17 at Line %d: Undefined structure \"%s\".\n",root->child[1]->lineno, varname);
			return NULL;
		}
		else
		{
			Type* type = SymbolTableGetType(varname); 
			return type;
		}
	}
	// StructureSpecifier -> STRUCT OptTag LC DefList RC
	else
	{
		char* varname = OptTag(root->child[1]);
		if(varname == NULL) // OptTag -> empty
		{
			Type* type = (Type*)malloc(sizeof(Type));
			type->kind = STRUCTURE; 
			fieldloca = 2;
			type->u.structure = DefList(root->child[3]);
			fieldloca = 1;
			return type;
		}
		else // OptTag -> ID
		{
			int ifdef = SymbolTableCheck(varname);
			if(ifdef == 1) // Structure defined
			{
				printf("Error type 16 at Line %d: Duplicated name \"%s\".\n",root->child[1]->lineno, varname);
				return NULL;
			}
			else
			{
				Type* type = (Type*)malloc(sizeof(Type));
				type->kind = STRUCTURE;
				fieldloca = 2;
				type->u.structure = DefList(root->child[3]);
				fieldloca = 1;
				SymbolTableInsert(varname,type);
				return type;
			}
		}
	}
}


//finished
char* OptTag(TreeNode* root)
{
	if(root->childnum == -1) // OptTag -> NULL
		return NULL;
	else  // OptTag -> ID
		return root->child[0]->id_name;
}


//finished
char* Tag(TreeNode* root) //Tag -> ID
{
	return root->child[0]->id_name;
}


char* VarDec(Type* type, TreeNode* root) // return id_name
{
	if(root->childnum == 1) // VarDec -> ID
	{
		char* varname = root->child[0]->id_name;
		int ifdef = SymbolTableCheck(varname);
		if(ifdef == 1) // ID is defined
		{
			if(fieldloca == 1)
				printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",root->child[0]->lineno,varname);
			else if(fieldloca == 2)
				printf("Error type 15 at Line %d: Redefined field \"%s\".\n",root->child[0]->lineno,varname);
			return varname;
		}
		else // ID is not defined
		{
			SymbolTableInsert(varname,type);
			return varname;
		}
	}

	else // VarDec -> VarDec LB INT RB
	{
		char* varname = VarDec(type, root->child[0]);
	
		Type* newtype = (Type*)malloc(sizeof(Type));
		newtype->kind = ARRAY;
		int s = root->child[2]->int_value;
		newtype->u.array.size = s;
		newtype->u.array.elem = NULL;

		Type* gettype = SymbolTableGetType(varname);
		newtype->u.array.elem = gettype;
		SymbolTableInsert(varname,newtype);
		return varname;
	}
}


void FunDec(Type* type, TreeNode* root)
{
	char* varname = root->child[0]->id_name; // Get ID name
	int ifdef = SymbolTableCheck(varname);
	if(ifdef == 1) // function is defined
	{
		printf("Error type 4 at Line %d: Redefined function \"%s\".\n",root->lineno,varname);
		return;
	}
	if(root->childnum == 3) // FunDec -> ID LP RP
	{
		Type* newtype = (Type*)malloc(sizeof(Type));
		newtype->kind = FUNCTION;
		newtype->u.function.backtype = type;
		newtype->u.function.param = NULL;
		SymbolTableInsert(varname,newtype);
		return;
	}
	else // FunDec -> ID LP VarList RP
	{
		Type* newtype = (Type*)malloc(sizeof(Type));
		newtype->kind = FUNCTION;
		newtype->u.function.backtype = type;
		newtype->u.function.param = VarList(root->child[2]);
		SymbolTableInsert(varname,newtype);
		return;
	}
}


FieldList* VarList(TreeNode* root)
{
	if(root->childnum == 1) // VarList -> ParamDec
	{
		return ParamDec(root->child[0]);
	}
	else // VarList -> ParamDec COMMA VarList
	{
		FieldList* list1 = ParamDec(root->child[0]);
		FieldList* list2 = VarList(root->child[2]);
		list1->next = list2;
		return list1;
	}
}


FieldList* ParamDec(TreeNode* root)
{
	// ParamDec -> Specifier VarDec
	FieldList* list = (FieldList*)malloc(sizeof(FieldList));
	Type* temp = Specifier(root->child[0]);
	list->name = VarDec(temp,root->child[1]);
	list->type = SymbolTableGetType(list->name);
	list->next = NULL;
	return list;
}


void CompSt(Type* backtype, TreeNode* root)
{
	// CompSt -> LC DefList StmtList RC
	DefList(root->child[1]);
	StmtList(backtype, root->child[2]);
}


void StmtList(Type* backtype, TreeNode* root)
{
	if(root->childnum == -1) // StmtList -> empty
	{
		return;
	}
	else // StmtList -> Stmt StmtList
	{
		Stmt(backtype, root->child[0]);
		StmtList(backtype, root->child[1]);
		return;
	}
}


void Stmt(Type* backtype, TreeNode* root)
{
	// Stmt -> CompSt
	if(root->childnum == 1)
	{
		CompSt(backtype, root->child[0]);
	}
	// Stmt -> Exp SEMI
	else if(root->childnum == 2)
	{
		Exp(root->child[0]);
	}
	// Stmt -> RETURN Exp SEMI
	else if(root->childnum == 3)
	{
		Type* exptype = Exp(root->child[1]);
		if(exptype==NULL || backtype == NULL) return;
		if( TypeEqual(backtype,exptype) == 0 ) // return type not equal
			printf("Error type 8 at Line %d: Type mismatched for return.\n",root->child[0]->lineno);
	}
	// Stmt -> IF LP Exp RP Stmt
	else if(root->childnum == 5 && strcmp(root->child[0]->else_name,"IF") == 0)
	{
		Exp(root->child[2]);
		Stmt(backtype,root->child[4]);
	}
	// Stmt -> WHILE LP Exp RP Stmt
	else if(root->childnum == 5 && strcmp(root->child[0]->else_name,"WHILE") == 0)
	{
		Exp(root->child[2]);
		Stmt(backtype,root->child[4]);
	}
	// Stmt -> IF LP Exp RP Stmt ELSE Stmt
	else
	{
		Exp(root->child[2]);
		Stmt(backtype,root->child[4]);
		Stmt(backtype,root->child[6]);
	}
}


FieldList* DefList(TreeNode* root)
{
	if(root->childnum == -1) // DefList -> empty
		return NULL;
	else // DefList -> Def DefList
	{
		FieldList* list1 = Def(root->child[0]);
		FieldList* list2 = DefList(root->child[1]);
		FieldList* temp = list1;
		while(temp->next!=NULL)
		{
			temp = temp->next;
		}
		temp->next = list2;
		return list1;
	}
}


FieldList* Def(TreeNode* root)
{
	// Def -> Specifier DecList SEMI
	Type* spe_type = Specifier(root->child[0]);
	FieldList* list = DecList(spe_type, root->child[1]);
	return list;
}


FieldList* DecList(Type* type, TreeNode* root)
{
	if(root->childnum == 1) // DecList -> Dec
	{
		char* varname = Dec(type, root->child[0]);
		FieldList* list = (FieldList*)malloc(sizeof(FieldList));
		list->name = varname;
		list->type = SymbolTableGetType(list->name);
		list->next = NULL;
		return list;
	}
	else // DecList -> Dec COMMA DecList
	{
		char* varname = Dec(type, root->child[0]);
		FieldList* list1 = (FieldList*)malloc(sizeof(FieldList));
		list1->name = varname;
		list1->type = SymbolTableGetType(list1->name);
		list1->next = NULL;
		FieldList* list2 = DecList(type, root->child[2]);
		list1->next = list2;
		return list1;
	}
}


char* Dec(Type* type, TreeNode* root)
{
	if(root->childnum == 1) // Dec -> VarDec
	{
		char* varname = VarDec(type,root->child[0]);
		return varname;
	}
	else // Dec -> VarDec ASSIGNOP Exp
	{
		char* varname = VarDec(type,root->child[0]);
		Type* exptype = Exp(root->child[2]);
		if(exptype == NULL)
			return varname;
		if( fieldloca == 2 )
		{
			printf("Error type 15 at line %d: Initialize a field of structure \"%s\".\n",root->child[1]->lineno, varname);
			return varname;
		}
		if( TypeEqual(type,exptype)==0 )
		{
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n", root->child[0]->lineno);
			return varname;
		}
		return varname;
	}	
}


Type* Exp(TreeNode* root)
{
	// Exp -> ID
	if(root->childnum == 1 && root->child[0]->l_cont == L_ID)
	{
		char* varname = root->child[0]->id_name;
		int ifdef = SymbolTableCheck(varname);
		if(ifdef == 0) // ID is not defined
		{
			printf("Error type 1 at Line %d: Undefined variable \"%s\".\n",root->child[0]->lineno,varname);
			return NULL;
		}
		else // ID is defined
		{
			Type* gettype = SymbolTableGetType(varname);
			//printf("%s's type is %d\n",varname,gettype->kind);
			return gettype;
		}
	}
	// Exp -> INT
	else if(root->childnum == 1 && root->child[0]->l_cont == L_INT)
	{
		Type* type = (Type*)malloc(sizeof(Type));
		type->kind = BASIC;
		type->u.basic = B_INT;
		return type;
	}
	// Exp -> FLOAT
	else if(root->childnum == 1 && root->child[0]->l_cont == L_FLOAT)
	{
		Type* type = (Type*)malloc(sizeof(Type));
		type->kind = BASIC;
		type->u.basic = B_FLOAT;
		return type;
	}
	// Exp -> MINUS Exp
	else if(root->childnum == 2 && strcmp(root->child[0]->else_name, "MINUS")==0)
	{
		Type* type = Exp(root->child[1]);
		if(type == NULL)
			return NULL;
		else
		{
			if(type->kind != BASIC)
			{
				printf("Error type 7 at Line %d: Type mismatched for operands.\n",root->child[0]->lineno);
				return NULL;
			}
			else
				return type;
		}
	}
	// Exp -> NOT Exp
	else if(root->childnum == 2 && strcmp(root->child[0]->else_name, "NOT")==0)
	{
		Type* type = Exp(root->child[1]);
		if(type == NULL)
			return NULL;
		else
		{
			if(type->kind != BASIC || type->u.basic != B_FLOAT)
			{
				printf("Error type 7 at Line %d: Type mismatched for operands.",root->child[0]->lineno);
				return NULL;
			}
			else
				return type;
		}
	}
	// Exp -> Exp ASSIGNOP Exp
	else if(root->childnum == 3 && strcmp(root->child[1]->else_name, "ASSIGNOP")==0)
	{
		Type* type1 = Exp(root->child[0]);
		Type* type2 = Exp(root->child[2]);
		//printf("left= %d, right=%d\n",type1->kind, type2->kind);
		if(type1 == NULL || type2 == NULL)
			return NULL;
		if( (type1->kind != BASIC && type1->kind != ARRAY && type1->kind != STRUCTURE) 
	|| (root->child[0]->childnum >= 3 && strcmp(root->child[0]->child[1]->else_name,"LP")==0))
		{
			printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",root->child[1]->lineno);
			return NULL;
		}
		else if(TypeEqual(type1, type2) == 0 ) // types are not equal
		{
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n",  root->child[1]->lineno);
			return NULL;
		}
		else
		{
			return type1;
		}
	}
	// Exp -> Exp AND Exp
	else if(root->childnum == 3 && strcmp(root->child[1]->else_name, "AND")==0)
	{
		Type* type1 = Exp(root->child[0]);
		Type* type2 = Exp(root->child[2]);
		if(type1 == NULL || type2 == NULL)
			return NULL;
		else
		{
			if(type1->kind == BASIC && type2->kind == BASIC && 
					type1->u.basic == type2->u.basic)
				return type1;
			else 
				return NULL;
		}
	}
	// Exp -> Exp OR Exp
	else if(root->childnum == 3 && strcmp(root->child[1]->else_name, "OR")==0)
	{
		Type* type1 = Exp(root->child[0]);
		Type* type2 = Exp(root->child[2]);
		if(type1 == NULL || type2 == NULL)
			return NULL;
		else
		{	
			if(type1->kind == BASIC && type2->kind == BASIC &&
					type1->u.basic == type2->u.basic)
				return type1;
			else
				return NULL;
				
		}
	}
	// Exp -> Exp RELOP Exp
	else if(root->childnum == 3 && strcmp(root->child[1]->else_name, "RELOP")==0)
	{
		Type* type1 = Exp(root->child[0]);
		Type* type2 = Exp(root->child[2]);
		if(type1 == NULL || type2 == NULL)
			return NULL;
		else
		{
			if(type1->kind == BASIC && type2->kind == BASIC &&
					type1->u.basic == type2->u.basic)
				return type1;
			else
			{
				printf("Error type 7 at line %d: Operands type mismatched.\n",root->child[1]->lineno);
				return NULL;
			}
		}
	}
	// Exp -> Exp PLUS Exp
	else if(root->childnum == 3 && strcmp(root->child[1]->else_name, "PLUS")==0)
	{
		Type* type1 = Exp(root->child[0]);
		Type* type2 = Exp(root->child[2]);
		if(type1 == NULL || type2 == NULL)
			return NULL;
		if(type1->kind == BASIC && type2->kind == BASIC)
		{
			if(type1->u.basic == type2->u.basic)
				return type1;
			else
			{
				printf("Error type 7 at Line %d: Type mismatched for operands.\n", root->child[1]->lineno);
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
	}
	// Exp -> Exp MINUS Exp
	else if(root->childnum == 3 && strcmp(root->child[1]->else_name, "MINUS")==0)
	{
		Type* type1 = Exp(root->child[0]);
		Type* type2 = Exp(root->child[2]);
		if(type1 == NULL && type2 == NULL)
			return NULL;
		else
		{
			if(type1->kind == BASIC && type2->kind == BASIC &&
					type1->u.basic == type2->u.basic)
				return type1;
			else
				return NULL;
		}
	}
	// Exp -> Exp STAR Exp
	else if(root->childnum == 3 && strcmp(root->child[1]->else_name, "STAR")==0)
	{
		Type* type1 = Exp(root->child[0]);
		Type* type2 = Exp(root->child[2]);
		if(type1 == NULL && type2 == NULL)
			return NULL;
		else
		{
			if(type1->kind == BASIC && type2->kind == BASIC && type1->u.basic == type2->u.basic)
				return type1;
			else
			{
				printf("Error type 7 at line %d: Operands type mismatched.\n",root->child[1]->lineno);
				return NULL;
			}
		}
	}
	// Exp -> Exp DIV Exp
	else if(root->childnum == 3 && strcmp(root->child[1]->else_name, "DIV")==0)
	{
		Type* type1 = Exp(root->child[0]);
		Type* type2 = Exp(root->child[2]);
		if(type1 == NULL || type2 == NULL)
			return NULL;
		else
		{
			if(type1->kind == BASIC && type2->kind == BASIC &&
					type1->u.basic == type2->u.basic)
				return type1;
			else
				return NULL;
		}
	}
	// Exp -> Exp DOT ID
	else if(root->childnum == 3 && strcmp(root->child[1]->else_name, "DOT")==0)
	{
		Type* type = Exp(root->child[0]);
		if(type == NULL)
			return NULL;
		else
		{
			if(type->kind != STRUCTURE)
			{
				printf("Error type 13 at Line %d: Illegal use of \".\".\n",root->child[1]->lineno);
				return NULL;
			}
			char* str = root->child[2]->id_name;
			Type* ID_type = FieldCheck(type->u.structure, str);
			if(ID_type==NULL) // ID is not a Field of Exp structure
			{
				printf("Error type 14 at Line %d: Non-existent field \"%s\".\n", root->child[1]->lineno, str);
				return NULL;
			}
			else
			{
				//printf("%s's type is %d\n",str,ID_type->kind);
				return ID_type;
			}
		}
	}
	// Exp -> LP Exp RP
	else if(root->childnum == 3 && strcmp(root->child[0]->else_name, "LP")==0)
	{
		return Exp(root->child[1]);
	}
	// Exp -> ID LP RP
	else if(root->childnum == 3 && root->child[0]->l_cont == L_ID)
	{
		char* varname = root->child[0]->id_name;
		int ifdef = SymbolTableCheck(varname);
		if(ifdef == 0) // ID is not defined
		{
			printf("Error type 2 at Line %d: Undefined function \"%s\".\n",root->child[0]->lineno,varname);
			return NULL;
		}
		else // ID is defined
		{
			Type* gettype =  SymbolTableGetType(varname);
			if(gettype->kind != FUNCTION)
			{
				printf("Error type 11 at Line %d: \"%s\" is not a function.\n", root->child[0]->lineno,varname);
				return NULL;
			}
			else
				return gettype;
		}
	}
	// Exp -> ID LP Args RP
	else if(root->childnum == 4 && root->child[0]->l_cont == L_ID)
	{
		char* varname = root->child[0]->id_name;
		int ifdef = SymbolTableCheck(varname);
		if(ifdef == 0) // Function not defined
		{
			printf("Error type 2 at Line %d: Undefined function \"%s\".\n", root->child[0]->lineno, varname);
			return NULL;
		}
		else // Function is defined
		{
			Type* functype = SymbolTableGetType(varname);
			if(functype->kind != FUNCTION)
			{
				printf("Error type 11 at Line %d: \"%s\" is not a function.\n", root->child[0]->lineno, varname);
				return NULL;
			}

			FieldList* argslist = Args(root->child[2]);
			if(FuncParamEqual(functype->u.function.param, argslist) == 1)
				return functype->u.function.backtype;
			else
			{
				printf("Error type 9 at Line %d: Function \"%s\" is not applicated for such arguments.\n", root->child[0]->lineno, varname);
				return NULL;
			}
		}

	}
	// Exp -> Exp LB Exp RB
	else
	{
		Type* type1 = Exp(root->child[0]);
		Type* type2 = Exp(root->child[2]);
		//printf("*******%d\n",type1->kind);
		if(type1 == NULL || type2 == NULL)
			return NULL;
		if( type1->kind != ARRAY && root->child[0]->childnum != 4)
		{
			printf("Error type 10 at Line %d: \"%s\" is not an array.\n", root->child[0]->lineno, root->child[0]->child[0]->id_name);
			return NULL;
		}
		if(type2->kind != BASIC || type2->u.basic != B_INT)
		{
			printf("Error type 12 at Line %d: value between \"[\" and \"]\" is not an integer.\n", root->child[2]->lineno);
			return NULL;
		}
		else
		{
			Type* temp = type1;
			while(temp->kind == ARRAY)
			{
				temp = temp->u.array.elem;
			}
			//printf("array's type is : %d\n",temp->kind);
			return temp;
		}
	}
}


FieldList* Args(TreeNode* root)
{
	if(root->childnum == 1) // Args -> Exp
	{
		FieldList* list = (FieldList*)malloc(sizeof(FieldList));
		list->name = NULL;
		list->type = Exp(root->child[0]);
		list->next = NULL;
		return list;
	}
	else // Args -> Exp COMMA Args
	{
		FieldList* list1 = (FieldList*)malloc(sizeof(FieldList));
		list1->name = NULL;
		list1->type = Exp(root->child[0]);
		list1->next = NULL;
		FieldList* list2 = Args(root->child[2]);
		list1->next = list2;
		return list1;
	}
}


int Empty_Node(TreeNode* Node)
{
	if(Node -> childnum == -1)
		return 1;
	else
		return 0;
}

// equal return 1
// not equal return 0
int TypeEqual(Type* type1, Type* type2)
{
//	printf("-------Check type equal:--------\n");

	if(type1 == NULL && type2 == NULL)
		return 1;
	if(type1 == NULL || type2 == NULL)
		return 0;
//	printf("type1->kind: %d  ",type1->kind);
//	printf("type2->kind: %d\n",type2->kind);

	if (type1->kind != type2->kind) // not equal
		return 0;
	else
	{
		if(type1->kind == BASIC) // BASIC
		{
			if(type1->u.basic == type2->u.basic)
				return 1;
			else
				return 0;
		}
		else if(type1->kind == ARRAY) // ARRAY
		{
			if(type1->u.array.size == type2->u.array.size)
				return TypeEqual(type1->u.array.elem, type2->u.array.elem);
			else
				return 0;
		}
		else if(type1->kind == STRUCTURE) // STRUCTURE
		{
			return FieldEqual(type1->u.structure, type2->u.structure);
		}
		else // FUNCTION
		{
			;
		}
	}

}


int FieldEqual(FieldList* list1, FieldList* list2)
{
	if(list1 == NULL && list2 == NULL)
		return 1;
	if(list1 == NULL || list2 == NULL)
		return 0;
/*	if(strcmp(list1->name, list2->name) != 0) // name are not equal
		return 0;
*/	
	if(TypeEqual(list1->type, list2->type) != 1) // type are not equal
		return 0;
	return FieldEqual(list1->next, list2->next);
}

// Equal return 1
// Not Equal return 0
int FuncParamEqual(FieldList* list1, FieldList* list2)
{
	if(list1 == NULL && list2 == NULL)
		return 1;
	else if(list1 == NULL || list2 == NULL)
		return 0;
//	printf("compare %s\n",list1->name);
	if(TypeEqual(list1->type, list2->type) == 1)
		return FuncParamEqual(list1->next, list2->next);
	else
		return 0;
}


// Exist field return 1
// Not Exist field return 0
Type* FieldCheck(FieldList* list, char* str)
{
	if(list == NULL)
		return NULL;
	//printf("%s, %s\n",list->name, str);
	if(strcmp(list->name, str)==0)
		return list->type;
	else
		return FieldCheck(list->next, str);
}

/*
void ShowField(FieldList* list)
{
	if(list == NULL)
		return;
	if(list->name == NULL)
		printf("name: NULL\n");
	else
		printf("name: %s\n",list->name);
	printf("type kind: %d\n", list->type->kind);
	ShowField(list->next);
}
*/
