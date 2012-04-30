//
//  LFException.h
//  Lite Foundation
//
//  Created by Sidney Just on 06.05.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef _LFEXCEPTION_H_
#define _LFEXCEPTION_H_

#include "LFRuntime.h"
#include "LFBase.h"
#include "LFString.h"
#include "LFDictionary.h"

struct __LFException
{
	LFRuntimeBase base;
	
	LFStringRef name;
	LFStringRef reason;
	LFDictionaryRef info;
	
	struct __LFException *prev;
};

typedef struct __LFException* LFExceptionRef;


LF_EXPORT LFExceptionRef LFExceptionCreate(LFStringRef name, LFStringRef reason, LFDictionaryRef info);
LF_EXPORT LFExceptionRef LFException(LFStringRef name, LFStringRef reason, LFDictionaryRef info);

LF_EXPORT void LFExceptionRaise(LFExceptionRef exception);

/**
 * @cond
 **/
// PRIVATE, DO NOT USE!
LF_EXPORT void __LFExceptionStackPush(jmp_buf *buf);
LF_EXPORT jmp_buf *__LFExceptionStackPop();
LF_EXPORT LFExceptionRef __LFExceptionLastException();
LF_EXPORT LFExceptionRef __LFExceptionPopLastException();

static jmp_buf __LFExceptionTempBuf;
static LFExceptionRef __LFTempException = NULL;
/**
 * @endcond
 **/


#define LFTry __LFExceptionStackPush(&__LFExceptionTempBuf); if(setjmp(__LFExceptionTempBuf) == 0)
#define LFCatch(exception) else {__LFExceptionStackPop(); exception = __LFTempException = __LFExceptionLastException(); if(__LFTempException)
#define LFFinally } if(!__LFExceptionPopLastException())

#define LFThrow(exception) LFExceptionRaise(exception)

#endif
