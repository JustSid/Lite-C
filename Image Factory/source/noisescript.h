#ifndef _NOISESCRIPT_H_
#define _NOISESCRIPT_H_

#include "types.h"

#define kNSExpressionTypeEqual 		0
#define kNSExpressionTypePlus  		1
#define kNSExpressionTypeMinus 		2
#define kNSExpressionTypeMultiply 	3
#define kNSExpressionTypeDivide		4

typedef struct ns_variable_s
{
	const char *name;

	int type, size;
	void *buffer;
	
	BOOL isFunction, isBound;
	
	struct ns_variable_s *pass; // Used by functions as the next parameter
	struct ns_variable_s *next;
} ns_variable_t;

typedef struct ns_expression_s
{
	int type;
	ns_variable_t *rvalue, *lvalue;
	
	struct ns_expression_s *follow;
	struct ns_expression_s *next;
} ns_expression_t;

typedef struct noisescript_s
{
	const char *file;
	ns_variable_t *firstVar;
	ns_expression_t *firstExpression;
	
	struct noisescript_s *next;
} noisescript_t;

#define NSVariableGet(v, type) (*(type *)v->buffer)
#define NSVariableSet(v, value, type) (*(type *)v->buffer) = value

void ns_bindVariable(noisescript_t *script, variable_t *variable, const char *name);
void ns_runScript(noisescript_t *script);

noisescript_t *ns_parseBytes(const char *bytes);
noisescript_t *ns_scriptWithFile(const char *file);

#include "noisescript.c"
#endif
