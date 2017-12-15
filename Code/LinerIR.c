#include "LinerIR.h"
#include <assert.h>

Operand newOperand(){
    return malloc(sizeof(struct Operand_));
}

InterCode newInterCode(){
    return malloc(sizeof(struct InterCode_));
}

void printOperand(Operand operand){
    if(operand == NULL) return;
    switch(operand->kind){
        case VARIABLE:
            {
                printf("v%d", operand->no);
                break;
            }
        case TEMP:
            {
                printf("t%d", operand->no);
                break;
            }
        case STRING:
            {
                printf("%s", operand->str);
                break;
            }
        case CONSTANT:
            {
                printf("#%d", operand->no);
                break;
            }
        case ADDRESS:
            {
                printf("please implement me\n");
                break;
            }
        default:
            break;
    }
}

InterCodes newInterCodes(){
    InterCodes interCodes = malloc(sizeof(struct InterCodes_));
    interCodes->prev = NULL;
    interCodes->next = NULL;
    return interCodes;
}

InterCodes genLabelCode(int label_no){
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
    InterCodes code_head= code;
    while(code_head->prev != NULL) 
        code_head= code_head->prev;
    return code_head;
}

InterCodes mergeCode(InterCodes code1, InterCodes code2){
    if(code1 == NULL) return code2;
    if(code2 == NULL) return code1;
    assert(code1 != NULL && code2 != NULL);
    InterCodes code1_tail = tail(code1);
    InterCodes code2_head = head(code2);
    code1_tail->next = code2_head;
    code2_head->prev = code1_tail;
    return head(code1);
}

void printInterCodes(InterCodes interCodes){
    if(interCodes == NULL) return;
    InterCodes first = head(interCodes);
    for(; first != NULL; first = first->next){
        InterCode interCode = first->code;
        switch(interCode->kind){
            case ASSIGN:
                {
                    printOperand(interCode->assign.left);
                    printf(" := ");
                    printOperand(interCode->assign.right);
                    printf("\n");
                    break;
                }
            case ADD:
                {
                    break;
                }
            case SUB:
                {
                    break;
                }
            case MINUS_:
                {
                    printOperand(interCode->binop.result);
                    printf(" := ");
                    printOperand(interCode->binop.op1);
                    printf(" - ");
                    printOperand(interCode->binop.op2);
                    printf("\n");
                    break;
                }
            case MUL:
                {
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
                    printf("FUNCTION ");
                    printOperand(interCode->funcdec.op);
                    printf(" :\n");
                    break;
                }
            case FUNCCALL:
                {
                    break;
                }
            case FUNCCALLREAD:
                {
                    printf("READ ");
                    printOperand(interCode->funccallread.op);
                    printf("\n");
                    break;
                }
            case FUNCCALLWRITE:
                {
                    printf("WRITE ");
                    printOperand(interCode->funccallwrite.op);
                    printf("\n");
                    break;
                }
            case SPFUNCCALL:
                {
                    break;
                }
            case ARG:
                {
                    break;
                }
            case LABEL:
                {
                    printf("LABEL label%d :\n",interCode->label.no);
                    break;
                }
            case COND:
                {
                    printf("IF ");
                    printOperand(interCode->cond.v1);
                    printf(" %s ", interCode->cond.op);
                    printOperand(interCode->cond.v2);
                    printf(" GOTO label%d\n", interCode->cond.label_no);

                    break;
                }
            case GOTO:
                {
                    printf("GOTO label%d\n",interCode->label.no);
                    break;
                }
            case RETURN_:
                {
                    printf("RETURN ");
                    printOperand(interCode->return_.op);
                    printf("\n");
                    break;
                }
            case PARAM:
                {
                    break;
                }
            case DEC:
                {
                    break;
                }
            case ADDR_ASSIGN:
                {
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


