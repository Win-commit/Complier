#include "middle.h"

int interError = 0;
pInterCodeList interCodeList;

// Operand func
pOperand newOperand(int kind, ...) {
    pOperand p = (pOperand)malloc(sizeof(Operand));

    p->kind = kind;
    va_list vaList;
    va_start(vaList, kind);
    switch (kind) {
        case OP_CONSTANT:
            p->u.value = va_arg(vaList, int);
            break;
        case OP_VARIABLE: 
        case OP_ADDRESS: 
        case OP_LABEL: 
        case OP_FUNCTION: 
        case OP_RELOP: 
            p->u.name = va_arg(vaList, char*);
            break;
    }


    return p;
}


void setOperand(pOperand p, int kind, void* val) {

    p->kind = kind;
    switch (kind) {
        case OP_CONSTANT:
            p->u.value = (int)val;
            break;
        case OP_VARIABLE: 
        case OP_ADDRESS: 
        case OP_LABEL: 
        case OP_FUNCTION: 
        case OP_RELOP:
            p->u.name = (char*)val;
            break;
    }
}


void printOp(FILE* fp, pOperand op) {

    if (fp == NULL) {
        switch (op->kind) {
            case OP_CONSTANT:
                printf("#%d", op->u.value);
                break;
            case OP_VARIABLE: 
            case OP_ADDRESS: 
            case OP_LABEL: 
            case OP_FUNCTION: 
            case OP_RELOP: 
                printf("%s", op->u.name);
                break;
        }
    } else {
        switch (op->kind) {
            case OP_CONSTANT:
                fprintf(fp, "#%d", op->u.value);
                break;
            case OP_VARIABLE: 
            case OP_ADDRESS:
            case OP_LABEL: 
            case OP_FUNCTION: 
            case OP_RELOP:
                fprintf(fp, "%s", op->u.name);
                break;
        }
    }
}

// InterCode func
pInterCode newInterCode(int kind, ...) {
    pInterCode p = (pInterCode)malloc(sizeof(InterCode));
  
    p->kind = kind;
    va_list vaList;
    va_start(vaList, kind);
    switch (kind) {
        case IR_LABEL: 
        case IR_FUNCTION: 
        case IR_GOTO: 
        case IR_RETURN: 
        case IR_ARG: 
        case IR_PARAM:
        case IR_READ: 
        case IR_WRITE: 
            p->u.oneOp.op = va_arg(vaList, pOperand);
            break;
        case IR_ASSIGN: 
        case IR_GET_ADDR: 
        case IR_READ_ADDR: 
        case IR_WRITE_ADDR: 
        case IR_CALL:
            p->u.assign.left = va_arg(vaList, pOperand);
            p->u.assign.right = va_arg(vaList, pOperand);
            break;
        case IR_ADD: 
        case IR_SUB: 
        case IR_MUL: 
        case IR_DIV:
            p->u.binOp.result = va_arg(vaList, pOperand);
            p->u.binOp.op1 = va_arg(vaList, pOperand);
            p->u.binOp.op2 = va_arg(vaList, pOperand);
            break;
        case IR_DEC:
            p->u.dec.op = va_arg(vaList, pOperand);
            p->u.dec.size = va_arg(vaList, int);
            break;
        case IR_IF_GOTO:
            p->u.ifGoto.x = va_arg(vaList, pOperand);
            p->u.ifGoto.relop = va_arg(vaList, pOperand);
            p->u.ifGoto.y = va_arg(vaList, pOperand);
            p->u.ifGoto.z = va_arg(vaList, pOperand);
    }
    return p;
}

