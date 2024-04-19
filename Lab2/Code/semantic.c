#include "semantic.h"


#define hash_array_size 0x3fff
#define TRUE 1
#define FALSE 0

ptable table;
int init=0;
// Type functions
pType newType(Kind kind, ...) {
    pType p = (pType)malloc(sizeof(Type));
    p->kind = kind;
    va_list vaList;
    switch (kind) {
        case BASIC:
            va_start(vaList, kind);
            p->u.basic = va_arg(vaList, BasicType);
            break;
        case ARRAY:
            va_start(vaList, kind);
            p->u.array.elem = va_arg(vaList, pType);
            p->u.array.size = va_arg(vaList, int);
            break;
        case STRUCTURE:
            va_start(vaList, kind);
            p->u.structure = va_arg(vaList, pFieldList);
            break;
        case FUNCTION:
            va_start(vaList, kind);
            p->u.function.argc = va_arg(vaList, int);
            p->u.function.argv = va_arg(vaList, pFieldList);
            p->u.function.returnType = va_arg(vaList, pType);
            break;
        case STRUCTURE_DEF:
            va_start(vaList, kind);
            p->u.structure = va_arg(vaList, pFieldList);
            break;
    }
    va_end(vaList);
    return p;
}



pType copyType(pType src) {
    if (src == NULL) return NULL;
    pType p = (pType)malloc(sizeof(Type));
    p->kind = src->kind;
    switch (p->kind) {
        case BASIC:
            p->u.basic = src->u.basic;
            break;
        case ARRAY:
            p->u.array.elem = copyType(src->u.array.elem);
            p->u.array.size = src->u.array.size;
            break;
        case STRUCTURE:
            p->u.structure= copyFieldList(src->u.structure);
            break;
        case FUNCTION:
            p->u.function.argc = src->u.function.argc;
            p->u.function.argv = copyFieldList(src->u.function.argv);
            p->u.function.returnType = copyType(src->u.function.returnType);
            break;
        case STRUCTURE_DEF:
            p->u.structure= copyFieldList(src->u.structure);
            break;
    }

    return p;
}



int checkType(pType type1, pType type2) {
    if (type1 == NULL || type2 == NULL) return TRUE;
    if (type1->kind == FUNCTION || type2->kind == FUNCTION) return FALSE;
    if (type1->kind != type2->kind) return FALSE;
    int res=1;
    pFieldList field1=type1->u.structure,  field2=type2->u.structure;
    switch (type1->kind) {
            case BASIC:
                return type1->u.basic == type2->u.basic;
            case ARRAY:
              return checkType(type1->u.array.elem, type2->u.array.elem);
            case STRUCTURE:
                while(field1!=NULL&&field2!=NULL){
                    res=res&checkType(field1->type,field2->type);
                    field1=field1->tail;
                    field2=field2->tail;
                }
                if(field1!=NULL || field2!=NULL) return FALSE;
                else  return res;
    }
}

// tableItem functions
pItem newItem(pFieldList pfield) {
    pItem p = (pItem)malloc(sizeof(TableItem));
    p->field = pfield;
    p->nextitem = NULL;
    return p;
}

// FieldList functions
pFieldList newFieldList(char* Name, pType newType) {
    pFieldList p = (pFieldList)malloc(sizeof(FieldList));
    p->name = Name;
    p->type = newType;
    p->tail = NULL;
    return p;
}


pFieldList copyFieldList(pFieldList src) {

    pFieldList head = NULL, cur = NULL;
    pFieldList temp = src;

    while (temp) {
        if (!head) {
            head = newFieldList(temp->name, copyType(temp->type));
            cur = head;
            temp = temp->tail;
        } else {
            cur->tail = newFieldList(temp->name, copyType(temp->type));
            cur = cur->tail;
            temp = temp->tail;
        }
    }
    return head;
}
char* generateRandomString(int length) {
    // 定义字符集
    char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    char *str = (char *)malloc((length + 1) * sizeof(char));
    // 初始化随机数生成器
    

    // 随机选取字符集中的字符
    for (int i = 0; i < length; i++) {
        int randomIndex = rand() % (sizeof(charset) - 1);
        str[i] = charset[randomIndex];
    }

    // 字符串最后一位设置为null，表示字符串结束
    str[length] = '\0';
    return str;
}

