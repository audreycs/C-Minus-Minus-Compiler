#ifndef _TYPENODE_H_
#define _TYPENODE_H_

typedef struct Type_ Type;
typedef struct FieldList_ FieldList;

typedef enum { BASIC, ARRAY, STRUCTURE, FUNCTION } TYPE_KIND;
typedef enum { B_INT, B_FLOAT } BASIC_KIND;

struct Type_
{
	TYPE_KIND kind;
	union
	{
		// basic type
		BASIC_KIND basic;

		// type array
		struct { 
			int size;
			Type* elem;
		} array;
		
		//type struct
		FieldList* structure;
		
		//type function
		struct {
			Type* backtype;
			FieldList* param;
		} function;
	} u;
};

struct FieldList_
{
	char* name;
	Type* type;
	FieldList* next;
};


#endif
