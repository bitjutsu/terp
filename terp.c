#include "stmt.h"
#include "parse.h"
#include "lex.h"
#include "khash.h"
#include "eval.h"
#include "terp.h"

#include <stdio.h>

// for history functionality
#include <readline/readline.h>
#include <readline/history.h>

// TODO: make global history in user's home directory
const char *HISTORY_FILENAME = ".terp_history";

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

void setupHistory() {
	rl_bind_key('\t', rl_complete);

	using_history();
	// set max history size
	// TODO: command line flag
	stifle_history(15);

	// read history file from disk
	read_history_range(HISTORY_FILENAME, 0, 15);
}

int main(void) {
	ParseNode *stmt = NULL;
	Element *result = NULL;
	khiter_t k = 0;
	int ret = 0;
	// TODO: handle arbitrary length input strings with buffers and scanf("%s%n")
	char *input;

	// interpreter state
	State *state = initState();

	setupHistory();
	hello();

	while (1) {
		input = readline("> ");

		if (strcmp(input, "quit") == 0)
			break;

		add_history(input);

		stmt = buildST(input);

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

		free(input);
	}

	write_history(HISTORY_FILENAME);

	freeNil();
	freeState(state);
}