void setFieldListName(pFieldList p, char* newName) {
    if(newName!=NULL) {p->name = newName;}
    else{
        p->name=generateRandomString(256);
    }
}


// Hash function
unsigned hash_pjw(char* name) 
{ 
unsigned val = 0, i; 
for (; *name; ++name) 
{ val = (val << 2) + *name; 
if (i = val & ~0x3fff) val = (val ^ (i >> 12)) & 0x3fff; 
} 

return val; 
}

//table functions
void newtable() {
    if(init) return;
    init+=1;
    table = (ptable)malloc(sizeof(HashTable));
    table->hashArray = (pItem*)malloc(sizeof(pItem) * hash_array_size);
    for (int i = 0; i <hash_array_size; i++) {
        table->hashArray[i] = NULL;
    }

}

pItem searchTableItem(char* name) {
    
    unsigned hashCode = hash_pjw(name);

    //printf("Search:%s,%d\n",name,hashCode);
    

    pItem head = table->hashArray[hashCode];
    //printf("%d\n",head);
    

    if (head == NULL) return NULL;

    while (head ) {
        if (!strcmp(head->field->name, name)) return head;
        head = head->nextitem;
        
    }
    return NULL;
}

//TRUE -> has Conflict 因为不做3.1，所以所有变量的定义对于我们都是全局的，包括结构体里面的域
int checkItemConflict(ptable table, pItem item) {
    if(item==NULL) return FALSE;
    Kind type_kind= item->field->type->kind;
    pItem temp;
    int res;
    switch (type_kind) {
        case BASIC:
            temp = searchTableItem(item->field->name);
            if (temp != NULL) return 3;
            return 0;
            break;
        case ARRAY:
            temp = searchTableItem(item->field->name);
            if (temp != NULL) return 3;
            return 0;
            break;
        case STRUCTURE:
            temp = searchTableItem(item->field->name);
            if (temp != NULL) return 16;
            return 0;
            break;
        case FUNCTION:
            temp = searchTableItem(item->field->name);
            if (temp != NULL) return 4;
            return 0;
            break;
        case STRUCTURE_DEF:
            temp = searchTableItem(item->field->name);
            if (temp != NULL) return 16;
            return 0;
            break;
    }
}

// int checkSTRUCTURE_DEFConflict(ptable table,pItem item){
//     pItem temp = searchTableItem(item->field->name);
//     if (temp != NULL) return 16;
//     pFieldList p=item->field->type->u.structure;
//     while (p!=NULL){
//         pItem temp_p=searchTableItem(p->name);
//         if(temp_p!=NULL){
//             return 3;
//         }
//         p=p->tail;
//         }
//     //错误类型15
//     p=item->field->type->u.structure;
//     while(p!=NULL){
//         char* key_name=p->name;
//         pFieldList pp=p->tail;
//         while(pp!=NULL){
//             if(!strcmp(key_name,pp->name)){return 15;}
//             pp=pp->tail;
//         }
//         p=p->tail;
//     }
// return 0;
// }

// int checkFUNCTIONConflict(ptable table,pItem item){
//     pItem temp = searchTableItem(item->field->name);
//     if (temp != NULL) return 4;
//     pFieldList p=item->field->type->u.function.argv;
//     while(p!=NULL){
//         pFieldList pp=p->tail;
//         pItem temp_p=searchTableItem(p->name);
//         if(temp_p!=NULL){
//             return 3;
//         }
//         while(pp!=NULL){
//             if(!strcmp(p->name,pp->name)){return 3;}
//             pp=pp->tail;
//         }
//         p=p->tail;
//     }
// return 0;
// }



