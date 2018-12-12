#include "ciLisp.h"

void yyerror(char *s) {
    printf("\nERROR: %s\n", s);
    // note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
}

//
// find out which function it is
//
int resolveFunc(char *func) {
    char *funcs[] = {"neg", "abs", "exp", "sqrt", "add", "sub", "mult", "div", "remainder", "log", "pow", "max", "min",
                     "exp2", "cbrt", "hypot"};
    int i = 0;
    while (funcs[i][0] != '\0') {
        if (strcmp(funcs[i], func) == 0)
            return i;

        i++;
    }
    yyerror("invalid function");
    return INVALID_OPER;
}

//
// create a node for a number
//
AST_NODE *number(double value) {
    AST_NODE *p;
    size_t nodeSize;

    printf("ran number function\n");

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
AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2) {
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
void freeNode(AST_NODE *p) {
    if (!p)
        return;

    if (p->type == FUNC_TYPE) {
        free(p->data.function.name);
        freeNode(p->data.function.op1);
        freeNode(p->data.function.op2);
    }

    free(p);
}

double calc(char *func, double op1, double op2) {
    switch (resolveFunc(func)) {
        case (0):
            return op1 * (-1);
        case (1):
            return fabs(op1);
        case (2):
            return exp(op1);
        case (3):
            return sqrt(op1);
        case (4):
            return op1 + op2;
        case (5):
            return op1 - op2;
        case (6):
            return op1 * op2;
        case (7):
            return op1 / op2;
        case (8):
            return (int) op1 % (int) op2;
        case (9):
            return log(op1);
        case (10):
            return pow(op1, op2);
        case (11):
            if (op1 > op2)
                return op1;
            else
                return op2;
        case (12):
            if (op1 < op2)
                return op1;
            else
                return op2;
        case (13):
            return exp2(op1);
        case (14):
            return cbrt(op1);
        case (15):
            return hypot(op1, op2);
        default:
            break;
    }
}

//
// evaluate an abstract syntax tree
//
// p points to the root
//
double eval(AST_NODE *p) {
    if (!p)
        return 0.0;

    if (p->type == NUM_TYPE) {
        return p->data.number.value;
    }
    if (p->type == SYMBOL_TYPE) {
        printf("eval symbol\n");
        symbol(p->data.symbol.name, p->data.number.value);
    }
    if (p->type == LET_LIST_TYPE) {
        printf("eval let list\n");
        let_list(&p->data.list, &p->data.element);
    }
    if (p->type == LET_ELEM_TYPE) {
        printf("eval let element\n");
        let_element(&p->data.symbol, p->data.element.sExpr);
    } else {
        if (p->data.function.op2 != NULL) {
            return calc(p->data.function.name, eval(p->data.function.op1), eval(p->data.function.op2));
        } else {
            return calc(p->data.function.name, eval(p->data.function.op1), 1.0);
        }
    }
}


LET_LIST_AST_NODE *let_list(LET_LIST_AST_NODE *theList, LET_ELEMENT_AST_NODE *theElement) {

    AST_NODE *current;
    size_t theSize;
    theSize = sizeof(LET_LIST_AST_NODE) + sizeof(LET_LIST_AST_NODE);

    current = malloc(theSize);
    if (current == NULL) {
        yyerror("an error has occured.\n");
    }
    current->type = LET_LIST_TYPE;
    current->data.list.let_elem = theElement;
    current->data.list.let_list = theList;

    printf("run let_list\n");
    return theList;
}

LET_ELEMENT_AST_NODE *let_element(SYMBOL_AST_NODE *input, AST_NODE *sExpr) {
    AST_NODE *temp;
    char *theName = input->name;
    double value = input->value;
    printf("ran let_element\n");
    size_t spaceNeeded;
    spaceNeeded = sizeof(AST_NODE) + sizeof(LET_ELEMENT_AST_NODE);
    temp = malloc(spaceNeeded);
    if (temp == NULL) {
        yyerror("Unable to allocate space.\n");
    }
    temp->type = LET_ELEM_TYPE;
    temp->data.element.symbol = symbol(theName, value);
    temp->data.element.sExpr = sExpr;
    return temp;
}

AST_NODE *scope(AST_NODE *letList, AST_NODE *sExpres) {
    AST_NODE *temp;
    size_t theSize;
    theSize = sizeof(AST_NODE) + sizeof(SCOPE_NODE);
    temp = malloc(theSize);
    if (temp == NULL) {
        yyerror("An error has occured\n");
    }
    printf("ran scope\n");

    temp->type = SCOPE_TYPE;
    temp->data.theScope.theList = letList;
    temp->data.theScope.sExpr = sExpres;

    return temp;
}

AST_NODE *symbol(char *theName, double theValue) {
    AST_NODE *currentSymbol;

    printf("ran symbol \n");

    if ((currentSymbol = malloc(sizeof(AST_NODE) + sizeof(SYMBOL_AST_NODE))) == NULL) {
        yyerror("An error has occured...:(\n");
    }

    currentSymbol->type = SYMBOL_TYPE;
    currentSymbol->data.symbol.name = theName;
    currentSymbol->data.symbol.value = theValue;

    return currentSymbol;
}