void printInterCode(FILE* fp, pInterCodeList interCodeList) {
    for (pInterCodes cur = interCodeList->head; cur != NULL; cur = cur->next) {
        if (fp == NULL) {
            switch (cur->code->kind) {
                case IR_LABEL:
                    printf("LABEL ");
                    printOp(fp, cur->code->u.oneOp.op);
                    printf(" :");
                    break;
                case IR_FUNCTION:
                    printf("FUNCTION ");
                    printOp(fp, cur->code->u.oneOp.op);
                    printf(" :");
                    break;
                case IR_ASSIGN:
                    printOp(fp, cur->code->u.assign.left);
                    printf(" := ");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_ADD:
                    printOp(fp, cur->code->u.binOp.result);
                    printf(" := ");
                    printOp(fp, cur->code->u.binOp.op1);
                    printf(" + ");
                    printOp(fp, cur->code->u.binOp.op2);
                    break;
                case IR_SUB:
                    printOp(fp, cur->code->u.binOp.result);
                    printf(" := ");
                    printOp(fp, cur->code->u.binOp.op1);
                    printf(" - ");
                    printOp(fp, cur->code->u.binOp.op2);
                    break;
                case IR_MUL:
                    printOp(fp, cur->code->u.binOp.result);
                    printf(" := ");
                    printOp(fp, cur->code->u.binOp.op1);
                    printf(" * ");
                    printOp(fp, cur->code->u.binOp.op2);
                    break;
                case IR_DIV:
                    printOp(fp, cur->code->u.binOp.result);
                    printf(" := ");
                    printOp(fp, cur->code->u.binOp.op1);
                    printf(" / ");
                    printOp(fp, cur->code->u.binOp.op2);
                    break;
                case IR_GET_ADDR:
                    printOp(fp, cur->code->u.assign.left);
                    printf(" := &");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_READ_ADDR:
                    printOp(fp, cur->code->u.assign.left);
                    printf(" := *");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_WRITE_ADDR:
                    printf("*");
                    printOp(fp, cur->code->u.assign.left);
                    printf(" := ");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_GOTO:
                    printf("GOTO ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
                case IR_IF_GOTO:
                    printf("IF ");
                    printOp(fp, cur->code->u.ifGoto.x);
                    printf(" ");
                    printOp(fp, cur->code->u.ifGoto.relop);
                    printf(" ");
                    printOp(fp, cur->code->u.ifGoto.y);
                    printf(" GOTO ");
                    printOp(fp, cur->code->u.ifGoto.z);
                    break;
                case IR_RETURN:
                    printf("RETURN ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
                case IR_DEC:
                    printf("DEC ");
                    printOp(fp, cur->code->u.dec.op);
                    printf(" ");
                    printf("%d", cur->code->u.dec.size);
                    break;
                case IR_ARG:
                    printf("ARG ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
                case IR_CALL:
                    printOp(fp, cur->code->u.assign.left);
                    printf(" := CALL ");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_PARAM:
                    printf("PARAM ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
                case IR_READ:
                    printf("READ ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
                case IR_WRITE:
                    printf("WRITE ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
            }
            printf("\n");
        } else {
            switch (cur->code->kind) {
                case IR_LABEL:
                    fprintf(fp, "LABEL ");

                    //printf("%d\n",cur->code->u.oneOp.op->kind);
                    //printf("%s\n",cur->code->u.oneOp.op->u.name);

                    printOp(fp, cur->code->u.oneOp.op);
                    fprintf(fp, " :");
                    break;
                case IR_FUNCTION:
                    fprintf(fp, "FUNCTION ");
                    printOp(fp, cur->code->u.oneOp.op);
                    fprintf(fp, " :");
                    break;
                case IR_ASSIGN:
                    printOp(fp, cur->code->u.assign.left);
                    fprintf(fp, " := ");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_ADD:
                    printOp(fp, cur->code->u.binOp.result);
                    fprintf(fp, " := ");
                    printOp(fp, cur->code->u.binOp.op1);
                    fprintf(fp, " + ");
                    printOp(fp, cur->code->u.binOp.op2);
                    break;
                case IR_SUB:
                    printOp(fp, cur->code->u.binOp.result);
                    fprintf(fp, " := ");
                    printOp(fp, cur->code->u.binOp.op1);
                    fprintf(fp, " - ");
                    printOp(fp, cur->code->u.binOp.op2);
                    break;
                case IR_MUL:
                    printOp(fp, cur->code->u.binOp.result);
                    fprintf(fp, " := ");
                    printOp(fp, cur->code->u.binOp.op1);
                    fprintf(fp, " * ");
                    printOp(fp, cur->code->u.binOp.op2);
                    break;
                case IR_DIV:
                    printOp(fp, cur->code->u.binOp.result);
                    fprintf(fp, " := ");
                    printOp(fp, cur->code->u.binOp.op1);
                    fprintf(fp, " / ");
                    printOp(fp, cur->code->u.binOp.op2);
                    break;
                case IR_GET_ADDR:
                    printOp(fp, cur->code->u.assign.left);
                    fprintf(fp, " := &");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_READ_ADDR:
                    printOp(fp, cur->code->u.assign.left);
                    fprintf(fp, " := *");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_WRITE_ADDR:
                    fprintf(fp, "*");
                    printOp(fp, cur->code->u.assign.left);
                    fprintf(fp, " := ");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_GOTO:
                    fprintf(fp, "GOTO ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
                case IR_IF_GOTO:
                    fprintf(fp, "IF ");
                    printOp(fp, cur->code->u.ifGoto.x);
                    fprintf(fp, " ");
                    printOp(fp, cur->code->u.ifGoto.relop);
                    fprintf(fp, " ");
                    printOp(fp, cur->code->u.ifGoto.y);
                    fprintf(fp, " GOTO ");
                    printOp(fp, cur->code->u.ifGoto.z);
                    break;
                case IR_RETURN:
                    fprintf(fp, "RETURN ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
                case IR_DEC:
                    fprintf(fp, "DEC ");
                    printOp(fp, cur->code->u.dec.op);
                    fprintf(fp, " ");
                    fprintf(fp, "%d", cur->code->u.dec.size);
                    break;
                case IR_ARG:
                    fprintf(fp, "ARG ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
                case IR_CALL:
                    printOp(fp, cur->code->u.assign.left);
                    fprintf(fp, " := CALL ");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_PARAM:
                    fprintf(fp, "PARAM ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
                case IR_READ:
                    fprintf(fp, "READ ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
                case IR_WRITE:
                    fprintf(fp, "WRITE ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
            }
            fprintf(fp, "\n");
        }
    }
}

// InterCodes func
pInterCodes newInterCodes(pInterCode code) {
    pInterCodes p = (pInterCodes)malloc(sizeof(InterCodes));
    p->code = code;
    p->prev = NULL;
    p->next = NULL;
    return p;
}

// Arg and ArgList func
pArg newArg(pOperand op) {
    pArg p = (pArg)malloc(sizeof(Arg));
    p->op = op;
    p->next = NULL;
    return p;
}

pArgList newArgList() {
    pArgList p = (pArgList)malloc(sizeof(ArgList));
    p->head = NULL;
    p->cur = NULL;
    return p;
}

void addArg(pArgList argList, pArg arg) {
    if (argList->head == NULL) {
        argList->head = arg;
        argList->cur = arg;
    } else {
        argList->cur->next = arg;
        argList->cur = arg;
    }
}


// InterCodeList func
pInterCodeList newInterCodeList() {
    pInterCodeList p = (pInterCodeList)malloc(sizeof(InterCodeList));
    p->head = NULL;
    p->cur = NULL;
    p->tempVarNum = 1;
    p->labelNum = 1;
    return p;
}


void addInterCode(pInterCodeList interCodeList, pInterCodes newCode) {
    if (interCodeList->head == NULL) {
        interCodeList->head = newCode;
        interCodeList->cur = newCode;
    } else {
        interCodeList->cur->next = newCode;
        newCode->prev = interCodeList->cur;
        interCodeList->cur = newCode;
    }
}


// traverse func
pOperand newTemp() {

    char tName[10] = {0};
    sprintf(tName, "t%d", interCodeList->tempVarNum);
    interCodeList->tempVarNum++;
    pOperand temp = newOperand(OP_VARIABLE, copyString(tName));
    return temp;
}

pOperand newLabel() {
    char lName[100] = {0};
    sprintf(lName, "label%d", interCodeList->labelNum);

    //printf("newlabel:\n");
    //printf("%s\n",lName);

    interCodeList->labelNum++;
    pOperand temp = newOperand(OP_LABEL, copyString(lName));

    return temp;
}


int getSize(pType type) {
    if (type == NULL)
        return 0;
    else if (type->kind == BASIC)
        return 4;
    else if (type->kind == ARRAY)
        return type->u.array.size * getSize(type->u.array.elem);
    else if (type->kind == STRUCTURE) {
        int size = 0;
        pFieldList temp = type->u.structure;
        while (temp) {
            size += getSize(temp->type);
            temp = temp->tail;
        }
        return size;
    }
    return 0;
}

void genInterCodes(pnode node) {
    if (node == NULL) return;
    if (!strcmp(node->name, "ExtDefList"))
        translateExtDefList(node);
    else {
        for (int i=0;i<9;i++)
        {genInterCodes(node->children[i]);}
    }
}

void genInterCode(int kind, ...) {
    va_list vaList;
    va_start(vaList,kind);
    pOperand temp = NULL;
    pOperand result = NULL, op1 = NULL, op2 = NULL, relop = NULL;
    int size = 0;
    pInterCodes newCode = NULL;
    switch (kind) {
        case IR_LABEL:
        case IR_FUNCTION:
        case IR_GOTO:
        case IR_RETURN:
        case IR_ARG:
        case IR_PARAM:
        case IR_READ:
        case IR_WRITE:
            op1 = va_arg(vaList, pOperand);
            if (op1->kind == OP_ADDRESS) {
                temp = newTemp();
                genInterCode(IR_READ_ADDR, temp, op1);
                op1 = temp;
            }
            newCode = newInterCodes(newInterCode(kind, op1));

            addInterCode(interCodeList, newCode);
            break;
        case IR_ASSIGN:
        case IR_GET_ADDR:
        case IR_READ_ADDR:
        case IR_WRITE_ADDR:
        case IR_CALL:
            op1 = va_arg(vaList, pOperand);
            op2 = va_arg(vaList, pOperand);
            if (kind == IR_ASSIGN &&
                (op1->kind == OP_ADDRESS || op2->kind == OP_ADDRESS)) {
                if (op1->kind == OP_ADDRESS && op2->kind != OP_ADDRESS)
                    genInterCode(IR_WRITE_ADDR, op1, op2);
                else if (op2->kind == OP_ADDRESS && op1->kind != OP_ADDRESS)
                    genInterCode(IR_READ_ADDR, op1, op2);
                else {
                    temp = newTemp();
                    genInterCode(IR_READ_ADDR, temp, op2);
                    genInterCode(IR_WRITE_ADDR, op1, temp);
                }
            } else {
                newCode = newInterCodes(newInterCode(kind, op1, op2));
                addInterCode(interCodeList, newCode);
            }
            break;
        case IR_ADD:
        case IR_SUB:
        case IR_MUL:
        case IR_DIV:
            result = va_arg(vaList, pOperand);
            op1 = va_arg(vaList, pOperand);
            op2 = va_arg(vaList, pOperand);
            if (op1->kind == OP_ADDRESS) {
                temp = newTemp();
                genInterCode(IR_READ_ADDR, temp, op1);
                op1 = temp;
            }
            if (op2->kind == OP_ADDRESS) {
                temp = newTemp();
                genInterCode(IR_READ_ADDR, temp, op2);
                op2 = temp;
            }
            newCode = newInterCodes(newInterCode(kind, result, op1, op2));
            addInterCode(interCodeList, newCode);
            break;
        case IR_DEC:
            op1 = va_arg(vaList, pOperand);
            size = va_arg(vaList, int);
            newCode = newInterCodes(newInterCode(kind, op1, size));
            addInterCode(interCodeList, newCode);
            break;
        case IR_IF_GOTO:
            result = va_arg(vaList, pOperand);
            relop = va_arg(vaList, pOperand);
            op1 = va_arg(vaList, pOperand);
            op2 = va_arg(vaList, pOperand);
            newCode = newInterCodes(newInterCode(kind, result, relop, op1, op2));
            addInterCode(interCodeList, newCode);
            break;
    }
}

void translateExtDefList(pnode node) {
    // ExtDefList -> ExtDef ExtDefList
    //             | e
    for(int i=0;i<9;i++)
    {   if(node->children[i]){
            if(!strcmp(node->children[i]->name,"ExtDef"))
                {translateExtDef(node->children[i]);}
            else
                {translateExtDefList(node->children[i]);}
    }
        }
    

}


void translateExtDef(pnode node) {
    if (interError) return;
    // ExtDef -> Specifier ExtDecList SEMI
    //         | Specifier SEMI
    //         | Specifier FunDec CompSt

    // 因为没有全局变量使用，
    // ExtDecList不涉及中间代码生成，类型声明也不涉及，所以只需要处理FunDec和CompSt
    if (node->children[1] && !strcmp(node->children[1]->name, "FunDec")) {
        translateFunDec(node->children[1]);
        translateCompSt(node->children[2]);
    }
}


void translateFunDec(pnode node) {
    if (interError) return;
    // FunDec -> ID LP VarList RP
    //         | ID LP RP
    genInterCode(IR_FUNCTION,newOperand(OP_FUNCTION, node->children[0]->value));
    
    pItem funcItem = searchTableItem(node->children[0]->value);
    pFieldList temp = funcItem->field->type->u.function.argv;
    pFieldList order[100];
    int index=0;
    while (temp) {

        //printf("FunDec: %s\n",temp->name);
        order[index++]=temp;
        temp = temp->tail;
    }
    for(int i=index-1;i>=0;i--){
        genInterCode(IR_PARAM, newOperand(OP_VARIABLE, order[i]->name));
    }
}

void translateCompSt(pnode node) {
    if (interError) return;
    // CompSt -> LC DefList StmtList RC
    pnode temp = node->children[1];
    if (temp && !strcmp(temp->name, "DefList")) {
        translateDefList(temp);
    }
    if (node->children[2] && !strcmp(node->children[2]->name, "StmtList")) {
        temp=node->children[2];
        translateStmtList(temp);
    }
}

void translateDefList(pnode node) {
    if (interError) return;
    // DefList -> Def DefList
    //          | e
    for(int i=0;i<9;i++)
    {   if(node->children[i]){
            if(!strcmp(node->children[i]->name,"Def"))
            {translateDef(node->children[i]);}
            else
            {translateDefList(node->children[i]);}
    }
        }
}

void translateDef(pnode node) {
    if (interError) return;
    // Def -> Specifier DecList SEMI
    translateDecList(node->children[1]);
}

void translateDecList(pnode node) {
    if (interError) return;
    // DecList -> Dec
    //          | Dec COMMA DecList
    pnode temp = node;
    while (temp) {
        translateDec(temp->children[0]);
        temp = temp->children[2];
    }
}

void translateDec(pnode node) {
    if (interError) return;
    // Dec -> VarDec
    //      | VarDec ASSIGNOP Exp

    // Dec -> VarDec
    if (node->children[1] == NULL) {
        translateVarDec(node->children[0], NULL);
    }
    // Dec -> VarDec ASSIGNOP Exp
    else {
        pOperand t1 = newTemp();
        translateVarDec(node->children[0], t1);
        pOperand t2 = newTemp();
        translateExp(node->children[2], t2);
        genInterCode(IR_ASSIGN, t1, t2);
    }
}


void translateVarDec(pnode node, pOperand place) {
    if (interError) return;
    // VarDec -> ID
    //         | VarDec LB INT RB

    if (node->children[0] && !strcmp(node->children[0]->name, "ID")) {
        pItem temp = searchTableItem(node->children[0]->value);
        pType type = temp->field->type;
        if (type->kind == BASIC) {
            if (place) {
                interCodeList->tempVarNum--;
                setOperand(place, OP_VARIABLE,(void*)temp->field->name);
            }
        } else if (type->kind == ARRAY) {
            // 4.2选做
            genInterCode(
                IR_DEC,
                newOperand(OP_VARIABLE, temp->field->name),
                getSize(type));
            
        } else if (type->kind == STRUCTURE) {
            // 不会出现结构体变量
            interError = 1;
            printf("Can't translate: Structure variable is not allowed.\n");
            return;
        }
    } else {
        translateVarDec(node->children[0], place);
    }
}


void translateStmtList(pnode node) {
    if (interError) return;
    // StmtList -> Stmt StmtList
    //           | e
    pnode temp = node;
    while (temp) {
        translateStmt(temp->children[0]);
        temp = temp->children[1];
    }
}


void translateStmt(pnode node) {
    if (interError) return;
    // Stmt -> Exp SEMI
    //       | CompSt
    //       | RETURN Exp SEMI
    //       | IF LP Exp RP Stmt
    //       | IF LP Exp RP Stmt ELSE Stmt
    //       | WHILE LP Exp RP Stmt

    // Stmt -> Exp SEMI

    if (!strcmp(node->children[0]->name, "Exp")) {
        translateExp(node->children[0], NULL);
    }

    // Stmt -> CompSt
    else if (!strcmp(node->children[0]->name, "CompSt")) {
        translateCompSt(node->children[0]);
    }

    // Stmt -> RETURN Exp SEMI
    else if (!strcmp(node->children[0]->name, "RETURN")) {
        pOperand t1 = newTemp();
        translateExp(node->children[1], t1);
        genInterCode(IR_RETURN, t1);
    }

    // Stmt -> IF LP Exp RP Stmt
    else if (!strcmp(node->children[0]->name, "IF")) {
        pnode exp = node->children[2];
        pnode stmt = node->children[4];

        pOperand label1 = newLabel();
        pOperand label2 = newLabel();

        //printf("Stmt IF1: %s\n",label1->u.name);
        //printf("Stmt IF2: %s\n",label2->u.name);


        translateCond(exp, label1, label2);
        genInterCode(IR_LABEL, label1);
        translateStmt(stmt);
        if (node->children[5] == NULL) {
            genInterCode(IR_LABEL, label2);
        }
        // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        else {
            pOperand label3 = newLabel();
            genInterCode(IR_GOTO, label3);
            genInterCode(IR_LABEL, label2);
            translateStmt(node->children[6]);
            genInterCode(IR_LABEL, label3);
        }

    }

    // Stmt -> WHILE LP Exp RP Stmt
    else if (!strcmp(node->children[0]->name, "WHILE")) {
        pOperand label1 = newLabel();
        pOperand label2 = newLabel();
        pOperand label3 = newLabel();

        genInterCode(IR_LABEL, label1);
        translateCond(node->children[2], label2, label3);
        genInterCode(IR_LABEL, label2);
        translateStmt(node->children[4]);
        genInterCode(IR_GOTO, label1);
        genInterCode(IR_LABEL, label3);
    }
}



void translateExp(pnode node, pOperand place) {
    if (interError) return;
    // Exp -> Exp ASSIGNOP Exp
    //      | Exp AND Exp
    //      | Exp OR Exp
    //      | Exp RELOP Exp
    //      | Exp PLUS Exp
    //      | Exp MINUS Exp
    //      | Exp STAR Exp
    //      | Exp DIV Exp

    //      | MINUS Exp
    //      | NOT Exp
    //      | ID LP Args RP
    //      | ID LP RP
    //      | Exp LB Exp RB
    //      | Exp DOT ID
    //      | ID
    //      | INT
    //      | FLOAT

    // Exp -> LP Exp RP
    if (!strcmp(node->children[0]->name, "LP"))
        translateExp(node->children[1], place);

    else if (!strcmp(node->children[0]->name, "Exp") ||
             !strcmp(node->children[0]->name, "NOT")) {
        // 条件表达式 和 基本表达式
        if (strcmp(node->children[1]->name, "LB") &&
            strcmp(node->children[1]->name, "DOT")) {
            // Exp -> Exp AND Exp
            //      | Exp OR Exp
            //      | Exp RELOP Exp
            //      | NOT Exp
            if (!strcmp(node->children[1]->name, "AND") ||
                !strcmp(node->children[1]->name, "OR") ||
                !strcmp(node->children[1]->name, "RELOP") ||
                !strcmp(node->children[1]->name, "NOT")) {
                pOperand label1 = newLabel();
                pOperand label2 = newLabel();
                pOperand true_num = newOperand(OP_CONSTANT, 1);
                pOperand false_num = newOperand(OP_CONSTANT, 0);
                genInterCode(IR_ASSIGN, place, false_num);
                translateCond(node, label1, label2);
                genInterCode(IR_LABEL, label1);
                genInterCode(IR_ASSIGN, place, true_num);
                genInterCode(IR_LABEL, label2);
            } else {
                // Exp -> Exp ASSIGNOP Exp
                if (!strcmp(node->children[1]->name, "ASSIGNOP")) {
                    pOperand t2 = newTemp();
                    translateExp(node->children[2], t2);

                    //printf("Exp assignop right:%s\n",t2->u.name);

                    pOperand t1 = newTemp();
                    translateExp(node->children[0], t1);

                    //printf("Exp assignop left:%s\n",t1->u.name);

                    genInterCode(IR_ASSIGN, t1, t2);
                } else {
                    pOperand t1 = newTemp();
                    translateExp(node->children[0], t1);
                    pOperand t2 = newTemp();
                    translateExp(node->children[2], t2);
                    // Exp -> Exp PLUS Exp
                    if (!strcmp(node->children[1]->name, "PLUS")) {
                        genInterCode(IR_ADD, place, t1, t2);
                    }
                    // Exp -> Exp MINUS Exp
                    else if (!strcmp(node->children[1]->name, "MINUS")) {
                        genInterCode(IR_SUB, place, t1, t2);
                    }
                    // Exp -> Exp STAR Exp
                    else if (!strcmp(node->children[1]->name, "STAR")) {
                        genInterCode(IR_MUL, place, t1, t2);
                    }
                    // Exp -> Exp DIV Exp
                    else if (!strcmp(node->children[1]->name, "DIV")) {
                        genInterCode(IR_DIV, place, t1, t2);
                    }
                }
            }

        }
        // 数组和结构体访问
        else {
            // Exp -> Exp LB Exp RB
            if (!strcmp(node->children[1]->name, "LB")) {
                //数组
                if (node->children[0]->children[1] &&
                    !strcmp(node->children[0]->children[1]->name, "LB")) {
                    //多维数组

                    pOperand indexs[200];
                    int i=0;
                    pnode temp = node;
                    while(strcmp(temp->children[0]->name,"ID")){
                        indexs[i]= newTemp();
                        translateExp(temp->children[2],indexs[i]);
                        i++;
                        temp = temp->children[0];
                    }
                    
                    pOperand base = newTemp();
                    translateExp(temp, base);
                    pOperand target = newTemp();
                    genInterCode(IR_GET_ADDR, target, base);
                    int copyi=i;
                    pItem item = searchTableItem(base->u.name);
                    pType subtype = item->field->type->u.array.elem;
                    while(i>0){
                        i--;
                        pOperand width= newOperand(OP_CONSTANT,getSize(subtype));
                        pOperand offset = newTemp();
                        genInterCode(IR_MUL, offset, indexs[i], width);
                        genInterCode(IR_ADD, target, target, offset);
                        if(subtype->kind == ARRAY)
                        {subtype = subtype->u.array.elem;}
                    }
                    genInterCode(IR_ASSIGN, place, target);

                    pType tmp=item->field->type;
                    int j=0;
                    while(tmp->kind==ARRAY){
                        j++;
                        tmp=tmp->u.array.elem;
                    }
                    //a[5][6][4]->a[3][2] 

                    if(copyi==j)
                    {place->kind = OP_ADDRESS;}
                    else
                    {place->kind = OP_VARIABLE;}

                } else {
                    pOperand idx = newTemp();
                    translateExp(node->children[2], idx);
                    pOperand base = newTemp();
                    translateExp(node->children[0], base);

                    pOperand width;
                    pOperand offset = newTemp();
                    pOperand target;
                    // 根据假设，Exp1只会展开为 ID
                    pItem item = searchTableItem(base->u.name);
    
                    width = newOperand(OP_CONSTANT, getSize(item->field->type->u.array.elem));
                    genInterCode(IR_MUL, offset, idx, width);
                    // ID[Exp],需要对ID取址

                    target = newTemp();

                    if(item->field->isArg){
                        genInterCode(IR_ASSIGN, target, base);
                    }
                    else
                    {genInterCode(IR_GET_ADDR, target, base);}

                    genInterCode(IR_ADD, place, target, offset);

                    //a[5][6]->a[3] 
                    if(item->field->type->u.array.elem->kind==ARRAY)
                    {place->kind = OP_VARIABLE;}
                    else
                    {place->kind = OP_ADDRESS;}

                }
            }
            // Exp -> Exp DOT ID(选做4.2不会出现结构体变量)
            else {
                //结构体
                interError = 1;
                printf("Can't translate: Structure variable is not allowed.\n");
                return;  
            }
        }
    }
    //单目运算符
    // Exp -> MINUS Exp
    else if (!strcmp(node->children[0]->name, "MINUS")) {
        pOperand t1 = newTemp();
        translateExp(node->children[1], t1);
        pOperand zero = newOperand(OP_CONSTANT, 0);
        genInterCode(IR_SUB, place, zero, t1);
    }

    // Exp -> ID LP Args RP
    //		| ID LP RP
    else if (!strcmp(node->children[0]->name, "ID") && node->children[1]) {
        pOperand funcTemp = newOperand(OP_FUNCTION, node->children[0]->value);
        // Exp -> ID LP Args RP
        if (!strcmp(node->children[2]->name, "Args")) {
            pArgList argList = newArgList();
            translateArgs(node->children[2], argList);
            if (!strcmp(node->children[0]->value, "write")) {
                genInterCode(IR_WRITE, argList->head->op);
            } else {
                pArg argTemp = argList->head;
                while (argTemp) {
                    if (argTemp->op->kind == OP_VARIABLE) {
                        pItem item = searchTableItem(argTemp->op->u.name);

                        // 一维数组作为参数要取地址
                        if (item && item->field->type->kind == ARRAY) {
                            pOperand varTemp = newTemp();
                            if(!item->field->isArg)
                            {genInterCode(IR_GET_ADDR, varTemp, argTemp->op);
                            pOperand varTempCopy = newOperand(OP_VARIABLE, varTemp->u.name);
                            genInterCode(IR_ARG, varTempCopy);
                            }
                            else{
                                genInterCode(IR_ARG, argTemp->op);
                            }
                        }
                        else {
                            genInterCode(IR_ARG, argTemp->op);
                        }
                    }
                    // 一般参数直接传值
                    else {
                        genInterCode(IR_ARG, argTemp->op);
                    }
                    argTemp = argTemp->next;
                }
                if (place) {
                    genInterCode(IR_CALL, place, funcTemp);
                } else {
                    pOperand temp = newTemp();
                    genInterCode(IR_CALL, temp, funcTemp);
                    
                }
            }
        }
        // Exp -> ID LP RP
        else {
            if (!strcmp(node->children[0]->value, "read")) {
                genInterCode(IR_READ, place);
            } else {
                if (place) {
                    genInterCode(IR_CALL, place, funcTemp);
                } else {
                    pOperand temp = newTemp();
                    genInterCode(IR_CALL, temp, funcTemp);
                }
            }
        }
    }
    // Exp -> ID
    else if (!strcmp(node->children[0]->name, "ID")) {
        pItem item = searchTableItem(node->children[0]->value);
        interCodeList->tempVarNum--;
        // if (item->field->isArg && item->field->type->kind == ARRAY) {
        //     setOperand(place, OP_ADDRESS, (void*)copyString(node->children[0]->value));

        // }

        setOperand(place, OP_VARIABLE, (void*)node->children[0]->value);

    } else {

        // 无浮点数常数
        // Exp -> INT
        interCodeList->tempVarNum--;
        setOperand(place, OP_CONSTANT, (void*)atoi(node->children[0]->value));

    }
}


void translateCond(pnode node, pOperand labelTrue, pOperand labelFalse) {
    if (interError) return;
    // Exp -> Exp AND Exp
    //      | Exp OR Exp
    //      | Exp RELOP Exp
    //      | NOT Exp

    // Exp -> NOT Exp
    if (!strcmp(node->children[0]->name, "NOT")) {
        translateCond(node->children[1], labelFalse, labelTrue);
    }
    // Exp -> Exp RELOP Exp
    else if (!strcmp(node->children[1]->name, "RELOP")) {
        pOperand t1 = newTemp();
        pOperand t2 = newTemp();
        translateExp(node->children[0], t1);
        translateExp(node->children[2], t2);

        pOperand relop = newOperand(OP_RELOP, node->children[1]->value);

        if (t1->kind == OP_ADDRESS) {
            pOperand temp = newTemp();
            genInterCode(IR_READ_ADDR, temp, t1);
            t1 = temp;
        }
        if (t2->kind == OP_ADDRESS) {
            pOperand temp = newTemp();
            genInterCode(IR_READ_ADDR, temp, t2);
            t2 = temp;
        }

        genInterCode(IR_IF_GOTO, t1, relop, t2, labelTrue);
        genInterCode(IR_GOTO, labelFalse);
    }

    // Exp -> Exp AND Exp
    else if (!strcmp(node->children[1]->name, "AND")) {
        pOperand label1 = newLabel();
        translateCond(node->children[0], label1, labelFalse);
        genInterCode(IR_LABEL, label1);
        translateCond(node->children[2], labelTrue, labelFalse);
    }
    // Exp -> Exp OR Exp
    else if (!strcmp(node->children[1]->name, "OR")) {
        pOperand label1 = newLabel();
        translateCond(node->children[0], labelTrue, label1);
        genInterCode(IR_LABEL, label1);
        translateCond(node->children[2], labelTrue, labelFalse);
    }
    // other cases
    else {
        pOperand t1 = newTemp();
        translateExp(node, t1);
        pOperand t2 = newOperand(OP_CONSTANT, 0);
        pOperand relop = newOperand(OP_RELOP, copyString("!="));

        if (t1->kind == OP_ADDRESS) {
            pOperand temp = newTemp();
            genInterCode(IR_READ_ADDR, temp, t1);
            t1 = temp;
        }
        genInterCode(IR_IF_GOTO, t1, relop, t2, labelTrue);
        genInterCode(IR_GOTO, labelFalse);
    }
}


void translateArgs(pnode node, pArgList argList) {
    if (interError) return;
    // Args -> Exp COMMA Args
    //       | Exp

    // Args -> Exp
    pArg temp = newArg(newTemp());
    translateExp(node->children[0], temp->op);

    if (temp->op->kind == OP_VARIABLE) {
        pItem item = searchTableItem(temp->op->u.name);
        if (item && item->field->type->kind ==STRUCTURE) {
            interError = 1;
            printf("Can't translate: Structure variable is not allowed.\n");
            return;
        }
    }
    addArg(argList, temp);

    // Args -> Exp COMMA Args
    if (node->children[1] != NULL) {
        translateArgs(node->children[2], argList);
    }
}
