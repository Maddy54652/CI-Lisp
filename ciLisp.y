%{
    #include "ciLisp.h"
%}

%union {
    double dval;
    char *sval;
    struct ast_node *astNode;
    struct symbol_table_node *symbolTableNode;
};

%token <sval> FUNC
%token <dval> NUMBER
%token <sval> SYMBOL
%token LPAREN RPAREN EOL QUIT LET

%type <astNode> f_expr s_expr
%type <symbolTableNode> let_list let_elem scope
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
    NUMBER {
        fprintf(stderr, "yacc: s_expr ::= NUMBER\n");
        $$ = number($1);
    }
    | f_expr {
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
    }
    | SYMBOL{
        fprintf(stderr, "yacc: s_expr ::= SYMBOL\n");
        $$ = symbol($1);
    }
    | LPAREN scope s_expr RPAREN{
        $$ =  scope($2,$3);
    };

f_expr:
    LPAREN FUNC s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC expr RPAREN\n");
        $$ = function($2, $3, 0);
    }
    | LPAREN FUNC s_expr s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC expr expr RPAREN\n");
        $$ = function($2, $3, $4);
    };

scope:
    /* empty */ {
        $$ = NULL;
    }
    |LPAREN let_list RPAREN{
        $$ = $2;
    };

let_list:
    let_elem{
        $$=$1;
    }
    | let_list let_elem{
        $$ = let_list($2,$1);
    };

let_elem:
    LPAREN LET SYMBOL s_expr RPAREN{
        $$ = let_element($3,$4);
    };
%%

