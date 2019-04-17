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
%token <dval> REAL_NUMBER INTEGER_NUMBER NUMBER
%token LPAREN RPAREN LET EOL QUIT

%type <astNode> s_expr f_expr
%type <symbolNode> let_elem let_section let_list

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
        $$ = function($2, $3, 0);
    }
    | LPAREN FUNC s_expr s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC expr expr RPAREN\n");
        $$ = function($2, $3, $4);
    };

let_section:
    LPAREN let_list RPAREN{
        $$ = $2;
    }
    | {
        $$ = NULL;
    };

let_list:
    let_list let_elem {
        $$ = addSymbolToList($1,$2);
    }
    | LET let_elem {
        $$ = $2;
    };

let_elem:
    LPAREN TYPE SYMBOL s_expr RPAREN {
        $$ = createSymbol($2,$3);
    };
type:
    INTEGER_NUMBER {
         fprintf(stderr, "yacc: s_expr ::= INTEGER_NUMBER\n");
         $$ = number($1);
    };
    | REAL_NUMBER{
         fprintf(stderr, "yacc: s_expr ::= REAL_NUMBER\n");
         $$ = number($1);
    };
     |{
        $$ = NULL; 
      }

%%

