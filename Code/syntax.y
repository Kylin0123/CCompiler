%{
#include "lex.yy.c"
#include <stdio.h>
#include "Type.h"
#include "NodeTree.h"
#include "SymbolTable.h"
#include "TypeStack.h"
#include "SymbolStack.h"
#include "translate.h"

int yylex();
void yyerror(const char* s);
int success = 1;   //Record the result of parsing.
extern SymbolTable symbolTable;   //Record symbols by the hash table.
extern SymbolTable structSymbolTable;   //The node in the hash table.
extern TypeStack typeStack;   //Record the values' type when calling function.
extern SymbolStack paraStack;  //Record the params' symbol when defining function.
extern TypeStack structStack;  //Record the struct levels and the current struct.
extern SymbolStack symbolStack;  //Record the symbol stack for action scope.
Type t;   //Pass the current type value when defining variables. It's important.

%}
%locations
%error-verbose
%union{
    struct Node* type_node;
}

%token INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE

%type<type_node>INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE

%type<type_node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left MINUS PLUS
%left DIV STAR
%right NOT UMINUS
%left DOT
%left LB RB LP RP LC RC COMMA
%%
Program: ExtDefList{
       $$=newNode("Program",1,$1);
       checkUndefinedFuncBeforeExit();
       if(!success) {
           exit(0);
       }
       assert($$ != NULL);
       parse2GenCode($$);
       //printNodeTree($$, 0);
       //printSymbolTable(structSymbolTable);
       //printSymbolTable(symbolTable);
       }
       ;
ExtDefList: ExtDef ExtDefList {
          $$=newNode("ExtDefList",2,$1,$2);
          }
          | /*empty*/{
          $$=newNode("ExtDefList",0,-1);
          }
          ;
ExtDef: Specifier ExtDecList SEMI {
      $$=newNode("ExtDef",3,$1,$2,$3);
      }
      | Specifier SEMI {
      $$=newNode("ExtDef",2,$1,$2);
      }
      | Specifier FunDec SEMI{
      newUndefinedFunc($2->id, $1->type, $2->lineno);
      }
      | Specifier FunDec CompSt{
      $$=newNode("ExtDef",3,$1,$2,$3);
      newDefinedFunc($2->id, $1->type, $2->lineno);
      //$->code
      matchReturnType($1->type, $3->type, $3->lineno);
      }
      ;
ExtDecList: VarDec{
          $$=newNode("ExtDecList",1,$1);
          newSymbol($1->id,$1->type);
          }
          | VarDec COMMA ExtDecList{
          $$=newNode("ExtDecList",3,$1,$2,$3);
          newSymbol($1->id,$1->type);
          }
          ;
Specifier: TYPE {
         $$=newNode("Specifier",1,$1);
         copytype(t, $1->type);
         }
         | StructSpecifier{
         $$=newNode("Specifier",1,$1);
         copytype(t, $1->type);
         }
         ;
StructSpecifier: STRUCT OptTag LC {
               Type type = newType();
               type->kind = STRUCTURE;
               addTypeStack(structStack, type);
               } DefList RC{
               $$=newNode("StructSpecifier",5,$1,$2,$3,$5,$6);
               Type structType = getTypeStackTop(structStack);
               char* structName = $2->id;
               if(strlen(structName) > 0){
                   /*have struct name*/
                   $$->type = newType();
                   $$->type->structure.list = NULL;
                   copystr($$->type->structure.tag, structName);
                   SymbolNode structSymbolNode
                        = newSymbolNode(structName, structType);
                   if(haveSymbolNode(structSymbolTable, structSymbolNode)){
                       printf("Error type 16 at Line %d: Duplicated name \"%s\".\n",
                       $2->lineno,
                       structName);
                       success = 0;
                   }
                   else
                       insertIntoSymbolTable(structSymbolTable, structSymbolNode);
               }
               else{
                   /*not have struct name*/
                   copytype($$->type, structType);
               }
               popTypeStack(structStack);
               }
               | STRUCT Tag{
               $$=newNode("StructSpecifier",2,$1,$2);
               Type type = getTagType($2->id);
               if(!type){
                   printf("Error type 17 at Line %d: Undefined structure \"%s\".\n",
                   yylineno,
                   $2->id);
                   success = 0;
               }
               else{
                   strcpy($$->id, $2->id);
                   copytype($$->type, type);
               }
               }
               ;
OptTag: ID{
      $$=newNode("OptTag",1,$1);
      }
      | /*empty*/{
      $$=newNode("OptTag",0,-1);
      }
      ;
Tag: ID{
   $$=newNode("Tag",1,$1);
   }
   ;
