#ifndef _LINERIR_H
#define _LINERIR_H

#include <malloc.h>

typedef struct Operand_* Operand;
struct Operand_ {
    enum { VARIABLE, TEMP, STRING, CONSTANT, ADDRESS } kind;
    union {
        int no;
        char* str;
    };
};
Operand newOperand();


typedef struct InterCode_* InterCode;
 struct InterCode_ 
 {
     enum {ASSIGN,ADD,SUB,MINUS_,MUL,DIV_,EMPTY,FUNCDEC,FUNCCALL,SPFUNCCALL,ARG,LABEL,COND,GOTO,RETURN_,PARAM,DEC,ADDR_ASSIGN,ARRAY_ASSIGN, FUNCCALLREAD, FUNCCALLWRITE} kind;
     union {
         struct { Operand right, left; } assign;
         struct { Operand result, op1, op2; } binop;
         struct { Operand op; } return_;
         struct { int no; } label;
         struct { Operand v1, v2; char* op; int label_no; } cond;
         struct { int label_no; } goto_;
         struct { Operand op; } funcdec;
         struct { Operand op; } funccallread;
         struct { Operand op; } funccallwrite;
         struct { Operand left, right; } funccall;
         struct { Operand op; } arg;
     };
 };

InterCode newInterCode();

typedef struct InterCodes_* InterCodes;
struct InterCodes_ { InterCode code; InterCodes prev, next; };
void printOperand(Operand operand);
InterCodes newInterCodes();
InterCodes genLabelCode(int label_no);
InterCodes genGotoCode(int label_no);
InterCodes tail(InterCodes code);
InterCodes head(InterCodes code);
InterCodes mergeCode(InterCodes code1, InterCodes code2);
void printInterCodes(InterCodes interCodes);


#endif
