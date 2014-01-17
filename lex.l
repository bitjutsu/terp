%{
#include "stmt.h"
#include "parse.h"

#include <stdio.h>
%}

%option outfile="lex.c" header-file="lex.h"
%option warn nodefault
 
%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

digit						[0-9]
char						[a-zA-Z]
ws							[ \r\t\n]*

%%

{ws}						{ /* Skip whitespace */ }

"if"						return IF_START;
"then"						return THEN;
"else"						return ELSE;
"end"						return IF_END;

"="							return ASSIGN_INTERMEDIATE;

"*"							return TOKEN_MULT;
"+"							return TOKEN_PLUS;
"-"							return TOKEN_SUB;
"/"							return TOKEN_DIV;

"true"						return TOKEN_TRUE;
"false"						return TOKEN_FALSE;

"<"							return LESS_THAN;
">"							return GREATER_THAN;
"=="						return EQUAL_TO;

{digit}+                    { sscanf(yytext, "%d", &yylval->value); return VAL; }
{char}({char}|{digit})*     { yylval->name = strdup(yytext); return VAR; }
.							{ /* Skip everything else */ }

%%
