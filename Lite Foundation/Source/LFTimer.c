//
//  LFTimer.c
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

#include "LFTimer.h"
#include "LFAutoreleasePool.h"
#include "LFRunLoop.h"

void __LFTimerInit(LFTypeRef ref);
void __LFTimerDealloc(LFTypeRef ref);

static LFRuntimeClass __LFTimerClass;
static LFTypeID __LFTimerTypeID = kLFInvalidTypeID;

void __LFRuntimeInitTimer()
{
	__LFTimerClass.version = 1;
	__LFTimerClass.name = "LFTimer";
	
	__LFTimerClass.init = __LFTimerInit;
	__LFTimerClass.copy = NULL;
	__LFTimerClass.dealloc = __LFTimerDealloc;
	__LFTimerClass.equal = NULL;
	__LFTimerClass.hash  = NULL;
	
	__LFTimerClass.superClass = kLFInvalidTypeID;
	__LFTimerClass.protocolBag = NULL;
	__LFTimerTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFTimerClass);
}

void __LFTimerInit(LFTypeRef ref)
{
	LFTimerRef timer = (LFTimerRef)ref;
	
	timer->fireDate = NULL;
	timer->fireInterval = 0.0;
	
	timer->repeats = 0;
	timer->scheduledRunLoop = NULL;
}

void __LFTimerDealloc(LFTypeRef ref)
{
	LFTimerRef timer = (LFTimerRef)ref;
	LFRelease(timer->fireDate);
}


LFTypeID LFTimerGetTypeID()
{
	return __LFTimerTypeID;
}


LFTimerRef LFTimerCreate(LFTimeInterval fire, LFTimerCallback callback, uint8_t repeats)
{
	LFTimerRef timer = LFRuntimeCreateInstance(__LFTimerTypeID, sizeof(struct __LFTimer));
	
	if(timer)
	{
		timer->fireDate = LFDateCreateWithTimeIntervalSinceNow(fire);
		timer->fireInterval = fire;
		
		timer->callback = callback;
		timer->repeats = repeats;
	}
	
	return timer;
}

LFTimerRef LFTimerScheduledTimer(LFTimeInterval fire, LFTimerCallback callback, uint8_t repeats)
{
	LFTimerRef timer = LFAutorelease(LFTimerCreate(fire, callback, repeats));
	LFRunLoopAddTimer(LFRunLoopCurrentRunLoop(), timer);
	
	return timer;
}
