%{
#include "lex.yy.c"
#include <stdio.h>
#include "NodeTree.h"
#include "SymbolTable.h"

int yylex();
void yyerror(const char* s);
int success = 1;
extern SymbolTable symbolTable;
extern TypeStack typeStack;

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
       if(!success) {
           exit(0);
       }
       assert($$ != NULL);
       printNodeTree($$, 0);
       printSymbolTable(symbolTable);
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
      newVar(2,$1,$2);
      }
      | Specifier SEMI {
      $$=newNode("ExtDef",2,$1,$2);
      }
      | Specifier FunDec CompSt{
      $$=newNode("ExtDef",3,$1,$2,$3);
      newFunc(2,$1,$2);
      matchReturnType($1->type, $3->type, $3->lineno);
      }
      ;
ExtDecList: VarDec{
          $$=newNode("ExtDecList",1,$1);
          }
          | VarDec COMMA ExtDecList{
          $$=newNode("ExtDecList",3,$1,$2,$3);
          }
          ;
Specifier: TYPE {
         $$=newNode("Specifier",1,$1);
         //$$->type = $1->type;
         copytype($$->type, $1->type);
         }
         | StructSpecifier{
         $$=newNode("Specifier",1,$1);
         }
         ;
StructSpecifier: STRUCT OptTag LC DefList RC{
               $$=newNode("StructSpecifier",5,$1,$2,$3,$4,$5);
               }
               | STRUCT Tag{
               $$=newNode("StructSpecifier",2,$1,$2);
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
      $$->type = newType();
      $$->type->kind = BASIC;
      }
      | VarDec LB INT RB{
      $$=newNode("VarDec",4,$1,$2,$3,$4);
      strcpy($$->id, $1->id);
      $$->type = newType();
      $$->type->kind = ARRAY;
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
        newVar(2,$1,$2);
        }
        ;
CompSt: LC DefList StmtList RC{
      $$=newNode("CompSt",4,$1,$2,$3,$4);
      $$->lineno = $3->lineno;
      copytype($$->type, $3->type);
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
   newVar(2,$1,$2);
   /*if($2->type != NULL){
       if($2->type->kind == ARRAY){
           printf("array!\n");
           //newArray(2,$1,$2);
           newVar(2,$1,$2);
       }
       else
           newVar(2,$1,$2);
   }*/
   }
   | error SEMI { success = 0; yyerrok; }
   ;
DecList: Dec {
       $$=newNode("DecList",1,$1);
       }
       | Dec COMMA DecList{
       $$=newNode("DecList",3,$1,$2,$3);
       copytype($$->type, $1->type);
       }
       ;
Dec: VarDec {
   $$=newNode("Dec",1,$1);
   }
   | VarDec ASSIGNOP Exp{
   $$=newNode("Dec",3,$1,$2,$3);
   strcpy($$->id, $1->id);
   }
   ;
Exp:Exp ASSIGNOP Exp{
   if(!isLeftVal($1)){
       printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", yylineno);
       success = 0;
   }
   $$=newNode("Exp",3,$1,$2,$3);
   //assert($1->type != NULL);
   assert($3->type != NULL);
   matchType($1->type, $3->type, true);
   }
   | Exp AND Exp{$$=newNode("Exp",3,$1,$2,$3);}
   | Exp OR Exp{$$=newNode("Exp",3,$1,$2,$3);}
   | Exp RELOP Exp{$$=newNode("Exp",3,$1,$2,$3);}
   | Exp PLUS Exp{
   $$=newNode("Exp",3,$1,$2,$3);
   $$->type = matchOperandsType($1->type, $3->type);
   }
   | Exp MINUS Exp{$$=newNode("Exp",3,$1,$2,$3);}
   | Exp STAR Exp{$$=newNode("Exp",3,$1,$2,$3);}
   | Exp DIV Exp{$$=newNode("Exp",3,$1,$2,$3);}
   | LP Exp RP{$$=newNode("Exp",3,$1,$2,$3);}
   | MINUS Exp %prec UMINUS{$$=newNode("Exp",2,$1,$2);}
   | NOT Exp{$$=newNode("Exp",2,$1,$2);}
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
   //TODO
   if(!isArray($3->type)){
       printf("Error type 10 at Line %d: \"%s\" is not an array.\n",
       yylineno,
       $1->id);
       success = 0;
   }
   }
   | Exp DOT ID{$$=newNode("Exp",3,$1,$2,$3);}
   | ID{
   $$=newNode("Exp",1,$1);
   Type type = haveVar($1->id);
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
    fprintf(stderr, "Error type B at: Line %d: %s\n", yylineno, s);
}