void addTableItem(pItem item) {
    if(item==NULL) return;
    if(item->field->name)
        {unsigned hashCode = hash_pjw(item->field->name);

        //printf("add:%s,%d\n",item->field->name,hashCode);
    
        if (table->hashArray[hashCode]==NULL){table->hashArray[hashCode]=item;
        // printf("%s\n",item->field->name);
        //printf("table:%d\n",table->hashArray[hashCode]);
        }
        else{
            item->nextitem=table->hashArray[hashCode];
            table->hashArray[hashCode]=item;
        }
        }
    pFieldList temp;
    switch (item->field->type->kind){
        case STRUCTURE:
            break;
        case STRUCTURE_DEF:
            // item=newItem(item->field->type->u.structure);
            // while(item!=NULL){
            //     addTableItem(item);
            //     if(item->field->tail!=NULL){item=newItem(item->field->tail);}
            //     else{break;}
            //     }
            break;
        case FUNCTION:
            // temp=item->field->type->u.function.argv;
            // while(temp!=NULL){
            //     addTableItem(temp);
            //     temp=temp->tail;
            // }
            break;
    }

}

void scanTree(pnode node){
 if (node == NULL) return;
 newtable();
    if (!strcmp(node->name, "ExtDef")) ExtDef(node);
    for(int i= 0; i < 9 ; i++){
        scanTree(node->children[i]);
    }
}


// ExtDef -> Specifier ExtDecList SEMI
//         | Specifier SEMI
//         | Specifier FunDec CompSt
void ExtDef(pnode node) {    
    pType specifierType = Specifier(node->children[0]);
    char* secondName = node->children[1]->name;

    // ExtDef -> Specifier ExtDecList SEMI
    if (!strcmp(secondName, "ExtDecList")) {
        ExtDecList(node->children[1], specifierType);
    }
    // ExtDef -> Specifier FunDec CompSt
    else if (!strcmp(secondName, "FunDec")) {
        FunDec(node->children[1], specifierType);
        CompSt(node->children[2], specifierType);
    }

    // Specifier SEMI
    // this situation has no meaning
    // or is struct define(have been processe in Specifier())
}


// ExtDecList -> VarDec
//             | VarDec COMMA ExtDecList
void ExtDecList(pnode node, pType specifier) {
    pnode temp = node;
    while (temp) {
        pItem item = VarDec(temp->children[0], specifier);
         int error= checkItemConflict(table, item);
        if (error) {
            char msg[100] = {0};
            sprintf(msg, "Redefined variable \"%s\".", item->field->name);
            pError(REDEF_VAR, temp->lineno, msg);
        } else {
            addTableItem(item);
        }
        if (temp->children[2]) {
            temp = temp->children[2];
        } else {
            break;
        }
    }
}

// Specifier -> TYPE
//            | StructSpecifier
pType Specifier(pnode node) {
    
    pnode t = node->children[0];
    // Specifier -> TYPE
    if (!strcmp(t->name, "TYPE")) {
        if (!strcmp(t->value, "float")) {
            return newType(BASIC, FLOAT_TYPE);
        } else {
            return newType(BASIC, INT_TYPE);
        }
    }
    // Specifier -> StructSpecifier
    else {
        return StructSpecifier(t);
    }
}

