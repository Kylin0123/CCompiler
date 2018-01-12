#include "LinerIR.h"
#include <assert.h>

Operand newOperand(){
    return malloc(sizeof(struct Operand_));
}

InterCode newInterCode(){
    return malloc(sizeof(struct InterCode_));
}

void printOperand(FILE* f, Operand operand){
    if(operand == NULL) return;
    switch(operand->kind){
        case VARIABLE:
            {
                fprintf(f, "v%d", operand->no);
                break;
            }
        case TEMP:
            {
                fprintf(f, "t%d", operand->no);
                break;
            }
        case STRING:
            {
                fprintf(f, "%s", operand->str);
                break;
            }
        case CONSTANT:
            {
                fprintf(f, "#%d", operand->no);
                break;
            }
        case ADDRESS:
            {
                fprintf(stderr, "please implement me\n");
                assert(0);
                break;
            }
        default:
            break;
    }
}

InterCodes newInterCodes(){
    InterCodes interCodes = malloc(sizeof(struct InterCodes_));
    interCodes->code = newInterCode();
    interCodes->prev = NULL;
    interCodes->next = NULL;
    return interCodes;
}

InterCodes genLabelCode(int label_no){
    assert(label_no > 0);
    InterCodes code_label = newInterCodes();
    code_label->code = newInterCode();
    code_label->code->kind = LABEL;
    code_label->code->label.no = label_no;
    return code_label; 
}

InterCodes genGotoCode(int label_no){
    InterCodes code_goto = newInterCodes();
    code_goto->code = newInterCode();
    code_goto->code->kind = GOTO;
    code_goto->code->goto_.label_no = label_no;
    return code_goto;
}

InterCodes tail(InterCodes code){
    assert(code != NULL);
    InterCodes code_tail = code;
    while(code_tail->next != NULL) 
        code_tail = code_tail->next;
    return code_tail;
}

InterCodes head(InterCodes code){
    assert(code != NULL);
    InterCodes code_head = code;
    while(code_head->prev != NULL) 
        code_head = code_head->prev;
    return code_head;
}

InterCodes mergeCode(InterCodes code1, InterCodes code2){
    if(code1 == NULL) return code2;
    if(code2 == NULL) return code1;
    assert(code1 != NULL && code2 != NULL);
    assert(code1 != code2);
    InterCodes code1_tail = tail(code1);
    InterCodes code2_head = head(code2);
    assert(code1_tail != NULL && code2_head != NULL);
    code1_tail->next = code2_head;
    code2_head->prev = code1_tail;
    InterCodes code = head(code1);
    return code;
}

void printInterCodes(FILE* f, InterCodes interCodes){
    if(interCodes == NULL) return;
    InterCodes first = head(interCodes);
    for(; first != NULL; first = first->next){
        InterCode interCode = first->code;
        switch(interCode->kind){
            case ASSIGN:
                {
                    printOperand(f, interCode->assign.left);
                    fprintf(f, " := ");
                    printOperand(f, interCode->assign.right);
                    fprintf(f, "\n");
                    break;
                }
            case ASSIGN_STAR:
                {
                    printOperand(f, interCode->assign.left);
                    fprintf(f, " := *");
                    printOperand(f, interCode->assign.right);
                    fprintf(f, "\n");
                    break;
                }
            case ASSIGN_ADDR:
                {
                    printOperand(f, interCode->assign.left);
                    fprintf(f, " := &");
                    printOperand(f, interCode->assign.right);
                    fprintf(f, "\n");
                    break;
                }
            case STAR_ASSIGN:
                {
                    fprintf(f, "*");
                    printOperand(f, interCode->assign.left);
                    fprintf(f, " := ");
                    printOperand(f, interCode->assign.right);
                    fprintf(f, "\n");
                    break;
                }
            case PLUS_:
                {
                    printOperand(f, interCode->binop.result);
                    fprintf(f, " := ");
                    printOperand(f, interCode->binop.op1);
                    fprintf(f, " + ");
                    printOperand(f, interCode->binop.op2);
                    fprintf(f, "\n");
                    break;
                }
            case MINUS_:
                {
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
                    break;
                }
            case EMPTY:
                {
                    break;
                }
            case FUNCDEC:
                {
                    fprintf(f, "FUNCTION ");
                    printOperand(f, interCode->funcdec.op);
                    fprintf(f, " :\n");
                    break;
                }
            case FUNCCALL:
                {
                    printOperand(f, interCode->funccall.left);
                    fprintf(f, " := CALL ");
                    printOperand(f, interCode->funccall.right);
                    fprintf(f, "\n");
                    break;
                }
            case FUNCCALLREAD:
                {
                    fprintf(f, "READ ");
                    printOperand(f, interCode->funccallread.op);
                    fprintf(f, "\n");
                    break;
                }
            case FUNCCALLWRITE:
                {
                    fprintf(f, "WRITE ");
                    printOperand(f, interCode->funccallwrite.op);
                    fprintf(f, "\n");
                    break;
                }
            case SPFUNCCALL:
                {
                    break;
                }
            case ARG:
                {
                    fprintf(f, "ARG ");
                    printOperand(f, interCode->arg.op);
                    fprintf(f, "\n");
                    break;
                }
            case LABEL:
                {
                    fprintf(f, "LABEL label%d :\n",interCode->label.no);
                    break;
                }
            case COND:
                {
                    fprintf(f, "IF ");
                    printOperand(f, interCode->cond.v1);
                    fprintf(f, " %s ", interCode->cond.op);
                    printOperand(f, interCode->cond.v2);
                    assert(interCode->cond.label_no > 0);
                    fprintf(f, " GOTO label%d\n", interCode->cond.label_no);

                    break;
                }
            case GOTO:
                {
                    fprintf(f, "GOTO label%d\n",interCode->label.no);
                    break;
                }
            case RETURN_:
                {
                    fprintf(f, "RETURN ");
                    printOperand(f, interCode->return_.op);
                    fprintf(f, "\n");
                    break;
                }
            case PARAM:
                {
                    fprintf(f, "PARAM ");
                    printOperand(f, interCode->param.op);
                    fprintf(f, "\n");
                    break;
                }
            case DEC:
                {
                    fprintf(f, "DEC ");
                    printOperand(f, interCode->dec.op);
                    fprintf(f, " %d\n", interCode->dec.size);
                    break;
                }
            case ADDR_ASSIGN:
                {
                    printOperand(f, interCode->assign.left);
                    fprintf(f, " := &");
                    printOperand(f, interCode->assign.right);
                    fprintf(f, "\n");
                    break;
                }
            case ARRAY_ASSIGN:
                {
                    break;
                }
            default:
                break;
        }
    }
}


