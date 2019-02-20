%option noyywrap

%{

// Avoid error "error: `fileno' was not declared in this scope".
extern "C" int fileno(FILE *stream);

// The tokens are defined in the parser.
#include "c_parser.tab.hpp"

#include <stdlib.h>

// Macro to keep count of the line.
unsigned int line_number = 1;
%}

/* Type of digits: octal, decimal and hexadecimal. */
O     [0-7]
D			[0-9]
H			[a-fA-F0-9]

/* Exponent. */
E			[Ee][+-]?{D}+

/* Float and integer suffixes. */
FS			(f|F|l|L)
IS			(u|U|l|L)*

/* Escape sequences. */
ESC    [abfnrtv\'\"\?\\]

/* Decimal or hexadecimal integer constant. */
INTEGER_CONSTANT ({D}+{IS}?|0[xX]{H}+{IS}?)

FLOAT_CONSTANT ({D}+{E}{FS}?|{D}*"."{D}+({E})?{FS}?|{D}+"."{D}*({E})?{FS}?)

/* TODO: find way to extract the actual char or the actual string from the token. */
CHARACTER_CONSTANT  [L]?\'(?:\\({ESC}|{O}{1,3}|x{H}+)|[^\\'])+\'

STRING_CONSTANT [L]?\"(?:\\({ESC}|[0-7]{1,3}|x[0-9A-Fa-f]+)|[^\\"])*\"

IDENTIFIER [_a-zA-Z][_a-zA-Z0-9]*

NEWLINE [\n\r]

WHITESPACE [ \t\v]+

/* TODO. deal with comments. */

%%

{INTEGER_CONSTANT} {
    yylval.integer_constant = atol(yytext);
    return INTEGER_CONSTANT;
  }

{FLOAT_CONSTANT} {
    yylval.float_constant = atof(yytext);
    return FLOAT_CONSTANT;
  }

{CHARACTER_CONSTANT} {
    yylval.char_string_constant = new std::string(yytext);
    return CHARACTER_CONSTANT;
  }

{STRING_CONSTANT} {
    yylval.char_string_constant = new std::string(yytext);
    return CHARACTER_CONSTANT;
  }

"auto"			{ return AUTO; }
"break"			{ return BREAK; }
"case"			{ return CASE; }
"char"			{ return CHAR; }
"const"			{ return CONST; }
"continue"	{ return CONTINUE; }
"default"		{ return DEFAULT; }
"do"			  { return DO; }
"double"		{ return DOUBLE; }
"else"			{ return ELSE; }
"enum"			{ return ENUM; }
"extern"		{ return EXTERN; }
"float"			{ return FLOAT; }
"for"			  { return FOR; }
"goto"			{ return GOTO; }
"if"			  { return IF; }
"int"			  { return INT; }
"long"			{ return LONG; }
"register"	{ return REGISTER; }
"return"		{ return RETURN; }
"short"			{ return SHORT; }
"signed"		{ return SIGNED; }
"sizeof"		{ return SIZEOF; }
"static"		{ return STATIC; }
"struct"		{ return STRUCT; }
"switch"		{ return SWITCH; }
"typedef"		{ return TYPEDEF; }
"union"			{ return UNION; }
"unsigned"	{ return UNSIGNED; }
"void"			{ return VOID; }
"volatile"	{ return VOLATILE; }
"while"			{ return WHILE; }

"..."			 { return ELLIPSIS; }
">>="			 { return RIGHT_ASSIGN; }
"<<="			 { return LEFT_ASSIGN; }
"+="			 { return ADD_ASSIGN; }
"-="			 { return SUB_ASSIGN; }
"*="			 { return MUL_ASSIGN; }
"/="			 { return DIV_ASSIGN; }
"%="			 { return MOD_ASSIGN; }
"&="			 { return AND_ASSIGN; }
"^="			 { return XOR_ASSIGN; }
"|="			 { return OR_ASSIGN; }
">>"			 { return RIGHT_OP; }
"<<"			 { return LEFT_OP; }
"++"			 { return INC_OP; }
"--"			 { return DEC_OP; }
"->"			 { return PTR_OP; }
"&&"			 { return AND_OP; }
"||"			 { return OR_OP; }
"<="			 { return LE_OP; }
">="			 { return GE_OP; }
"=="			 { return EQ_OP; }
"!="			 { return NE_OP; }
";"			   { return ';'; }
("{"|"<%") { return '{'; }
("}"|"%>") { return '}'; }
","			   { return ','; }
":"			   { return ':'; }
"="			   { return '='; }
"("			   { return '('; }
")"			   { return ')'; }
("["|"<:") { return '['; }
("]"|":>") { return ']'; }
"."			   { return '.'; }
"&"			   { return '&'; }
"!"			   { return '!'; }
"~"			   { return '~'; }
"-"			   { return '-'; }
"+"			   { return '+'; }
"*"			   { return '*'; }
"/"			   { return '/'; }
"%"			   { return '%'; }
"<"			   { return '<'; }
">"			   { return '>'; }
"^"			   { return '^'; }
"|"			   { return '|'; }
"?"			   { return '?'; }

{IDENTIFIER} {
    yylval.string = new std::string(yytext);
    return IDENTIFIER;
  }

{NEWLINE} { line_number++; }

{WHITESPACE} { ; }

.  {
    std::cerr << "\n=> Lexer error: Invalid Token" << std::endl
              << "-> Line: " << line_number << std::endl
              << "-> Token: '" << yytext << "'" << std::endl;
    exit(1);
  }
%%

void yyerror (char const *s)
{
  std::cerr << "=> Parse error: " << s << std::endl
            << "-> Last line analyzed: " << line_number << std::endl
            << "-> Last token analyzed: '" << yytext << "'" << std::endl;
  exit(1);
}