// StructSpecifier -> STRUCT OptTag LC DefList RC
//                  | STRUCT Tag
pType StructSpecifier(pnode node) {
    // OptTag -> ID | e
    // Tag -> ID

    pType returnType = NULL;
    pnode t = node->children[1];
    // StructSpecifier->STRUCT OptTag LC DefList RC
    if (t==NULL || !strcmp(t->name, "OptTag")) {

        pItem structItem = newItem(newFieldList("", newType(STRUCTURE_DEF, NULL)));
        if(t!=NULL) {setFieldListName(structItem->field, t->children[0]->value);}
        else {setFieldListName(structItem->field, NULL);}

        t = node->children[3];
        DefList(t, structItem);
        int error=checkItemConflict(table, structItem);
        if (error) {
            char msg[100] = {0};
            sprintf(msg, "Duplicated name \"%s\".", structItem->field->name);
            pError(error, node->lineno, msg);
        } else {
            returnType = newType(STRUCTURE_DEF, structItem->field->type->u.structure);
            addTableItem(structItem);
        }

    }

    // StructSpecifier->STRUCT Tag
    else {
        pItem structItem = searchTableItem(t->children[0]->value);
        if (structItem == NULL ||  structItem->field->type->kind != STRUCTURE_DEF) {
            char msg[100] = {0};
            sprintf(msg, "Undefined structure \"%s\".", t->children[0]->value);
            pError(UNDEF_STRUCT, node->lineno, msg);
        } else
            returnType = newType(STRUCTURE,structItem->field->type->u.structure);
    }
    return returnType;
}


// VarDec -> ID
//         | VarDec LB INT RB
pItem VarDec(pnode node, pType specifier) {
    if (specifier==NULL) return NULL;
    pnode id = node;
    // get ID
    while (id->children[0]) id = id->children[0];
    pItem p = newItem(newFieldList(id->value, NULL));

    pType temp=copyType(specifier);
    if(temp->kind==STRUCTURE_DEF) temp->kind=STRUCTURE;


    // VarDec -> ID
    if (!strcmp(node->children[0]->name, "ID")) {
        p->field->type= temp;
    }
    // VarDec -> VarDec LB INT RB
    else {
        
        while (node->children[1]) {

            p->field->type =newType(ARRAY, temp, atoi(node->children[2]->value));
            temp = p->field->type;
            node = node->children[0];
        }
    }
    return p;
}


// FunDec -> ID LP VarList RP
//         | ID LP RP
void FunDec(pnode node, pType returnType) {    
    pItem p = newItem(newFieldList(node->children[0]->value,newType(FUNCTION, 0, NULL, returnType)));

    // FunDec -> ID LP VarList RP
    if (!strcmp(node->children[2]->name, "VarList")) {
        VarList(node->children[2], p);
    }

    // FunDec -> ID LP RP don't need process

    // check redefine
    int res=checkItemConflict(table, p);
    if (res) {
        char msg[100] = {0};
        sprintf(msg, "Redefined function \"%s\".", p->field->name);
        pError(res, node->lineno, msg);
    } else {
        addTableItem(p);
    }
}

// VarList -> ParamDec COMMA VarList
//          | ParamDec
void VarList(pnode node, pItem func) {
    int argc = 0;
    pnode temp = node->children[0];
    pFieldList cur = NULL;

    // VarList -> ParamDec
    pFieldList paramDec = ParamDec(temp);
    if(paramDec)
    {func->field->type->u.function.argv = paramDec;
    cur = func->field->type->u.function.argv;
    argc++;
    }

    // VarList -> ParamDec COMMA VarList
    while (node->children[2]) {
        node=node->children[2];
        temp = node->children[0];
        paramDec = ParamDec(temp);
        if (paramDec) {
            if(cur)
                {cur->tail = paramDec;
                cur = cur->tail;
                argc++;}
            else
                {
                    func->field->type->u.function.argv = paramDec;
                    cur = func->field->type->u.function.argv;
                    argc++;
                }
        }
    }

    func->field->type->u.function.argc = argc;

}

//ParamDec → Specifier VarDec 
pFieldList ParamDec(pnode node) {
    pType specifierType = Specifier(node->children[0]);
    pItem p = VarDec(node->children[1], specifierType);
    int res=checkItemConflict(table, p);
    if (res) {
        char msg[100] = {0};
        sprintf(msg, "Redefined variable \"%s\".", p->field->name);
        pError(res, node->lineno, msg);
    } else {
        addTableItem(p);
        if(p) {return p->field;}
        return NULL;
    }
}

