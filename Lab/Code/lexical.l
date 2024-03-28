%{
#include "syntax.tab.h"
#include "syntaTree.h"


int yycolumn = 1;
extern int lexerr;

#define YY_USER_ACTION \
yylloc.first_line = yylloc.last_line = yylineno; \
yylloc.first_column = yycolumn; \
yylloc.last_column = yycolumn + yyleng - 1; \
yycolumn += yyleng;
# define Token 1
%}

%option yylineno


INT  0|[1-9][0-9]*
FLOAT ([0-9]+\.[0-9]+)|(([0-9]+\.[0-9]*|\.[0-9]+)([Ee][+-]?[0-9]+))
ID  [a-zA-Z_][a-zA-Z_0-9]{0,30}
SEMI  ";" 
COMMA  "," 
ASSIGNOP  "=" 
RELOP  ">"|"<"|">="|"<="|"=="|"!="
PLUS  "+" 
MINUS  "-" 
STAR  "*" 
DIV  "/" 
AND  "&&" 
OR  "||" 
DOT  "." 
NOT  "!" 
TYPE  "int"|"float"
LP  "(" 
RP  ")" 
LB  "[" 
RB  "]" 
LC  "{" 
RC  "}" 
STRUCT  "struct" 
RETURN  "return" 
IF  "if" 
ELSE  "else" 
WHILE  "while" 
%%
{TYPE} {yylval.pnode=CreateLeaveNode(yylineno,"TYPE",Token,yytext);return TYPE;}
{STRUCT} {yylval.pnode=CreateLeaveNode(yylineno,"STRUCT",Token,yytext);return STRUCT;}
{RETURN} {yylval.pnode=CreateLeaveNode(yylineno,"RETURN",Token,yytext);return RETURN;}
{IF} {yylval.pnode=CreateLeaveNode(yylineno,"IF",Token,yytext);return IF;}
{ELSE} {yylval.pnode=CreateLeaveNode(yylineno,"ELSE",Token,yytext);return ELSE;}
{WHILE} {yylval.pnode=CreateLeaveNode(yylineno,"WHILE",Token,yytext);return WHILE;}
{INT} { yylval.pnode=CreateLeaveNode(yylineno,"INT",Token,yytext);return INT;}
{FLOAT} {yylval.pnode=CreateLeaveNode(yylineno,"FLOAT",Token,yytext);return FLOAT;}
{ID} {yylval.pnode=CreateLeaveNode(yylineno,"ID",Token,yytext);return ID;}
{SEMI} {yylval.pnode=CreateLeaveNode(yylineno,"SEMI",Token,yytext);return SEMI;}
{COMMA} {yylval.pnode=CreateLeaveNode(yylineno,"COMMA",Token,yytext);return COMMA;}
{ASSIGNOP} {yylval.pnode=CreateLeaveNode(yylineno,"ASSIGNOP",Token,yytext);return ASSIGNOP;}
{RELOP} {yylval.pnode=CreateLeaveNode(yylineno,"RELOP",Token,yytext);return RELOP;}
{PLUS} {yylval.pnode=CreateLeaveNode(yylineno,"PLUS",Token,yytext);return PLUS;}
{MINUS} {yylval.pnode=CreateLeaveNode(yylineno,"MINUS",Token,yytext);return MINUS;}
{STAR} {yylval.pnode=CreateLeaveNode(yylineno,"STAR",Token,yytext);return STAR;}
{DIV} {yylval.pnode=CreateLeaveNode(yylineno,"DIV",Token,yytext);return DIV;}
{AND} {yylval.pnode=CreateLeaveNode(yylineno,"AND",Token,yytext);return AND;}
{OR} {yylval.pnode=CreateLeaveNode(yylineno,"OR",Token,yytext);return OR;}
{DOT} {yylval.pnode=CreateLeaveNode(yylineno,"DOT",Token,yytext);return DOT;}
{NOT} {yylval.pnode=CreateLeaveNode(yylineno,"NOT",Token,yytext);return NOT;}
{LP} {yylval.pnode=CreateLeaveNode(yylineno,"LP",Token,yytext);return LP;}
{RP} {yylval.pnode=CreateLeaveNode(yylineno,"RP",Token,yytext);return RP;}
{LB} {yylval.pnode=CreateLeaveNode(yylineno,"LB",Token,yytext);return LB;}
{RB} {yylval.pnode=CreateLeaveNode(yylineno,"RB",Token,yytext);return RB;}
{LC} {yylval.pnode=CreateLeaveNode(yylineno,"LC",Token,yytext);return LC;}
{RC} {yylval.pnode=CreateLeaveNode(yylineno,"RC",Token,yytext);return RC;}
[\n\r] {yycolumn = 1;}
[ \t\v\f] { }

. {lexerr=1;printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno, yytext);}
%%
