//
//  LFCondition.c
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


#include "LFCondition.h"
#include "LFDebugInternal.h"

//
// The idea for conditions on Win32 comes from: http://www1.cse.wustl.edu/~schmidt/win32-cv-1.html
//
#ifdef LF_TARGET_WIN32
int pthread_cond_init(pthread_cond_t *cv, void *unused)
{
	cv->waiters_count_ = 0;
	cv->wait_generation_count_ = 0;
	cv->release_count_ = 0;
	
	// Create a manual-reset event.
	cv->event_ = CreateEvent (NULL,TRUE, FALSE, NULL);
}

void pthread_cond_destory(pthread_cond_t *cv)
{
	CloseHandle(cv->event_);
}

int pthread_cond_wait(pthread_cond_t *cv, void *external_mutex)
{
	// Avoid race conditions.
	EnterCriticalSection (&cv->waiters_count_lock_);
	
	// Increment count of waiters.
	cv->waiters_count_ ++;
	 
	// Store current generation in our activation record.
	int my_generation = cv->wait_generation_count_;
	
	LeaveCriticalSection (&cv->waiters_count_lock_);
	LeaveCriticalSection (external_mutex);
	
	while(1)
	{
		// Wait until the event is signaled.
		WaitForSingleObject (cv->event_, INFINITE);
		
		EnterCriticalSection (&cv->waiters_count_lock_);
		// Exit the loop when the <cv->event_> is signaled and
		// there are still waiting threads from this <wait_generation>
		// that haven't been released from this wait yet.
		int wait_done = cv->release_count_ > 0 && cv->wait_generation_count_ != my_generation;
		LeaveCriticalSection (&cv->waiters_count_lock_);
		
		if (wait_done)
			break;
	}
	
	EnterCriticalSection (external_mutex);
	EnterCriticalSection (&cv->waiters_count_lock_);
	cv->waiters_count_--;
	cv->release_count_--;
	int last_waiter = cv->release_count_ == 0;
	LeaveCriticalSection (&cv->waiters_count_lock_);
	
	if (last_waiter)
		// We're the last waiter to be notified, so reset the manual event.
		ResetEvent (cv->event_);
}

int pthread_cond_signal(pthread_cond_t *cv)
{
	EnterCriticalSection (&cv->waiters_count_lock_);
	if (cv->waiters_count_ > cv->release_count_) {
		SetEvent (cv->event_); // Signal the manual-reset event.
		cv->release_count_++;
		cv->wait_generation_count_++;
	}
	LeaveCriticalSection (&cv->waiters_count_lock_);
}

int pthread_cond_broadcast(pthread_cond_t *cv)
{
	EnterCriticalSection (&cv->waiters_count_lock_);
	if (cv->waiters_count_ > 0) {  
		SetEvent (cv->event_);
		// Release all the threads in this generation.
		cv->release_count_ = cv->waiters_count_;
		
		// Start a new generation.
		cv->wait_generation_count_++;
	}
	LeaveCriticalSection (&cv->waiters_count_lock_);
}
#endif

void __LFConditionInit(LFTypeRef ref);
void __LFConditionDealloc(LFTypeRef ref);

static LFRuntimeClass __LFConditionClass;
static LFTypeID __LFConditionTypeID = kLFInvalidTypeID;


void __LFRuntimeInitCondition()
{
	__LFConditionClass.version = 1;
	__LFConditionClass.name = "LFCondition";
	
	__LFConditionClass.init = __LFConditionInit;
	__LFConditionClass.copy = NULL;
	__LFConditionClass.dealloc = __LFConditionDealloc;
	__LFConditionClass.equal = NULL;
	__LFConditionClass.hash  = NULL;
	
	__LFConditionClass.superClass = kLFInvalidTypeID;
	__LFConditionClass.protocolBag = NULL;
	__LFConditionTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFConditionClass);
}


void __LFConditionInit(LFTypeRef ref)
{
	LFConditionRef condition = (LFConditionRef)ref;
	pthread_cond_init(&condition->conditon, NULL);
}

void __LFConditionDealloc(LFTypeRef ref)
{
	LFConditionRef condition = (LFConditionRef)ref;
	pthread_cond_destroy(&condition->conditon);
}


LFTypeID LFConditionGetTypeID()
{
	return __LFConditionTypeID;
}

LFConditionRef LFConditionCreate()
{
	LFConditionRef condition = LFRuntimeCreateInstance(__LFConditionTypeID, sizeof(struct __LFCondition));
	return condition;
}


void LFConditionWait(LFConditionRef condition, LFLockRef lock)
{
	if(LFNoLockZombieScribble(condition, "LFConditionWait"))
		return;
	
	pthread_cond_wait(&condition->conditon, &lock->mutex);
}

void LFConditionSignal(LFConditionRef condition)
{
	if(LFNoLockZombieScribble(condition, "LFConditionSignal"))
		return;
	
	pthread_cond_signal(&condition->conditon);
}

void LFConditionBroadcast(LFConditionRef condition)
{
	if(LFNoLockZombieScribble(condition, "LFConditionBroadcast"))
		return;
	
	pthread_cond_broadcast(&condition->conditon);
}
