//
//  LFException.c
//  Lite Foundation
//
//  Created by Sidney Just on 06.05.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "LFException.h"
#include "LFAutoreleasePool.h"
#include "LFInternal.h"

// Exception jump buffer stack:
struct __LFExceptionStackObject
{
	jmp_buf *buf;
	struct __LFExceptionStackObject *prev;
};

struct __LFExceptionStackObject *__LFExceptionStack = NULL;

void __LFExceptionStackPush(jmp_buf *buf)
{
	struct __LFExceptionStackObject *object = (struct __LFExceptionStackObject *)malloc(sizeof(struct __LFExceptionStackObject));
	object->buf = buf;
	object->prev = __LFExceptionStack;
	
	__LFExceptionStack = object;
}

jmp_buf *__LFExceptionStackPop()
{
	if(!__LFExceptionStack)
		return NULL;
	
	jmp_buf *buf;
	
	struct __LFExceptionStackObject *object = __LFExceptionStack;
	buf = object->buf;
	__LFExceptionStack = object->prev;
	
	free(object);
	return buf;
}

static LFExceptionRef __LFExceptionLastExceptionStack = NULL;

LFExceptionRef __LFExceptionLastException()
{
	LFExceptionRef exception = __LFExceptionLastExceptionStack;
	return exception;
}

LFExceptionRef __LFExceptionPopLastException()
{
	LFExceptionRef exception = __LFExceptionLastExceptionStack;
	
	if(exception)
	{
		__LFExceptionLastExceptionStack = exception->prev;
		exception->prev = NULL;
	}
	else
		__LFExceptionLastExceptionStack = NULL;
	
	return exception;
}

void __LFExceptionPushLastException(LFExceptionRef exception)
{
	exception->prev = __LFExceptionLastExceptionStack;
	__LFExceptionLastExceptionStack = exception;
}


// Exception class
void		__LFExceptionInit(LFTypeRef ref);
LFTypeRef	__LFExceptionCopy(LFTypeRef ref);

static LFRuntimeClass __LFExceptionClass;
static LFTypeID __LFExceptionTypeID = kLFInvalidTypeID;

void __LFRuntimeInitException()
{
	__LFExceptionClass.version = 1;
	__LFExceptionClass.name = "LFException";
	
	__LFExceptionClass.init = __LFExceptionInit;
	__LFExceptionClass.copy = __LFExceptionCopy;
	__LFExceptionClass.dealloc = NULL;
	__LFExceptionClass.equal = NULL;
	__LFExceptionClass.hash  = NULL;
	
	__LFExceptionClass.superClass = kLFInvalidTypeID;
	__LFExceptionClass.protocolBag = NULL;
	__LFExceptionTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFExceptionClass);
	
	memset(&__LFExceptionTempBuf, 0, sizeof(jmp_buf));
	__LFTempException = NULL;
}


void __LFExceptionInit(LFTypeRef ref)
{
	LFExceptionRef exception = (LFExceptionRef)ref;
	
	exception->name = NULL;
	exception->reason = NULL;
	exception->info = NULL;
	
	exception->prev = NULL;
}

LFTypeRef __LFExceptionCopy(LFTypeRef ref)
{
	LFExceptionRef source = (LFExceptionRef)ref;
	return LFExceptionCreate(source->name, source->reason, source->info);
}



LFExceptionRef LFExceptionCreate(LFStringRef name, LFStringRef reason, LFDictionaryRef info)
{
	LFExceptionRef exception = LFRuntimeCreateInstance(__LFExceptionTypeID, sizeof(struct __LFException));
	exception->name = LFRetain(name);
	exception->reason = LFRetain(reason);
	exception->info = LFRetain(info);
	
	return exception;
}

LFExceptionRef LFException(LFStringRef name, LFStringRef reason, LFDictionaryRef info)
{
	return LFAutorelease(LFExceptionCreate(name, reason, info));
}



void LFExceptionRaise(LFExceptionRef exception)
{
	struct __LFExceptionStackObject *object = __LFExceptionStack;
	LFAssert2(object, "Uncaught exception '%s'. Reason: '%s'", LFStringGetCString(exception->name), LFStringGetCString(exception->reason));
	
	__LFExceptionPushLastException(exception);
	longjmp(*object->buf, 0);
}
