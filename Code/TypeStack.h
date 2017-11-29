/*************************************************************************
	> File Name: TypeStack.h
	> Author: 
	> Mail: 
	> Created Time: Mon 27 Nov 2017 11:30:08 PM CST
 ************************************************************************/

#ifndef _TYPESTACK_H
#define _TYPESTACK_H

#include "SymbolTable.h"

typedef struct TypeStack_* TypeStack;

struct TypeStack_{
    int num;
    Type stack[100];
};

TypeStack newTypeStack();
void addTypeStack(TypeStack typeStack, Type type);
Type getTypeStackTop(TypeStack typeStack);
void popTypeStack(TypeStack typeStack);
void clearTypeStack(TypeStack typeStack);

#endif
