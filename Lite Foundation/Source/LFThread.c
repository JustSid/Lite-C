//
//  LFThread.c
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

#include "LFThread.h"
#include "LFSet.h"
#include "LFEnumerator.h"
#include "LFRunLoopSource.h"

static char __LFThreadMultithreadedEnvironment = 0;

void __LFThreadInit(LFTypeRef ref);
void __LFThreadDealloc(LFTypeRef ref);

static LFRuntimeClass __LFThreadClass;
static LFTypeID __LFThreadTypeID = kLFInvalidTypeID;
static LFSetRef __LFThreadPool = NULL;
static LFLockRef __LFThreadLock = NULL; // NOTE: Don't spend too much time waiting for this lock because it might block the whole runtime and main thread!

static LFThreadRef __LFThreadMainThread = NULL;
static uint32_t __LFThreadThreadCount = 0;

void __LFRuntimeInitThread()
{
	__LFThreadClass.version = 1;
	__LFThreadClass.name = "LFThread";
	
	__LFThreadClass.init = __LFThreadInit;
	__LFThreadClass.copy = NULL;
	__LFThreadClass.dealloc = __LFThreadDealloc;
	__LFThreadClass.equal = NULL;
	__LFThreadClass.hash  = NULL;
	
	__LFThreadClass.superClass = kLFInvalidTypeID;
	__LFThreadClass.protocolBag = NULL;
	__LFThreadTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFThreadClass);
	
	__LFThreadPool = LFSetCreate(NULL);
	__LFThreadLock = LFLockCreate();
	
	// Create a wrapper for the main thread and initialize it
	__LFThreadMainThread = LFRuntimeCreateInstance(__LFThreadTypeID, sizeof(struct __LFThread));
	LFRelease(__LFThreadMainThread->name);
	
#ifdef LF_TARGET_OSX
	__LFThreadMainThread->thread = pthread_self();
#else
	__LFThreadMainThread->threadID = GetCurrentThreadId();
#endif
	__LFThreadMainThread->state = kLFThreadStateExecuting;
	__LFThreadMainThread->name = LFStringCreateWithCString("Main Thread");

	LFSetAddValue(__LFThreadPool, __LFThreadMainThread);
}

#ifdef LF_TARGET_LITEC
int __cdecl sprintf(char *, const char *, ...);
#endif

LFRunLoopRef __LFRunLoopCreate();

void __LFThreadInit(LFTypeRef ref)
{
	LFThreadRef thread = (LFThreadRef)ref;
	
	thread->thread = 0;
	thread->state = kLFThreadStatePending;
    thread->idle = 0;
	
	thread->entry = NULL;
	thread->callback = NULL;
    thread->callbackThread = NULL;
	
	thread->runLoop = __LFRunLoopCreate();
	thread->pool = NULL;
	
	thread->info = NULL;
	
	
	LFLockLock(__LFThreadLock);
	
	char buffer[32];
	sprintf(buffer, "Thread %i", __LFThreadThreadCount);
	
	thread->name = LFStringCreateWithCString(buffer);	
	
	__LFThreadThreadCount ++;
	LFLockUnlock(__LFThreadLock);
}

void __LFThreadDealloc(LFTypeRef ref)
{
	LFThreadRef thread = (LFThreadRef)ref;	
	
	LFRelease(thread->callbackThread);
	LFRelease(thread->pool);
	
	LFRelease(thread->name);
	LFRelease(thread->info);
}


uint8_t LFThreadIsMultithreaded()
{
	return __LFThreadMultithreadedEnvironment;	
}



LFThreadRef LFThreadCreate(LFThreadEntry entry)
{
	if(!entry)
		return NULL;
	
	LFThreadRef thread = LFRuntimeCreateInstance(__LFThreadTypeID, sizeof(struct __LFThread));
    
	if(thread)
        thread->entry = entry;
	
	return thread;
}

