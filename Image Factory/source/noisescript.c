#include "helper.h"
#include "noisescript.h"

void ns_bindVariable(noisescript_t *script, variable_t *variable, const char *name)
{
	ns_variable_t *nvar = script->firstVar;
	while(nvar)
	{
		if(strcmp(nvar->name, name) == 0)
		{
			if(nvar->buffer)
				free(nvar->buffer);
				
			nvar->isBound = true;
			nvar->buffer = variable->buffer;
			break;
		}
		
		nvar = nvar->next;
	}
}

void ns_runScriptAftermath(noisescript_t *script)
{
	ns_variable_t *nvar = script->firstVar;
	while(nvar)
	{
		if(nvar->isBound)
			nvar->buffer = NULL;
		
		nvar = nvar->next;
	}
}


// Variable handling
ns_variable_t *__ns_variableCopy(ns_variable_t *source)
{
	// Can only be used as intermediate variable!!!
	ns_variable_t *v = calloc(1, sizeof(ns_variable_t));
	memcpy(v, source, sizeof(ns_variable_t));
	
	if(source->isBound)
	{
		v->buffer = source->buffer;
	}
	else
	{
		if(source->type != kVariableTypeBMAP)
		{
			v->buffer = calloc(1, v->size);
			memcpy(v->buffer, source->buffer, v->size);
		}
	}

	v->next = NULL;
	v->pass = NULL;
	return v;
}

void __ns_variableFree(ns_variable_t *v)
{
	
}

// Arithmetic functions
void ns_variableArithmeticEqual(int type, ns_variable_t *lvalue, ns_variable_t *rvalue)
{
	switch(type)
	{
		case kVariableTypeBMAP:
			lvalue->buffer = rvalue->buffer;
			break;
			
		case kVariableTypeFloat:
			*((float *)lvalue->buffer) = *((float *)rvalue->buffer);
			break;
	}
}

void ns_variableArithmeticAdd(int type, ns_variable_t *receiver, ns_variable_t *lvalue, ns_variable_t *rvalue)
{
	switch(type)
	{
		case kVariableTypeFloat:
		{
			float intermediate = *((float *)lvalue->buffer) + *((float *)rvalue->buffer);
			*((float *)receiver->buffer) = intermediate;
			break;
		}
	}
}

void ns_variableArithmeticMinus(int type, ns_variable_t *receiver, ns_variable_t *lvalue, ns_variable_t *rvalue)
{
	switch(type)
	{
		case kVariableTypeFloat:
		{
			float intermediate = *((float *)lvalue->buffer) - *((float *)rvalue->buffer);
			*((float *)receiver->buffer) = intermediate;
			break;
		}
	}
}

void ns_variableArithmeticMultiply(int type, ns_variable_t *receiver, ns_variable_t *lvalue, ns_variable_t *rvalue)
{
	switch(type)
	{
		case kVariableTypeFloat:
		{
			float intermediate = *((float *)lvalue->buffer) * *((float *)rvalue->buffer);
			*((float *)receiver->buffer) = intermediate;
			break;
		}
	}
}

void ns_variableArithmeticDivide(int type, ns_variable_t *receiver, ns_variable_t *lvalue, ns_variable_t *rvalue)
{
	switch(type)
	{
		case kVariableTypeFloat:
		{
			float intermediate = *((float *)lvalue->buffer) / *((float *)rvalue->buffer);
			*((float *)receiver->buffer) = intermediate;
			break;
		}
	}
}

void ns_variableArithmeticOperation(int operation, ns_variable_t *receiver, ns_variable_t *lvalue, ns_variable_t *rvalue)
{
	if(!receiver)
	{
		if(operation != kNSExpressionTypeEqual)
			return;
		
		receiver = lvalue;
	}
	
	if(lvalue->type != rvalue->type)
	{
		printf("Can't convert from %i to %i!", lvalue->type, rvalue->type);
		return;
	}
	
	if(rvalue->isFunction)
	{
		int i=0;
		ns_variable_t *blob[10];
		ns_variable_t *pass = rvalue->pass;
		
		while(pass)
		{
			blob[i] = pass;
			i++;
			
			pass = pass->pass;
		}
		
		ns_variable_t *_functor(ns_variable_t **);
		_functor = rvalue->buffer;
		
		pass = _functor(blob);
		ns_variableArithmeticOperation(operation, receiver, lvalue, pass);
		__ns_variableFree(pass);
		
		return;
	}
	
	
	switch(operation)
	{
		case kNSExpressionTypeEqual:
			ns_variableArithmeticEqual(lvalue->type, lvalue, rvalue);
			ns_variableArithmeticEqual(lvalue->type, receiver, lvalue);
			break;
			
		case kNSExpressionTypePlus:
			ns_variableArithmeticAdd(lvalue->type, receiver, lvalue, rvalue);
			break;
			
		case kNSExpressionTypeMinus:
			ns_variableArithmeticMinus(lvalue->type, receiver, lvalue, rvalue);
			break;
		
		case kNSExpressionTypeMultiply:
			ns_variableArithmeticMultiply(lvalue->type, receiver, lvalue, rvalue);
			break;
		
		case kNSExpressionTypeDivide:
			ns_variableArithmeticDivide(lvalue->type, receiver, lvalue, rvalue);
			break;
	}
}



// Evaluation handling
void ns_evaluateExpression(noisescript_t *script, ns_expression_t *expression, ns_variable_t *receiver)
{
	ns_variable_t *intermediate = __ns_variableCopy(expression->rvalue);
	
	if(expression->follow)
		ns_evaluateExpression(script, expression->follow, intermediate);
	
	
	ns_variableArithmeticOperation(expression->type, receiver, expression->lvalue, intermediate);
	__ns_variableFree(intermediate);
}



