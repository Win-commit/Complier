%{  
#include <stdio.h> 
#include "syntaTree.h"
#include"lex.yy.c"

extern int synerr;
node* root;

#define Not_a_Token 0
%} 

%locations


%union {   
int type_int; 
float type_float; 
struct node* pnode
}  

%token <pnode> INT
%token <pnode> FLOAT
%token <pnode> ID
%token <pnode> SEMI
%token <pnode> COMMA
%token <pnode> TYPE
%token <pnode> LC RC
%token <pnode> STRUCT
%token <pnode> RETURN
%token <pnode> IF
%token <pnode> ELSE
%token <pnode> WHILE
%token <pnode> ASSIGNOP
%token <pnode> OR
%token <pnode> AND
%token <pnode> RELOP
%token <pnode> PLUS MINUS
%token <pnode> STAR DIV
%token <pnode> NOT
%token <pnode> LP RP LB RB DOT

%type <pnode> Program ExtDefList ExtDef ExtDecList   
%type <pnode> Specifier StructSpecifier OptTag Tag   
%type <pnode> VarDec FunDec VarList ParamDec         
%type <pnode> CompSt StmtList Stmt                   
%type <pnode> DefList Def Dec DecList               
%type <pnode> Exp Args                             

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
Program : ExtDefList {$$ = CreateInsideNode(@$.first_line,"Program",Not_a_Token,"__Value__",1,$1);root = $$;}
 ;
ExtDefList :  /* empty */ {$$ = NULL;}
 | ExtDef ExtDefList {$$ = CreateInsideNode(@$.first_line,"ExtDefList",Not_a_Token,"__Value__",2,$1,$2);}
 ;