VarDec: ID{
      $$=newNode("VarDec",1,$1);
      copytype($$->type, t);
      }
      | VarDec LB INT RB{
      $$=newNode("VarDec",4,$1,$2,$3,$4);
      Type newT = newType();
      newT->kind = ARRAY;
      copytype(newT->array.elem, $1->type);
      newT->array.size = $3->i;
      copytype($$->type, newT);
      strcpy($$->id, $1->id);
      }
      ;
FunDec: ID LP VarList RP{
      $$=newNode("FunDec",4,$1,$2,$3,$4);
      strcpy($$->id, $1->id);
      }
      | ID LP RP{
      $$=newNode("FunDec",3,$1,$2,$3);
      strcpy($$->id, $1->id);
      }
      ;
VarList: ParamDec COMMA VarList{
       $$=newNode("VarList",3,$1,$2,$3); 
       }
       | ParamDec{
       $$=newNode("VarList",1,$1);
       }
       ;
ParamDec: Specifier VarDec{
        $$=newNode("ParamDec",2,$1,$2); 
        newParam(2,$1,$2);
        }
        ;
CompSt: {
      SymbolNode symbolNode = newSymbolNode(NULL, NULL);
      pushIntoSymbolStack(symbolStack, symbolNode);
      SymbolNode head = paraStack->head;
      for(; head != NULL; head = head->tail){
          newSymbol(head->name, head->type);
      }
      } LC DefList StmtList RC{
      $$=newNode("CompSt",4,$2,$3,$4,$5);
      $$->lineno = $4->lineno;
      copytype($$->type, $4->type);
      SymbolNode s = popFromSymbolStack(symbolStack);
      if(s != NULL){
        s = s->stack_next;
        while(s != NULL){
            SymbolNode deleteNode = s;
            s = s->stack_next;
            deleteFromSymbolTable(symbolTable, deleteNode);
        }
      }
      }
      ;
StmtList: Stmt StmtList{
        $$=newNode("StmtList",2,$1,$2);
        if($2->type == NULL){
            copytype($$->type, $1->type);
            $$->lineno = $1->lineno;
        }
        else
            copytype($$->type, $2->type);
        }
        | /*empty*/{
        $$=newNode("StmtList",0,-1);
        assert($$->type == NULL);
        }
        ;
Stmt: Exp SEMI{
    $$=newNode("Stmt",2,$1,$2);
    }
    | CompSt{
    $$=newNode("Stmt",1,$1);
    }
    | RETURN Exp SEMI{
    $$=newNode("Stmt",3,$1,$2,$3);
    copytype($$->type, $2->type);
    }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE{
    $$=newNode("Stmt",5,$1,$2,$3,$4,$5);
    }
    | IF LP Exp RP Stmt ELSE Stmt{
    $$=newNode("Stmt",7,$1,$2,$3,$4,$5,$6,$7);
    }
    | IF LP Exp RP error SEMI {success = 0; yyerrok;}
    | WHILE LP Exp RP Stmt{
    $$=newNode("Stmt",5,$1,$2,$3,$4,$5);
    }
    ;
DefList: Def DefList{
       $$=newNode("DefList",2,$1,$2);
       }
       | /*empty*/{
       $$=newNode("DefList",0,-1);
       }
       ;
Def: Specifier DecList SEMI{
   $$=newNode("Def",3,$1,$2,$3);
   Type structType;
   if((structType = getTypeStackTop(structStack)) != NULL){
       /*in struct*/
   }
   else{
       /*not in struct*/
   }
   }
   | error SEMI { success = 0; yyerrok; }
   ;
DecList: Dec {
       $$=newNode("DecList",1,$1);
       }
       | Dec COMMA DecList{
       $$=newNode("DecList",3,$1,$2,$3);
       strcmp($$->id, $1->id);
       copytype($$->type, $1->type);
       }
       ;
Dec: VarDec {
   $$=newNode("Dec",1,$1);
   Type structType;
   if((structType = getTypeStackTop(structStack)) == NULL){
       /*not in struct*/
       newSymbol($1->id,$1->type);
   }
   else{
        /*in struct*/
        FieldList fieldList = newFieldList();
        if(structType->structure.list == NULL){
            structType->structure.list = newFieldList();
            copystr(structType->structure.list->name, $1->id);
            copytype(structType->structure.list->type, $1->type);
        }
        else{
            FieldList fieldList = newFieldList();
            copystr(fieldList->name, $1->id);
            copytype(fieldList->type, $1->type);
            addFieldList(structType->structure.list, fieldList);
        }       
   }
   }
   | VarDec ASSIGNOP Exp{
   $$=newNode("Dec",3,$1,$2,$3);
   if(getTypeStackTop(structStack) == NULL)
       newSymbol($1->id,$1->type);
   else
       assert(0); //TODO
   }
   ;
