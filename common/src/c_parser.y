%code requires{
  #include "../inc/ast.hpp"

  // Our ast root.
  // TODO(marco): figure out a way to handle multiple roots (e.g. 2 functions in the
  // global scope).
  extern const Expression *ast_root;

  //! This is to fix problems when generating C++
  // We are declaring the functions provided by Flex, so
  // that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);

  // Allow external files to call this function.
  extern void yyset_in(FILE* fd);
}

// Represents the value associated with any kind of AST node.
%union{
  const Expression* expression;
  std::string* string;
}

%token T_IDENTIFIER

%type <expression> EXPR
%type <string> T_IDENTIFIER;

%start ROOT

%%

ROOT : EXPR { ast_root = $1; }

/* TODO : that is wrong, replace with the correct grammar. */
EXPR : T_IDENTIFIER { $$ = new Identifier(*$1); }

%%

// Definition of variable (to match declaration earlier).
const Expression* ast_root;

const Expression* parseAST() {
  ast_root = 0;
  yyparse();
  return ast_root;
}
