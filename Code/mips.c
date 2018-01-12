#include "mips.h"
#include <assert.h>
#include <string.h>
#include "Type.h"

MIPS newMIPS() {
    return malloc(sizeof(struct MIPS_));
}

MIPS mips = NULL;

#define SAVE(OP, N)\
    fprintf(f, "sw $t"#N", %d($sp)\n", getSymbolOffset(OP))

#define LOAD(OP, N)\
    fprintf(f, "lw $t"#N", %d($sp)\n", getSymbolOffset(OP))


char* getReg(FILE* f, Operand op){

    //assert(0);
    return NULL;
    /*
    switch(op->kind) {
        case TEMP:
            {
                return "$t0";
                break;
            }
        case VARIABLE:
            {
                break;
            }
        default:
            break;
    }
    return "$?";
    */
}

bool isTempOrVariable(Operand op){
    assert(op != NULL);
    return op->kind == TEMP || op->kind == VARIABLE;
}

//Record the local symbols in functions.
struct localSymbol {
    Operand op;
    int offset;
};

struct localSymbolTable {
    int size;
    struct localSymbol localSymbol[1024];
} localSymbolTable;

#define GET_TABLE_ENTRY(localSymbol, N)\
    localSymbol##Table.localSymbol[N]

void printLocalSymbolTable(){
    
    printf("***************\n");
    for(int i = 0; i < localSymbolTable.size; i++){
        printf("%d\t", i);
        printOperand(stdout, GET_TABLE_ENTRY(localSymbol, i).op);
        printf("\t %d\n", GET_TABLE_ENTRY(localSymbol, i).offset);
    }
    printf("***************\n");

}

bool isEqualLocalSymbol(Operand op1, Operand op2) {
    if(op1 == op2) return true;
    if(op1->kind != op2->kind) return false;
    if(op1->kind == TEMP || op1->kind == VARIABLE){
        if(op1->no == op2->no) {
            return true;
        }
        else {
            return false;
        }
    }
    assert(0);
    return false;
}

bool isExistLocalSymbol(Operand op) {
    assert(op != NULL);
    if(op->kind == CONSTANT) 
        return true;
    for(int i = 0; i < localSymbolTable.size; i++){
        if(isEqualLocalSymbol(op, localSymbolTable.localSymbol[i].op)){
            return true;
        }
    }
    return false;
}

#define TABLE_TOP(localSymbol)\
    localSymbol##Table.localSymbol[localSymbol##Table.size]

#define TABLE_SIZE_ADD(localSymbol, N)\
    localSymbol##Table.size += N

#define TABLE_PUSH(localSymbol, OP, OFFSET)\
    TABLE_TOP(localSymbol).op = OP;\
    TABLE_TOP(localSymbol).offset = OFFSET;\
    TABLE_SIZE_ADD(localSymbol, 1);

void addNewLocalSymbol(Operand op) {
    /*
    TABLE_TOP(localSymbol).op = op;
    TABLE_TOP(localSymbol).offset = localSymbolTable.size;
    TABLE_SIZE_ADD(localSymbol, 1);

    the following macro is equal to the above
    */
    TABLE_PUSH(localSymbol, op, (localSymbolTable.size * 4));    
}

int getSymbolOffset(Operand op) {
    for(int i = 0; i < localSymbolTable.size; i++) {
        if(isEqualLocalSymbol(op, GET_TABLE_ENTRY(localSymbol, i).op)) {
            int offset = GET_TABLE_ENTRY(localSymbol, i).offset;
            return offset;
        }
    }
    assert(0);
    return 0;
}

int getStackSize(InterCodes funCode) {

    assert(funCode != NULL);

    int size = 0;

    for(; funCode != NULL && funCode->code->kind != FUNCDEC; funCode = funCode->next) {
        switch(funCode->code->kind){
            /*assign*/
            case ASSIGN:
            case ASSIGN_STAR:
            case ASSIGN_ADDR:
            case STAR_ASSIGN:
            case ADDR_ASSIGN:
                {
                    Operand opl = funCode->code->assign.left;
                    Operand opr = funCode->code->assign.right;

                    if(!isExistLocalSymbol(opr)){
                        printLocalSymbolTable();
                        printOperand(stdout, opr);
                        printf("\n");
                        assert(0);
                    }

                    if(!isExistLocalSymbol(opl)){
                        addNewLocalSymbol(opl);
                        size += 4;
                    }
                    break;
                }

            /*binop*/
            case PLUS_:
            case MINUS_:
            case MUL:
            case DIV_:
                {
                    Operand result = funCode->code->binop.result;
                    Operand op1 = funCode->code->binop.op1;
                    Operand op2 = funCode->code->binop.op2;
                    assert(isExistLocalSymbol(op1));
                    assert(isExistLocalSymbol(op2));
                    if(!isExistLocalSymbol(result)) {
                        addNewLocalSymbol(result);
                        size += 4;
                    }

                    break;
                }
                
            /*1 op*/
            case FUNCCALL:
                {
                    //TODO
                    break;
                }
            case FUNCCALLREAD:
                {
                    Operand op = funCode->code->funccallread.op;
                    if(!isExistLocalSymbol(op)){
                        addNewLocalSymbol(op);
                        size += 4;
                    }
                    break;
                }
            case FUNCCALLWRITE:
                {
                    assert(isExistLocalSymbol(funCode->code->funccallwrite.op));
                    break;
                }
            /*multiple space*/
            case DEC: 
            
            /*0 op*/
            case ARG:
            case LABEL:
            case COND:
            case RETURN_:
            case PARAM:


            default:
                break;
        }
    }
    
    //printLocalSymbolTable();

    return size;

}

int printMIPSfuncdec(FILE* f, Operand operand, InterCodes funCode) {
    
    assert(operand->kind == STRING);

    fprintf(f, "%s", operand->str);
    fprintf(f, ":\n");

    int stackSize = getStackSize(funCode->next);

    fprintf(f, "addi $sp, $sp, -%d\n", stackSize);

    return stackSize;

}

void printMIPSinit(FILE* f){
    
    fprintf(f, ".data\n");
    fprintf(f, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(f, "_ret: .asciiz \"\\n\"\n");
    fprintf(f, ".globl main\n");
    fprintf(f, ".text\n");
    fprintf(f, "read:\n");
    fprintf(f, "li $v0, 4\n");
    fprintf(f, "la $a0, _prompt\n");
    fprintf(f, "syscall\n");
    fprintf(f, "li $v0, 5\n");
    fprintf(f, "syscall\n");
    fprintf(f, "jr $ra\n");
    fprintf(f, "\n");
    fprintf(f, "write:\n");
    fprintf(f, "li $v0, 1\n");
    fprintf(f, "syscall\n");
    fprintf(f, "li $v0, 4\n");
    fprintf(f, "la $a0, _ret\n");
    fprintf(f, "syscall\n");
    fprintf(f, "move $v0, $0\n");
    fprintf(f, "jr $ra\n");
    fprintf(f, "\n");
}

void initLocalSymbolTable() {
    localSymbolTable.size = 0;
}

void printMIPS(FILE* f, InterCodes interCodes){

    if(interCodes == NULL) return;

    mips = newMIPS();

    printMIPSinit(f);
    initLocalSymbolTable();

    int stackSize = 0; //Record the stack size of a function.

    InterCodes first = head(interCodes);
    for(; first != NULL; first = first->next){
        InterCode interCode = first->code;
        switch(interCode->kind){
            case ASSIGN:
                {
                    Operand opl = interCode->assign.left;
                    Operand opr = interCode->assign.right;
                    if(isTempOrVariable(opl) && opr->kind == CONSTANT){
                        fprintf(f, "li $t0, %d\n", opr->no); 
                        SAVE(opl, 0);
                        //fprintf(f, "sw $t0, %d($sp)\n", getSymbolOffset(opl));
                    }
                    else if(isTempOrVariable(opl) && isTempOrVariable(opr)){
                        fprintf(f, "lw $t0, %d($sp)\n", getSymbolOffset(opr));
                        fprintf(f, "move $t1, $t0\n");
                        SAVE(opl, 0);
                        //fprintf(f, "sw $t1, %d($sp)\n", getSymbolOffset(opl));
                    }
                    else{
                        assert(0);
                    }
                    break;
                }
            case ASSIGN_STAR:
                {
                    //TODO
                    printOperand(f, interCode->assign.left);
                    fprintf(f, " := *");
                    printOperand(f, interCode->assign.right);
                    fprintf(f, "\n");
                    break;
                }
            case ASSIGN_ADDR:
                {
                    //TODO
                    printOperand(f, interCode->assign.left);
                    fprintf(f, " := &");
                    printOperand(f, interCode->assign.right);
                    fprintf(f, "\n");
                    break;
                }
            case STAR_ASSIGN:
                {
                    //TODO
                    fprintf(f, "*");
                    printOperand(f, interCode->assign.left);
                    fprintf(f, " := ");
                    printOperand(f, interCode->assign.right);
                    fprintf(f, "\n");
                    break;
                }
            case PLUS_:
                {
                    Operand result = interCode->binop.result;
                    Operand op1 = interCode->binop.op1;
                    Operand op2 = interCode->binop.op2;

                    if(isTempOrVariable(result) 
                    && isTempOrVariable(op1) 
                    && op2->kind == CONSTANT){
                        LOAD(op1, 0);
                        fprintf(f, "addi $t1, $t0, %d\n", op2->no);
                        SAVE(op2, 1);
                    }
                    else if(isTempOrVariable(result)
                    && isTempOrVariable(op1)
                    && isTempOrVariable(op2)){
                        LOAD(op1, 0);
                        LOAD(op2, 1);
                        fprintf(f, "add $t2, $t0, $t1\n");
                        SAVE(result, 2);
                    }
                    else{
                        assert(0);
                        /*
                        printOperand(f, interCode->binop.result);
                        fprintf(f, " := ");
                        printOperand(f, interCode->binop.op1);
                        fprintf(f, " + ");
                        printOperand(f, interCode->binop.op2);
                        fprintf(f, "\n");
                        */
                    }
                    break;
                }
            case MINUS_:
                {
                    //TODO
                    printOperand(f, interCode->binop.result);
                    fprintf(f, " := ");
                    printOperand(f, interCode->binop.op1);
                    fprintf(f, " - ");
                    printOperand(f, interCode->binop.op2);
                    fprintf(f, "\n");
                    break;
                }
            case MUL:
                {
                    //TODO
                    printOperand(f, interCode->binop.result);
                    fprintf(f, " := ");
                    printOperand(f, interCode->binop.op1);
                    fprintf(f, " * ");
                    printOperand(f, interCode->binop.op2);
                    fprintf(f, "\n");
                    break;
                }
            case DIV_:
                {
                    //TODO
                    break;
                }
            case EMPTY:
                {
                    break;
                }
            case FUNCDEC:
                {
                    stackSize = printMIPSfuncdec(f, interCode->funcdec.op, first);
                    break;
                }
            case FUNCCALL:
                {
                    //TODO
                    printOperand(f, interCode->funccall.left);
                    fprintf(f, " := CALL ");
                    printOperand(f, interCode->funccall.right);
                    fprintf(f, "\n");
                    break;
                }
            case FUNCCALLREAD:
                {
                    fprintf(f, "addi $sp, $sp, -4\n");
                    fprintf(f, "sw $ra, 0($sp)\n");
                    fprintf(f, "jal read\n");
                    fprintf(f, "lw $ra, 0($sp)\n");
                    fprintf(f, "addi $sp, $sp, 4\n");
                    fprintf(f, "move $t0, $v0\n");
                    SAVE(interCode->funccallread.op, 0);
                    //fprintf(f, "sw $t0, %d($sp)\n", getSymbolOffset(interCode->funccallread.op));
                    //getReg(f, interCode->funccallread.op);
                    /*
                    fprintf(f, "READ ");
                    printOperand(f, interCode->funccallread.op);
                    fprintf(f, "\n");
                    */
                    break;
                }
            case FUNCCALLWRITE:
                {
                    LOAD(interCode->funccallwrite.op, 0);
                    fprintf(f, "move $a0, $t0\n");
                    fprintf(f, "addi $sp, $sp, -4\n");
                    fprintf(f, "sw $ra, 0($sp)\n");
                    fprintf(f, "jal write\n");
                    fprintf(f, "lw $ra, 0($sp)\n");
                    fprintf(f, "addi $sp, $sp, 4\n");
                    /*
                    fprintf(f, "WRITE ");
                    printOperand(f, interCode->funccallwrite.op);
                    fprintf(f, "\n");
                    */
                    break;
                }
            case SPFUNCCALL:
                {
                    break;
                }
            case ARG:
                {
                    //TODO
                    fprintf(f, "ARG ");
                    printOperand(f, interCode->arg.op);
                    fprintf(f, "\n");
                    break;
                }
            case LABEL:
                {
                    fprintf(f, "label%d:\n", interCode->label.no);
                    break;
                }
            case COND:
                {
                    Operand op1 = interCode->cond.v1;
                    char* op = interCode->cond.op;
                    Operand op2 = interCode->cond.v2;
                    int label_no = interCode->cond.label_no;
                    if(!strcmp(op, "<")){
                        LOAD(op1, 0);
                        LOAD(op2, 1);
                        fprintf(f, "blt $t0, $t1, label%d\n", label_no);
                    }
                    else if(!strcmp(op, ">")){
                        LOAD(op1, 0);
                        LOAD(op2, 1);
                        fprintf(f, "bgt $t0, $t1, label%d\n", label_no);
                    }
                    else if(!strcmp(op, "=")){
                        LOAD(op1, 0);
                        LOAD(op2, 1);
                        fprintf(f, "beq $t0, $t1, label%d\n", label_no);
                    }
                    else{
                        assert(0);
                    }
                    /*
                    fprintf(f, "IF ");
                    printOperand(f, interCode->cond.v1);
                    fprintf(f, " %s ", interCode->cond.op);
                    printOperand(f, interCode->cond.v2);
                    assert(interCode->cond.label_no > 0);
                    fprintf(f, " GOTO label%d\n", interCode->cond.label_no);
                    */
                    break;
                }
            case GOTO:
                {
                    fprintf(f, "j label%d\n", interCode->label.no);
                    //fprintf(f, "GOTO label%d\n",interCode->label.no);
                    break;
                }
            case RETURN_:
                {
                    LOAD(interCode->return_.op, 0);
                    fprintf(f, "move $v0, $t0\n");
                    fprintf(f, "addi $sp, $sp, %d\n", stackSize);
                    //TODO:pop frame
                    fprintf(f, "jr $ra\n");
                    /*
                    fprintf(f, "RETURN ");
                    printOperand(f, interCode->return_.op);
                    fprintf(f, "\n");
                    */
                    break;
                }
            case PARAM:
                {
                    //TODO
                    fprintf(f, "PARAM ");
                    printOperand(f, interCode->param.op);
                    fprintf(f, "\n");
                    break;
                }
            case DEC:
                {
                    //TODO
                    fprintf(f, "DEC ");
                    printOperand(f, interCode->dec.op);
                    fprintf(f, " %d\n", interCode->dec.size);
                    break;
                }
            case ADDR_ASSIGN:
                {
                    //TODO
                    printOperand(f, interCode->assign.left);
                    fprintf(f, " := &");
                    printOperand(f, interCode->assign.right);
                    fprintf(f, "\n");
                    break;
                }
            case ARRAY_ASSIGN:
                {
                    //TODO
                    break;
                }
            default:
                break;
        }
    }
}


void genMIPS(FILE* out, InterCodes code){
    printMIPS(stdout, code); 
}



