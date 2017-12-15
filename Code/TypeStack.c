/*************************************************************************
	> File Name: TypeStack.c
	> Author: 
	> Mail: 
	> Created Time: Mon 27 Nov 2017 11:32:21 PM CST
 ************************************************************************/

#include "TypeStack.h"
#include "stdlib.h"
#include "string.h"

TypeStack newTypeStack(){
    TypeStack typeStack = (TypeStack)malloc(sizeof(struct TypeStack_));
    typeStack->num = 0;
    return typeStack;
}

void addTypeStack(TypeStack typeStack, Type type){
    int size = sizeof(struct Type_);
    Type newType = (Type)malloc(sizeof(struct Type_));
    memcpy(newType, type, size);
    typeStack->stack[typeStack->num] = newType;
    typeStack->num++;
}

Type getTypeStackTop(TypeStack typeStack){
    if(typeStack->num == 0) return NULL;
    return typeStack->stack[typeStack->num - 1];
}

void popTypeStack(TypeStack typeStack){
    typeStack->num--;
}

void clearTypeStack(TypeStack typeStack){
    typeStack->num = 0;
}