// CompSt -> LC DefList StmtList RC
void CompSt(pnode node, pType returnType) {

    pnode temp = node->children[1];
    DefList(temp, NULL);

    temp = node->children[2];
    StmtList(temp, returnType);
    

}


// StmtList -> Stmt StmtList
//           | e
void StmtList(pnode node, pType returnType) {

    while (node) {
        Stmt(node->children[0], returnType);
        node = node->children[1];
    }
}

// Stmt -> Exp SEMI
//       | CompSt
//       | RETURN Exp SEMI
//       | IF LP Exp RP Stmt
//       | IF LP Exp RP Stmt ELSE Stmt
//       | WHILE LP Exp RP Stmt
void Stmt(pnode node, pType returnType) {

    pType expType = NULL;
    // Stmt -> Exp SEMI
    if (!strcmp(node->children[0]->name, "Exp")) expType = Exp(node->children[0]);

    // Stmt -> CompSt
    else if (!strcmp(node->children[0]->name, "CompSt"))
        CompSt(node->children[0], returnType);

    // Stmt -> RETURN Exp SEMI
    else if (!strcmp(node->children[0]->name, "RETURN")) {
        expType = Exp(node->children[1]);

        // check return type
        if (!checkType(returnType, expType))
            pError(TYPE_MISMATCH_RETURN, node->lineno,"Type mismatched for return.");
    }

    // Stmt -> IF LP Exp RP Stmt
    else if (!strcmp(node->children[0]->name, "IF")) {
        pnode stmt = node->children[4];
        expType = Exp(node->children[2]);
        //printf("%d",expType->kind);
        if(expType && expType->kind!=INT_TYPE){
        char msg[100] = {0};
        sprintf(msg, "The Exp in IF must be INT_TYPE");
        pError(TYPE_MISMATCH_OP, node->lineno, msg);
        }
        Stmt(stmt, returnType);
        // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        if (node->children[6]!= NULL) Stmt(node->children[6], returnType);
    }

    // Stmt -> WHILE LP Exp RP Stmt
    else if (!strcmp(node->children[0]->name, "WHILE")) {
        expType = Exp(node->children[2]);
        if(expType && expType->kind!=INT_TYPE){
        char msg[100] = {0};
        sprintf(msg, "The Exp in WHILE must be INT_TYPE");
        pError(TYPE_MISMATCH_OP, node->lineno, msg);
        }
        Stmt(node->children[4], returnType);
    }

}

// DefList -> Def DefList
//          | e
void DefList(pnode node, pItem structInfo) {
    
    while (node) {
        Def(node->children[0], structInfo);
        node = node->children[1];
    }
}


// Def -> Specifier DecList SEMI
void Def(pnode node, pItem structInfo) {

    pType dectype = Specifier(node->children[0]);
    //你总会得到一个正确的type
    DecList(node->children[1], dectype, structInfo);
}


// DecList -> Dec
//          | Dec COMMA DecList
void DecList(pnode node, pType specifier, pItem structInfo) { 
    pnode temp = node;
    while (temp) {
        Dec(temp->children[0], specifier, structInfo);
        if (temp->children[2]!=NULL)
            temp = temp->children[2];
        else
            break;
    }
}


