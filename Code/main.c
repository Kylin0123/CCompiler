#include <stdio.h>
#include <assert.h>
#include "NodeTree.h"
#include "SymbolTable.h"

extern void yyrestart(FILE*);
extern void yyparse();
SymbolTable symbolTable;
SymbolTable structSymbolTable;
TypeStack typeStack;
TypeStack structStack;

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
    structStack = newTypeStack();
    yyrestart(f);
    yyparse();
    return 0;
}
