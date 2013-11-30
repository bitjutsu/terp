#ifndef __STMT_H__
#define __STMT_H__

typedef enum tagBoolOp {
  bLESSTHAN,
  bGREATERTHAN,
  bEQUALTO
} BoolOp;

typedef enum tagArithOp {
	aPLUS,
	aDIV,
	aSUB,
	aMULT
} ArithOp;

typedef enum tagStmtType {
	sASSIGN,
	sIF,
	sIFELSE,
	sBOOL,
	sINT,
	sVAR,
	sARITH
} StmtType;

typedef enum tagValueType {
	tNIL,
	tBOOL,
	tINT,
	tREAL,
	tSTR,
	tSET
} ValueType;

// forward-declare tagElement to solve circular dependency
struct tagElement;

typedef union tagValue {
	int integer;
	int boolean;
	char *string;
	struct tagElement **set;
} Value;

typedef struct tagElement {
	ValueType type;
	Value value;
} Element;

typedef union tagOp {
	ArithOp arithop;
	BoolOp boolop;
} Op;

// TODO: not everything has a name or an operation
typedef struct tagParseNode {
  StmtType sType;
	Op op;

  char *name;

	// all statements have a value - gets propagated up tree from leaves when evaluating
	ValueType vType;
	Value value;

	// tree isn't necessarily binary
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
ParseNode *createBoolTerminal(int value);

// Create an integer value
ParseNode *createInt(int value);

// Create a variable
ParseNode *createVariable(char *name);

// Create an arithmetic expression
ParseNode *createArith(ArithOp op, ParseNode *left, ParseNode *right);

// Delete a statement (free from memory)
void deleteStatement(ParseNode *node);
#endif