LFThreadRef LFThread(LFThreadEntry entry)
{
	LFThreadRef thread = LFThreadCreate(entry);
	LFThreadStart(thread);
    
	return LFAutorelease(thread);
}

LFThreadRef LFThreadGetCurrentThread()
{
	if(__LFThreadMultithreadedEnvironment == 0)
		return __LFThreadMainThread;
	
	LFLockLock(__LFThreadLock);

#ifdef LF_TARGET_OSX
	pthread_t pthread = pthread_self();
#else
	DWORD threadID = GetCurrentThreadId();
#endif
	
	LFEnumeratorRef enumerator = LFEnumeratorCreate(__LFThreadPool);
	LFThreadRef thread;
	
	while((thread = LFEnumeratorNextObject(enumerator)))
	{
#ifdef LF_TARGET_OSX
		if(pthread_equal(pthread, thread->thread))
#else
		if(threadID == thread->threadID)
#endif
		{
			LFRelease(enumerator);
			LFLockUnlock(__LFThreadLock);
			
			return thread;
		}
	}
	
	// The thread couldn't be found :(
	LFRelease(enumerator);
	LFLockUnlock(__LFThreadLock);
	
	return NULL;
}

LFThreadRef LFThreadGetMainThread()
{
	return __LFThreadMainThread;
}


void __LFThreadRunLoopSource(LFRunLoopSourceRef source)
{
	LFThreadRef thread = source->data;
	
#ifndef LF_TARGET_LITEC
	LFThreadCallback callback = thread->callback;
	callback(thread);
#else
	void __LFThreadGenericCallback(void *param);
	
	__LFThreadGenericCallback = thread->callback;
	__LFThreadGenericCallback(thread);
#endif
	
    thread->idle = 0;
	LFRelease(thread);
}

void *__LFThreadEntry(void *_thread)
{
	LFThreadRef thread = (LFThreadRef)_thread;
	LFRuntimeLock(thread);
	
	LFLockLock(__LFThreadLock);
	LFSetAddValue(__LFThreadPool, thread);
	LFLockUnlock(__LFThreadLock);
	
	LFRuntimeUnlock(thread);
	
	
#ifndef LF_TARGET_LITEC
	LFThreadEntry entry = (LFThreadEntry)thread->entry;
	entry(thread);
#else
	void __LFThreadGenericCallback(void *param);
	
	__LFThreadGenericCallback = thread->entry;
	__LFThreadGenericCallback(thread);
#endif
	
	
	LFRuntimeLock(thread);
	thread->state = kLFThreadStateFinished;
	
	if(thread->callback)
	{
		LFDateRef date = LFDateCreate();
		LFRunLoopSourceRef source = LFRunLoopSourceCreateWithDate(__LFThreadRunLoopSource, date, date, LFRetain(thread));
		LFRunLoopAddSource(thread->callbackThread->runLoop, source);
		
		LFRelease(date);
		LFRelease(source);
	}
	
	LFLockLock(__LFThreadLock);
	LFSetRemoveValue(__LFThreadPool, thread);
	LFLockUnlock(__LFThreadLock);
	
	LFRuntimeUnlock(thread);
	LFRelease(thread);
	
	return NULL;
}

void LFThreadStart(LFThreadRef thread)
{
    LFRuntimeLock(thread);
    
	if(LFZombieScribble(thread, "LFThreadStart") || !LFRuntimeValidate(thread, __LFThreadTypeID))
    {
        LFRuntimeUnlock(thread);
		return;
    }
	
	// Assuming that the only write access is made from a single threaded environment, there is no need for a lock as the write access is atomic in this case
    __LFThreadMultithreadedEnvironment = 1;
	
	if(thread->state == kLFThreadStatePending)
	{
#ifdef LF_TARGET_OSX
		int _result = pthread_create(&thread->thread, NULL, __LFThreadEntry, thread);
		if(_result == 0)
#endif
#ifdef LF_TARGET_WIN32
		thread->thread = CreateThread(NULL, 0, __LFThreadEntry, thread, 0, &thread->threadID);
		if(thread->thread != NULL)
#endif
		{
			// No matter what happens, this runs _BEFORE_ the thread calls its actual entry point!
			thread->state = kLFThreadStateExecuting;
			LFRetain(thread);
		}
	}
	
	LFRuntimeUnlock(thread);
}

