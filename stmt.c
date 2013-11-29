#include "stmt.h"

#include <stdlib.h>
#include <string.h>

ParseNode *allocateNode(int childNum) {
    ParseNode *node = (ParseNode *)malloc(sizeof *node);

    // child nodes
    node->children = malloc(childNum * sizeof(ParseNode *));

    return node;
}

ParseNode *createAssign(ParseNode *var, ParseNode *val) {
    // 2 child nodes required
    ParseNode *assign = allocateNode(2);

    // assign takes value 0 (nil)
		assign->type = tASSIGN;
    assign->value = 0;
    assign->children[0] = var;
    assign->children[1] = val;

    return assign;
}

ParseNode *createIf(ParseNode *cond, ParseNode *true) {
	// 2 child nodes
	ParseNode *stmt = allocateNode(2);

	// will take value of true if cond is true, otherwise 0 (nil)
	stmt->type = tIF;
	stmt->value = 0;
	stmt->children[0] = cond;
	stmt->children[1] = true;

	return stmt;
}

ParseNode *createIfElse(ParseNode *cond, ParseNode *true, ParseNode *false) {
  // 3 child nodes
  ParseNode *stmt = allocateNode(3);

  // takes value of true if cond is true false if cond is false
	stmt->type = tIFELSE;
  stmt->value = 0;
  stmt->children[0] = cond;
  stmt->children[1] = true;
  stmt->children[2] = false;

  return stmt;
}

ParseNode *createBool(BoolOp op, ParseNode *left, ParseNode *right) {
  // 2 children
  ParseNode *stmt = allocateNode(2);

  // can either be false or true (0 or 1)
	stmt->type = tBOOL;
  stmt->value = 0;
  stmt->op = op;
  stmt->children[0] = left;
  stmt->children[1] = right;

  return stmt;
}

ParseNode *createNumber(int value) {
  ParseNode *stmt = allocateNode(0);

	stmt->type = tNUM;
  stmt->value = value;

  return stmt;
}

ParseNode *createVariable(char *name) {
  ParseNode *stmt = allocateNode(0);

	stmt->type = tVAR;
  stmt->name = strdup(name);
  // default to value 0
  stmt->value = 0;

  return stmt;
}

void deleteStatement(ParseNode *node) {
  if (node == NULL)
    return;

	int i = 0;
	/*for (i = 0; i < length(node->children); i++) {
		deleteStatement(node->children[i]);
	}*/
	while (node->children[i] != NULL) {
		deleteStatement(node->children[i]);
		i++;
	}

	free(node);
}

int length(ParseNode **nodes) {
	int n = sizeof nodes;
	return n / sizeof(ParseNode *);
}
