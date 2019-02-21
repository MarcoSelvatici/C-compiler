%code requires{
  #include "../inc/ast.hpp"
  #include <vector>

  // Our ast root.
  // Vector of ast trees. Each tree is a global unit (e.g. function declaration, include
  // statement, global variable declaration etc...).
  extern std::vector<const Node*> ast_roots;

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
  const Node*  node;
  std::string* string;

  // Constants.
  long long int integer_constant;
  double        float_constant;
  std::string*  char_string_constant;
}

%type <node> external_declaration function_definition declaration_expression declarator direct_declarator assignment_expression assignment_expression_rhs logical_or_arithmetic_expression arguments_list compound_statement statement statement_list expression_statement expression
%type <string> IDENTIFIER type_specifier assignment_operator
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
	: external_declaration                   { ast_roots.push_back($1); }
	| translation_unit external_declaration  { ast_roots.push_back($2); }
	;

/* [OK] A single top level declaration. */
external_declaration
	: function_definition        { $$ = $1; }
	| declaration_expression ';' { $$ = $1; }
	;

/* Define function.
 * TODO check for return statement in non-void functions.*/
function_definition
	: type_specifier declarator arguments_list compound_statement { $$ = new FunctionDefinition(*$1, $2, $3, $4); delete $1; }
	;

/* Only accept no arguments. */
arguments_list
  : '(' ')' { $$ = new ArgumentsList(); }
  ;

/* Sequence of statements. */
compound_statement
  : '{' statement_list '}' { $$ = $2; }
  ;

/* [OK] One or more statements. */
statement_list
  : statement statement_list { $$ = new StatementListNode($1, $2); }
  | statement                { $$ = new StatementListNode($1, nullptr); }
  ;

/* Possible statements. */
/* TODO implement remaining statemet types. */
statement
	: compound_statement   { $$ = $1; }
	| expression_statement { $$ = $1; }
	/*| selection_statement
	| iteration_statement*/
	;

/* [OK] Expression. */
expression_statement
	: ';'            { $$ = new EmptyExpression(); }
	| expression ';' { $$ = $1; }
	;

/* Every simple expression. Could be an assignment, a declaration, a function call
 * etc..
 * Only assignment and declaration for now. */
expression
  : declaration_expression            { $$ = $1; }
  | assignment_expression             { $$ = $1; }
  | logical_or_arithmetic_expression  { $$ = $1; }
  ;

/* Assignment_expressions are like
 * var_name = var_name 
 * var_name = some arithmetic expr */
assignment_expression
  : declarator assignment_operator assignment_expression_rhs { $$ = new AssignmentExpression($1, *$2, $3); delete $2;}
  ;

assignment_expression_rhs
  : logical_or_arithmetic_expression  { $$ = $1; }
  | declarator                        { $$ = $1; }
  ;

assignment_operator
	: '='          { $$ = new std::string("="); }
	| MUL_ASSIGN   { $$ = new std::string("*="); }
	| DIV_ASSIGN   { $$ = new std::string("/="); }
	| MOD_ASSIGN   { $$ = new std::string("%="); }
	| ADD_ASSIGN   { $$ = new std::string("+="); }
	| SUB_ASSIGN   { $$ = new std::string("-="); }
	| LEFT_ASSIGN  { $$ = new std::string("<<="); }
	| RIGHT_ASSIGN { $$ = new std::string(">>="); }
	| AND_ASSIGN   { $$ = new std::string("&="); }
	| XOR_ASSIGN   { $$ = new std::string("^="); }
	| OR_ASSIGN    { $$ = new std::string("|="); }
	;

/* Declaration expressions are like
 * type var_name = smth
 * type var_name */
declaration_expression
  : type_specifier declarator '=' assignment_expression_rhs { $$ = new DeclarationExpression(*$1, $2, $4); delete $1; }
  | type_specifier declarator                               { $$ = new DeclarationExpression(*$1, $2); delete $1; }
  ;

/* Logical or arithmetic expressions are like
 * var_name + 
 * TODO define the correct one.
 */
logical_or_arithmetic_expression
  : INTEGER_CONSTANT  { $$ = new IntegerConstant( $1 ); }
  ;

/* ============== BEGIN Arithmetic and logical expressions ordereing */
primary_expression
	: IDENTIFIER
  | INTEGER_CONSTANT
  /*| FLOAT_CONSTANT
  | CHARACTER_CONSTANT
  | STRING_CONSTANT */
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

/* TODO(fabio) implenet this at the end. */
conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression

/* ============== END Arithmetic and logical expressions ordereing */

/* Declarator for a variable. Only direct name allowed, no pointers.*/
declarator
	: direct_declarator { $$ = $1; }
	;

/* Only simple types allowed, e.g. int, float or defined types.
 * No arrays or struct allowed. */
direct_declarator
	: IDENTIFIER  { $$ = new Variable( *$1, "normal" ); delete $1; }
	;

/* Only INT allowed for now. */
type_specifier
  : INT { $$ = new std::string("int"); }
  ;

%%

// Definition of variable (to match declaration earlier).
std::vector<const Node*> ast_roots;

std::vector<const Node*> parseAST() {
  yyparse();
  return ast_roots;
}
