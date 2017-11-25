/*************************************************************************
> File Name: SymbolTable.h
> Author: 
> Mail: 
> Created Time: Sun 19 Nov 2017 02:32:26 PM CST
************************************************************************/

#ifndef _SYMBOLTABLE_H
#define _SYMBOLTABLE_H

#define bool int
#define true 1
#define false 0

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Node Node;

struct Type_{
    enum { BASIC, ARRAY, STRUCTURE, FUNCTION } kind;
    union
    {
        // 基本类型 "int","float"
        char* basic;
        // 数组类型信息包括元素类型与数组大小构成
        struct { Type elem; int size; } array;
        // 结构体类型信息是一个链表
        struct { char* tag; FieldList list; } structure;
        //char* structure;
        // the structure of function type's information 
        struct {
            Type retType;
            int paraNum;
            Type* para;
        } function;
    };
};

struct FieldList_{
    char* name; // 域的名字
    Type type; // 域的类型
    FieldList tail; // 下一个域
};

typedef struct SymbolNode_* SymbolNode;
typedef struct  SymbolTable_* SymbolTable;
//typedef struct StructSymbolTable_* StructSymbolTable;
//typedef struct StructSymbolNode_* StructSymbolNode;

struct SymbolNode_{
    char* name;
    Type type; 
    SymbolNode tail;
};

struct SymbolTable_{
    SymbolNode bucket[0x3fff];
};

/*
struct StructSymbolNode_{
    char* tag;
    FieldList structure;
    StructSymbolNode tail;
};

struct StructSymbolTable_{
    StructSymbolNode bucket[0x3fff];
};
*/

SymbolTable newSymbolTable();
bool haveSymbolNode(SymbolTable symbolTable, SymbolNode symbolNode);
void insert(SymbolTable symbolTable, SymbolNode insertNode);
void printSymbolTable(SymbolTable symboltable);

SymbolNode newSymbolNode(char* name, Type type);

FieldList newFieldList();
void addFieldList(FieldList head, FieldList fieldList);

Type newType();
Type matchType(Type t1, Type t2, bool isShowError);
Type matchOperandsType(Type t1, Type t2);
Type matchReturnType(Type t1, Type t2, int lineno);
Type matchArgsType(Type funcType, char* funcName);
void printType(Type type);

void printExp(Node*);

void newSymbol(char* name, Type type);

/*void newVar(int num, ...);*/
Type haveVar(char* varName);

bool getStructVarOffset(char* name, Type type, int* offset);

/*void newArray(int num, ...);*/
    
void newParam(int num, ...);

void newArg(Type type);

void newStruct(int num, ...);

Type getTagType(char* tag);

/*bool isArray(Type type);*/

void newFunc(int num, ...);
Type haveFunc(char* funcName);

#define copystr(dst, src)\
do{\
   size_t len = strlen(src);\
   dst = malloc(len);\
   strcpy(dst, src);\
  }while(0)

#define copytype(dst, src)\
do{\
   if(src == NULL) break;\
   dst = newType();\
   memcpy(dst, src, sizeof(struct Type_));\
  }while(0)

#endif
