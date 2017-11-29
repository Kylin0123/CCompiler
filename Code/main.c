#include <stdio.h>
#include <assert.h>
#include "Type.h"
#include "NodeTree.h"
#include "SymbolTable.h"
#include "TypeStack.h"
#include "SymbolStack.h"

extern void yyrestart(FILE*);
extern void yyparse();
SymbolTable symbolTable;
SymbolTable structSymbolTable;
TypeStack typeStack;
SymbolStack paraStack;
TypeStack structStack;
SymbolStack symbolStack;

int main(int argc, char** argv)
{
    if (argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    symbolTable = newSymbolTable();
    structSymbolTable = newSymbolTable();
    typeStack = newTypeStack();
    paraStack = newSymbolStack();
    structStack = newTypeStack();
    symbolStack = newSymbolStack();
    yyrestart(f);
    yyparse();
    return 0;
}
