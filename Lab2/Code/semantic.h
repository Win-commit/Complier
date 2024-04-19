#ifndef SEMANTIC_H
#define SEMANTIC_H
#include "syntaTree.h"

typedef struct Type_* pType;
typedef struct FieldList_* pFieldList;
typedef struct tableItem* pItem;
typedef struct hashTable* ptable;

typedef enum BASICType_ {INT_TYPE,FLOAT_TYPE} BasicType;
typedef enum kind_ { BASIC, ARRAY, STRUCTURE, FUNCTION,STRUCTURE_DEF} Kind;

struct Type_ 
{ 
Kind kind; 
union 
{ 
// 基本类型 
BasicType basic; 
// 数组类型信息包括元素类型与数组大小构成 
struct { pType elem; int size; } array; 
// 结构体类型信息是一个链表 (变量与定义共享)
pFieldList structure; 
//函数信息包括函数名，参数个数，参数名，返回值类型
struct {
int argc; 
pFieldList argv;
pType returnType;} function;
} u; 
}Type; 

struct FieldList_ 
{ 
char* name;  // 域的名字 
pType type;  // 域的类型 
pFieldList tail;  // 下一个域 
}FieldList;


typedef struct tableItem {
    pFieldList field;
    pItem nextitem;  
   
} TableItem;

typedef struct hashTable {
   pItem *hashArray;
} HashTable;



typedef enum _errorType {
    UNDEF_VAR=1,         // Undefined Variable
    UNDEF_FUNC,            // Undefined Function
    REDEF_VAR,             // Redefined Variable
    REDEF_FUNC,            // Redefined Function
    TYPE_MISMATCH_ASSIGN,  // Type mismatched for assignment.
    LEFT_VAR_ASSIGN,  // The left-hand side of an assignment must be a variable.
    TYPE_MISMATCH_OP,      // Type mismatched for operands.
    TYPE_MISMATCH_RETURN,  // Type mismatched for return.
    FUNC_AGRC_MISMATCH,    // Function is not applicable for arguments
    NOT_A_ARRAY,           // Variable is not a Array
    NOT_A_FUNC,            // Variable is not a Function
    NOT_A_INT,             // Variable is not a Integer in []
    ILLEGAL_USE_DOT,       // Illegal use of "."
    NONEXISTFIELD,         // Non-existent field
    REDEF_FEILD,           // Redefined field or assign in inia
    DUPLICATED_NAME,       // Duplicated struct name
    UNDEF_STRUCT           // Undefined structure
} ErrorType;


static inline void pError(ErrorType type, int line, char* msg) {
    printf("Error type %d at Line %d: %s\n", type, line, msg);
}

//utilities
void scanTree(pnode node);
pType newType(Kind kind, ...);
pType copyType(pType src) ;
pType deepCopyTypePtr(const pType src);
int checkType(pType type1, pType type2);
pItem newItem(pFieldList pfield);
pFieldList newFieldList(char* Name, pType newType);
pFieldList copyFieldList(pFieldList src);
void setFieldListName(pFieldList p, char* newName);
unsigned hash_pjw(char* name);
void newtable() ;
pItem searchTableItem(char* name);
int checkItemConflict(ptable table, pItem item);
int checkSTRUCTURE_DEFConflict(ptable table,pItem item);
int checkFUNCTIONConflict(ptable table,pItem item);
void addTableItem(pItem item);
char* generateRandomString(int length);


// table functions
void ExtDef(pnode node);
void ExtDecList(pnode node, pType specifier);
pType Specifier(pnode node);
pType StructSpecifier(pnode node);
pItem VarDec(pnode node, pType specifier);
void FunDec(pnode node, pType returnType);
void VarList(pnode node, pItem func);
pFieldList ParamDec(pnode node);
void CompSt(pnode node, pType returnType);
void StmtList(pnode node, pType returnType);
void Stmt(pnode node, pType returnType);
void DefList(pnode node, pItem structInfo);
void Def(pnode node, pItem structInfo);
void DecList(pnode node, pType specifier, pItem structInfo);
void Dec(pnode node, pType specifier, pItem structInfo);
pType Exp(pnode node);
void Args(pnode node, pItem funcInfo);

#endif
