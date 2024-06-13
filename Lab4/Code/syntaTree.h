#ifndef SYNTATREE_H 
#define SYNTATREE_H

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <stdarg.h>
#include <time.h>
typedef struct synaNode{
    char* name; //e.x.:ID,Program,INT,FLOAT...
    int _type;//is tocken or not[1 or 0]
    char* value;// yylval for token and "Value" for others
    unsigned lineno;// the position of object
    struct synaNode* children[9]; //children Node,and in this case,the most children nodes of one parent node is 7
}node;
typedef node* pnode;

static inline char* copyString(const char* src) {
    if (src == NULL) return NULL;
    size_t len = strlen(src) + 1;
    char* dest = (char*)malloc(len);
    if (dest != NULL) {
        strncpy(dest, src, len - 1);
        dest[len - 1] = '\0';
    }
    return dest;
}

static inline node* CreateLeaveNode(unsigned lineno,char* name,int _type,char* value){
    node* newNode = (node*)malloc(sizeof(node));
    newNode->lineno = lineno;
    newNode->name = copyString(name); 
    newNode->_type = _type;
    newNode->value = copyString(value);

    for (int i = 0; i < 9; i++) {
        newNode->children[i] = NULL;
    }
    
    return newNode;
}

static inline node* CreateInsideNode(unsigned lineno,char* name, int _type,char* value,int n_child,...){
   node* InitialNode = CreateLeaveNode(lineno,name,_type,value);
   va_list args;
   va_start(args, n_child);
   for (int i = 0; i < n_child ; i++){ 
        node* tmp = va_arg(args, node*); 
        InitialNode->children[i] = tmp;
}
    va_end(args);
    return InitialNode;
}

static inline void printftree(node* Node,int height){
    if(Node == NULL)
    {return;}
    for (int i = 0; i < height; i++) {
        printf("  ");
    }
    printf("%s", Node->name);
    if (Node->_type ==0){
        printf(" (%d)", Node->lineno);
    }
    else if (strcmp(Node->name, "TYPE") == 0 || strcmp(Node->name, "INT") == 0 || strcmp(Node->name, "ID") == 0){
        printf(": %s", Node->value);
    }
    else if (strcmp(Node->name, "FLOAT") == 0){
        printf(": %lf", atof(Node->value));
    }

    printf("\n");
    for(int i = 0; i < 9 ; i++){
        printftree(Node->children[i],height+1);
    }
}

#endif