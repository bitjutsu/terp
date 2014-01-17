#ifndef __EVAL_H__
#define __EVAL_H__

#include "stmt.h"
#include "terp.h"

Element *evaluate(ParseNode *stmt, State *state);
Element *evaluateLine(char *line, State *state);
void freeNil();

#endif
