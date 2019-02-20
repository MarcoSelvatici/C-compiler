%code requires{
  #include "../inc/ast.hpp"
  #include <vector>

  // Our ast root.
  // Vector of ast trees. Each tree is a global unit (e.g. function declaration, include
  // statement, global variable declaration etc...).
  extern std::vector<const Expression*> ast_roots;

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

  // Constants.
  long long int integer_constant;
  double        float_constant;
  std::string*  char_string_constant;
}

%type <expression> external_declaration function_definition
%type <string> IDENTIFIER
%type <integer_constant> INTEGER_CONSTANT
%type <float_constant> FLOAT_CONSTANT
%type <char_string_constant> CHARACTER_CONSTANT STRING_CONSTANT

%token IDENTIFIER
%token INTEGER_CONSTANT FLOAT_CONSTANT CHARACTER_CONSTANT STRING_CONSTANT

// C keywords.
%token AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE ELSE ENUM EXTERN FLOAT FOR GOTO IF INT LONG REGISTER RETURN SHORT SIGNED SIZEOF STATIC STRUCT SWITCH TYPEDEF UNION UNSIGNED VOID VOLATILE WHILE

// C operators.
// Operators composed by only one character are defined as is (e.g. + is '+').
%token ELLIPSIS RIGHT_ASSIGN LEFT_ASSIGN ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN RIGHT_OP LEFT_OP INC_OP DEC_OP PTR_OP AND_OP OR_OP LE_OP GE_OP EQ_OP NE_OP

%start translation_unit

%%

/* [OK] Every top level declaration. */
translation_unit
	: external_declaration                   /*{ ast_roots.push_back($1); }*/
	| translation_unit external_declaration  /*{ ast_roots.push_back($2); }*/
	;

/* [OK] A single top level declaration. */
external_declaration
	: function_definition    /*{ $$ = $1; }*/
	| declaration_expression /*{ $$ = $1; }*/
	;

/* Define function.
 * TODO check for return statement in non-void functions.*/
function_definition
	: type_specifier declarator arguments_list compound_statement
	;

/* Only accept no arguments. */
arguments_list
  : '(' ')'
  ;

/* Sequence of statements. */
compound_statement
  : '{' statement_list '}'
  ;

/* [OK] One or more statements. */
statement_list
  : statement
  | statement_list statement
  ;

/* Possible statements. */
statement
	: compound_statement
	| expression_statement
  | declaration_statement
	/*| selection_statement
	| iteration_statement*/
	;

/* [OK] Expression. */
expression_statement
	: ';'
	| expression ';'
	;

/* Every simple expression. Could be an assignent, a declaration, a function call
 * etc..
 * Only assignment and declaration for now. */
expression
  : declaration_expression
  | assignment_expression
  | logical_or_arithmetic_expression
  ;

/* Assignment_expressions are like
 * var_name = var_name 
 * var_name = some arithmetic expr */
assignment_expression
  : declarator assignment_operator assignment_expression_rhs
  ;

assignment_expression_rhs
  : logical_or_arithmetic_expression
  | declarator
  ;

assignment_operator
	: '='
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
	;

/* Declaration expressions are like
 * type var_name = smth
 * type var_name */
declaration_expression
  : type_specifier declaration_expression_rhs
  ;

declaration_expression_rhs
  : assignment_expression
  | declarator
  ;

/* Logical or arithmetic expressions are like
 * var_name + */
logical_or_arithmetic_expression
  : declarator 
  ;

primary_expression
	: IDENTIFIER
  | INTEGER_CONSTANT
  | FLOAT_CONSTANT
  | CHARACTER_CONSTANT
  | STRING_CONSTANT
	| '(' logical_or_arithmetic_expression ')'
	;

postfix_expression
	: primary_expression
	| postfix_expression INC_OP
	| postfix_expression DEC_OP
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression
	| DEC_OP unary_expression
	| unary_operator unary_expression
	;

unary_operator
	: '&'
	| '*'
	| '+'
	| '-'
	| '~'
	| '!'
	;

/* Arithmetic and logical expressions ordereing */
multiplicative_expression
	: unary_expression
	| multiplicative_expression '*' unary_expression
	| multiplicative_expression '/' unary_expression
	| multiplicative_expression '%' unary_expression
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression
	| additive_expression '-' multiplicative_expression
	;

shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression LE_OP shift_expression
	| relational_expression GE_OP shift_expression
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression
	| equality_expression NE_OP relational_expression
	;

and_expression
	: equality_expression
	| and_expression '&' equality_expression
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OR_OP logical_and_expression
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression



/* Declarator for a variable. Only direct name allowed, no pointers.*/
declarator
	: direct_declarator
	;

/* Only simple types allowed, e.g. int, float or defined types.
 * No arrays or struct allowed. */
direct_declarator
	: IDENTIFIER                                     /*{ $$ = new Identifier( *$1 );
                                                     delete $1;
                                                   }*/
	;

/* Only INT allowed for now. */
type_specifier
  : INT
  ;

%%

// Definition of variable (to match declaration earlier).
std::vector<const Expression*> ast_roots;

std::vector<const Expression*> parseAST() {
  yyparse();
  return ast_roots;
}
