#include "ciLisp.h"

void yyerror(char *s) {
    fprintf(stderr, "\nERROR: %s\n", s);
    // note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
}

//
// find out which function it is
//

char *func[] = {"neg", "abs", "exp", "sqrt", "add", "sub", "mult", "div", "remainder", "log", "pow", "max", "min",
                "exp2", "cbrt", "hypot", "print", "read", "rand", "equal", "smaller", "larger", ""};

OPER_TYPE resolveFunc(char *funcName) {
    int i = 0;
    while (func[i][0] != '\0') {
        if (strcmp(func[i], funcName) == 0)
            return i;

        i++;
    }
    return CUSTOM_FUNC;
}

//
// create a node for a number
//
AST_NODE *number(double value, DATA_TYPE theType) {
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed size and the variable part (union)
    nodeSize = sizeof(AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = NUM_TYPE;
    p->data.number.value = value;

    if(theType == INTEGER_TYPE){
        p->data.number.type = INTEGER_TYPE;
    }else if(theType == REAL_TYPE){
        p->data.number.type = REAL_TYPE;
    }else{
        p->data.number.type = NO_TYPE;
    };

    return p;
}

//
// create a node for a function
//
AST_NODE *function(char *funcName, AST_NODE *oplist) {
    AST_NODE *p;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = FUNC_TYPE;
//    p->data.function.name = (char*)malloc(strlen(funcName)+1);
//    strcpy(p->data.function.name,funcName);

    p->data.function.name = funcName;



    if(oplist != NULL){
        p->data.function.opList = oplist;
        p->data.function.opList->parent = p;
    }

    return p;
}

//
// free a node
//
void freeNode(AST_NODE *p) {
    if (!p)
        return;

    if (p->type == FUNC_TYPE) {
        free(p->data.function.name);
        if(p->data.function.opList != NULL){
            AST_NODE* temp;
            temp = p->data.function.opList;
            if (temp->next != NULL){
                freeNode(temp->next);
            }
        }
    }

    free(p);
}

//
// evaluate an abstract syntax tree
//
// p points to the root
//
RETURN_VALUE eval(AST_NODE *p) {
    RETURN_VALUE temp;
    AST_NODE* gioerhgv;
    SYMBOL_TABLE_NODE* auohfou;

    switch(p->type){
        case NUM_TYPE:
            switch (p->data.number.type){
                case INTEGER_TYPE:
                    gioerhgv = number(p->data.number.value,INTEGER_TYPE);
                    temp.value = gioerhgv->data.number.value;
                    temp.type = INTEGER_TYPE;
                    break;
                case REAL_TYPE:
                    gioerhgv = number(p->data.number.value,REAL_TYPE);
                    temp.value = gioerhgv->data.number.value;
                    temp.type = REAL_TYPE;
                    break;
                case NO_TYPE:
                    gioerhgv = number(p->data.number.value,NO_TYPE);
                    temp.value = gioerhgv->data.number.value;
                    temp.type = NO_TYPE;
                    break;
            }
            break;
        case FUNC_TYPE:
            //I was too tired to come up with a decent name
            return evalFunction(p);


        case SYMB_TYPE:
            auohfou = resolveSymbol(p->data.symbol.name,p);
            temp = eval(auohfou->val);
            temp.type = auohfou->val_type;
            if(temp.type == INTEGER_TYPE){
                temp.value = floor(temp.value);
            }
            break;

        default:
            temp.type = NO_TYPE;
            temp.value = 0.0;
            break;
    }
    return temp;
}

AST_NODE *setSymbolTable(SYMBOL_TABLE_NODE *symbolTable, AST_NODE *s_expr) {

    if (s_expr == NULL) {
        return NULL;
    }
    s_expr->symbolTable = symbolTable;
    SYMBOL_TABLE_NODE *temp;
    temp = s_expr->symbolTable;
    while (temp != NULL) {
        temp->val->parent = s_expr;
        temp = temp->next;
    }
    return s_expr;

}

AST_NODE *symbol(char *name) {

    AST_NODE *temp;
    temp = (AST_NODE *) malloc(sizeof(AST_NODE));

    if (temp == NULL) {
        puts("Not enough space.");
        exit(1);
    }

    temp->type = SYMB_TYPE;
    temp->symbolTable = NULL;
    temp->parent = NULL;

//    temp->data.symbol.name = (char *) malloc(strlen(name) + 1);
//    if (temp->data.symbol.name == NULL) {
//        puts("no space 8");
//        exit(1);
//    }
//
//    strcpy(temp->data.symbol.name, name);
    temp->data.symbol.name = name;

    return temp;

}

SYMBOL_TABLE_NODE *createSymbol(DATA_TYPE type,char *name, AST_NODE *value) {//defining a symbol

    if (value == NULL) {
        puts("you know, the value is NULL, and I'm pretty sure this isn't supposed to happen.");
        return NULL;
    }

    SYMBOL_TABLE_NODE *temp;
    temp = (SYMBOL_TABLE_NODE *) malloc(sizeof(SYMBOL_TABLE_NODE));

    if (temp == NULL) {
        puts("Error: not enough space");
        exit(1);
    }
//    temp->ident = (char *) malloc(strlen(name) + 1);
//    if (temp->ident == NULL) {
//        puts("no space");
//        exit(1);
//    }
//
//    strcpy(temp->ident, name);

    temp->ident = name;
    temp->val = value;
    temp->next = NULL;
    temp->val_type = type;


    return temp;

}

SYMBOL_TABLE_NODE *findSymbol(SYMBOL_TABLE_NODE *symbolTable, SYMBOL_TABLE_NODE *symbol) {

    if (symbol == NULL) {
        return NULL;
    }

    while (symbolTable!=NULL) {
        if(strcmp(symbolTable->ident,symbol->ident)==0){
            return symbolTable;
        }
        symbolTable = symbolTable->next;
    }

    return NULL;
}

SYMBOL_TABLE_NODE *addSymbolToList(SYMBOL_TABLE_NODE *symbolTable, SYMBOL_TABLE_NODE *newSymbol) {

    if (newSymbol == NULL) {
        return symbolTable;
    }
    if (findSymbol(symbolTable, newSymbol) == NULL) {
        newSymbol->next = symbolTable;
        return newSymbol;
    }

    SYMBOL_TABLE_NODE *temp = findSymbol(symbolTable, newSymbol);
    temp->val = newSymbol->val;
    free(newSymbol);
    return symbolTable;

}


SYMBOL_TABLE_NODE *resolveSymbol(char *name, AST_NODE *node) {

    AST_NODE *parent = node;
    SYMBOL_TABLE_NODE *symbol;
    while (parent != NULL) {
        symbol = parent->symbolTable;
        while (symbol != NULL) {
            if (strcmp(symbol->ident, name) == 0) {
                return symbol;
            }
            symbol = symbol->next;
        }
        parent = parent->parent;
    }
    return NULL;


}

RETURN_VALUE evalFunction(AST_NODE *p){

    RETURN_VALUE temp;
    RETURN_VALUE temp2;
    RETURN_VALUE temp3;
    struct ast_node* tracker;


//    if(p->data.function.op1->data.number.type == INTEGER_TYPE){
//        if(p->data.function.op2->data.number.type != INTEGER_TYPE){
//            printf("WARNING: precision loss in the assignment for variable %s\n",p->data.function.op2->data.symbol.name);
//
//            p->data.function.op2->data.number.value = round(p->data.function.op2->data.number.value);
//            p->data.function.op2->data.number.type = INTEGER_TYPE;
//        }
//    }else{
//        if(p->data.function.op2->data.number.type == INTEGER_TYPE){
//            printf("WARNING: precision loss in the assignment for variable %s\n",p->data.function.op1->data.symbol.name);
//            p->data.function.op1->data.number.value = round(p->data.function.op1->data.number.value);
//            p->data.function.op1->data.number.type = INTEGER_TYPE;
//        }
//    }
    switch (resolveFunc(p->data.function.name)) {
        case NEG_OPER:
            if(countOperators(p->data.function.opList)<1){
                errorMessages(3,"NEG_OPER");

                temp.value = 0.0;
                temp.type = REAL_TYPE;

                return temp;
            }else if(countOperators(p->data.function.opList)>1){
                errorMessages(2,"NEG_OPER");
            }

            temp = eval(p->data.function.opList);
            temp.value = (temp.value)*(-1);

            return temp;

        case ABS_OPER:
            if(countOperators(p->data.function.opList)<1){
                errorMessages(3,"ABS_OPER");

                temp.value = 0.0;
                temp.type = REAL_TYPE;

                return temp;
            }else if(countOperators(p->data.function.opList)>1){
                errorMessages(2,"ABS_OPER");
            }

            temp = eval(p->data.function.opList);
            temp.value = fabs(temp.value);

            return temp;
        case EXP_OPER:
            if(countOperators(p->data.function.opList)<1){
                errorMessages(3,"EXPR_OPER");

                temp.value = 0.0;
                temp.type = REAL_TYPE;

                return temp;
            } else if(countOperators(p->data.function.opList)>1){
                errorMessages(2,"EXPR_OPER");
            }
            temp = eval(p->data.function.opList);
            temp.value = exp(temp.value);
            return temp;
        case SQRT_OPER:
            if(countOperators(p->data.function.opList)>1){
                errorMessages(2,"SQRT_OPER");
            }else if(countOperators(p->data.function.opList)<1){
                errorMessages(3,"SQRT_OPER");

                temp.value = 0.0;
                temp.type = REAL_TYPE;

                return temp;
            }
            temp = eval(p->data.function.opList);
            temp.value = sqrt(temp.value);

            return temp;
        case ADD_OPER:

            if(countOperators(p->data.function.opList)<0) {
                errorMessages(3, "ADD_OPER");

                temp.value = 0.0;
                temp.type = REAL_TYPE;

                return temp;
            }

            temp = eval(p->data.function.opList);
            tracker = p->data.function.opList->next;

            while(tracker!=NULL){
                temp2.value = eval(tracker->data.function.opList).value;
                temp.value += temp2.value;
                tracker = tracker->next;
            }

            return temp;

        case SUB_OPER:
            if(countOperators(p->data.function.opList)<2){
                errorMessages(3,"SUB_OPER");

                temp.value = 0.0;
                temp.type = REAL_TYPE;

                return temp;

            }else if(countOperators(p->data.function.opList)>2){
                errorMessages(2,"SUB_OPER");
            }

            temp = eval(p->data.function.opList);
            temp2 = eval(p->data.function.opList->next);

            temp3.value = temp.value-temp2.value;
            temp3.type = temp.type;

            return temp3;

        case MULT_OPER:
            if(countOperators(p->data.function.opList)<2){
                errorMessages(3,"MULT_OPER");

                temp.value = 0.0;
                temp.type = REAL_TYPE;
                return temp;
            }

            temp = eval(p->data.function.opList);
            tracker = p->data.function.opList->next;

            while(tracker!=NULL){
                temp2.value = eval(tracker->data.function.opList).value;
                temp.value = temp2.value*temp.value;
                tracker = tracker->next;
            }

            return temp;

        case DIV_OPER:
            if(countOperators(p->data.function.opList)<2){
                errorMessages(3,"DIV_OPER");

                temp.value = 0.0;
                temp.type = REAL_TYPE;
                return temp;

            }else if(countOperators(p->data.function.opList)>2){
                errorMessages(2,"DIV_OPER");
            }

            temp = eval(p->data.function.opList);
            temp2 = eval(p->data.function.opList->next);

            temp3.value = temp.value /temp2.value;
            temp3.type = temp.type;

            return temp3;

        case REMAINDER_OPER:
            if(countOperators(p->data.function.opList)<2){
                errorMessages(3,"REMAINDER_OPER");

                temp.type = REAL_TYPE;
                temp.value = 0.0;

                return temp;
            }else if(countOperators(p->data.function.opList)>2){
                errorMessages(2,"REMAINDER_OPER");
            }

            temp = eval(p->data.function.opList);
            temp2 = eval(p->data.function.opList->next);

            temp3.value = remainder(temp.value,temp2.value);
            temp3.type = temp.type;

            return temp3;

        case LOG_OPER:
            if(countOperators(p->data.function.opList)<1){
                errorMessages(3,"LOG_OPER");

                temp.value = 0.0;
                temp.type = REAL_TYPE;

                return temp;

            } else if(countOperators(p->data.function.opList)>1){
                errorMessages(2,"LOG_OPER");
            }

            temp = eval(p->data.function.opList);

            temp.value = log(temp.value);

            return temp;

        case POW_OPER:
            if(countOperators(p->data.function.opList)<2){
                errorMessages(3,"POW_OPER");

                temp.type = REAL_TYPE;
                temp.value = 0.0;
                return temp;

            } else if(countOperators(p->data.function.opList)>2){
                errorMessages(2,"POW_OPER");
            }

            temp = eval(p->data.function.opList);
            temp2 = eval(p->data.function.opList->next);

            temp3.value = pow(temp.value,temp2.value);
            temp3.type = temp.type;

            return temp3;

        case MAX_OPER:

            if(countOperators(p->data.function.opList)>2){
                errorMessages(2,"MAX_OPER");
            } else if(countOperators(p->data.function.opList)<2){
                errorMessages(3,"MAX_OPER");

                temp.value = 0.0;
                temp.type = REAL_TYPE;
                return temp;
            }

            temp = eval(p->data.function.opList);
            temp2 = eval(p->data.function.opList->next);

            if (temp.value > temp2.value) {
                return temp;
            } else {
                return temp2;
            }

        case MIN_OPER:

            if(countOperators(p->data.function.opList)<2){
                errorMessages(3,"MIN_OPER");

                temp.type = REAL_TYPE;
                temp.value = 0.0;
                return temp;

            }else if(countOperators(p->data.function.opList)>2){

                errorMessages(2,"MIN_OPER");

            }

            temp = eval(p->data.function.opList);
            temp2 = eval(p->data.function.opList->next);

            if (temp.value < temp2.value) {
                return temp;
            } else {
                return temp2;
            }

        case EXP2_OPER:

            if(countOperators(p->data.function.opList)<1){

                errorMessages(3,"EXPR2_OPER");

                temp.value = 0.0;
                temp.type = REAL_TYPE;

                return temp;

            } else if(countOperators(p->data.function.opList)>1){

                errorMessages(2,"EXPR2_OPER");
            }

            temp = eval(p->data.function.opList);

            temp.value = exp2(temp.value);

            return temp;

        case CBRT_OPER:

            if(countOperators(p->data.function.opList)>1){
                errorMessages(2,"CBRT_OPER");
            } else if(countOperators(p->data.function.opList)<1){
                errorMessages(3,"CBRT_OPER");
                temp.type = REAL_TYPE;
                temp.value = 0.0;
                return temp;
            }

            temp = eval(p->data.function.opList);
            temp.value = cbrt(temp.value);

            return temp;

        case HYPOT_OPER:
            if(countOperators(p->data.function.opList)<2){
                errorMessages(3,"HYPOT_OPER");

                temp.value = 0.0;
                temp.type = REAL_TYPE;

                return temp;

            } else if(countOperators(p->data.function.opList)>2){
                errorMessages(2,"HYPOT_OPER");
            }
            temp = eval(p->data.function.opList);
            temp2 = eval(p->data.function.opList->next);

            temp3.value = hypot(temp.value,temp2.value);
            temp3.type = temp.type;
            return temp3;

        case PRINT:
            if(countOperators(p->data.function.opList)<= 0){
                errorMessages(3,"PRINT_OPER");

                temp.type = REAL_TYPE;
                temp.value = 0.0;
                return temp;
            }

            temp = eval(p->data.function.opList);
            tracker = p->data.function.opList->next;

            while(tracker!=NULL){
                if(temp.type == INTEGER_TYPE){
                    printf("%d\n",(int)temp.value);
                } else{
                    printf("%.2lf\n",temp.value);
                }
                tracker = tracker->next;
                temp.value = eval(tracker->data.function.opList).value;
            }

            return temp;

        default:
            temp.value = 0.0;
            temp.type = NO_TYPE;
            return temp;
    }
}


AST_NODE* conformToList(AST_NODE* sExpression, AST_NODE* list){

    AST_NODE* temp;
    temp = list;
    while (temp->next != NULL){
        temp = temp->next;
    }
    temp->next = sExpression;
    return list;
}

int countOperators(AST_NODE* list){
    int i = 0;

    AST_NODE* temp;
    temp = list;

    while (temp->next != NULL){
        temp = temp->next;
        i++;
    }
    i++;
    return i;
}

void errorMessages(int errorCode, char* funcName){

    //2-->too big
    //3-->too small

    if(errorCode == 2){
        printf("Warning: too many parameters for function <%s>\n",funcName);
    } else{
        printf("ERROR: too few parameters for function <%s>\n",funcName);
    }
}
