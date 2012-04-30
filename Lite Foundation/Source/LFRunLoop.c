//
//  LFRunLoop.c
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

#include "LFRunLoop.h"
#include "LFEnumerator.h"
#include "LFThread.h"

void __LFRunLoopInit(LFTypeRef ref);

static LFRuntimeClass __LFRunLoopClass;
static LFTypeID __LFRunLoopTypeID = kLFInvalidTypeID;

void __LFRuntimeInitRunLoop()
{
	__LFRunLoopClass.version = 1;
	__LFRunLoopClass.name = "LFRunLoop";
	
	__LFRunLoopClass.init = __LFRunLoopInit;
	__LFRunLoopClass.copy = NULL;
	__LFRunLoopClass.dealloc = NULL;
	__LFRunLoopClass.equal = NULL;
	__LFRunLoopClass.hash  = NULL;
	
	__LFRunLoopClass.superClass = kLFInvalidTypeID;
	__LFRunLoopClass.protocolBag = NULL;
	__LFRunLoopTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFRunLoopClass);
}

void __LFRunLoopInit(LFTypeRef ref)
{
	LFRunLoopRef runLoop = (LFRunLoopRef)ref;
	
	runLoop->sources = LFSetCreate(NULL);
	runLoop->timers = LFSetCreate(NULL);
}


// Used by LFThread
LFRunLoopRef __LFRunLoopCreate()
{
	LFRunLoopRef runLoop = LFRuntimeCreateInstance(__LFRunLoopTypeID, sizeof(struct __LFRunLoop));
	return runLoop;
}


LFRunLoopRef LFRunLoopCurrentRunLoop()
{
	LFThreadRef thread = LFThreadGetCurrentThread();
	return thread->runLoop;
}

void __LFRunLoopRunTimers(LFRunLoopRef runLoop, LFDateRef currentDate)
{
	LFSetRef timerCopy = LFCopy(runLoop->timers);
	LFEnumeratorRef enumerator = LFEnumeratorCreate(timerCopy);
	
	LFTimerRef _timer;
	
	while((_timer = LFEnumeratorNextObject(enumerator)))
	{
		LFRuntimeLock(_timer);
		
		if(LFDateCompare(currentDate, _timer->fireDate) >= kLFCompareEqualTo)
		{
#ifndef LF_TARGET_LITEC
			LFTimerCallback callback = (LFTimerCallback)_timer->callback;
			callback(_timer);
#else
			void __LFRunLoopGenericCallback(void *param);
			
			__LFRunLoopGenericCallback = (LFTimerCallback)_timer->callback;
			__LFRunLoopGenericCallback(_timer);
#endif
			
			if(_timer->repeats)
			{
				LFDateUpdateDate(_timer->fireDate);
				LFDateAddTimeInterval(_timer->fireDate, _timer->fireInterval);
			}
			else
			{
				LFRunLoopRemoveTimer(runLoop, _timer);
			}
		}
		
		LFRuntimeUnlock(_timer);
	}
	
	LFRelease(enumerator);
	LFRelease(timerCopy);
}

void __LFRunLoopRunSources(LFRunLoopRef runLoop, LFDateRef currentDate)
{
	LFSetRef sourcesCopy = LFCopy(runLoop->sources);
	LFEnumeratorRef enumerator = LFEnumeratorCreate(sourcesCopy);
	
	LFRunLoopSourceRef source;
	
	while((source = LFEnumeratorNextObject(enumerator)))
	{
		LFRuntimeLock(source);
		
		uint8_t executeSource = 0;
		uint8_t cleanUpSource = 0;
		
		if(source->startDate)
		{
			if(LFDateCompare(currentDate, source->startDate) >= kLFCompareEqualTo)
				executeSource ++;
		}
		else
			executeSource ++;
		
		if(source->endDate)
		{
			if(LFDateCompare(currentDate, source->endDate) < kLFCompareEqualTo)
			{
				executeSource ++;
			}
			else
			{
				executeSource ++;
				cleanUpSource = 1;
			}
		}
		else
			executeSource ++;
		
		
		if(executeSource >= 2)
		{
#ifndef LF_TARGET_LITEC
			LFRunLoopSourceCallback callback = source->callback;
			callback(source);
#else
			void __LFRunLoopGenericCallback(void *param);
			
			__LFRunLoopGenericCallback = source->callback;
			__LFRunLoopGenericCallback(source);
#endif
			
			
		}
		
		if(cleanUpSource)
		{
			LFRunLoopRemoveSource(runLoop, source);
		}

		LFRuntimeUnlock(source);
	}
	
	LFRelease(enumerator);
	LFRelease(sourcesCopy);
}


void __LFRunLoopDoStep(LFRunLoopRef runLoop, LFDateRef date)
{    
    LFRuntimeLock(runLoop);
	__LFRunLoopRunTimers(runLoop, date);
	__LFRunLoopRunSources(runLoop, date);
    LFRuntimeUnlock(runLoop);
}



void LFRunLoopStep(LFRunLoopRef runLoop)
{
	LFDateRef date = LFDateCreate();
	__LFRunLoopDoStep(runLoop, date);
	LFRelease(date);
}

