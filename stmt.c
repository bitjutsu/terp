#include "stmt.h"

#include <stdlib.h>
#include <string.h>

ParseNode *allocateNode(int childNum) {
	ParseNode *node = (ParseNode *)malloc(sizeof *node);

	// child nodes
	if (childNum != 0) {
		node->children = (ParseNode **)malloc((childNum+1)*sizeof(ParseNode *));
		// add null pointer for great justice (and also to avoid segfaults)
		node->children[childNum] = NULL;
	} else {
		node->children = NULL;
	}

	return node;
}

ParseNode *createAssign(ParseNode *var, ParseNode *val) {
	// 2 child nodes required
	ParseNode *assign = allocateNode(2);

	assign->sType = sASSIGN;
	// assign takes value 0 (nil)
	assign->vType = tNIL;

	assign->children[0] = var;
	assign->children[1] = val;

	return assign;
}

ParseNode *createIf(ParseNode *cond, ParseNode *true) {
	// 2 child nodes
	ParseNode *stmt = allocateNode(2);

	stmt->sType = sIF;
	// will take value of true if cond is true, otherwise 0 (nil)
	stmt->vType = true->vType;

	stmt->children[0] = cond;
	stmt->children[1] = true;

	return stmt;
}

ParseNode *createIfElse(ParseNode *cond, ParseNode *true, ParseNode *false) {
	// 3 child nodes
	ParseNode *stmt = allocateNode(3);

	stmt->sType = sIFELSE;

	// takes value of true if cond is true false if cond is false (assume true->vType == false->vType for building tree)
	stmt->vType = true->vType;

	stmt->children[0] = cond;
	stmt->children[1] = true;
	stmt->children[2] = false;

	return stmt;
}

ParseNode *createBool(BoolOp op, ParseNode *left, ParseNode *right) {
	// 2 children
	ParseNode *stmt = allocateNode(2);

	stmt->sType = sBOOL;

	// can either be false or true (0 or 1)
	stmt->vType = tBOOL;
	stmt->value.boolean = 0;

	stmt->op.boolop = op;

	stmt->children[0] = left;
	stmt->children[1] = right;

	return stmt;
}

ParseNode *createBoolTerminal(int value) {
	ParseNode *stmt = allocateNode(0);

	stmt->sType = sBOOL;
	stmt->vType = tBOOL;
	stmt->value.boolean = value;

	return stmt;
}

ParseNode *createInt(int value) {
	ParseNode *stmt = allocateNode(0);

	stmt->sType = sINT;

	stmt->vType = tINT;
	stmt->value.integer = value;

	return stmt;
}

ParseNode *createVariable(char *name) {
	ParseNode *stmt = allocateNode(0);

	stmt->sType = sVAR;
	stmt->name = strdup(name);

	// make no assumptions about vType or value

	return stmt;
}

ParseNode *createArith(ArithOp op, ParseNode *left, ParseNode *right) {
	ParseNode *stmt = allocateNode(2);

	stmt->sType = sARITH;
	stmt->op.arithop = op;

	// real/int => real, int/int => int, int/real => real, real/real => real
	// real*real => real, int*int => int, real*int => real
	// real-real => real, real-int => real, int-int => int, int-real => real
	// real+real => real, real+int => real, int+int => int
	if (left->vType == tINT && right->vType == tINT) {
		stmt->vType = tINT;
	} else {
		stmt->vType = tREAL;
	}

	stmt->children[0] = left;
	stmt->children[1] = right;

	return stmt;
}

void deleteStatement(ParseNode *node) {
	if (node == NULL)
		return;

	int i = 0;
	while (node->children != NULL && node->children[i] != NULL) {
		deleteStatement(node->children[i]);
		i++;
	}

	if (node->children != NULL)
		free(node->children);

	free(node);
	node = NULL;
}