// Dec -> VarDec
//      | VarDec ASSIGNOP Exp
void Dec(pnode node, pType specifier, pItem structInfo) {
    // Dec -> VarDec
    if (node->children[1] == NULL) {

        if (structInfo != NULL) {
            // 结构体内，将VarDec返回的Item中的filedLis
            pItem decitem = VarDec(node->children[0], specifier);
            pFieldList temp=structInfo->field->type->u.structure;
            if(decitem){
                int error=0;
                while(temp){
                    if(!strcmp(temp->name,decitem->field->name)) 
                    {error=15;break;}
                    temp=temp->tail;
                }
                temp=structInfo->field->type->u.structure;
                if(error){
                    char msg[100] = {0};
                    sprintf(msg, "REDEF_FEILD \"%s\".",decitem->field->name);
                    pError(error, node->lineno, msg);}
                else{
                    int second_error=checkItemConflict(table, decitem);
                    if (second_error) {
                        //出现冲突，报错
                        char msg[100] = {0};
                        sprintf(msg, "Redefined variable \"%s\".",decitem->field->name);
                        pError(error, node->lineno, msg);
                                        } 
                    else{
                            if (temp==NULL){structInfo->field->type->u.structure = decitem->field;}
                            else{
                                while(temp->tail!=NULL){temp=temp->tail;}
                                temp->tail=decitem->field;
                                }
                            addTableItem(decitem);
                        }

                }

                
            }
         
        } else{
            // 判断返回的item有无冲突，无冲突放入表中，有冲突报错
            pItem decitem = VarDec(node->children[0], specifier);
            int error=checkItemConflict(table, decitem);
            if (error) {
                //出现冲突，报错
                char msg[100] = {0};
                sprintf(msg, "Redefined variable \"%s\".",decitem->field->name);
                pError(error, node->lineno, msg);
            } else {
                addTableItem(decitem);
            }
        }
        
    }
    // Dec -> VarDec ASSIGNOP Exp
    else {
        if (structInfo != NULL) {
            // 结构体内不能赋值，报错
            pError(REDEF_FEILD, node->lineno,
                   "Illegal initialize variable in struct.");
        } else {
            // 判断赋值类型是否相符
            //如果成功，注册该符号
            pItem decitem = VarDec(node->children[0], specifier);
            pType exptype = Exp(node->children[2]);
            int error=checkItemConflict(table, decitem);
            if (error) {
                //出现冲突，报错
                char msg[100] = {0};
                sprintf(msg, "Redefined variable \"%s\".",
                        decitem->field->name);
                pError(error, node->lineno, msg);
            }
            else if (!checkType(decitem->field->type, exptype)) {
                //类型不相符
                pError(TYPE_MISMATCH_ASSIGN, node->lineno,
                       "Type mismatchedfor assignment.");
            }
            else {
                addTableItem(decitem);
            }
        }
    }
}


// Exp -> Exp ASSIGNOP Exp
//      | Exp AND Exp
//      | Exp OR Exp
//      | Exp RELOP Exp
//      | Exp PLUS Exp
//      | Exp MINUS Exp
//      | Exp STAR Exp
//      | Exp DIV Exp

//      | LP Exp RP
//      | MINUS Exp
//      | NOT Exp
//      | ID LP Args RP
//      | ID LP RP

//      | Exp LB Exp RB
//      | Exp DOT ID

