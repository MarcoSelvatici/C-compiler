%option noyywrap

%{
// Avoid error "error: `fileno' was not declared in this scope".
extern "C" int fileno(FILE *stream);

// The tokens are defined in the parser.
#include "c_paerser.tab.hpp"

// Macro to keep count of the line.
unsigned int line_number = 1;
%}

IDENTIFIER [_a-zA-Z][_a-zA-Z0-9]*

NEWLINE [\n]

WHITESPACE [ \t\r]+

%%

{IDENTIFIER} {
    yyval.string = yytext;
    return T_IDENTIFIER;
  }

{NEWLINE} { line_number++; }

{WHITESPACE} { ; }

.  {
    std::cerr << "\n=> Lexer error: Invalid Token << std::endl
              << "-> Line: " << line_number << std::endl
              << "-> Token: << yytext << std::endl;
    exit(1);
  }
%%

void yyerror (char const *s)
{
  std::cerr << "Parse error: << s << std::endl;
  exit(1);
}
