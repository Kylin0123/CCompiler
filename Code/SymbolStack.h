/*************************************************************************
	> File Name: SymbolStack.h
	> Author: 
	> Mail: 
	> Created Time: Tue 28 Nov 2017 09:28:02 AM CST
 ************************************************************************/

#ifndef _SYMBOLSTACK_H
#define _SYMBOLSTACK_H

typedef struct SymbolStack_* SymbolStack;

struct SymbolStack_{
    SymbolNode head;
};

SymbolStack newSymbolStack();
void pushIntoSymbolStack(SymbolStack symbolStack, SymbolNode symbolNode);
SymbolNode popFromSymbolStack(SymbolStack symbolStack);
SymbolNode getTopFromSymbolStack(SymbolStack symbolStack);
void clearSymbolStack(SymbolStack symbolStack);
int getLengthOfSymbolStack(SymbolStack symbolStack);

#endif
