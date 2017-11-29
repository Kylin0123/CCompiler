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
    assert(symbolStack != NULL);
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
    assert(symbolStack != NULL);
    if(symbolStack->head == NULL)
        return NULL;
    SymbolNode ret = symbolStack->head;
    symbolStack->head = ret->tail;
    return ret;
}

SymbolNode getTopFromSymbolStack(SymbolStack symbolStack){
    assert(symbolStack != NULL);
    return symbolStack->head;
}

void clearSymbolStack(SymbolStack symbolStack){
    //NOTE: memory leak
    symbolStack->head = NULL;
}

int getLengthOfSymbolStack(SymbolStack symbolStack){
    int len = 0;
    SymbolNode head = symbolStack->head;
    for(; head != NULL; head = head->tail)
        len++;
    return len;
}