//      | ID
//      | INT
//      | FLOAT
pType Exp(pnode node) {

    pnode t = node->children[0];


    //二值运算
// Exp -> Exp ASSIGNOP Exp
//      | Exp AND Exp
//      | Exp OR Exp
//      | Exp RELOP Exp
//      | Exp PLUS Exp
//      | Exp MINUS Exp
//      | Exp STAR Exp
//      | Exp DIV Exp
//      | Exp LB Exp RB
//      | Exp DOT ID
    if (!strcmp(t->name, "Exp")) {
        // 基本数学运算符
        if (node->children[1] && strcmp(node->children[1]->name, "LB") && strcmp(node->children[1]->name, "DOT")) {
            pType p1 = Exp(t);
            pType p2 = Exp(node->children[2]);
            pType returnType = NULL;

            // Exp -> Exp ASSIGNOP Exp
            if (!strcmp(node->children[1]->name, "ASSIGNOP")) {
                //检查左值
                //strcmp(t->children[0]->name, "ID")!=0 && 
                          //(t->children[1]!=NULL && strcmp(t->children[1]->name, "LB")!=0 ) &&
                           //(t->children[1]!=NULL && strcmp(t->children[1]->name, "DOT")!=0 )
                if ((t->children[1]!=NULL && strcmp(t->children[1]->name, "LB")!=0 && strcmp(t->children[1]->name, "DOT")!=0) || 
                    (t->children[1]==NULL && strcmp(t->children[0]->name, "ID")!=0 )) {
                    //报错，左值
                    pError(LEFT_VAR_ASSIGN, t->lineno,"The left-hand side of an assignment must be avariable.");

                } 
                else {
                    if (!checkType(p1, p2)) {
                        //报错，类型不匹配
                        pError(TYPE_MISMATCH_ASSIGN, t->lineno,
                               "Type mismatched for assignment.");
                    } else
                        returnType = copyType(p1);
                }

            }
            // Exp -> Exp AND Exp
            //      | Exp OR Exp
            //      | Exp RELOP Exp
            //      | Exp PLUS Exp
            //      | Exp MINUS Exp
            //      | Exp STAR Exp
            //      | Exp DIV Exp
            else {
                if (p1 && p2 && (p1->kind != BASIC || p2->kind != BASIC)) {
                    //报错，数组，结构体运算
                    pError(TYPE_MISMATCH_OP, t->lineno,"Type mismatched for operands.");
                } else if (!checkType(p1, p2)) {
                    //报错，类型不匹配
                    pError(TYPE_MISMATCH_OP, t->lineno,"Type mismatched for operands.");
                } else if(p1 && p2 && p1->u.basic==FLOAT_TYPE && (!strcmp(node->children[1]->name, "AND") || !strcmp(node->children[1]->name, "OR") )){
                    pError(TYPE_MISMATCH_OP, t->lineno,"Type mismatched for operands.");
                }else {
                    if (p1 && p2) {
                        returnType = copyType(p1);
                    }
                }
            }

            return returnType;
        }
        // 数组和结构体访问
        else {
            // Exp -> Exp LB Exp RB
            if (!strcmp(node->children[1]->name, "LB")) {
                //数组
                pType p1 = Exp(t);
                pType p2 = Exp(node->children[2]);
                pType returnType = NULL;

                if (!p1) {
                    // 第一个exp为null，上层报错，这里不用再管
                } else if (p1 && p1->kind != ARRAY) {
                    //报错，非数组使用[]运算符
                    char msg[100] = {0};
                    sprintf(msg, "\"%s\" is not an array.", t->children[0]->value);
                    pError(NOT_A_ARRAY, t->lineno, msg);
                } else if (!p2 || p2->kind != BASIC ||
                           p2->u.basic != INT_TYPE) {
                    //报错，不用int索引[]
                    char msg[100] = {0};
                    sprintf(msg, "\"%s\" is not an integer.",node->children[2]->children[0]->value);
                    pError(NOT_A_INT, t->lineno, msg);
                } else {
                    returnType = copyType(p1->u.array.elem);
                }
                return returnType;
            }
            // Exp -> Exp DOT ID
            else {
                
                pType p1 = Exp(node->children[0]);

                pType returnType = NULL;
                if (!p1 || p1->kind != STRUCTURE ) {
                    //报错，对非结构体使用.运算符
                    pError(ILLEGAL_USE_DOT, t->lineno, "Illegal use of \".\".");
                } else {
                    pnode ref_id = node->children[2];
                    //printf("ref_id:%s\n",ref_id->value);
                    
                    pFieldList structfield = p1->u.structure;
                    while (structfield != NULL) {
                        //printf("strucfied_name:%s\n",structfield->name);
                        if (!strcmp(structfield->name, ref_id->value)) {
                            break;
                        }
                        structfield = structfield->tail;
                    }
                    if (structfield == NULL) {
                        //报错，没有可以匹配的域名
                        pError(NONEXISTFIELD, t->lineno, "no Existed field");
                        
                    } else {
                        returnType = copyType(structfield->type);
                    }
                }
                return returnType;
            }
        }
    }
    //单目运算符
    // Exp -> MINUS Exp
    //      | NOT Exp
    //      | LP Exp RP
    else if (!strcmp(t->name, "MINUS") || !strcmp(t->name, "NOT")) {
        pType p1 = Exp(node->children[1]);
        pType returnType = NULL;
        if (!p1 || p1->kind != BASIC || (p1->u.basic==FLOAT_TYPE && !strcmp(t->name, "NOT"))) {
            //报错，数组，结构体运算
            pError(TYPE_MISMATCH_OP, t->lineno, "TYPE_MISMATCH_OP");
        } else {
            returnType = copyType(p1);
        }
        return returnType;
    } else if (!strcmp(t->name, "LP")) {
        return Exp(node->children[1]);
    }

    // Exp -> ID LP Args RP
    //		| ID LP RP   
    else if (!strcmp(t->name, "ID") && node->children[1]) {
        pItem funcInfo = searchTableItem(t->value);

        // function not find
        if (funcInfo == NULL) {
            char msg[100] = {0};
            sprintf(msg, "Undefined function \"%s\".", t->value);
            pError(UNDEF_FUNC, node->lineno, msg);
            return NULL;
        } else if (funcInfo->field->type->kind != FUNCTION) {
            char msg[100] = {0};
            sprintf(msg, "\"%s\" is not a function.", t->value);
            pError(NOT_A_FUNC, node->lineno, msg);
            return NULL;
        }

        // Exp -> ID LP Args RP
        else if (!strcmp(node->children[2]->name, "Args")) {
            Args(node->children[2], funcInfo);
            return copyType(funcInfo->field->type->u.function.returnType);
        }
        // Exp -> ID LP RP
        else {
            if (funcInfo->field->type->u.function.argc != 0) {
                char msg[100] = {0};
                sprintf(msg,
                        "too few arguments to function \"%s\", except %d args.",
                        funcInfo->field->name,
                        funcInfo->field->type->u.function.argc);
                pError(FUNC_AGRC_MISMATCH, node->lineno, msg);
            }
            return copyType(funcInfo->field->type->u.function.returnType);
        }
    }
    // Exp -> ID
    else if (!strcmp(t->name, "ID")) {
        pItem tp = searchTableItem(t->value);
        if (tp == NULL ) {
            char msg[100] = {0};
            sprintf(msg, "Undefined variable \"%s\".", t->value);
            pError(UNDEF_VAR, t->lineno, msg);
            return NULL;
        } else {
            // good
            return tp->field->type;
        }
    } else {
        // Exp -> FLOAT
        if (!strcmp(t->name, "FLOAT")) {
            return newType(BASIC, FLOAT_TYPE);
        }
        // Exp -> INT
        else {
            return newType(BASIC, INT_TYPE);
        }
    }
}