void LFThreadJoin(LFThreadRef thread)
{
    LFRuntimeLock(thread);
    
	if(LFZombieScribble(thread, "LFThreadJoin") || !LFRuntimeValidate(thread, __LFThreadTypeID))
    {
        LFRuntimeUnlock(thread);
		return;
    }
	
	if(thread->state == kLFThreadStateExecuting)
	{
		LFRuntimeUnlock(thread);
        
        
#ifdef LF_TARGET_OSX
		pthread_join(thread->thread, NULL);
#else
		WaitForSingleObject(thread->thread, INFINITE);
#endif
	}
	
    LFRuntimeUnlock(thread);
}


void LFThreadSetName(LFThreadRef thread, LFStringRef name)
{
    LFRuntimeLock(name);
    LFRuntimeLock(thread);

	if(LFZombieScribble(thread, "LFThreadSetName") || LFZombieScribble(name, "LFThreadSetName") || 
       !LFRuntimeValidate(thread, __LFThreadTypeID) || !LFRuntimeValidate(name, LFStringGetTypeID()))
    {
        LFRuntimeUnlock(thread);
        LFRuntimeUnlock(name);
		return;
    }
	
	LFRelease(thread->name);
	thread->name = LFCopy(name);
	
	LFRuntimeUnlock(thread);
    LFRuntimeUnlock(name);
}

LFStringRef LFThreadGetName(LFThreadRef thread)
{
    LFRuntimeLock(thread);
    
	if(LFZombieScribble(thread, "LFThreadGetName") || !LFRuntimeValidate(thread, __LFThreadTypeID))
    {
        LFRuntimeUnlock(thread);
        
		return NULL;
    }
	
	LFStringRef name = LFCopy(thread->name);
	LFAutorelease(name);
	
	LFRuntimeUnlock(thread);
	return name;
}

LFStringRef LFThreadCopyName(LFThreadRef thread)
{
    LFRuntimeLock(thread);
    
	if(LFZombieScribble(thread, "LFThreadCopyName") || !LFRuntimeValidate(thread, __LFThreadTypeID))
    {
        LFRuntimeUnlock(thread);
		return NULL;
    }
	
	LFStringRef name = LFCopy(thread->name);	
	LFRuntimeUnlock(thread);
	
	return name;
}



void LFThreadSetDictionary(LFThreadRef thread, LFDictionaryRef dictionary)
{
    LFRuntimeLock(thread);
    LFRuntimeLock(dictionary);
    
	if(LFZombieScribble(thread, "LFThreadSetDictionary") || LFZombieScribble(dictionary, "LFThreadSetDictionary") ||
       !LFRuntimeValidate(thread, __LFThreadTypeID) || !LFRuntimeValidate(dictionary, LFDictionaryGetTypeID()))
    {
        LFRuntimeUnlock(thread);
        LFRuntimeUnlock(dictionary);
		return;
    }
	
	
	LFRelease(thread->info);
	thread->info = LFCopy(dictionary);
	
	LFRuntimeUnlock(thread);
    LFRuntimeUnlock(dictionary);
}

LFDictionaryRef LFThreadGetDictionary(LFThreadRef thread)
{
    LFRuntimeLock(thread);
    
	if(LFZombieScribble(thread, "LFThreadGetDictionary") || !LFRuntimeValidate(thread, __LFThreadTypeID))
    {
        LFRuntimeUnlock(thread);
		return NULL;
    }
	
	LFDictionaryRef dictionary = LFCopy(thread->info); // For sake of atomically we create a straight copy of the dictionary
	LFAutorelease(dictionary);
	
	LFRuntimeUnlock(thread);	
	return dictionary;
}



