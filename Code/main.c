#include <stdio.h>
#include <assert.h>
#include "Type.h"
#include "NodeTree.h"
#include "SymbolTable.h"
#include "TypeStack.h"
#include "SymbolStack.h"
#include "mips.h"

extern void yyrestart(FILE*);
extern void yyparse();
SymbolTable symbolTable;
SymbolTable structSymbolTable;
TypeStack typeStack;
SymbolStack paraStack;
TypeStack structStack;
SymbolStack symbolStack;
InterCodes code;
FILE* out;

int main(int argc, char** argv)
{
    FILE* f = fopen(argv[1], "r");
    out = fopen(argv[2], "w");
    if (!f)
    {
        printf("parser:\033[0;31m fatal error:\033[0m no input file\n");
        printf("compilation terminated.\n");
        return 1;
    }
    if (!out)
    {
        printf("parser:\033[0;31m fatal error:\033[0m no output file\n");
        printf("compilation terminated.\n");
        return 1;
    }
    symbolTable = newSymbolTable();
    structSymbolTable = newSymbolTable();
    typeStack = newTypeStack();
    paraStack = newSymbolStack();
    structStack = newTypeStack();
    symbolStack = newSymbolStack();

    //start to parse
    yyrestart(f);
    yyparse();
    //get code

    genMIPS(out, code);

    return 0;
}
