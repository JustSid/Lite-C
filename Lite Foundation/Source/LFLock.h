//
//  LFLock.h
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

#ifndef _LFLOCK_H_
#define _LFLOCK_H_

/**
 * @defgroup LFLockRef LFLockRef
 * @{
 * @brief The LFLockRef is an object that allows one to synchronize multiple threads
 *
 * A LFLockRef can be used to synchronize multiple threads that need to access the same resource by allowing only one thread to execute some code at a time.
 * The LFLockRef has two states, locked and unlocked. If a thread wants to execute some code, it must first obtain the lock by locking it. If the lock is already locked,
 * the thread will wait until the owning thread unlocks the lock.
 *
 * @sa LFConditionRef
 * @sa LFThreadRef
 **/

#include "LFRuntime.h"
#include "LFBase.h"

/**
 * The LFLockRef layout
 **/
struct __LFLock
{
	LFRuntimeBase base;
	
	/**
	 * @cond
	 **/
#ifdef LF_TARGET_OSX
	pthread_mutex_t mutex;
	pthread_mutexattr_t attribute;
#endif
#ifdef LF_TARGET_WIN32
	HANDLE mutex;
#endif
	
	/**
	 * @endcond
	 **/
	
	/**
	 * Type of the lock.
	 * @sa kLFLockTypeUnknown
	 * @sa kLFLockTypeNormal
	 * @sa kLFLockTypeRecursive
	 **/
	uint8_t type;
};

/**
 * Lock type that is assigned to unitialized locks. You will usually never get a lock with this type
 **/
#define kLFLockTypeUnknown		0
/**
 * Type for a normal lock. A normal lock is a lock that can be locked once by a thread.
 * @remark This type is only available on POSIX conform operating systems!
 **/
#define kLFLockTypeNormal		1
/**
 * Type for a recursive lock. A recursive lock can be obtained multiple times by the very same thread.
 **/
#define kLFLockTypeRecursive	2

/**
 * The lock type.
 **/
typedef struct __LFLock* LFLockRef;

/**
 * Returns the type ID of the lock. DON'T use this to create your own locks, they won't work properly!
 **/
LF_EXPORT LFTypeID LFLockGetTypeID();

/**
 * Creates a new, normal, lock.
 * @remark Normal locks are only supported by POSIX conform operating systems, on non POSIX conform operating systems (eg. Windows), the function will create a recursive lock!
 **/
LF_EXPORT LFLockRef LFLockCreate();
/**
 * Creates a new recursive lock.
 **/
LF_EXPORT LFLockRef LFLockCreateRecursiveLock();

/**
 * Locks the given lock.
 * @remark If the lock is already locked, the calling thread will wait until the lock is unlocked.
 **/
LF_EXPORT void LFLockLock(LFLockRef lock);
/**
 * Unlocks the given lock.
 * @remark A lock must only be unlocked by a thread that locked it.
 **/
LF_EXPORT void LFLockUnlock(LFLockRef lock);
/**
 * Tries to lock the lock and if the lock is already locked, the function will return 0.
 * @return 1 if the lock could be obtained, otherwise 0.
 **/
LF_EXPORT uint8_t LFLockTryLock(LFLockRef lock);

/**
 * @}
 **/

#endif
