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
                "exp2", "cbrt", "hypot", "read", "rand", "print", "equal", "smaller", "larger", ""};

OPER_TYPE resolveFunc(char *funcName)
{
    int i = 0;
    while (func[i][0] != '\0')
    {
        if (strcmp(func[i], funcName) == 0)
            return i;

        i++;
    }
    return CUSTOM_FUNC;
}

//
// create a node for a number
//
AST_NODE *number(double value)
{
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(NUMBER_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = NUM_TYPE;
    p->data.number.value = value;

    return p;
}

//
// create a node for a function
//
AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2)
{
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(FUNCTION_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = FUNC_TYPE;
    p->data.function.name = funcName;
    p->data.function.op1 = op1;
    p->data.function.op2 = op2;

    return p;
}

//
// free a node
//
void freeNode(AST_NODE *p)
{
    if (!p)
        return;

    if (p->type == FUNC_TYPE)
    {
        free(p->data.function.name);
        freeNode(p->data.function.op1);
        freeNode(p->data.function.op2);
    }

    free(p);
}

//
// evaluate an abstract syntax tree
//
// p points to the root
//
double eval(AST_NODE *p)
{
    if (!p)
        return 0.0;

// TBD: implement

    switch(p->type){
        case NUM_TYPE:
            return p->data.number.value;
        case FUNC_TYPE:
            switch (resolveFunc(p->data.function.name)){
                case NEG_OPER:
                    return ((-1)*eval(p->data.function.op1));
                case ABS_OPER:
                    return fabs(eval(p->data.function.op1));
                case EXP_OPER:
                    return exp(eval(p->data.function.op1));
                case SQRT_OPER:
                    return sqrt(eval(p->data.function.op1));
                case ADD_OPER:
                    return ((eval(p->data.function.op1))+(eval(p->data.function.op2)));
                case SUB_OPER:
                    return ((eval(p->data.function.op1))-(eval(p->data.function.op2)));
                case MULT_OPER:
                    return ((eval(p->data.function.op1))*(eval(p->data.function.op2)));
                case DIV_OPER:
                    return ((eval(p->data.function.op1))/(eval(p->data.function.op2)));
                case REMAINDER_OPER:
                    return remainder((eval(p->data.function.op1)),(eval(p->data.function.op2)));
                case LOG_OPER:
                    return(log(eval(p->data.function.op1)));
                case POW_OPER:
                    return (pow((eval(p->data.function.op1)),(eval(p->data.function.op2))));
                case MAX_OPER:
                    if((eval(p->data.function.op1))>(eval(p->data.function.op2))){
                        return eval(p->data.function.op1);
                    }
                    else{
                        return eval(p->data.function.op2);
                    }
                case MIN_OPER:
                    if((eval(p->data.function.op1))<(eval(p->data.function.op2))){
                        return eval(p->data.function.op1);
                    }
                    else{
                        return eval(p->data.function.op2);
                    }
                case EXP2_OPER:
                    return exp2((eval(p->data.function.op1)));
                case CBRT_OPER:
                    return (cbrt(eval(p->data.function.op1)));
                case HYPOT_OPER:
                    return(hypot((eval(p->data.function.op1)),(eval(p->data.function.op2))));
            }
                case SYMB_TYPE://resolve function
                /*
                 * call resolve symbol
                 * */
                resolveSymbol(p->data.symbol.name,p);
                break;
            ;
    }
    return 0.0;
}

AST_NODE *setSymbolTable(SYMBOL_TABLE_NODE *symbolTable, AST_NODE *s_expr){

    if(s_expr == NULL){
        return NULL;
    }
    s_expr->symbolTable = symbolTable;
    SYMBOL_TABLE_NODE* temp;
    temp = s_expr->symbolTable;
    while(temp->next!= NULL){
        temp->val->parent = s_expr;
    }
    return s_expr;

}

AST_NODE *symbol (char* name){

    AST_NODE* temp;
    temp = (AST_NODE*)malloc(sizeof(AST_NODE));

    if(temp == NULL){
        puts("Not enough space.");
        exit(1);
    }
    temp->symbolTable = (SYMBOL_TABLE_NODE*)malloc(sizeof(SYMBOL_TABLE_NODE));
    if(temp->symbolTable == NULL){
        puts("An error has occured");
        exit(1);
    }

    temp->type = SYMB_TYPE;
    temp->symbolTable = NULL;
    temp->parent = NULL;

    temp->data.symbol.name = (char*)malloc(sizeof(name));
    if(temp->data.symbol.name == NULL){
        puts("no space");
        exit(1);
    }

    stpcpy(temp->data.symbol.name,name);

    return temp;

}

SYMBOL_TABLE_NODE *createSymbol(char *name, AST_NODE *value){//defining a symbol

    if(value == NULL){
        puts("you know, the value is NULL, and I'm pretty sure this isn't supposed to happen.");
        return NULL;
    }

    SYMBOL_TABLE_NODE* temp;
    temp = (SYMBOL_TABLE_NODE*)malloc(sizeof(SYMBOL_TABLE_NODE));

    if(temp == NULL){
        puts("Error: not enough space");
        exit(1);
    }
    temp->ident = (char*)malloc(sizeof(name));
    if(temp->ident == NULL){
        puts("no space");
        exit(1);
    }

    strcpy(temp->ident,name);
    temp->val = value;
    temp->next = NULL;

    return temp;

}

SYMBOL_TABLE_NODE* findSymbol(SYMBOL_TABLE_NODE *symbolTable, SYMBOL_TABLE_NODE *symbol){

    if(symbol == NULL){
        return NULL;
    }

    SYMBOL_TABLE_NODE* temp;
    temp = symbolTable;

    while(temp->val->data.symbol.name != symbol->val->data.symbol.name && temp->next != NULL){
        temp = temp->next;
    }

    if(temp->val->data.symbol.name == symbol->val->data.symbol.name){
        return temp;
    }

    return NULL;
}

SYMBOL_TABLE_NODE *addSymbolToList(SYMBOL_TABLE_NODE *symbolTable, SYMBOL_TABLE_NODE *newSymbol){

    if(newSymbol == NULL){
        return symbolTable;
    }
    if(findSymbol(symbolTable,newSymbol)==NULL){
        newSymbol->next = symbolTable;
        return newSymbol;
    }

    SYMBOL_TABLE_NODE* temp = findSymbol(symbolTable,newSymbol);
    temp->val = newSymbol->val;
    temp->val->data = temp->val->data;
    free(newSymbol);
    return symbolTable;

}


SYMBOL_TABLE_NODE* resolveSymbol(char* name,AST_NODE* node){

    AST_NODE* parent = node;
    SYMBOL_TABLE_NODE* symbol;
    while(parent != NULL){
        symbol = parent->symbolTable;
        while(symbol != NULL){
            if(symbol->ident == name){
                return symbol;
            }
        }
        parent = parent->parent;
    }
    return NULL;


}