void LFRunLoopRunUntilDate(LFRunLoopRef runLoop, LFDateRef date)
{
    LFRuntimeLock(date);
    
    if(LFZombieScribble(date, "LFRunLoopRunUntilDate") || !LFRuntimeValidate(date, LFDateGetTypeID()))
    {
        LFRuntimeUnlock(date);
        return;
    }
    
    LFDateRef endDate = LFCopy(date);
    LFDateRef currentDate = LFDateCreate();
    
    LFRuntimeUnlock(date);
    
	runLoop->stopped = 0;
	
	while(1)
	{
		
		if(LFDateCompare(currentDate, endDate) >= kLFCompareEqualTo || runLoop->stopped)
		{
			LFRelease(currentDate);
			LFRelease(endDate);
            
			break;
		}
		
		__LFRunLoopDoStep(runLoop, currentDate);
		LFDateUpdateDate(currentDate);
	}
}

void LFRunLoopRun(LFRunLoopRef runLoop)
{	
	LFDateRef currentDate = LFDateCreate();

	runLoop->stopped = 0;
    
	while(!runLoop->stopped)
	{
		__LFRunLoopDoStep(runLoop, currentDate);
		LFDateUpdateDate(currentDate);
	}
	
	LFRelease(currentDate);
}

void LFRunLoopStop(LFRunLoopRef runLoop)
{
	runLoop->stopped = 1;
}




void LFRunLoopAddTimer(LFRunLoopRef runLoop, LFTimerRef _timer)
{
    LFRuntimeLock(_timer);
    LFRuntimeLock(runLoop);
    
    if(LFZombieScribble(_timer, "LFRunLoopAddTimer") || !LFRuntimeValidate(_timer, LFTimerGetTypeID()))
    {
        LFRuntimeUnlock(_timer);
        LFRuntimeUnlock(runLoop);
        
        return;
    }
    
	
	LFRetain(_timer);
	
	if(_timer->scheduledRunLoop && _timer->scheduledRunLoop != runLoop)
	{
		LFRunLoopRemoveTimer(_timer->scheduledRunLoop, _timer);
	}
	else
	if(_timer->scheduledRunLoop == runLoop)
	{
		LFRelease(_timer);
        
        LFRuntimeUnlock(_timer);
		LFRuntimeUnlock(runLoop);
		return;
	}
	
    _timer->scheduledRunLoop = runLoop;

	LFSetAddValue(runLoop->timers, _timer);
	LFRelease(_timer);
    
	LFRuntimeUnlock(runLoop);
    LFRuntimeUnlock(_timer);
}

void LFRunLoopRemoveTimer(LFRunLoopRef runLoop, LFTimerRef _timer)
{
	LFRuntimeLock(runLoop);
    LFRuntimeLock(_timer);
	
    if(LFZombieScribble(_timer, "LFRunLoopRemoveTimer") || !LFRuntimeValidate(_timer, LFTimerGetTypeID()))
    {
        LFRuntimeUnlock(_timer);
        LFRuntimeUnlock(runLoop);
        
        return;
    }
    
	if(LFSetContainsValue(runLoop->timers, _timer))
	{
		_timer->scheduledRunLoop = NULL;
		LFSetRemoveValue(runLoop->timers, _timer);
	}
	
    LFRuntimeUnlock(_timer);
	LFRuntimeUnlock(runLoop);
}



void LFRunLoopAddSource(LFRunLoopRef runLoop, LFRunLoopSourceRef source)
{
	LFRuntimeLock(runLoop);
    LFRuntimeLock(source);
    
    if(LFZombieScribble(source, "LFRunLoopAddSource") || !LFRuntimeValidate(source, LFRunLoopSourceGetTypeID()))
    {
        LFRuntimeUnlock(source);
        LFRuntimeUnlock(runLoop);
        
        return;
    }
    
	LFRetain(source);
	
	if(source->runLoop && source->runLoop != runLoop)
	{
		LFRunLoopRemoveSource(source->runLoop, source);
	}
	else
	if(source->runLoop == runLoop)
	{
		LFRelease(source);
        
        LFRuntimeUnlock(source);
		LFRuntimeUnlock(runLoop);
		return;
	}
	
    source->runLoop = runLoop;
	LFSetAddValue(runLoop->sources, source);
	LFRelease(source);
	
    LFRuntimeUnlock(source);
	LFRuntimeUnlock(runLoop);
}

void LFRunLoopRemoveSource(LFRunLoopRef runLoop, LFRunLoopSourceRef source)
{	
	LFRuntimeLock(runLoop);
	LFRuntimeLock(source);
    
    if(LFZombieScribble(source, "LFRunLoopRemoveSource") || !LFRuntimeValidate(source, LFRunLoopSourceGetTypeID()))
    {
        LFRuntimeUnlock(runLoop);
        LFRuntimeUnlock(source);
        
        return;
    }

    
	if(LFSetContainsValue(runLoop->sources, source))
	{
		source->runLoop = NULL;
		LFSetRemoveValue(runLoop->sources, source);
	}
	
	LFRuntimeUnlock(runLoop);
    LFRuntimeUnlock(source);
}

