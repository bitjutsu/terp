#include "stmt.h"
#include "parse.h"
#include "lex.h"
#include "khash.h"

#include <stdio.h>

// setup hashmap stuff
KHASH_MAP_INIT_STR(32, int)

void hello(void) {
	printf("terp (v2.awesome - compiled with --<3-bugs option)\n\n");
	printf("Enter 'quit' to confirm your status as a quitter. Enter code to get yelled at by a computer.\n");
}

typedef struct tagState {
	khash_t(32) *h;
} State;

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

int evaluate(ParseNode *stmt, State *state) {
	khiter_t k = 0;
	int left = 0, right = 0, ret = 0;
	switch(stmt->type) {
	case tASSIGN:
		// evaluate var (to add it to state)
		if (!exists(state->h, stmt->children[0]->name))
			evaluate(stmt->children[0], state);

		k = kh_get(32, state->h, stmt->children[0]->name);
		kh_val(state->h, k) = evaluate(stmt->children[1], state);
		return kh_val(state->h, k);
	case tIF:
		// evaluate branch iff cond = true
		if (evaluate(stmt->children[0], state))
			return evaluate(stmt->children[1], state);
		else
			return 0;
	case tIFELSE:
		// evaluate b_true if cond = true else evaluate b_false
		if (evaluate(stmt->children[0], state))
			return evaluate(stmt->children[1], state);
		else
			return evaluate(stmt->children[2], state);
	case tBOOL:
		// evaluate and store 0/1 in stmt->value
		// make sure both sides resolve into an integer (should always as ints are initialized to 0)
		left = evaluate(stmt->children[0], state);
		right = evaluate(stmt->children[1], state);
		switch(stmt->op) {
		case bLESSTHAN:
			return left < right;
		case bGREATERTHAN:
			return left > right;
		case bEQUALTO:
			return left == right;
		default:
			// this is a bad problem
			return -1;
		}
	case tNUM:
		return stmt->value;
	case tVAR:
		// make sure variable is in state, if it isn't add it
		if (!exists(state->h, stmt->name)) {
			k = kh_put(32, state->h, stmt->name, &ret);
			// the stmt->value must be the current true value of the variable
			// (can't have been changed, hasn't been added to state yet)
			kh_val(state->h, k) = stmt->value;
			return stmt->value;
		} else {
			// stmt->value might not be correct, obtain value from state
			k = kh_get(32, state->h, stmt->name);
			return kh_val(state->h, k);
		}
	default:
		// if you reach here you have a bad problem
		// and you will not evaluate a statement today (or maybe ever)
		return -1;
	}
}

int main(void) {
	ParseNode *stmt = NULL;
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
		printf(": %d\n", evaluate(stmt, state));

		// TODO: cleanup
		//deleteStatement(stmt);
	}

	kh_destroy(32, state->h);
	return 0;
}

