/*************************************************************************
	> File Name: NodeTree.h
	> Author: 
	> Mail: 
	> Created Time: Fri 15 Sep 2017 01:57:08 PM CST
 ************************************************************************/

#ifndef _NODETREE_H
#define _NODETREE_H

#define MAX_TYPE_NAME 32
#define MAX_VALUE_LEN 32

#include "Type.h"

/*Node*/
typedef struct Node{
    char tag_name[MAX_TYPE_NAME];
    Type type;
    int lineno;
    struct Node* child;
    struct Node* sibling;
    union{
        char id[MAX_VALUE_LEN];
        int i;
        float f;
    };
} Node;

Node* newNode(char* name, int num, ...);
void printNodeTree(Node* root, int blankNum);

#endif
