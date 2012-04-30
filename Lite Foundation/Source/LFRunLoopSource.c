//
//  LFRunLoopSource.c
//  Lite Foundation
//
//  Copyright (c) 2011 by Sidney Just
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
//  documentation files (the "Software"), to deal in the Software without restriction, including without limitation 
//  the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
//  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
//  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
//  FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
//  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include "LFRunLoopSource.h"

void __LFRunLoopSourceInit(LFTypeRef ref);
void __LFRunLoopSourceDealloc(LFTypeRef ref);

static LFRuntimeClass __LFRunLoopSourceClass;
static LFTypeID __LFRunLoopSourceTypeID = kLFInvalidTypeID;

void __LFRuntimeInitRunLoopSource()
{
	__LFRunLoopSourceClass.version = 1;
	__LFRunLoopSourceClass.name = "LFRunLoopSource";
	
	__LFRunLoopSourceClass.init = __LFRunLoopSourceInit;
	__LFRunLoopSourceClass.copy = NULL;
	__LFRunLoopSourceClass.dealloc = __LFRunLoopSourceDealloc;
	__LFRunLoopSourceClass.equal = NULL;
	__LFRunLoopSourceClass.hash  = NULL;
	
	__LFRunLoopSourceClass.superClass = kLFInvalidTypeID;
	__LFRunLoopSourceClass.protocolBag = NULL;
	__LFRunLoopSourceTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFRunLoopSourceClass);
}

void __LFRunLoopSourceInit(LFTypeRef ref)
{
	LFRunLoopSourceRef source = (LFRunLoopSourceRef)ref;
	
	source->callback = NULL;
	source->runLoop = NULL;
	
	source->startDate = NULL;
	source->endDate = NULL;
}

void __LFRunLoopSourceDealloc(LFTypeRef ref)
{
    LFRunLoopSourceRef source = (LFRunLoopSourceRef)ref;
    
    LFRelease(source->startDate);
    LFRelease(source->endDate);
}


LFTypeID LFRunLoopSourceGetTypeID()
{
    return __LFRunLoopSourceTypeID;
}

LFRunLoopSourceRef LFRunLoopSourceCreate(LFRunLoopSourceCallback callback, void *data)
{
	return LFRunLoopSourceCreateWithDate(callback, NULL, NULL, data);
}

LFRunLoopSourceRef LFRunLoopSourceCreateWithDate(LFRunLoopSourceCallback callback, LFDateRef start, LFDateRef end, void *data)
{
	if(!callback)
		return NULL;
	
	LFRunLoopSourceRef source = LFRuntimeCreateInstance(__LFRunLoopSourceTypeID, sizeof(struct __LFRunLoopSource));
	
	if(source)
	{
		source->callback = callback;
		source->startDate = LFRetain(start);
		source->endDate = LFRetain(end);
		source->data = data;
	}
	
	return source;
}