ExtDef : Specifier ExtDecList SEMI {$$ = CreateInsideNode(@$.first_line,"ExtDef",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | Specifier SEMI {$$ = CreateInsideNode(@$.first_line,"ExtDef",Not_a_Token,"__Value__",2,$1,$2);}
 | Specifier FunDec CompSt {$$ = CreateInsideNode(@$.first_line,"ExtDef",Not_a_Token,"__Value__",3,$1,$2,$3);}
 ;
ExtDecList : VarDec {$$ = CreateInsideNode(@$.first_line,"ExtDecList",Not_a_Token,"__Value__",1,$1);}
 | VarDec COMMA ExtDecList {$$ = CreateInsideNode(@$.first_line,"ExtDecList",Not_a_Token,"__Value__",3,$1,$2,$3);}
 ;
Specifier : TYPE {$$ = CreateInsideNode(@$.first_line,"Specifier",Not_a_Token,"__Value__",1,$1);}
 | StructSpecifier {$$ = CreateInsideNode(@$.first_line,"Specifier",Not_a_Token,"__Value__",1,$1);}
 ;
StructSpecifier : STRUCT OptTag LC DefList RC {$$ = CreateInsideNode(@$.first_line,"StructSpecifier",Not_a_Token,"__Value__",5,$1,$2,$3,$4,$5);}
 | STRUCT Tag {$$ = CreateInsideNode(@$.first_line,"StructSpecifier",Not_a_Token,"__Value__",2,$1,$2);}
 ;
OptTag : /* empty */ {$$ = NULL;}
 | ID {$$ = CreateInsideNode(@$.first_line,"OptTag",Not_a_Token,"__Value__",1,$1);}
 ;
Tag: ID {$$ = CreateInsideNode(@$.first_line,"Tag",Not_a_Token,"__Value__",1,$1);}
 ;
VarDec : ID {$$ = CreateInsideNode(@$.first_line,"VarDec",Not_a_Token,"__Value__",1,$1);}
 | VarDec LB INT RB {$$ = CreateInsideNode(@$.first_line,"VarDec",Not_a_Token,"__Value__",4,$1,$2,$3,$4);}
 ;
FunDec : ID LP VarList RP  {$$ = CreateInsideNode(@$.first_line,"FunDec",Not_a_Token,"__Value__",4,$1,$2,$3,$4);}
 | ID LP RP  {$$ = CreateInsideNode(@$.first_line,"FunDec",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | ID LP error RP {synerr = 1;}
 ;
VarList: ParamDec COMMA VarList  {$$ = CreateInsideNode(@$.first_line,"VarList",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | ParamDec  {$$ = CreateInsideNode(@$.first_line,"VarList",Not_a_Token,"__Value__",1,$1);}
 ;
ParamDec : Specifier VarDec  {$$ = CreateInsideNode(@$.first_line,"ParamDec",Not_a_Token,"__Value__",2,$1,$2);}
 ;
CompSt : LC DefList StmtList RC {$$ = CreateInsideNode(@$.first_line,"CompSt",Not_a_Token,"__Value__",4,$1,$2,$3,$4);}
 ;
StmtList : /* empty */ {$$ = NULL;}
 | Stmt StmtList {$$ = CreateInsideNode(@$.first_line,"StmtList",Not_a_Token,"__Value__",2,$1,$2);}
 ;
Stmt : Exp SEMI {$$ = CreateInsideNode(@$.first_line,"Stmt",Not_a_Token,"__Value__",2,$1,$2);}
 | CompSt {$$ = CreateInsideNode(@$.first_line,"Stmt",Not_a_Token,"__Value__",1,$1);}
 | RETURN Exp SEMI  {$$ = CreateInsideNode(@$.first_line,"Stmt",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE  {$$ = CreateInsideNode(@$.first_line,"Stmt",Not_a_Token,"__Value__",5,$1,$2,$3,$4,$5);}
 | IF LP Exp RP Stmt ELSE Stmt  {$$ = CreateInsideNode(@$.first_line,"Stmt",Not_a_Token,"__Value__",7,$1,$2,$3,$4,$5,$6,$7);}
 | WHILE LP Exp RP Stmt  {$$ = CreateInsideNode(@$.first_line,"Stmt",Not_a_Token,"__Value__",5,$1,$2,$3,$4,$5);}
 | error SEMI {synerr = 1;}
 ;
DefList : /* empty */  {$$ = NULL;}
 | Def DefList  {$$ = CreateInsideNode(@$.first_line,"DefList",Not_a_Token,"__Value__",2,$1,$2);}
 ;
Def : Specifier DecList SEMI  {$$ = CreateInsideNode(@$.first_line,"Def",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | Specifier error SEMI {synerr = 1;}
 ;
DecList : Dec  {$$ = CreateInsideNode(@$.first_line,"DecList",Not_a_Token,"__Value__",1,$1);}
 | Dec COMMA DecList  {$$ = CreateInsideNode(@$.first_line,"DecList",Not_a_Token,"__Value__",3,$1,$2,$3);}
 ;
Dec : VarDec  {$$ = CreateInsideNode(@$.first_line,"Dec",Not_a_Token,"__Value__",1,$1);}
 | VarDec ASSIGNOP Exp {$$ = CreateInsideNode(@$.first_line,"Dec",Not_a_Token,"__Value__",3,$1,$2,$3);}
 ;
Exp : Exp ASSIGNOP Exp {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | Exp AND Exp  {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | Exp OR Exp  {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | Exp RELOP Exp  {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | Exp PLUS Exp  {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | Exp MINUS Exp  {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | Exp STAR Exp  {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | Exp DIV Exp  {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | LP Exp RP {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | MINUS Exp {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",2,$1,$2);}
 | NOT Exp {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",2,$1,$2);}
 | ID LP Args RP  {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",4,$1,$2,$3,$4);}
 | ID LP RP  {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | Exp LB Exp RB  {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",4,$1,$2,$3,$4);}
 | Exp DOT ID  {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | ID  {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",1,$1);}
 | INT {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",1,$1);}
 | FLOAT {$$ = CreateInsideNode(@$.first_line,"Exp",Not_a_Token,"__Value__",1,$1);}
 | error {synerr = 1;}
 ;
Args : Exp COMMA Args  {$$ = CreateInsideNode(@$.first_line,"Args",Not_a_Token,"__Value__",3,$1,$2,$3);}
 | Exp {$$ = CreateInsideNode(@$.first_line,"Args",Not_a_Token,"__Value__",1,$1);}
 ;
%%

yyerror(char* msg) { 
    printf("Error type B at Line %d : %s\n",yylineno,msg);
  }