void LFThreadSetCallback(LFThreadRef thread, LFThreadCallback callback, LFThreadRef callbackThread)
{
    LFRuntimeLock(thread);
    LFRuntimeLock(callbackThread);
    
	if(LFZombieScribble(thread, "LFThreadSetCallback") || LFZombieScribble(callbackThread, "LFThreadSetCallback") ||
       !LFRuntimeValidate(thread, __LFThreadTypeID) || !LFRuntimeValidate(callbackThread, __LFThreadTypeID))
    {
        LFRuntimeUnlock(thread);
        LFRuntimeUnlock(callbackThread);
        
		return;
    }
	

	thread->callback = callback;
	thread->callbackThread = LFRetain(callbackThread);
	
	LFRuntimeUnlock(thread);
    LFRuntimeUnlock(callbackThread);
}

uint8_t LFThreadIsCancelled(LFThreadRef thread)
{
    LFRuntimeLock(thread);
    
	if(LFZombieScribble(thread, "LFThreadIsCancelled") || !LFRuntimeValidate(thread, __LFThreadTypeID))
    {
        LFRuntimeUnlock(thread);
		return 1;
    }
	
	uint8_t state = thread->state;

    LFRuntimeUnlock(thread);
	return (state == kLFThreadStateCancelled);
}


// Message forwarding
struct LFThreadInvocation
{
	LFThreadGenericCallback callback;
	void *parameter;
};

void LFThreadForwardInvocation(LFRunLoopSourceRef source)
{
	struct LFThreadInvocation *invocation = (struct LFThreadInvocation *)source->data;
#ifndef LF_TARGET_LITEC
	invocation->callback(invocation->parameter);
#else
	void LFInvocationCallback(void *param);
	
	LFInvocationCallback = invocation->callback;
	LFInvocationCallback(invocation->parameter);
#endif
	
	free(invocation); // We are responsible for cleaning the invocation
}



void LFThreadInvokeFunction(LFThreadRef thread, LFThreadGenericCallback fnct, void *parameter, uint8_t waitForInvocation)
{
    LFRuntimeLock(thread);
    
	if(LFZombieScribble(thread, "LFThreadPerformFunction") || !LFRuntimeValidate(thread, __LFThreadTypeID))
    {
        LFRuntimeUnlock(thread);
		return;
    }
	
	
	if(thread->state == kLFThreadStateExecuting)
	{
		if(thread == LFThreadGetCurrentThread())
		{
#ifndef LF_TARGET_LITEC
			fnct(parameter);
#else
            void __LFThreadGenericCallback(void *param);
            __LFThreadGenericCallback = fnct;
            __LFThreadGenericCallback(parameter);
#endif
			return;
		}
		
        if(waitForInvocation)
            thread->idle = 1;
        
		// Create and forward the invocation
		struct LFThreadInvocation *invocation = (struct LFThreadInvocation *)malloc(sizeof(struct LFThreadInvocation));
		invocation->callback = fnct;
		invocation->parameter = parameter;
		
		LFDateRef date = LFDateCreate();
		LFRunLoopSourceRef source = LFRunLoopSourceCreateWithDate(LFThreadForwardInvocation, date, date, invocation);
		LFRunLoopAddSource(thread->runLoop, source);
		
		LFRelease(date);
		LFRelease(source);
        LFRuntimeUnlock(thread);
        
        while(thread->idle)
        {
        }
	}
	else
        LFRuntimeUnlock(thread);
}

LFRunLoopRef LFThreadGetRunLoop(LFThreadRef thread)
{
    LFRuntimeLock(thread);
    
	if(LFZombieScribble(thread, "LFThreadGetRunLoop") || !LFRuntimeValidate(thread, __LFThreadTypeID))
		return NULL;
	
	LFRunLoopRef runLoop = NULL;
	if(thread->state == kLFThreadStateExecuting)
	{
		runLoop = thread->runLoop;
	}
	
	LFRuntimeUnlock(thread);
	return runLoop;
}

