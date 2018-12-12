#ifndef __cilisp_h_
#define __cilisp_h_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "ciLispParser.h"

int yyparse(void);

int yylex(void);

void yyerror(char *);

typedef enum oper { // must be in sync with funcs in resolveFunc()
    NEG,
    ABS,
    EXP,
    SQRT,
    ADD,
    SUB,
    MULT,
    DIV,
    REMAINDER,
    LOG,
    POW,
    MAX,
    MIN,
    EXP2,
    CBRT,
    HYPOT,
    INVALID_OPER=255
} OPER_TYPE;

typedef enum {
    NUM_TYPE, FUNC_TYPE,SYMBOL_TYPE,LET_LIST_TYPE,LET_ELEM_TYPE,SCOPE_TYPE
} AST_NODE_TYPE;

typedef struct {
    double value;
} NUMBER_AST_NODE;

typedef struct{
    char* name;
    double value;
}SYMBOL_AST_NODE;

typedef struct {
    char *name;
    struct ast_node *op1;
    struct ast_node *op2;
} FUNCTION_AST_NODE;

typedef struct{
    struct ast_node *let_elem;
    struct ast_node *let_list;
}LET_LIST_AST_NODE;

typedef struct{
    struct ast_node *symbol;
    struct ast_node *sExpr;
}LET_ELEMENT_AST_NODE;

typedef struct symbol_table_node{
    char* ident;
    struct ast_node *val;
    struct symbol_table_node *next;
    struct symbol_table_node *prev;
}SYMBOL_TABLE_NODE;

typedef struct symbol_table{
    SYMBOL_TABLE_NODE* head;
    SYMBOL_TABLE_NODE* tail;
}SYMBOL_TABLE;

typedef struct scope{
    struct ast_node* theList;
    struct ast_node* sExpr;
}SCOPE_NODE;

typedef struct ast_node {
    AST_NODE_TYPE type;
    union {
        NUMBER_AST_NODE number;
        FUNCTION_AST_NODE function;
        SYMBOL_AST_NODE symbol;
        LET_LIST_AST_NODE list;
        LET_ELEMENT_AST_NODE element;
        SCOPE_NODE theScope;
    } data;
} AST_NODE;


//the symbol table is used to keep track of symbols and values
SYMBOL_TABLE* symbolTable;

//currentNode keeps the spot in the symbol table
SYMBOL_TABLE_NODE* currentNode;

AST_NODE *number(double value);

AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2);

void freeNode(AST_NODE *p);

double eval(AST_NODE *ast);

AST_NODE* scope(AST_NODE* letList,AST_NODE* sExpres);

AST_NODE* symbol(char* theName,double theValue);

LET_LIST_AST_NODE* let_list(LET_LIST_AST_NODE* theList,LET_ELEMENT_AST_NODE* theElement);

LET_ELEMENT_AST_NODE* let_element(SYMBOL_AST_NODE* input, AST_NODE* sExpr);
#endif
