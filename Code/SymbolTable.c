/*************************************************************************
	> File Name: SymbolTable.c
	> Author: 
	> Mail: 
	> Created Time: Sun 19 Nov 2017 03:18:18 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "SymbolTable.h"
#include "NodeTree.h"
#include "TypeStack.h"
#include "SymbolStack.h"

extern SymbolTable symbolTable;
extern SymbolTable structSymbolTable;
extern TypeStack typeStack;
extern SymbolStack paraStack;
extern TypeStack structStack;
extern SymbolStack symbolStack;
extern int success;
extern int yylineno;
extern int yycolumn;


unsigned int hash_pjw(char* name){
    unsigned int val = 0, i;
    for (; *name; ++name) {
        val = (val << 2) + *name;
        if (i = val & ~0x3fff)
            val = (val ^ (i >> 12)) & 0x3fff;
    }
    return val;
}

SymbolTable newSymbolTable(){
    SymbolTable symbolTable = malloc(sizeof(struct SymbolTable_));
    memset((void*)symbolTable, 0, sizeof(struct SymbolTable_));
    return symbolTable;
}

void insert(SymbolTable symbolTable, SymbolNode insertNode){
    assert(symbolTable != NULL);
    char* name = insertNode->name;
    unsigned int hash_num = hash_pjw(name);
    SymbolNode symbolNode = symbolTable->bucket[hash_num];
    symbolTable->bucket[hash_num] = insertNode;
    insertNode->tail = symbolNode;
}

void delete(SymbolTable symbolTable, SymbolNode deleteNode){
    assert(symbolTable != NULL);
    assert(deleteNode != NULL);
    char* name = deleteNode->name;
    unsigned int hash_num = hash_pjw(name);
    SymbolNode symbolNode = symbolTable->bucket[hash_num];
    if(symbolNode == deleteNode){
        symbolTable->bucket[hash_num] = deleteNode->tail;
        free(deleteNode);
    }
    for(; symbolNode != NULL && symbolNode->tail != deleteNode;
        symbolNode = symbolNode->tail);
    if(symbolNode != NULL){
        symbolNode->tail = deleteNode->tail;
        free(deleteNode);
    }
}

bool haveSymbolNode(SymbolTable symbolTable, SymbolNode symbolNode){
    assert(symbolTable != NULL);
    unsigned int hash_num = hash_pjw(symbolNode->name);
    SymbolNode head = symbolTable->bucket[hash_num];
    for(; head != NULL; head = head->tail){
        if(!strcmp(head->name, symbolNode->name)){
            return true;
        }
    }
    return false;
}

SymbolNode newSymbolNode(char* name, Type type){
    SymbolNode symbolNode = malloc(sizeof(struct SymbolNode_));
    symbolNode->name = name;
    symbolNode->type = type;
    symbolNode->tail = NULL;
}

void printFieldList(FieldList fieldList){
    if(fieldList == NULL) return;
    printf("%s:",fieldList->name);
    printType(fieldList->type);
    printFieldList(fieldList->tail);
}

FieldList newFieldList(){
    FieldList fieldList = malloc(sizeof(struct FieldList_));
    return fieldList;
}

void addFieldList(FieldList head, FieldList fieldList){
    assert(head != NULL);
    for(; head->tail != NULL; head = head->tail){
        if(!strcmp(head->name, fieldList->name)){
            printf("Error type 15 at Line %d: Redefined field \"%s\".\n",
                  yylineno,
                  head->name);
            success = 0;
            return;
        }
    }
    head->tail = fieldList;
}

Type newType(){
    Type type = malloc(sizeof(struct Type_));
    return type;
}

void printType(Type type){
    if(type == NULL) return;
    switch(type->kind){
        case BASIC:{
            printf("%s", type->basic);
            break;
        }
        case ARRAY:{
            printType(type->array.elem);
            printf("[%d]", type->array.size);
            break;
        }
        case FUNCTION:{
            printType(type->function.retType);
            printf("(");
            for(int i = 0; i < type->function.paraNum; i++){
                printType(type->function.para[i]);
                printf(",");
            }
            printf(")");
            break;
        }
        case STRUCTURE:{
            printf("struct %s{", type->structure.tag);
            FieldList head = type->structure.list;
            for(; head != NULL; head = head->tail){
                printType(head->type);
                printf(" %s;", head->name);
            }
            printf("}");
            break;
        }
        default:
            assert(0);
    }
}

void printExp(Node* node){
    if(!strcmp(node->child->tag_name, "FLOAT")){
        printf("%g", node->child->f);
    }
}

bool isLeftVal(Node* node){
    assert(!strcmp(node->tag_name, "Exp"));
    if(!strcmp(node->child->tag_name, "ID") && node->child->sibling == NULL)
        return true;
    else if(!strcmp(node->child->tag_name, "Exp")
           && !strcmp(node->child->sibling->tag_name, "DOT")
           && !strcmp(node->child->sibling->sibling->tag_name, "ID"))
        return true;
    else if(!strcmp(node->child->tag_name, "Exp")
           && !strcmp(node->child->sibling->tag_name, "LB")
           && !strcmp(node->child->sibling->sibling->tag_name, "Exp")
           && !strcmp(node->child->sibling->sibling->sibling->tag_name, "RB"))
        return true;
    return false;
}

bool matchType(Type t1, Type t2){
    if(t1 == NULL || t2 == NULL) 
        return false;
    if(t1->kind != t2->kind)
        return false;
    switch(t1->kind){
    case BASIC:{
        return !strcmp(t1->basic, t2->basic);
    }
    case FUNCTION:{
        if(t1->function.paraNum != t2->function.paraNum)
            return false;
        bool flag = matchType(t1->function.retType, t2->function.retType);
        if(!flag)
            return false;
        for(int i = 0; i < t1->function.paraNum; i++){
            flag = flag && matchType(t1->function.para[i], t2->function.para[i]);
        }
        return flag;
    }
    case STRUCTURE:{
        bool flag = true;
        FieldList head1 = t1->structure.list;
        FieldList head2 = t2->structure.list;
        for(; head1 != NULL && head2 != NULL; head1=head1->tail,head2=head2->tail){
            flag = flag && matchType(head1->type, head2->type);
        }
        if(head1 != NULL || head2 != NULL)
            flag = false;
        return flag;
    }
    case ARRAY:{
        return matchType(t1->array.elem, t2->array.elem);
    }
    default:
        assert(0);
    }
}

Type matchReturnType(Type t1, Type t2, int lineno){
    if(t1 == NULL || t2 == NULL) 
        return newType();
    if(t1->kind != t2->kind || 
       (t1->kind == BASIC && t2->kind == BASIC
        && strcmp(t1->basic, t2->basic)
       )){
           printf("Error type 8 at Line %d: Type mismatched for return.\n"
                  ,lineno);
           success = 0;
           return NULL;
       }
    return t1; 
}

Type matchArgsType(Type funcType, char* funcName){
    if(funcType->kind != FUNCTION)
        return NULL;
    bool error = false;
    int paraNum = funcType->function.paraNum;
    int argNum = typeStack->num;
    if(paraNum != argNum){
        error = true;
    }
    for(int i = 0; i < paraNum; i++){
        Type t1 = funcType->function.para[i];
        Type t2 = typeStack->stack[paraNum - i - 1];
        if(!matchType(t1, t2))
            error = true;
    }
    clearTypeStack(typeStack);
    if(error){
        printf("Error type 9 at Line 8: Function \"");
        printf("%s(", funcName);
        if(paraNum > 0){
            printType(funcType->function.para[0]);
            for(int i = 1; i < paraNum; i++){
                printf(", ");
                printType(funcType->function.para[i]);
            }
        }
        printf(")\" is not applicable for arguments \"(");
        if(argNum > 0){
            printType(typeStack->stack[argNum - 1]);
            for(int i = 1; i < argNum; i++){
                printf(", ");
                printType(typeStack->stack[argNum - i - 1]);
            }
        }
        printf(")\".\n");
        success = 0;
        return NULL;
    }
    return funcType;
}

void printSymbolNode(SymbolNode symbolNode){
    if(symbolNode == NULL) return;
    printf("%s|", symbolNode->name);
    printType(symbolNode->type);
    printf("\t-->\t");
    printSymbolNode(symbolNode->tail);
}

void printSymbolTable(SymbolTable symbolTable){
    printf("---printing symbol table now---\n");
    for(unsigned int i = 0; i < 0x3fff; i++){
        SymbolNode symbolNode = symbolTable->bucket[i];
        if(symbolNode != NULL){
            printf("%d:  ", i);
            printSymbolNode(symbolNode);
            printf("\n");
        }
    }
    printf("------------ over--------------\n");
}

Type getSymbol(char* name){
    unsigned int hash_num = hash_pjw(name);
    SymbolNode head = symbolTable->bucket[hash_num];
    for(; head != NULL; head = head->tail){
        if(!strcmp(head->name, name))
            return head->type;
    }
    return NULL;
}

bool haveSymbol(char* name, Type type){
    unsigned int hash_num = hash_pjw(name);
    SymbolNode head = symbolTable->bucket[hash_num];
    
    SymbolNode stack_head = getTopFromSymbolStack(symbolStack);

    for(; head != NULL; head = head->tail){
        if(!strcmp(name, head->name) && matchType(head->type, type)){
            for(; stack_head != NULL; stack_head = stack_head->stack_next){
                if(stack_head == head){
                    switch(type->kind){
                        //TODO: other types
                        case BASIC:{
                            printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",
                                   yylineno,
                                   name);
                            break;
                        }
                        default:assert(0);
                    }
                    success = 0;
                    return true;
                }
            }
        }
    }
    return false;
}

void newSymbol(char* name, Type type){
    if(haveSymbol(name, type)) return;
    SymbolNode currentScope = getTopFromSymbolStack(symbolStack);
    SymbolNode symbolNode = newSymbolNode(name, type);
    if(currentScope != NULL){
        SymbolNode head = currentScope->stack_next;
        currentScope->stack_next = symbolNode;
        symbolNode->stack_next = head;
    }
    insert(symbolTable, symbolNode);
}

bool getStructVarOffset(char* name, Type type, int* retOffset, Type* retType){
    assert(type->kind == STRUCTURE);
    FieldList head = type->structure.list;
    for(; head != NULL; head = head->tail){
        if(!strcmp(head->name, name)){
            //TODO: should set *offset
            *retType = head->type;
            //assert(0);
            return true;
        }
    }
    return false;
}

void newParam(int num, ...){
    va_list valist;
    va_start(valist, num);
    if(num == 2){
        Node* specifierNode = va_arg(valist, Node*);
        Node* decNode = va_arg(valist, Node*);
        Type type;
        copytype(type, specifierNode->type);
        SymbolNode symbolNode = newSymbolNode(decNode->id, type);
        
        pushIntoSymbolStack(paraStack, symbolNode);
    }
}

void newArg(Type type){
    addTypeStack(typeStack, type);
}

Type getTagType(char* tag){
    unsigned int hash_num = hash_pjw(tag);
    SymbolNode head = structSymbolTable->bucket[hash_num];
    for(; head != NULL; head = head->tail){
        if(!strcmp(head->name, tag))
            return head->type;
    }
    return NULL;
}

void newUndefinedFunc(char* name, Type retType, int lineno){

        Type type = newType();
        type->kind = FUNCTION;

        type->function.isDefined = false;
        type->function.lineno = lineno;
        type->function.retType = retType;
        type->function.paraNum = getLengthOfSymbolStack(paraStack);
        int len = type->function.paraNum * sizeof(struct Type_);
        type->function.para = malloc(len);
        SymbolNode head = paraStack->head;
        for(int i = 0; head != NULL; head = head->tail, i++){
            copytype(type->function.para[i], head->type);
        }
        clearSymbolStack(paraStack);

        Type tmpType = haveFunc(name);

        if(tmpType == NULL){
            SymbolNode symbolNode = newSymbolNode(name, type);
            insert(symbolTable, symbolNode);
        }
        else if(tmpType->function.isDefined == false){
            if(matchType(tmpType, type) == false){
                printf("Error type 19 at Line %d: Inconsistent declaration of function \"%s\".\n",
                      yylineno,
                      name);
                success = 0;
                return;
            }
            SymbolNode symbolNode = newSymbolNode(name, type);
            insert(symbolTable, symbolNode);
        }
        else if(tmpType->function.isDefined == true){
            printf("Error type 4 at Line %d: Redefined function \"%s\".\n",
                lineno,
                name
                  );
            success = 0;
            return;
        }
}

void newDefinedFunc(char* name, Type retType, int lineno){

        Type type = newType();
        type->kind = FUNCTION;

        type->function.isDefined = true;
        type->function.retType = retType;
        type->function.paraNum = getLengthOfSymbolStack(paraStack);
        int len = type->function.paraNum * sizeof(struct Type_);
        type->function.para = malloc(len);
        SymbolNode head = paraStack->head;
        for(int i = 0; head != NULL; head = head->tail, i++){
            copytype(type->function.para[i], head->type);
        }
        clearSymbolStack(paraStack);

        Type tmpType = haveFunc(name);

        if(tmpType == NULL){
            SymbolNode symbolNode = newSymbolNode(name, type);
            insert(symbolTable, symbolNode);
        }
        else if(tmpType->function.isDefined == false){
            if(matchType(tmpType, type) == false){
                printf("Error type 19 at Line %d: Inconsistent declaration of function \"%s\".\n",
                      yylineno,
                      name);
                success = 0;
                return;
            }
            tmpType->function.isDefined = true;
            SymbolNode symbolNode = newSymbolNode(name, type);
            insert(symbolTable, symbolNode);
        }
        else if(tmpType->function.isDefined == true){
            printf("Error type 4 at Line %d: Redefined function \"%s\".\n",
                lineno,
                name
                  );
            success = 0;
            return;
        }
}

Type haveFunc(char* funcName){
    unsigned int hash_num = hash_pjw(funcName);
    SymbolNode head = symbolTable->bucket[hash_num];
    for(; head != NULL; head = head->tail){
        if(!strcmp(head->name, funcName)){
            Type type = head->type;
            if(type->kind != FUNCTION){
                printf("Error type 11 at Line %d: \"%s\" is not a function.\n",
                      yylineno,
                      funcName);
                success = 0;
            }
            return type;
        }
    }
    return NULL;
}

void checkUndefinedFuncBeforeExit(){
    for(unsigned int i = 0; i < 0x3fff; i++){
        SymbolNode symbolNode = symbolTable->bucket[i];
        if(symbolNode != NULL){
            for(; symbolNode != NULL; symbolNode = symbolNode->tail){
                if(symbolNode->type->kind == FUNCTION
                   && symbolNode->type->function.isDefined == false){
                        printf(
                        "Error type 18 at Line %d: Undefined function \"%s\" .\n",
                        symbolNode->type->function.lineno,
                        symbolNode->name);
                   }
            }
        }
    }
}
