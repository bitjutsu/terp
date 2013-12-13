#include "stmt.h"
#include "parse.h"
#include "lex.h"
#include "khash.h"
#include "eval.h"
#include "terp.h"

#include <stdio.h>

void error(char *msg) {
	printf("%s\n", msg);
}

void hello(void) {
	printf("terp (v2.awesome - compiled with --<3-bugs option)\n\n");
	printf("Enter 'quit' to confirm your status as a quitter. Enter code to get yelled at by a computer.\n");
}

void print(Element *result) {
	switch(result->type) {
	case tNIL:
		printf(": nil\n");
		break;
	case tINT:
		printf(": %d\n", result->value.integer);
		break;
	case tSTR:
		printf(": %s\n", result->value.string);
		break;
	case tBOOL:
		printf(": %s\n", (result->value.boolean) ? "true" : "false");
		break;
	default:
		printf(": Error, could not identify return type\n");
		break;
	}
}

State *initState() {
	State *ret = (State *)malloc(sizeof(State *));
	ret->h = kh_init(32);

	return ret;
}

ParseNode *buildST(const char *input) {
	ParseNode *stmt;
    yyscan_t scanner;
    YY_BUFFER_STATE state;

    if (yylex_init(&scanner)) {
        // couldn't initialize
        return NULL;
    }

	state = yy_scan_string(input, scanner);

    if (yyparse(&stmt, scanner)) {
        // error parsing
        return NULL;
    }

    yy_delete_buffer(state, scanner);

    yylex_destroy(scanner);

    return stmt;
}

// convenience function for hashmap "exists"
int exists(kh_32_t *h, char *key) {
	khiter_t k = kh_get(32, h, key);
	return !(k == kh_end(h));
}

void freeState(State *state) {
	int i;
	// start at i=1, because that's the first value in khash's implementation
	for (i = 1; i <= kh_size(state->h); i++)
		free(state->h->vals[i]);

	kh_destroy(32, state->h);
	free(state);
}

int main(void) {
	ParseNode *stmt = NULL;
	Element *result = NULL;
	khiter_t k = 0;
	int ret = 0;
	// TODO: handle arbitrary length input strings with buffers and scanf("%s%n")
	char test[256];

	// interpreter state
	State *state = initState();

	hello();

	while (1) {
		printf("> ");
		fgets(test, 256, stdin);

		if (strcmp(test, "quit\n") == 0)
			break;

		stmt = buildST(test);

		if (stmt == NULL) {
			printf("Could not build syntax tree.\n");
			continue;
		}

		// evaluate the syntax tree
		result = evaluate(stmt, state);
		print(result);

		// cleanup
		deleteStatement(stmt);
		stmt = NULL;

		// make sure to not free the singleton
		if (result->type != tNIL)
			free(result);
	}

	freeNil();
	freeState(state);
}