void ns_runScript(noisescript_t *script)
{
	ns_expression_t *expression = script->firstExpression;
	while(expression)
	{
		ns_evaluateExpression(script, expression, NULL);
		expression = expression->next;
	}
	
	ns_runScriptAftermath(script);
}


#define kScriptStateLookingFor 	0
#define kScriptStateVariable	1
#define kScriptStateExpression	2

ns_variable_t *ns_scriptVariableWithName(noisescript_t *script, const char *name)
{
	ns_variable_t *v = script->firstVar;
	while(v)
	{
		if(strcmp(v->name, name) == 0)
			return v;
			
		v = v->next;
	}
	
	return NULL;
}


ns_expression_t *ns_expressionCreate(noisescript_t *script, int type, ns_variable_t *lvalue, ns_variable_t *rvalue)
{
	ns_expression_t *expression = calloc(1, sizeof(ns_expression_t));
	
	expression->type = type;
	expression->lvalue = lvalue;
	expression->rvalue = rvalue;
	
	return expression;
}

ns_variable_t *ns_variableCreate(noisescript_t *script, int type, const char *name)
{
	ns_variable_t *v = calloc(1, sizeof(ns_variable_t));
	v->name = strmcpy(name);
	
	switch(type)
	{
		case kVariableTypeBMAP:
		{
			v->buffer = NULL;
			break;
		}
		
		case kVariableTypeFloat:
		{
			v->buffer = calloc(1, sizeof(float));
			v->size = sizeof(float);
			break;
		}
	}
	
	
	v->type = type;
	
	v->next = script->firstVar;
	script->firstVar = v;
	
	return v;
}


void ns_scriptAppendExpression(noisescript_t *script, ns_expression_t *expression)
{
	if(!script->firstExpression)
	{
		script->firstExpression = expression;
		return;
	}
	
	ns_expression_t *e = script->firstExpression;
	while(e)
	{
		if(!e->next)
		{
			e->next = expression;
			return;
		}
		
		e = e->next;
	}
}


noisescript_t *ns_parseBytes(const char *bytes)
{
	noisescript_t *script = calloc(1, sizeof(noisescript_t));
	int state = kScriptStateLookingFor;
	int varType = 0;
	
	while(*bytes != '\0')
	{
		if(state == kScriptStateLookingFor && !isspace(*bytes))
		{
			if(strstr(bytes, "var") == bytes)
			{
				state = kScriptStateVariable;
				varType = kVariableTypeFloat;
				
				bytes += 4;
			}
			else if(strstr(bytes, "bmap") == bytes)
			{
				state = kScriptStateVariable;
				varType = kVariableTypeBMAP;
				
				bytes += 5;
				
			}
			else
				state = kScriptStateExpression;
		}
		
		if(state == kScriptStateVariable)
		{
			char *end = strstr(bytes, ";");
			*end = '\0';
			
			ns_variableCreate(script, varType, bytes);
			
			bytes = end;
			state = kScriptStateLookingFor;
		}
		
		if(state == kScriptStateExpression)
		{
			BOOL isSemicolon = false;
			const char *ptokenName = NULL;
			ns_variable_t *prvalue = NULL;
			int opType;
			int popType;
			
			ns_expression_t *pexpression = NULL;
			
			while(1)
			{
				char *token = strpbrk(bytes, ";+-/*=");
				if(!token)
					break;
				
				switch(*token)
				{
					case '=':
						opType = kNSExpressionTypeEqual;
						break;
						
					case '+':
						opType = kNSExpressionTypePlus;
						break;
						
					case '*':
						opType = kNSExpressionTypeMultiply;
						break;
				
					case '-':
						opType = kNSExpressionTypeMinus;
						break;
				
					case '/':
						opType = kNSExpressionTypeDivide;
						break;
						
					case ';':
						isSemicolon = true;
						break;
				}

				const char *tokenName = bytes;
				const char ttoken = *token;
				
				bytes = token + 1;
				
				while(isspace(*token) || *token == ttoken)
				{
					*token = '\0';
					token --;
				}
				
				while(isspace(*tokenName))
					tokenName ++;
				
				ns_variable_t *vtemp = NULL;
				if(prvalue)
				{
					ns_variable_t *rvalue = ns_scriptVariableWithName(script, tokenName);					
					if(!rvalue)
					{
						if(isNumber(tokenName))
						{
							vtemp = ns_variableCreate(script, kVariableTypeFloat, "_!temp!_");
							*((float *)vtemp->buffer) = (float)atof(tokenName);
							
							rvalue = vtemp;
						}
						else
						{
							printf("Unknown variable '%s'!", tokenName);
						
							free(script);
							return NULL;
						}
					}
					
					ns_expression_t *expression = ns_expressionCreate(script, popType, prvalue, rvalue);
					if(pexpression == NULL)
					{
						ns_scriptAppendExpression(script, expression);
					}
					else
						pexpression->follow = expression;
					
					pexpression = expression;
				}
				
				ptokenName = tokenName;
				popType = opType;
				
				prvalue = ns_scriptVariableWithName(script, tokenName);	
				if(vtemp)
					prvalue = vtemp;
				
				if(isSemicolon)
					break;
			}
		
			state = kScriptStateLookingFor;
		}
		
		bytes ++;
	}
	
	return script;
}

noisescript_t *ns_firstScript = NULL;

noisescript_t *ns_scriptWithFile(const char *file)
{
	noisescript_t *script = ns_firstScript;
	while(script)
	{
		if(strcmp(script->file, file) == 0)
			return script;
	}
	
	
	char *bytes = buffer_fromFile(file);
	noisescript_t *script = ns_parseBytes(bytes);
	script->file = strmcpy(file);
		
	script->next = ns_firstScript;
	ns_firstScript = script;
	
	return script;
}