Exp:Exp ASSIGNOP Exp{
   if(!isLeftVal($1)){
       printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", yylineno);
       success = 0;
   }
   $$=newNode("Exp",3,$1,$2,$3);
   if($1->type != NULL && $3->type != NULL && !matchType($1->type, $3->type)){
       printf("Error type 5 at Line %d: Type mismatched for assignment.\n"
                ,yylineno);
       success = 0;
   }
   }
   | Exp AND Exp{$$=newNode("Exp",3,$1,$2,$3);}
   | Exp OR Exp{$$=newNode("Exp",3,$1,$2,$3);}
   | Exp RELOP Exp{$$=newNode("Exp",3,$1,$2,$3);}
   | Exp PLUS Exp{
   $$=newNode("Exp",3,$1,$2,$3);
   if(!matchType($1->type, $3->type)){
       printf("Error type 7 at Line %d: Type mismatched for operands.\n"
               ,yylineno);
       success = 0;
   }
   else{
       copytype($$->type, $1->type);
   }
   }
   | Exp MINUS Exp{$$=newNode("Exp",3,$1,$2,$3);
   if(!matchType($1->type, $3->type)){
       printf("Error type 7 at Line %d: Type mismatched for operands.\n"
               ,yylineno);
       success = 0;
   }
   else{
       copytype($$->type, $1->type);
   }
   }
   | Exp STAR Exp{$$=newNode("Exp",3,$1,$2,$3);
   if(!matchType($1->type, $3->type)){
       printf("Error type 7 at Line %d: Type mismatched for operands.\n"
               ,yylineno);
       success = 0;
   }
   else{
       copytype($$->type, $1->type);
   }
   }
   | Exp DIV Exp{$$=newNode("Exp",3,$1,$2,$3);
   if(!matchType($1->type, $3->type)){
       printf("Error type 7 at Line %d: Type mismatched for operands.\n"
               ,yylineno);
       success = 0;
   }
   else{
       copytype($$->type, $1->type);
   }
   }
   | LP Exp RP{$$=newNode("Exp",3,$1,$2,$3);
   copytype($$->type, $2->type);
   }
   | MINUS Exp %prec UMINUS{$$=newNode("Exp",2,$1,$2);
   copytype($$->type, $2->type);
   }
   | NOT Exp{$$=newNode("Exp",2,$1,$2);
   copytype($$->type, $2->type);
   }
   | ID LP Args RP{
   $$=newNode("Exp",4,$1,$2,$3,$4);
   Type type = haveFunc($1->id);
   if(!type){
       printf("Error type 2 at Line %d: Undefined function \"%s\".\n",
       yylineno,
       $1->id);
       success = 0;
   }
   else
       matchArgsType(type, $1->id); 
   }
   | ID LP RP{
   $$=newNode("Exp",3,$1,$2,$3);
   Type type = haveFunc($1->id);
   if(!type){
       printf("Error type 2 at Line %d: Undefined function \"%s\".\n",
       yylineno,
       $1->id);
       success = 0;
   }
   else
       matchArgsType(type, $1->id); 
   }
   | Exp LB Exp RB{
   $$=newNode("Exp",4,$1,$2,$3,$4);
   if($1->type->kind != ARRAY){
       printf("Error type 10 at Line %d: \"%s\" is not an array.\n",
       yylineno,
       $1->id);
       success = 0;
   }
   if($3->type->kind != BASIC || 
   ($3->type->kind == BASIC && strcmp($3->type->basic, "int"))){
       printf("Error type 12 at Line %d: \"",yylineno);
       printExp($3);
       printf("\" is not an integer.\n");
       success = 0;
   }
   }
   | Exp DOT ID{
   $$=newNode("Exp",3,$1,$2,$3);
   if($1->type->kind != STRUCTURE){
       printf("Error type 13 at Line %d: Illegal use of \".\".\n",
       yylineno);
       success = 0;
   }
   else{
       int offset;
       Type type;
       bool ret = getStructVarOffset($3->id, $1->type, &offset, &type);
       if(!ret){
           printf("Error type 14 at Line %d: Non-existent field \"%s\".\n",
           yylineno,
           $3->id);
           success = 0;
       }
       else{
           //get the variable in struct's offset
           copytype($$->type, type);
       }
   }
   }
   | ID{
   $$=newNode("Exp",1,$1);
   Type type = getSymbol($1->id);
   if(!type){ 
       printf("Error type 1 at Line %d: Undefined variable \"%s\".\n",
       yylineno,
       $1->id);
       success = 0;
   }
   else{
       $$->type = type;
   }
   }
   | INT{$$=newNode("Exp",1,$1);}
   | FLOAT{
   $$=newNode("Exp",1,$1);
   }
   /*| error {success = 0; yyerrok;}*/
   ;
Args: Exp COMMA Args{
    $$=newNode("Args",3,$1,$2,$3);
   newArg($1->type);
    }
    | Exp{
    $$=newNode("Args",1,$1);
    newArg($1->type);
    }
    ;
%%
void yyerror(const char* s){
    fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, s);
}
