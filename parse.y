%{
#include "stmt.h"
#include "parse.h"
#include "lex.h"

#include <stdio.h>

int yyerror(ParseNode **expression, yyscan_t scanner, const char *msg) {
	printf("Error: %s\n", msg);
}
%}

%code requires {
	#ifndef YY_TYPEDEF_YY_SCANNER_T
	#define YY_TYPEDEF_YY_SCANNER_T
	typedef void* yyscan_t;
	#endif
}

%output  "parse.c"
%defines "parse.h"

%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { ParseNode **statement }
%parse-param { yyscan_t scanner }

%union {
	int value;
	char *name;
	ParseNode *statement;
}

%left '<' '>' '==' LESS_THAN GREATER_THAN EQUAL_TO
%left '+' '-' '*' '/' TOKEN_PLUS TOKEN_SUB TOKEN_MULT TOKEN_DIV

%token IF_START
%token THEN
%token ELSE
%token IF_END

%token TOKEN_TRUE
%token TOKEN_FALSE

%token ASSIGN_INTERMEDIATE

%token <name> VAR
%token <value> VAL

%type <statement> stmt
%type <statement> exp
%type <statement> bool
%type <statement> arith

%%
input
	: stmt { *statement = $1; }
	;

stmt
	: VAR ASSIGN_INTERMEDIATE stmt { $$ = createAssign(createVariable($1), $3); free($1);}
	| IF_START bool THEN stmt IF_END { $$ = createIf($2, $4); }
	| IF_START bool THEN stmt ELSE stmt IF_END { $$ = createIfElse($2, $4, $6); }
	| exp
	| bool
	;

bool
	: exp LESS_THAN exp { $$ = createBool(bLESSTHAN, $1, $3); }
	| exp GREATER_THAN exp { $$ = createBool(bGREATERTHAN, $1, $3); }
	| exp EQUAL_TO exp { $$ = createBool(bEQUALTO, $1, $3); }
	| TOKEN_TRUE { $$ = createBoolTerminal(1); }
	| TOKEN_FALSE { $$ = createBoolTerminal(0); }
	;

exp
	: arith
	| VAL { $$ = createInt($1); }
	| VAR { $$ = createVariable($1); free($1); }
	;

arith
	: exp TOKEN_MULT exp { $$ = createArith(aMULT, $1, $3); }
	| exp TOKEN_PLUS exp { $$ = createArith(aPLUS, $1, $3); }
	| exp TOKEN_SUB exp { $$ = createArith(aSUB, $1, $3); }
	| exp TOKEN_DIV exp { $$ = createArith(aDIV, $1, $3); }
	;

%%
