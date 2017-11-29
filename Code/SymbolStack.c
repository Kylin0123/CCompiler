/*************************************************************************
	> File Name: SymbolStack.c
	> Author: 
	> Mail: 
	> Created Time: Tue 28 Nov 2017 09:38:34 AM CST
 ************************************************************************/

#include <stdlib.h>
#include <assert.h>
#include "SymbolTable.h"
#include "SymbolStack.h"

SymbolStack newSymbolStack(){
    return malloc(sizeof(struct SymbolStack_));
}

void pushIntoSymbolStack(SymbolStack symbolStack, SymbolNode symbolNode){
    if(symbolStack->head == NULL){
        symbolStack->head = symbolNode;
    }
    else{
        SymbolNode tmp = symbolStack->head;
        symbolStack->head = symbolNode;
        symbolNode->tail = tmp;
    }
}

SymbolNode popFromSymbolStack(SymbolStack symbolStack){
    assert(symbolStack->head != NULL);
    SymbolNode ret = symbolStack->head;
    symbolStack->head = ret->tail;
    return ret;
}

SymbolNode getTopFromSymbolStack(SymbolStack symbolStack){
    assert(symbolStack->head != NULL);
    return symbolStack->head;
}
