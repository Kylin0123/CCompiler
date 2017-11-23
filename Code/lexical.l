%{
    #include "syntax.tab.h"
    #include <assert.h>
    #include "NodeTree.h"

    extern int success;
    int yycolumn = 1;
    
    #define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
%}
%option yylineno    
digit [0-9]
digits {digit}+
letter [_a-zA-Z]

BLANK [ \r\t]

INT8 0[0-7]+
INT16 0x[0-9A-Fa-f]+
INT ([1-9]{digit}*)|0|{INT8}|{INT16}
SCIENCE {digits}(\.{digits})?[Ee][+-]?{digits}
FLOAT ({digit}*\.{digit}+)|{SCIENCE}
ID {letter}({letter}|{digit})*
SEMI [;]
COMMA [,]
ASSIGNOP [=]
RELOP >|<|(>=)|(<=)|(==)|(!=)
PLUS [\+]
MINUS [-]
STAR [\*]
DIV [/]
AND &&
OR \|\|
DOT [\.]
NOT [!]
TYPE (int)|(float)
LP [\(]
RP [\)]
LB [\[]
RB [\]]
LC [\{]
RC [\}]
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
COMMENTS (\/{2}.*[\n])|(\/\*(([^\*]*(\*[^\/])?)*)\*\/)

%%
{
    {COMMENTS} {
                    /*This is a comment and do nothing.*/
               }
    {INT} { 
            yylval.type_node = newNode("INT",0,yylineno);
            return INT;
          }
    {FLOAT} {
                yylval.type_node = newNode("FLOAT",0,yylineno);
                return FLOAT;
            }
    {SEMI} {
                yylval.type_node = newNode("SEMI",0,yylineno);
                return SEMI;
           }
    {COMMA} {
                yylval.type_node = newNode("COMMA",0,yylineno);
                return COMMA;
            }
    {ASSIGNOP} {
                    yylval.type_node = newNode("ASSIGNOP",0,yylineno);
                    return ASSIGNOP;
               }
    {RELOP} { 
                yylval.type_node = newNode("RELOP",0,yylineno);
                return RELOP;
            }
    {PLUS} { 
                yylval.type_node = newNode("PLUS",0,yylineno);
                return PLUS;
           }
    {MINUS} { 
                yylval.type_node = newNode("MINUS",0,yylineno);
                return MINUS;
            }
    {STAR} {  
                yylval.type_node = newNode("STAR",0,yylineno);
                return STAR;
            }
    {DIV} { 
                yylval.type_node = newNode("DIV",0,yylineno);
                return DIV;
          }
    {AND} { 
                yylval.type_node = newNode("AND",0,yylineno);
                return AND;
          }
    {OR} { 
            yylval.type_node = newNode("OR",0,yylineno);
            return OR;
         }
    {DOT} { 
            yylval.type_node = newNode("DOT",0,yylineno);
            return DOT;
          }
    {NOT} { 
            yylval.type_node = newNode("NOT",0,yylineno);
            return NOT;
          }
    {TYPE} { 
                yylval.type_node = newNode("TYPE",0,yylineno);
                return TYPE;
           }
    {LP} { 
            yylval.type_node = newNode("LP",0,yylineno);
            return LP;
         }
    {RP} { 
            yylval.type_node = newNode("RP",0,yylineno);
            return RP;
         }
    {LB} { 
            yylval.type_node = newNode("LB",0,yylineno);
            return LB;
         }
    {RB} { 
            yylval.type_node = newNode("RB",0,yylineno);
            return RB;
         }
    {LC} { 
            yylval.type_node = newNode("LC",0,yylineno);
            return LC;
         }
    {RC} { 
            yylval.type_node = newNode("RC",0,yylineno);
            return RC;
         }
    {STRUCT} { 
                yylval.type_node = newNode("STRUCT",0,yylineno);
                return STRUCT;
             }
    {RETURN} { 
                yylval.type_node = newNode("RETURN",0,yylineno);
                return RETURN;
             }
    {IF} { 
            yylval.type_node = newNode("IF",0,yylineno);
            return IF;
         }
    {ELSE} { 
                yylval.type_node = newNode("ELSE",0,yylineno);
                return ELSE;
           }
    {WHILE} { 
                yylval.type_node = newNode("WHILE",0,yylineno);
                return WHILE;
            }

    {ID} { 
            yylval.type_node = newNode("ID",0,yylineno);
            return ID;
         }

    {BLANK} {  }
    [\n] { yycolumn = 1;}
    . {
        success = 0;
        printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);
    }
}
%%
