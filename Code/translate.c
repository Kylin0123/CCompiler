#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "translate.h"
#include "LinerIR.h"
#include "NodeTree.h"
#include "SymbolTable.h"

#define CHECK(NODE)\
        assert(!strcmp(node->tag_name, #NODE))

#define MATCH1(CHILD1)\
        if(node->child != NULL && !strcmp(node->child->tag_name, #CHILD1))

#define MATCH2(CHILD1, CHILD2)\
        if(node->child != NULL\
           && !strcmp(node->child->tag_name, #CHILD1)\
           && node->child->sibling != NULL\
           && !strcmp(node->child->sibling->tag_name, #CHILD2)\
          )

#define MATCH3(CHILD1, CHILD2, CHILD3)\
        if(node->child != NULL\
           && !strcmp(node->child->tag_name, #CHILD1)\
           && node->child->sibling != NULL\
           && !strcmp(node->child->sibling->tag_name, #CHILD2)\
           && node->child->sibling->sibling != NULL\
           && !strcmp(node->child->sibling->sibling->tag_name, #CHILD3)\
          )

#define MATCH4(CHILD1, CHILD2, CHILD3, CHILD4)\
        if(node->child != NULL\
           && !strcmp(node->child->tag_name, #CHILD1)\
           && node->child->sibling != NULL\
           && !strcmp(node->child->sibling->tag_name, #CHILD2)\
           && node->child->sibling->sibling != NULL\
           && !strcmp(node->child->sibling->sibling->tag_name, #CHILD3)\
           && node->child->sibling->sibling->sibling != NULL\
           && !strcmp(node->child->sibling->sibling->sibling->tag_name, #CHILD4)\
          )

#define MATCH5(CHILD1, CHILD2, CHILD3, CHILD4, CHILD5)\
        if(node->child != NULL\
           && !strcmp(node->child->tag_name, #CHILD1)\
           && node->child->sibling != NULL\
           && !strcmp(node->child->sibling->tag_name, #CHILD2)\
           && node->child->sibling->sibling != NULL\
           && !strcmp(node->child->sibling->sibling->tag_name, #CHILD3)\
           && node->child->sibling->sibling->sibling != NULL\
           && !strcmp(node->child->sibling->sibling->sibling->tag_name, #CHILD4)\
           && node->child->sibling->sibling->sibling->sibling != NULL\
           && !strcmp(node->child->sibling->sibling->sibling->sibling->tag_name, #CHILD5)\
          )

#define MATCH6(CHILD1, CHILD2, CHILD3, CHILD4, CHILD5, CHILD6)\
        if(node->child != NULL\
           && !strcmp(node->child->tag_name, #CHILD1)\
           && node->child->sibling != NULL\
           && !strcmp(node->child->sibling->tag_name, #CHILD2)\
           && node->child->sibling->sibling != NULL\
           && !strcmp(node->child->sibling->sibling->tag_name, #CHILD3)\
           && node->child->sibling->sibling->sibling != NULL\
           && !strcmp(node->child->sibling->sibling->sibling->tag_name, #CHILD4)\
           && node->child->sibling->sibling->sibling->sibling != NULL\
           && !strcmp(node->child->sibling->sibling->sibling->sibling->tag_name, #CHILD5)\
           && node->child->sibling->sibling->sibling->sibling->sibling != NULL\
           && !strcmp(node->child->sibling->sibling->sibling->sibling->sibling->tag_name, #CHILD6)\
          )

#define MATCH7(CHILD1, CHILD2, CHILD3, CHILD4, CHILD5, CHILD6, CHILD7)\
        if(node->child != NULL\
           && !strcmp(node->child->tag_name, #CHILD1)\
           && node->child->sibling != NULL\
           && !strcmp(node->child->sibling->tag_name, #CHILD2)\
           && node->child->sibling->sibling != NULL\
           && !strcmp(node->child->sibling->sibling->tag_name, #CHILD3)\
           && node->child->sibling->sibling->sibling != NULL\
           && !strcmp(node->child->sibling->sibling->sibling->tag_name, #CHILD4)\
           && node->child->sibling->sibling->sibling->sibling != NULL\
           && !strcmp(node->child->sibling->sibling->sibling->sibling->tag_name, #CHILD5)\
           && node->child->sibling->sibling->sibling->sibling->sibling != NULL\
           && !strcmp(node->child->sibling->sibling->sibling->sibling->sibling->tag_name, #CHILD6)\
           && node->child->sibling->sibling->sibling->sibling->sibling->sibling != NULL\
           && !strcmp(node->child->sibling->sibling->sibling->sibling->sibling->sibling->tag_name, #CHILD7)\
          )


struct OperandList{
    Operand op;
    struct OperandList* next;
};


Node* parse_Program(Node* node);
Node* parse_ExtDefList(Node* node);
Node* parse_ExtDef(Node* node);
Node* parse_ExtDecList(Node* node);
Node* parse_Specifier(Node* node);
Node* parse_StructSpecifier(Node* node);
Node* parse_OptTag(Node* node);
Node* parse_Tag(Node* node);
Node* parse_VarDec(Node* node);
Node* parse_FunDec(Node* node);
Node* parse_VarList(Node* node);
Node* parse_ParamDec(Node* node);
Node* parse_CompSt(Node* node);
Node* parse_StmtList(Node* node);
Node* parse_Stmt(Node* node);
Node* parse_DefList(Node* node);
Node* parse_Def(Node* node);
Node* parse_DecList(Node* node);
Node* parse_Dec(Node* node);
Node* parse_Exp(Node* node, Operand place);
Node* parse_Cond(Node* node, int label_true, int label_false);
Node* parse_Args(Node* node, struct OperandList ** arg_list);

extern SymbolTable symbolTable;

char* local_sym[100];
int local_sym_size = 0;

Operand number(int n){
    Operand op = newOperand();
    op->kind = CONSTANT;
    op->no = n;
    return op;
}

Operand lookup(char* id){
    
    Type type = getSymbolType(symbolTable, id);
    
    Operand operand = newOperand();

    if(type == NULL || type->kind == BASIC){

        for(int i = 0; i < local_sym_size; i++){
            if(!strcmp(local_sym[i], id)){
                operand->kind = VARIABLE;
                operand->no = i+1;
                return operand;
            }
        }

        copystr(local_sym[local_sym_size], id);
        local_sym_size++;
        operand->kind = VARIABLE;
        operand->no= local_sym_size;
    }
    else if(type->kind == FUNCTION){
        operand->kind = STRING;
        operand->str = id;
    }
    else{
        assert(0); //please implement me
    }
    return operand;
}

Operand new_temp(){
    static int cnt_temp = 1;
    Operand operand = newOperand();
    operand->kind = TEMP;
    operand->no = cnt_temp; 
    cnt_temp++;
    return operand;
}

int new_label(){
    static int cnt_label = 0;
    cnt_label++;
    return cnt_label;
}

Node* parse_Program(Node* node){
    CHECK(Program);
    MATCH1(ExtDefList){
        node->code = parse_ExtDefList(node->child)->code;
        return node;
    }
    assert(0);
    return NULL;
}

Node* parse_ExtDefList(Node* node){
    CHECK(ExtDefList);
    MATCH2(ExtDef, ExtDefList){
        Node* n1 = parse_ExtDef(node->child);
        Node* n2 = parse_ExtDefList(node->child->sibling);
        node->code = mergeCode(n1->code, n2->code);
        return node;
    }
    /*empty*/
    node->code = NULL;
    return node;
}

Node* parse_ExtDef(Node* node){
    CHECK(ExtDef);
    MATCH3(Specifier, ExtDecList, SEMI){
        parse_Specifier(node->child);
        node->code = parse_ExtDecList(node->child->sibling)->code;
        return node;
    }
    MATCH2(Specifier, SEMI){
        node->code = parse_Specifier(node->child)->code;
        return node;
    }
    MATCH3(Specifier, FunDec, SEMI){
        parse_Specifier(node->child);
        node->code = parse_FunDec(node->child->sibling)->code;
        return node;
    }
    MATCH3(Specifier, FunDec, CompSt){
        InterCodes interCodes = newInterCodes();
        interCodes->code = newInterCode();
        interCodes->code->kind = FUNCDEC;
        interCodes->code->funcdec.op = newOperand();
        interCodes->code->funcdec.op->kind = STRING;
        interCodes->code->funcdec.op->str = node->child->sibling->id;
        
        parse_Specifier(node->child);
        InterCodes paraCode = parse_FunDec(node->child->sibling)->code;
        
        InterCodes compStCode = parse_CompSt(node->child->sibling->sibling)->code;
       
        node->code = mergeCode(mergeCode(interCodes, paraCode), compStCode);
        return node;
    }
    assert(0);
    return NULL;
}

Node* parse_ExtDecList(Node* node){
    MATCH1(VarDec){
        parse_VarDec(node->child);
        return node;
    }
    MATCH3(VarDec, COMMA, ExtDeclist){
        parse_VarDec(node->child);
        parse_ExtDecList(node->child->sibling->sibling);
        return node;
    }
    assert(0);
    return NULL;
}

Node* parse_Specifier(Node* node){
    MATCH1(TYPE){
        /*do something*/
        return node;
    }
    MATCH1(StructSpecifier){
        parse_StructSpecifier(node->child);
        return node;
    }
    assert(0);
    return NULL;
}

Node* parse_StructSpecifier(Node* node){
    MATCH5(STRUCT, OptTag, LC, DefList, RC){
        parse_OptTag(node->child->sibling);
        parse_DefList(node->child->sibling->sibling->sibling);
        return node;
    }
    MATCH2(STRUCT, Tag){
        parse_Tag(node->child->sibling);
        return node;
    }
    assert(0);
    return NULL;
}

Node* parse_OptTag(Node* node){
    MATCH1(ID){
        /*do something*/
        return node;
    }
    /*empty*/
    return node;
}

Node* parse_Tag(Node* node){
    MATCH1(ID){
        /*do something*/
        return node;
    }
    assert(0);
    return NULL;
}

Node* parse_VarDec(Node* node){
    MATCH1(ID){
        Operand op = lookup(node->child->id);
        InterCodes paramCode = newInterCodes();
        paramCode->code = newInterCode();
        paramCode->code->kind = PARAM;
        paramCode->code->param.op = op;

        node->code = paramCode;
        return node;
    }
    MATCH4(VarDec, LB, INT, RB){
        parse_VarDec(node->child);
        /*do something*/
        return node;
    }
    assert(0);
    return NULL;
}

Node* parse_FunDec(Node* node){
    MATCH4(ID, LP, VarList, RP){
        node->code = parse_VarList(node->child->sibling->sibling)->code;
        return node;
    }
    MATCH3(ID, LP, RP){
        node->code = NULL;
        return node;
    }
    assert(0);
    return NULL;
}

Node* parse_VarList(Node* node){
    MATCH3(ParamDec, COMMA, VarList){
        InterCodes code1 = parse_ParamDec(node->child)->code;
        InterCodes code2 = parse_VarList(node->child->sibling->sibling)->code;
        node->code = mergeCode(code1, code2);
        return node;
    }
    MATCH1(ParamDec){
        node->code = parse_ParamDec(node->child)->code;
        return node;
    }
    assert(0);
    return NULL;
}

Node* parse_ParamDec(Node* node){
    MATCH2(Specifier, VarDec){
        parse_Specifier(node->child);
        node->code = parse_VarDec(node->child->sibling)->code;
        return node;
    }
    assert(0);
    return NULL;
}

Node* parse_CompSt(Node* node){
    MATCH4(LC, DefList, StmtList, RC){
        Node* nodeDefList = parse_DefList(node->child->sibling);
        printInterCodes(nodeDefList->code);
        Node* nodeStmtList = parse_StmtList(node->child->sibling->sibling);
        node->code = mergeCode(nodeDefList->code, nodeStmtList->code);
        return node;
    }
    assert(0);
    return NULL;
}

Node* parse_StmtList(Node* node){
    MATCH2(Stmt, StmtList){
        Node* nodeStmt = parse_Stmt(node->child);
        Node* nodeStmtList = parse_StmtList(node->child->sibling);
        node->code = mergeCode(nodeStmt->code, nodeStmtList->code);
        return node;
    }
    /*empty*/
    node->code = NULL;
    return node;
}

Node* parse_Stmt(Node* node){
    MATCH2(Exp, SEMI){
        node->code = parse_Exp(node->child, NULL)->code;
        return node;
    }
    MATCH1(CompSt){
        node->code = parse_CompSt(node->child)->code;
        return node;
    }
    MATCH3(RETURN, Exp, SEMI){
        Operand t1 = new_temp();
        InterCodes code1 = parse_Exp(node->child->sibling, t1)->code;
        InterCodes code2 = newInterCodes();
        code2->code = newInterCode();
        code2->code->kind = RETURN_;
        code2->code->return_.op = t1;
        
        node->code = mergeCode(code1, code2);
        return node;
    }
    MATCH7(IF, LP, Exp, RP, Stmt, ELSE, Stmt){

        Node* nodeExp = node->child->sibling->sibling;
        Node* nodeStmt1 = node->child->sibling->sibling->sibling->sibling;
        Node* nodeStmt2 = node->child->sibling->sibling->sibling->sibling->sibling->sibling;

        int label1 = new_label();
        int label2 = new_label();
        int label3 = new_label();
        
        InterCodes code1 = parse_Cond(nodeExp, label1, label2)->code;
        InterCodes code2 = parse_Stmt(nodeStmt1)->code;
        InterCodes code3 = parse_Stmt(nodeStmt2)->code;

        node->code = mergeCode(
            mergeCode(
                mergeCode(code1, genLabelCode(label1)),
                mergeCode(code2, genGotoCode(label3))
                ),
            mergeCode(
                mergeCode(genLabelCode(label2), code3),
                genLabelCode(label3)
                )
        );

        return node;
    }
    MATCH5(IF, LP, Exp, RP, Stmt){
        Node* nodeExp = node->child->sibling->sibling;
        Node* nodeStmt = node->child->sibling->sibling->sibling->sibling;
        int label1 = new_label();
        int label2 = new_label();
        InterCodes code1 = parse_Cond(nodeExp, label1, label2)->code;
        InterCodes code2 = parse_Stmt(nodeStmt)->code;

        node->code = mergeCode(mergeCode(code1, genLabelCode(label1)), mergeCode(code2, genLabelCode(label2)));
        return node;
    }
    MATCH5(WHILE, LP, Exp, RP, Stmt){
        Node* nodeExp = node->child->sibling->sibling;
        Node* nodeStmt = node->child->sibling->sibling->sibling->sibling;

        int label1 = new_label();
        int label2 = new_label();
        int label3 = new_label();
        
        InterCodes code1 = parse_Cond(nodeExp, label2, label3)->code;
        InterCodes code2 = parse_Stmt(nodeStmt)->code;
        
        node->code = mergeCode(
            mergeCode(
                mergeCode(genLabelCode(label1), code1),
                mergeCode(genLabelCode(label2), code2)),
            mergeCode(genGotoCode(label1), genLabelCode(label3)));

        return node;
    }
    assert(0);
    return NULL;
}

Node* parse_DefList(Node* node){
    MATCH2(Def, DefList){
        parse_Def(node->child);
        parse_DefList(node->child->sibling);
        return node;
    }
    /*empty*/
    return node;
}

Node* parse_Def(Node* node){
    MATCH3(Specifier, DecList, SEMI){
        parse_Specifier(node->child);
        parse_DecList(node->child->sibling);
        return node;
    }
    assert(0);
    return NULL;
}

Node* parse_DecList(Node* node){
    MATCH1(Dec){
        parse_Dec(node->child);
        return node;
    }
    MATCH3(Dec, COMMA, DecList){
        parse_Dec(node->child);
        parse_DecList(node->child->sibling->sibling);
        return node;
    }
    assert(0);
    return NULL;
}

Node* parse_Dec(Node* node){
    MATCH1(VarDec){
        parse_VarDec(node->child);
        return node;
    }
    MATCH3(VarDec, ASSIGNOP, Exp){
        parse_VarDec(node->child);
        //parse_Exp(node->child->sibling->sibling);
        return node;
    }
    assert(0);
    return NULL;
}

Node* parse_Cond(Node* node, int label_true, int label_false){
    MATCH3(Exp, RELOP, Exp){
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        InterCodes code1 = parse_Exp(node->child, t1)->code;
        InterCodes code2 = parse_Exp(node->child->sibling->sibling, t2)->code;
        Node* nodeOp = node->child->sibling;
        char* op = nodeOp->id;
        InterCodes code3 = newInterCodes();
        code3->code = newInterCode();
        code3->code->kind = COND;
        code3->code->cond.v1 = t1;
        code3->code->cond.op = op;
        code3->code->cond.v2 = t2;
        code3->code->cond.label_no = label_true;
        assert(code3->code->cond.label_no > 0);
        node->code = mergeCode(
            mergeCode(code1, code2),
            mergeCode(code3, genGotoCode(label_false))
            );

        return node;
    }
    MATCH2(NOT, Exp){
        node->code = parse_Cond(node->child->sibling, label_false, label_true)->code;
        return node;
    }
    MATCH3(Exp, AND, Exp){
        int label1 = new_label();
        InterCodes code1 = parse_Cond(node->child, label1, label_false)->code;
        InterCodes code2 = parse_Cond(node->child->sibling->sibling, label_true, label_false)->code;
        node->code = mergeCode(mergeCode(code1, genLabelCode(label1)), code2);
        return node;
    }
    MATCH3(Exp, OR, Exp){
        int label1 = new_label();
        InterCodes code1 = parse_Cond(node->child, label_true, label1)->code;
        InterCodes code2 = parse_Cond(node->child->sibling->sibling, label_true, label_false)->code;
        node->code = mergeCode(mergeCode(code1, genLabelCode(label1)), code2);
        return node;
    }
    Operand t1 = new_temp();
    InterCodes code1 = parse_Exp(node, t1)->code;
    InterCodes code2 = newInterCodes();
    code2->code = newInterCode();
    code2->code->kind = COND;
    code2->code->cond.v1 = t1;
    code2->code->cond.op = "!=";
    code2->code->cond.v2 = number(0);
    code2->code->cond.label_no = label_true;

    node->code = mergeCode(mergeCode(code1, code2),
                          genGotoCode(label_false));
    return node;
}

Node* parse_Exp(Node* node, Operand place){
    MATCH3(Exp, ASSIGNOP, Exp){
        Node* nodeExp1 = node->child;
        Node* nodeExp2 = node->child->sibling->sibling;
        Operand variable = lookup(nodeExp1->id);
        Operand t1 = new_temp();
        InterCodes code1 = parse_Exp(nodeExp2, t1)->code;

        InterCodes code2 = newInterCodes();
        code2->code = newInterCode();
        code2->code->kind = ASSIGN;
        code2->code->assign.left = variable;
        code2->code->assign.right = t1;
        
        InterCodes code_tmp = newInterCodes();
        if(place == NULL) 
            code_tmp = NULL;
        else {
            code_tmp->code = newInterCode();
            code_tmp->code->kind = ASSIGN;
            code_tmp->code->assign.left = place;
            code_tmp->code->assign.right = variable;
        }

        node->code = mergeCode(code1, mergeCode(code2, code_tmp));
        return node;
    }
    MATCH3(Exp, PLUS, Exp){
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        InterCodes code1 = parse_Exp(node->child, t1)->code;
        InterCodes code2 = parse_Exp(node->child->sibling->sibling, t2)->code;
        InterCodes code3 = newInterCodes();
        code3->code = newInterCode();
        code3->code->kind = PLUS_;
        code3->code->binop.op1 = t1;
        code3->code->binop.op2 = t2;
        code3->code->binop.result = place;
        
        node->code = mergeCode(mergeCode(code1, code2), code3);
        return node;
    }
    MATCH3(Exp, MINUS, Exp){
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        InterCodes code1 = parse_Exp(node->child, t1)->code;
        InterCodes code2 = parse_Exp(node->child->sibling->sibling, t2)->code;
        InterCodes code3 = newInterCodes();
        code3->code = newInterCode();
        code3->code->kind = MINUS_;
        code3->code->binop.op1 = t1;
        code3->code->binop.op2 = t2;
        code3->code->binop.result = place;

        node->code = mergeCode(mergeCode(code1, code2), code3);
        return node;
    }
        MATCH3(Exp, STAR, Exp){
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        InterCodes code1 = parse_Exp(node->child, t1)->code;
        InterCodes code2 = parse_Exp(node->child->sibling->sibling, t2)->code;
        InterCodes code3 = newInterCodes();
        code3->code = newInterCode();
        code3->code->kind = MUL;
        code3->code->binop.op1 = t1;
        code3->code->binop.op2 = t2;
        code3->code->binop.result = place;

        node->code = mergeCode(mergeCode(code1, code2), code3);
        return node;
    }
    MATCH3(Exp, DIV, Exp){
        assert(0);
        /*parse_Exp(node->child);
        parse_Exp(node->child->sibling->sibling);
        return node;*/
    }
    MATCH3(LP, Exp, RP){
        node->code = parse_Exp(node->child->sibling, place)->code;
        return node;
    }
    MATCH2(MINUS, Exp){
        Node* nodeExp = node->child->sibling;
        Operand t1 = new_temp();
        InterCodes code1 = parse_Exp(nodeExp, t1)->code;
        InterCodes code2 = newInterCodes();
        code2->code = newInterCode();
        code2->code->kind = MINUS_;
        code2->code->binop.result = place;
        code2->code->binop.op1 = number(0);
        code2->code->binop.op2 = t1;
        
        node->code = mergeCode(code1, code2);
        return node;
    }
    MATCH4(ID, LP, Args, RP){
        Node* nodeID = node->child;
        Operand function = lookup(nodeID->id);

        Node* nodeArgs = node->child->sibling->sibling;
       
        struct OperandList *arg_list = NULL;
        InterCodes code1 = parse_Args(nodeArgs, &arg_list)->code;

        if(!strcmp(function->str, "write")){
            InterCodes interCodes = newInterCodes();
            interCodes->code = newInterCode();
            interCodes->code->kind = FUNCCALLWRITE;
            interCodes->code->funccallwrite.op = arg_list->op;
            node->code = mergeCode(code1, interCodes);
            return node;
        }
        else{
            InterCodes interCodes = newInterCodes();
            interCodes->code = newInterCode();
            interCodes->code->kind = FUNCCALL;
            interCodes->code->funccall.left = place;
            interCodes->code->funccall.right = function;
            struct OperandList* head = arg_list;
            InterCodes code2 = NULL;
            for(; head != NULL; head = head->next){
                InterCodes arg_code = newInterCodes();
                arg_code->code = newInterCode();
                arg_code->code->kind = ARG;
                arg_code->code->arg.op = head->op;
                code2 = mergeCode(code2, arg_code);
            }
            node->code = mergeCode(mergeCode(code1, code2), interCodes);
            return node;
        }
    }
    MATCH3(ID, LP, RP){
        Node* nodeID = node->child;
        Operand function = lookup(nodeID->id);
        InterCodes interCodes = newInterCodes();
        interCodes->code = newInterCode();
        if(!strcmp(function->str, "read")){
            interCodes->code->kind = FUNCCALLREAD;
            interCodes->code->funccallread.op = place;
        }
        else{
            interCodes->code->kind = FUNCCALL;
            interCodes->code->funccall.left = place;
            interCodes->code->funccall.right = function;
        }
        node->code = interCodes;
        return node;
    }
    MATCH4(Exp, LB, Exp, RB){
        /*parse_Exp(node->child);
        parse_Exp(node->child->sibling->sibling);
        return node;*/
        assert(0);
    }
    MATCH3(Exp, DOT, ID){
        /*parse_Exp(node->child);
        return node;*/
        assert(0);
    }
    MATCH1(ID){
        /*do something*/ 
        Node* nodeID = node->child;
        Operand variable = lookup(nodeID->id);
        InterCodes interCodes = newInterCodes();
        interCodes->code = newInterCode();
        interCodes->code->kind = ASSIGN; 
        interCodes->code->assign.left = place;
        interCodes->code->assign.right = variable;
        node->code = interCodes;
        return node;
    }
    MATCH1(INT){
        /*do something*/
        int value = node->child->i;

        InterCode interCode = newInterCode();
        assert(interCode != NULL);
        interCode->kind = ASSIGN;
        interCode->assign.left = place;
        interCode->assign.right = newOperand();
        interCode->assign.right->kind = CONSTANT;
        interCode->assign.right->no = value;

        InterCodes interCodes = newInterCodes();
        interCodes->prev = NULL;
        interCodes->next = NULL;
        interCodes->code = interCode;

        node->code = interCodes;
        return node;
    }
    MATCH1(FLOAT){
    }
    MATCH3(Exp, RELOP, Exp){
    }
    MATCH3(Exp, OR, Exp){
    }
    MATCH3(Exp, AND, Exp){
    }
    MATCH2(NOT, Exp){
    }
    int label1 = new_label();
    int label2 = new_label();
    InterCodes code0 = newInterCodes();
    code0->code = newInterCode();
    code0->code->kind = ASSIGN;
    code0->code->assign.left = place;
    code0->code->assign.right = number(0);
    printf("not test:     %d %d\n", label1, label2);
    InterCodes code1 = parse_Cond(node->child->sibling, label1, label2)->code;
    InterCodes code2 = newInterCodes();
    code2->code = newInterCode();
    code2->code->kind = ASSIGN;
    code2->code->assign.left = place;
    code2->code->assign.right = number(1);
    code2 = mergeCode(genLabelCode(label1), code2);

        node->code = mergeCode(
            mergeCode(code0, code1),
            mergeCode(code2, genLabelCode(label2))
            );

    return NULL;
}

Node* parse_Args(Node* node, struct OperandList ** arg_list){
    MATCH3(Exp, COMMA, Args){
        Operand t1 = new_temp();
        InterCodes code1 = parse_Exp(node->child, t1)->code;
        node->code = code1;
        
        struct OperandList* head = malloc(sizeof(struct OperandList));
        head->op = t1;
        head->next = *arg_list;
        *arg_list = head;

        InterCodes code2 = parse_Args(node->child->sibling->sibling, &head)->code;
        node->code = mergeCode(code1, code2);
        return node;
    }
    MATCH1(Exp){
        Operand t1 = new_temp();
        InterCodes code1 = parse_Exp(node->child, t1)->code;
        struct OperandList* head = malloc(sizeof(struct OperandList));
        head->op = t1;
        head->next = *arg_list;
        *arg_list = head;
        node->code = code1;
        return node;
    }
    assert(0);
    return NULL;
}

Node* parse2GenCode(Node* node){
    InterCodes code = parse_Program(node)->code;
    printInterCodes(code);
    return node;
}
