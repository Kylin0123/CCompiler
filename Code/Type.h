/*************************************************************************
	> File Name: Type.h
	> Author: 
	> Mail: 
	> Created Time: Mon 27 Nov 2017 11:36:06 PM CST
 ************************************************************************/

#ifndef _TYPE_H
#define _TYPE_H

#include <assert.h>
#include <string.h>

#define bool int
#define true 1
#define false 0

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;

struct Type_{
    enum { BASIC, ARRAY, STRUCTURE, FUNCTION } kind;
    union
    {
        // 基本类型 "int","float"
        char* basic;
        // 数组类型信息包括元素类型与数组大小构成
        struct { Type elem; Type prev; int size; } array;
        // 结构体类型信息是一个链表
        struct { char* tag; FieldList list; } structure;
        // the structure of function type's information 
        struct {
            bool isDefined;
            int lineno;
            Type retType;
            int paraNum;
            Type* para;
        } function;
    };
};

struct FieldList_{
    char* name; // 域的名字
    Type type; // 域的类型
    FieldList tail; // 下一个域
};
#define copystr(dst, src)\
do{\
   size_t len = strlen(src);\
   dst = (char*)malloc(len);\
   strcpy(dst, src);\
  }while(0)

#define copytype(dst, src)\
do{\
   if(src == NULL) break;\
   dst = newType();\
   memcpy(dst, src, sizeof(struct Type_));\
  }while(0)

#endif
