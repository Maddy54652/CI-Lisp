%{
    #include "ciLisp.h"
%}

%union {
    double dval;
    char *sval;
    struct ast_node *astNode;
    struct symbol_table_node *symbolNode;
};

%token <sval> FUNC SYMBOL
%token <dval> REAL_NUMBER INTEGER_NUMBER

%type <astNode> s_expr f_expr number
%type <symbolNode> let_elem let_section let_list

%token LPAREN RPAREN LET EOL QUIT INTEGER REAL

%%

program:
    s_expr EOL {
        fprintf(stderr, "yacc: program ::= s_expr EOL\n");
        if ($1) {
            printf("%lf", eval($1));
            freeNode($1);
        }
    };

s_expr:
    f_expr {
        $$ = $1;
    }
    | SYMBOL {
        $$ = symbol($1);
    }
    | LPAREN let_section s_expr RPAREN {
        $$ = setSymbolTable($2,$3);
    }
    | number {
        $$ = $1;
    }
    | QUIT {
        fprintf(stderr, "yacc: s_expr ::= QUIT\n");
        exit(EXIT_SUCCESS);
    }
    | error {
        fprintf(stderr, "yacc: s_expr ::= error\n");
        yyerror("unexpected token");
        $$ = NULL;
    };

f_expr:
    LPAREN FUNC s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC expr RPAREN\n");
        $$ = function($2, $3, NULL);
    }
    | LPAREN FUNC s_expr s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC expr expr RPAREN\n");
        $$ = function($2, $3, $4);
    };

let_section:
    LPAREN LET let_list RPAREN{
        $$ = $3;
    }
    | {
        $$ = NULL;
    };

let_list:
    let_list let_elem {
        $$ = addSymbolToList($1,$2);
    }
    | let_elem {
        $$ = $1;
    };

let_elem:
    LPAREN INTEGER SYMBOL s_expr RPAREN {
        $$ = createSymbol(INTEGER_TYPE, $3, $4);
    }
    | LPAREN REAL SYMBOL s_expr RPAREN {
        $$ = createSymbol(REAL_TYPE, $3, $4);
    };

number:
    INTEGER_NUMBER {
        fprintf(stderr, "yacc: s_expr ::= INTEGER_NUMBER\n");
        $$ = number($1,INTEGER_TYPE);
    }
    | REAL_NUMBER{
        fprintf(stderr, "yacc: s_expr ::= REAL_NUMBER\n");
        $$ = number($1, REAL_TYPE);
    };

%%
