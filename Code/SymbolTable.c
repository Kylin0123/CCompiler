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


extern SymbolTable symbolTable;
extern SymbolTable structSymbolTable;
extern TypeStack typeStack;
extern TypeStack structStack;
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

/*StructSymbolTable newStructSymbolTable(){
    StructSymbolTable structSymbolTable = malloc(sizeof(struct StructSymbolTable_));
    return structSymbolTable;
}*/

/*void insertStructSymbolTable(StructSymbolTable structSymbolTable, StructSymbolNode structSymbolNode){
    
}*/

/*StructSymbolNode newStructSymbolNode(char* tag, Type type){
    StructSymbolNode structSymbolNode = malloc(sizeof(struct StructSymbolNode_));
    structSymbolNode->tag = tag;
    //structSymbolNode->type = type;
}*/

void printFieldList(FieldList fieldList){
    if(fieldList == NULL) return;
    printf("%s:",fieldList->name);
    printType(fieldList->type);
    printFieldList(fieldList->tail);
}

/*
void printStructSymbolNode(StructSymbolNode structSymbolNode){
    if(structSymbolNode == NULL) return;
    printf("%s|", structSymbolNode->tag);
    printFieldList(structSymbolNode->structure);
    printf("\t-->\t");
    printStructSymbolNode(structSymbolNode->tail);
}
*/

/*
void printStructSymbolTable(StructSymbolTable structSymbolTable){
    printf("---printing struct symbol table now---\n");
    for(unsigned int i = 0; i < 0x3fff; i++){
        StructSymbolNode structSymbolNode = structSymbolTable->bucket[i];
        if(structSymbolNode != NULL){
            printf("%d:  ", i);
            printStructSymbolNode(structSymbolNode);
            printf("\n");
        }
    }
    printf("--------------- over -----------------\n");
}
*/

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

Type matchType(Type t1, Type t2, bool isShowError){
    if(t1 == NULL || t2 == NULL) 
        return NULL;
    if(t1->kind != t2->kind || 
       (t1->kind == BASIC && t2->kind == BASIC
       && strcmp(t1->basic, t2->basic)
       )){
        if(isShowError)
            printf("Error type 5 at Line %d: Type mismatched for assignment.\n"
                ,yylineno);
        success = 0;
        return NULL;
    }
    return t1;
}

Type matchOperandsType(Type t1, Type t2){
    if(t1 == NULL || t2 == NULL) 
        return NULL;
    if(t1->kind != t2->kind || 
       (t1->kind == BASIC && t2->kind == BASIC
       && strcmp(t1->basic, t2->basic)
       )){
        printf("Error type 7 at Line %d: Type mismatched for operands.\n"
               ,yylineno);
        success = 0;
        return NULL;
    }
    return t1;
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
        if(!matchType(t1, t2, false))
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

bool haveSymbol(char* name, Type type){
    unsigned int hash_num = hash_pjw(name);
    SymbolNode head = symbolTable->bucket[hash_num];
    for(; head != NULL; head = head->tail){
        if(!strcmp(name, head->name) && matchType(head->type, type, false)){
            switch(type->kind){
                case BASIC:{
                    printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",
                          yylineno,
                          name);
                    break;
                }
                case ARRAY:{
                    break;
                }
                case FUNCTION:{
                    break;
                }
                case STRUCTURE:{
                    break;
                }
                default:assert(0);
            }
            success = 0;
            return true;
        }
    }
    return false;
}

void newSymbol(char* name, Type type){
    if(haveSymbol(name, type)) return;
    SymbolNode symbolNode = newSymbolNode(name, type);
    insert(symbolTable, symbolNode);
}

/*
void newVar(int num, ...){
    va_list valist;
    va_start(valist, num);
    if(num == 2){
        Node* specifierNode = va_arg(valist, Node*);
        Node* decNode = va_arg(valist, Node*);
        
        if(decNode == NULL)
            return;
        char* varName = decNode->child->id;
        
        if(haveVar(varName)){
            printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",
                yylineno,
                varName
                  );
            success = 0;
            return;
        }

        Type type = newType();
        type->kind = BASIC;

        assert(!strcmp(specifierNode->id, "float")
              || !strcmp(specifierNode->id, "int"));

        copystr(type->basic, specifierNode->id);
        
        SymbolNode symbolNode = newSymbolNode(varName, type);
        
        if(decNode->child != NULL);
        if(decNode->child->sibling != NULL)
        if(decNode->child->sibling->sibling !=NULL){
            newVar(2, specifierNode, decNode->child->sibling->sibling);
        }
        insert(symbolTable, symbolNode);
    }
}
*/

