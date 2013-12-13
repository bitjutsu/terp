#include "eval.h"
#include "stmt.h"
#include "terp.h"
#include "khash.h"

#define NIL nil()

// nil singleton
Element *_nil = NULL;
Element *nil() {
	if (_nil == NULL) {
		_nil = (Element *)malloc(sizeof(Element));
  	_nil->type = tNIL;
	}

  return _nil;
}

// TODO: alias Element to something more appropriate
Element *evaluate(ParseNode *stmt, State *state) {
  khiter_t k = 0;
  Element *left, *right, *returnValue;
  int ret;
  switch(stmt->sType) {
  case sASSIGN:
    // TODO: undeclared variables should not be added to state
    // TODO: set in stone types, or no? (Default: no)
    // add var to state if it doesn't exist already
    if (!exists(state->h, stmt->children[0]->name)) {
      // set variable type to expression's value type
      stmt->children[0]->vType = stmt->children[1]->vType;
      k = kh_put(32, state->h, stmt->children[0]->name, &ret);
			// no need to evaluate variable, its value is being destroyed
    } else {
			// variable already exists in hashmap
	    k = kh_get(32, state->h, stmt->children[0]->name);
		}

    kh_val(state->h, k) = evaluate(stmt->children[1], state);

    return kh_val(state->h, k);
  case sIF:
    // evaluate branch iff cond = true
		returnValue = evaluate(stmt->children[0], state);

		if (returnValue->type == tNIL)
			return NIL;

    if (returnValue->value.boolean) {
      return evaluate(stmt->children[1], state);
    } else {
			// there's no else branch of the statement, so its value becomes nil
      return NIL;
    }
  case sIFELSE:
    // evaluate b_true if cond = true else evaluate b_false
		returnValue = evaluate(stmt->children[0], state);

		if (returnValue->type == tNIL)
			return NIL;

    if (returnValue->value.boolean)
      return evaluate(stmt->children[1], state);
    else
      return evaluate(stmt->children[2], state);
  case sBOOL:
		// if no children, then it's true/false, no evaluation
    if (stmt->children == NULL) {
  	  returnValue = (Element *)malloc(sizeof(Element));
	    returnValue->type = tBOOL;
		  returnValue->value.boolean = stmt->value.boolean;
      return returnValue;
    }

    // evaluate and store 0/1 in stmt->value
    left = evaluate(stmt->children[0], state);
    right = evaluate(stmt->children[1], state);

		// TODO: this will need to be changed for reference types
		if (left->type == tNIL || right->type == tNIL)
			return NIL;

    returnValue = (Element *)malloc(sizeof(Element));
    returnValue->type = tBOOL;
    // TODO: handle real numbers
    switch(stmt->op.boolop) {
    case bLESSTHAN:
      returnValue->value.boolean = left->value.integer < right->value.integer;
      break;
    case bGREATERTHAN:
      returnValue->value.boolean = left->value.integer > right->value.integer;
      break;
    case bEQUALTO:
      returnValue->value.boolean = left->value.integer == right->value.integer;
      break;
    default:
      // this is a bad problem
      error("Unknown boolean operation");
      return NIL;
    }

    return returnValue;
  case sINT:
    returnValue = (Element *)malloc(sizeof(Element));
    returnValue->type = tINT;
    returnValue->value = stmt->value;
    return returnValue;
  case sVAR:
    // make sure variable is in state, if it isn't that's a bit of a problem
    if (!exists(state->h, stmt->name)) {
      error("Variable doesn't exist");
      return NIL;
    } else {
      // stmt->value might not be correct, obtain value from state
      k = kh_get(32, state->h, stmt->name);
      return kh_val(state->h, k);
    }
  case sARITH:
    // TODO: handle real numbers
    left = evaluate(stmt->children[0], state);
    right = evaluate(stmt->children[1], state);

    returnValue = (Element *)malloc(sizeof(Element));
    returnValue->type = tINT;

    switch(stmt->op.arithop) {
    case aPLUS:
      returnValue->value.integer = left->value.integer + right->value.integer;
      break;
    case aSUB:
      returnValue->value.integer = left->value.integer - right->value.integer;
      break;
    case aDIV:
      returnValue->value.integer = left->value.integer / right->value.integer;
      break;
    case aMULT:
      returnValue->value.integer =  left->value.integer * right->value.integer;
      break;
    default:
      // whoops
      error("Unknown arithmetic operation");
      return NIL;
    }

    return returnValue;
  default:
    // if you reach here you have a bad problem
    // and you will not evaluate a statement today (or maybe ever)
    error("Fatal: unknown statement type");
    return NIL;
  }
}
