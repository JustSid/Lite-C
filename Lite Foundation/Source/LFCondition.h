//
//  LFCondition.h
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

#ifndef _LFCONDITION_H_
#define _LFCONDITION_H_

/**
 * @defgroup LFConditionRef LFConditionRef
 * @{
 * @brief The LFConditionRef is a LFLockRef like object which also acts a checkpoint for threads.
 *
 * A LFConditionRef is a object which locks a thread from execution using a provided LFLockRef object. The thread will only execute further when the thread which locked the lock
 * signals the execution.
 **/

#include "LFRuntime.h"
#include "LFBase.h"
#include "LFLock.h"

#ifdef LF_TARGET_WIN32
typedef void* CRITICAL_SECTION;

typedef struct
{
	int waiters_count_;
	int release_count_;	
	int wait_generation_count_;
	
	CRITICAL_SECTION waiters_count_lock_;	
	HANDLE event_;
} pthread_cond_t;
#endif

/**
 * The conditions layout
 **/
struct __LFCondition
{
	LFRuntimeBase base;
	
	/**
	 * @cond
	 **/
	pthread_cond_t conditon;
	/**
	 * @endcond
	 **/
};

/**
 * The condition type
 **/
typedef struct __LFCondition* LFConditionRef;

LF_EXPORT LFTypeID LFConditionGetTypeID();

/**
 * Creates a new condition.
 **/
LF_EXPORT LFConditionRef LFConditionCreate();

/**
 * Waits if the lock is already obtained by a thread until the execution is signaled by the owning thread.
 **/
LF_EXPORT void LFConditionWait(LFConditionRef condition, LFLockRef lock);
/**
 * Signals one waiting thread to continue exection.
 **/
LF_EXPORT void LFConditionSignal(LFConditionRef condition);
/**
 * Signals all waiting threads to continue execution.
 **/
LF_EXPORT void LFConditionBroadcast(LFConditionRef condition);

/**
 * @}
 **/

#endif