Type haveVar(char* varName){
    unsigned int hash_num = hash_pjw(varName);
    SymbolNode head = symbolTable->bucket[hash_num];
    for(; head != NULL; head = head->tail){
        if(!strcmp(head->name, varName))
            return head->type;
    }
    return NULL;
}

bool getStructVarOffset(char* name, Type type, int* offset){
    assert(type->kind == STRUCTURE);
    FieldList head = type->structure.list;
    for(; head != NULL; head = head->tail){
        if(!strcmp(head->name, name)){
            // should set *offset
            assert(0);
            return true;
        }
    }
    return false;
}

void fillBackArray(Type arrayT, Type type){
    if(arrayT->kind == ARRAY && arrayT->array.elem->kind != ARRAY){
        copytype(arrayT->array.elem, type);
        return;
    }
    fillBackArray(arrayT->array.elem, type);
}

/*
void newArray(int num, ...){
    va_list valist;
    va_start(valist, num);
    if(num == 2){
        Node* specifierNode = va_arg(valist, Node*);
        Node* decNode = va_arg(valist, Node*);
        
        if(decNode == NULL)
            return;
        char* varName = decNode->child->id;

        assert(decNode->child->child->type->kind == ARRAY);
        
        if(haveVar(varName)){
            //TODO: maybe function or array
            printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",
                yylineno,
                varName
                  );
            success = 0;
            return;
        }

        Type type = newType();
        type->kind = BASIC;

        assert(!strcmp(specifierNode->id, "float")
              || !strcmp(specifierNode->id, "int"));

        copystr(type->basic, specifierNode->id);

        fillBackArray(decNode->child->child->type, type);
        
        SymbolNode symbolNode = newSymbolNode(varName, decNode->child->child->type);
        
        if(decNode->child != NULL);
        if(decNode->child->sibling != NULL)
        if(decNode->child->sibling->sibling !=NULL){
            newArray(2, specifierNode, decNode->child->sibling->sibling);
        }
        insert(symbolTable, symbolNode);
  
    }
}
*/

void newParam(int num, ...){
    va_list valist;
    va_start(valist, num);
    if(num == 2){
        Node* specifierNode = va_arg(valist, Node*);
        Node* decNode = va_arg(valist, Node*);
        Type type;
        copytype(type, specifierNode->type);
        addTypeStack(typeStack, type);
    }
}

void newArg(Type type){
    addTypeStack(typeStack, type);
}

/*
void newStruct(int num, ...){
    va_list valist;
    va_start(valist, num);
    if(num == 2){
        Node* specifierNode = va_arg(valist, Node*);
        Node* decNode = va_arg(valist, Node*);
        
        Type type;
        copytype(type, specifierNode->type); 
        assert(type != NULL);
        
        SymbolNode symbolNode = newSymbolNode(decNode->id, type);
        insert(symbolTable, symbolNode);
    }
}
*/

Type getTagType(char* tag){
    unsigned int hash_num = hash_pjw(tag);
    SymbolNode head = structSymbolTable->bucket[hash_num];
    for(; head != NULL; head = head->tail){
        if(!strcmp(head->name, tag))
            return head->type;
    }
    return NULL;
}

/*
bool isArray(Type type){
    return type->kind == ARRAY;
}
*/

void newFunc(int num, ...){
    va_list valist;
    va_start(valist, num);
    if(num == 2){
        Node* specifierNode = va_arg(valist, Node*);
        assert(strcmp(specifierNode->tag_name,"Specifier")==0);

        Node* decNode = va_arg(valist, Node*);
        char* funcName = decNode->id;

        if(haveVar(funcName)){
            printf("Error type 4 at Line %d: Redefined function \"%s\".\n",
                decNode->lineno,
                funcName
                  );
            success = 0;
            return;
        }
  
        Type type = newType();
        type->kind = FUNCTION;

        Type type2 = newType();
        assert(specifierNode->type != NULL);
        memcpy(type2, specifierNode->type, sizeof(struct Type_));
        
        type->function.retType = type2;
        type->function.paraNum = typeStack->num;
        int len = typeStack->num * sizeof(struct Type_);
        type->function.para = malloc(len);
        memcpy(type->function.para, &typeStack->stack[0], len);
        clearTypeStack(typeStack);
        assert(typeStack->num == 0);

        SymbolNode symbolNode = newSymbolNode(decNode->id, type);
        insert(symbolTable, symbolNode);
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
