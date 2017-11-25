/*************************************************************************
	> File Name: NodeTree.c
	> Author: 
	> Mail: 
	> Created Time: Fri 15 Sep 2017 01:55:46 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include "NodeTree.h"

extern char* yytext;
extern int yylineno;
extern int success;

Node* newNode(char* name, int num, ...){
    assert(name != NULL);
    Node* retNode = malloc(sizeof(Node));
    strcpy(retNode->tag_name, name);

    va_list valist;
    va_start(valist, num);
    if(num == 0){
        int lineno = va_arg(valist, int);
        retNode->lineno = lineno;
        retNode->child = retNode->sibling = NULL;

        if(!strcmp(retNode->tag_name, "ID")){
            strcpy(retNode->id, yytext);
        }
        else if(!strcmp(retNode->tag_name, "TYPE")){
            strcpy(retNode->id, yytext);
            Type type = newType();
            type->kind = BASIC;
            copystr(type->basic, yytext);
            retNode->type = type;
        }
        else if(!strcmp(retNode->tag_name, "INT")){
            if(strlen(yytext) > 2 && yytext[0] == '0' && yytext[1] == 'x')
                retNode->i = strtol(yytext, NULL, 16);
            else if(strlen(yytext) > 1 && yytext[0] == '0')
                retNode->i = strtol(yytext, NULL, 8);
            else
                retNode->i = atoi(yytext);
            Type type = newType();
            type->kind = BASIC;
            copystr(type->basic, "int");
            retNode->type = type;
        }
        else if(!strcmp(retNode->tag_name, "FLOAT")){
            retNode->f = atof(yytext);
            Type type = newType();
            type->kind = BASIC;
            copystr(type->basic, "float");
            retNode->type = type;
        }
    }
    else if(num > 0){
        Node* tmp = va_arg(valist, Node*);
        
        /*if(tmp == NULL){
            fprintf(stderr, "warning:tmp==NULL\n");
            return retNode;
        }*/
        assert(tmp != NULL);

        if(num == 1){
            strcpy(retNode->id, tmp->id);
            copytype(retNode->type, tmp->type);
        }
        
        retNode->child = tmp;
        retNode->lineno = tmp->lineno;
        
        for(int i = 0; i < num - 1; i++){
            tmp->sibling = va_arg(valist, Node*);
            tmp = tmp->sibling;
        }
    }
    return retNode;
}

void printNodeTree(Node* root, int blankNum){
    if(!success) 
        return;
    if(root == NULL) 
        return;
    if(root->lineno <= 0) 
        return;
    
    for(int i = 0; i < blankNum; i++) 
        printf(" ");
    
    if(!strcmp(root->tag_name, "TYPE")
      || !strcmp(root->tag_name, "ID"))
        printf("%s: %s\n", root->tag_name, root->id);
    else if(!strcmp(root->tag_name, "INT")){
        printf("%s: %d\n", root->tag_name, root->i);
    }
    else if(!strcmp(root->tag_name, "FLOAT")){
        printf("%s: %f\n", root->tag_name, root->f);
    }
    else
        printf("%s (%d)\n", root->tag_name, root->lineno);

    Node* tmp = root->child;
    
    for(;tmp != NULL;tmp = tmp->sibling){
        printNodeTree(tmp, blankNum + 1);
    }
}

/*TypeStack*/
TypeStack newTypeStack(){
    TypeStack typeStack = malloc(sizeof(struct TypeStack_));
    typeStack->num = 0;
    return typeStack;
}

void addTypeStack(TypeStack typeStack, Type type){
    int size = sizeof(struct Type_);
    Type newType = malloc(sizeof(struct Type_));
    memcpy(newType, type, size);
    typeStack->stack[typeStack->num] = newType;
    typeStack->num++;
}

Type getTypeStackTop(TypeStack typeStack){
    if(typeStack->num == 0) return NULL;
    return typeStack->stack[typeStack->num - 1];
}

void popTypeStack(TypeStack typeStack){
    typeStack->num--;
}

void clearTypeStack(TypeStack typeStack){
    typeStack->num = 0;
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

