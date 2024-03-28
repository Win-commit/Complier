#include "syntax.tab.h"
#include "syntaTree.h"
int synerr=0,lexerr=0;
extern node* root;
int main(int argc, char** argv)
{ 
if (argc <= 1) return 1; 
FILE* f = fopen(argv[1], "r"); 
if (!f) 
{ 
perror(argv[1]); 
return 1; 
} 

yyrestart(f); 
yyparse(); 
if (!synerr && !lexerr){
    printftree(root,0);
}
return 0;   
} 

