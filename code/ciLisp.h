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
    NEG_OPER, // 0
    ABS_OPER, // 1
    EXP_OPER, // 2
    SQRT_OPER, // 3
    ADD_OPER, // 4
    SUB_OPER, // 5
    MULT_OPER, // 6
    DIV_OPER, // 7
    REMAINDER_OPER, // 8
    LOG_OPER, // 9
    POW_OPER, // 10
    MAX_OPER, // 11
    MIN_OPER, // 12
    EXP2_OPER, // 13
    CBRT_OPER, // 14
    HYPOT_OPER, // 15
    PRINT,//16
    CUSTOM_FUNC=255
} OPER_TYPE;

OPER_TYPE resolveFunc(char *);

typedef enum {
    NUM_TYPE, FUNC_TYPE, SYMB_TYPE
} AST_NODE_TYPE;

typedef enum{
    NO_TYPE, INTEGER_TYPE, REAL_TYPE
}DATA_TYPE;

typedef struct {
    double value;
} NUMBER_AST_NODE;

typedef struct {
    char *name;
    struct ast_node *opList;
} FUNCTION_AST_NODE;

typedef struct symbol_ast_node{
    char* name;
}SYMBOL_AST_NODE;

typedef struct symbol_table_node{
    DATA_TYPE val_type;
    char* ident;
    struct ast_node *val;
    struct symbol_table_node *next;

}SYMBOL_TABLE_NODE;

typedef struct return_value{
    DATA_TYPE type;
    double value;
}RETURN_VALUE;

typedef struct ast_node {
    AST_NODE_TYPE type;
    SYMBOL_TABLE_NODE *symbolTable;
    struct ast_node *parent;
    union {
        RETURN_VALUE number;
        FUNCTION_AST_NODE function;
        SYMBOL_AST_NODE symbol;
    } data;
    struct ast_node *next;
} AST_NODE;

AST_NODE *number(double value, DATA_TYPE theType);

AST_NODE *function(char *funcName, AST_NODE *oplist);

void freeNode(AST_NODE *p);

RETURN_VALUE eval(AST_NODE *p);

AST_NODE *setSymbolTable(SYMBOL_TABLE_NODE *symbolTable, AST_NODE *s_expr);

AST_NODE *symbol (char* name);

SYMBOL_TABLE_NODE *createSymbol(DATA_TYPE type,char *name, AST_NODE *value);

SYMBOL_TABLE_NODE* findSymbol(SYMBOL_TABLE_NODE *symbolTable, SYMBOL_TABLE_NODE *symbol);

SYMBOL_TABLE_NODE *addSymbolToList(SYMBOL_TABLE_NODE *symbolTable, SYMBOL_TABLE_NODE *newSymbol);

SYMBOL_TABLE_NODE* resolveSymbol(char* name,AST_NODE* node);

//AST_NODE* evalSymbol(AST_NODE* p);

RETURN_VALUE evalFunction(AST_NODE *p);

AST_NODE* conformToList(AST_NODE* sExpression, AST_NODE* list);

int countOperators(AST_NODE* list);

void errorMessages(int errorCode, char* funcName);

#endif
