#ifndef __STMT_H__
#define __STMT_H__

typedef enum tagBoolOp {
  bLESSTHAN,
  bGREATERTHAN,
  bEQUALTO
} BoolOp;

typedef enum tagType {
	tASSIGN,
	tIF,
	tIFELSE,
	tBOOL,
	tNUM,
	tVAR
} Type;

// TODO: make a union, not everything has a name or an operation
typedef struct tagParseNode {
  Type type;
	BoolOp op;
  char *name;
  int value; // all statements have a value - gets propagated up tree from leaves when evaluating

  struct tagParseNode **children;
} ParseNode;

// Create an assignment (this and all below will be added to the parse tree in the parser)
ParseNode *createAssign(ParseNode *var, ParseNode *val);

// Create an if statement
ParseNode *createIf(ParseNode *cond, ParseNode *true);

// Create an if/else statement
ParseNode *createIfElse(ParseNode *cond, ParseNode *true, ParseNode *false);

// Create a boolean statement
ParseNode *createBool(BoolOp op, ParseNode *left, ParseNode *right);

// Create an integer value
ParseNode *createNumber(int value);

// Create a variable
ParseNode *createVariable(char *name);

// Delete a statement (free from memory)
void deleteStatement(ParseNode *node);
#endif

