/*************************************************************************
> File Name: SymbolTable.h
> Author: 
> Mail: 
> Created Time: Sun 19 Nov 2017 02:32:26 PM CST
************************************************************************/

#ifndef _SYMBOLTABLE_H
#define _SYMBOLTABLE_H

#include "Type.h"

typedef struct Node Node;
typedef struct SymbolNode_* SymbolNode;
typedef struct  SymbolTable_* SymbolTable;

struct SymbolNode_{
    char* name;
    Type type; 
    SymbolNode tail;
    SymbolNode stack_next;
};

struct SymbolTable_{
    SymbolNode bucket[0x3fff];
};

SymbolTable newSymbolTable();
bool haveSymbolNode(SymbolTable symbolTable, SymbolNode symbolNode);
void insert(SymbolTable symbolTable, SymbolNode insertNode);
void delete(SymbolTable symbolTable, SymbolNode deleteNode);
void printSymbolTable(SymbolTable symboltable);

SymbolNode newSymbolNode(char* name, Type type);

FieldList newFieldList();
void addFieldList(FieldList head, FieldList fieldList);

Type newType();
void printType(Type type);

bool matchType(Type t1, Type t2);
Type matchReturnType(Type t1, Type t2, int lineno);
Type matchArgsType(Type funcType, char* funcName);

void printExp(Node*);
bool isLeftVal(Node* node);

void newSymbol(char* name, Type type);
Type getSymbol(char* name);

void newUndefinedFunc(char* name, Type retType, int lineno);
void newDefinedFunc(char* name, Type retType, int lineno);
Type haveFunc(char* funcName);
void checkUndefinedFuncBeforeExit();

void newStruct(int num, ...);
bool getStructVarOffset(char* name, Type type, int* offset);
Type getTagType(char* tag);
    
void newParam(int num, ...);

void newArg(Type type);

#endif
