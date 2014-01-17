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

Element *evaluateLine(char *line, State *state) {
	ParseNode *stmt = buildST(line);
	Element *val = NULL;

	if (stmt == NULL) {
		error("Could not build syntax tree.");
		return NULL;
	}

	/* Evaluate the syntax tree */
	val = evaluate(stmt, state);
	deleteStatement(stmt);

	return val;
}

void interpretScript(char *file, State *state) {
	/* Open the script file specified on the command line */
	FILE *script = fopen(file, "r");

	char *line;
	size_t len = 0;
	Element *result;

	if (script) {
		while (getline(&line, &len, script) != -1) {

			/* Don't care about the return value of each statement, the script will handle its own output.
			Still take return value so that it can be freed. */
			result = evaluateLine(line, state);
			free(result);

			free(line);
			line = NULL;
		}
	} else {
		error("Could not open script!");
		exit(-1);
	}
}

int main(int argc, char *argv[]) {
	Element *result = NULL;
	char *input;

	/* Interpreter session state */
	State *state = initState();

	if (argc > 1) {
		/* For now, accept no command line arguments apart from script interpreter functionality */

		interpretScript(argv[1], state);
		return 0;
	}

	setupHistory();
	hello();

	while (1) {
		input = readline("> ");

		if (strcmp(input, "quit") == 0)
			break;

		add_history(input);

		result = evaluateLine(input, state);
		print(result);

		// cleanup

		// make sure to not free the singleton
		if (result->type != tNIL)
			free(result);

		free(input);
	}

	write_history(HISTORY_FILENAME);

	freeNil();
	freeState(state);
}