// Args -> Exp COMMA Args
//       | Exp
void Args(pnode node, pItem funcInfo) {
    pnode temp = node;
    pFieldList arg = funcInfo->field->type->u.function.argv;

    while (temp) {
        if (arg == NULL) {
            char msg[100] = {0};
            sprintf(
                msg, "too many arguments to function \"%s\", except %d args.",
                funcInfo->field->name, funcInfo->field->type->u.function.argc);
            pError(FUNC_AGRC_MISMATCH, node->lineno, msg);
            break;
        }
        pType realType = Exp(temp->children[0]);
        // printf("=======arg type=========\n");
        // printType(realType);
        // printf("===========end==========\n");
        if (!checkType(realType, arg->type)) {
            char msg[100] = {0};
            sprintf(msg, "Function \"%s\" is not applicable for arguments.",
                    funcInfo->field->name);
            pError(FUNC_AGRC_MISMATCH, node->lineno, msg);
            return;
        }

        arg = arg->tail;
        if (temp->children[2]) {
            temp = temp->children[2];
        } else {
            break;
        }
    }
    if (arg != NULL) {
        char msg[100] = {0};
        sprintf(msg, "too few arguments to function \"%s\", except %d args.",
                funcInfo->field->name, funcInfo->field->type->u.function.argc);
        pError(FUNC_AGRC_MISMATCH, node->lineno, msg);
    }
}
