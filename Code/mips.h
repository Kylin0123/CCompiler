#undef _MIPS_H
#define _MIPS_H 

#include "LinerIR.h"

void genMIPS(FILE* out, InterCodes code);

typedef struct MIPS_* MIPS;

struct MIPS_ {
    union {
        struct {
            Operand zero;       //0
            Operand at;         //AT
            Operand v[2];       //Values
            Operand a[4];       //Arguments
            Operand t[8];       //Temporaries
            Operand s[8];       //Saved Values
            Operand t_8, t_9;   //t8, t9
            Operand k[2];       //Interrupt
            Operand gp;         //Global Pointer
            Operand sp;         //Stack Pointer
            Operand fp;         //s8(mips) or Frame Pointer(gcc)
            Operand ra;         //Return Address

        };
        struct {
            Operand reg[32];
        };
    };
};

MIPS newMIPS();

#undef _MIPS_H
