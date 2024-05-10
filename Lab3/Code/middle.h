#ifndef MIDDLE_H
#define MIDDLE_H
#include "syntaTree.h"
#include "semantic.h"

typedef struct _operand* pOperand;
typedef struct _interCode* pInterCode;
typedef struct _interCodes* pInterCodes;
typedef struct _arg* pArg;
typedef struct _argList* pArgList;
typedef struct _interCodeList* pInterCodeList;

typedef struct _operand {
    enum {
        OP_VARIABLE,
        OP_CONSTANT,
        OP_ADDRESS,
        OP_LABEL,
        OP_FUNCTION,
        OP_RELOP,
    } kind;

    union {
        int value;
        char* name;
    } u;

} Operand;

typedef struct _interCode {
    enum {
        IR_LABEL,
        IR_FUNCTION,
        IR_ASSIGN,
        IR_ADD,
        IR_SUB,
        IR_MUL,
        IR_DIV,
        IR_GET_ADDR,
        IR_READ_ADDR,
        IR_WRITE_ADDR,
        IR_GOTO,
        IR_IF_GOTO,
        IR_RETURN,
        IR_DEC,
        IR_ARG,
        IR_CALL,
        IR_PARAM,
        IR_READ,
        IR_WRITE,
    } kind;

    union {
        struct {
            pOperand op;
        } oneOp;
        struct {
            pOperand right, left;
        } assign;
        struct {
            pOperand result, op1, op2;
        } binOp;
        struct {
            pOperand x, relop, y, z;
        } ifGoto;
        struct {
            pOperand op;
            int size;
        } dec;
    } u;
} InterCode;

typedef struct _interCodes {
    pInterCode code;
    pInterCodes *prev, *next;
} InterCodes;

typedef struct _arg {
    pOperand op;
    pArg next;
} Arg;

typedef struct _argList {
    pArg head;
    pArg cur;
} ArgList;

typedef struct _interCodeList {
    pInterCodes head;
    pInterCodes cur;
    int tempVarNum;
    int labelNum;
} InterCodeList;

extern int interError;
extern pInterCodeList interCodeList;

// Operand func
pOperand newOperand(int kind, ...);
void setOperand(pOperand p, int kind, void* val);
void printOp(FILE* fp, pOperand op);

// InterCode func
pInterCode newInterCode(int kind, ...);
void printInterCode(FILE* fp, pInterCodeList interCodeList);

// InterCodes func
pInterCodes newInterCodes(pInterCode code);

// Arg and ArgList func
pArg newArg(pOperand op);
pArgList newArgList();

void addArg(pArgList argList, pArg arg);

// InterCodeList func
pInterCodeList newInterCodeList();

void addInterCode(pInterCodeList interCodeList, pInterCodes newCode);

// traverse func
pOperand newTemp();
pOperand newLabel();
int getSize(pType type);
void genInterCodes(pnode node);
void genInterCode(int kind, ...);
void translateExp(pnode node, pOperand place);
void translateArgs(pnode node, pArgList argList);
void translateCond(pnode node, pOperand labelTrue, pOperand labelFalse);
void translateVarDec(pnode node, pOperand place);
void translateDec(pnode node);
void translateDecList(pnode node);
void translateDef(pnode node);
void translateDefList(pnode node);
void translateCompSt(pnode node);
void translateStmt(pnode node);
void translateStmtList(pnode node);
void translateFunDec(pnode node);
void translateExtDef(pnode node);
void translateExtDefList(pnode node);

#endif