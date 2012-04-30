//
//  LFLock.c
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

#include "LFLock.h"

void __LFLockInit(LFTypeRef ref);
void __LFLockDealloc(LFTypeRef ref);

static LFRuntimeClass __LFLockClass;
static LFTypeID __LFLockTypeID = kLFInvalidTypeID;
static LFLockRef __LFLockGlobalLock = NULL;

void __LFRuntimeInitLock()
{
	__LFLockClass.version = 1;
	__LFLockClass.name = "LFLock";
	
	__LFLockClass.init = __LFLockInit;
	__LFLockClass.copy = NULL;
	__LFLockClass.dealloc = __LFLockDealloc;
	__LFLockClass.equal = NULL;
	__LFLockClass.hash  = NULL;
	
	__LFLockClass.superClass = kLFInvalidTypeID;
	__LFLockClass.protocolBag = NULL;
	__LFLockTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFLockClass);
	
	__LFLockGlobalLock = LFLockCreate();
}

void __LFLockInit(LFTypeRef ref)
{
	LFLockRef lock = (LFLockRef)ref;
	lock->type = kLFLockTypeUnknown;
}

void __LFLockDealloc(LFTypeRef ref)
{
	LFLockRef lock = (LFLockRef)ref;
	
#ifdef LF_TARGET_OSX
	pthread_mutex_destroy(&lock->mutex);
	pthread_mutexattr_destroy(&lock->attribute);
#endif
#ifdef LF_TARGET_WIN32
	CloseHandle(lock->mutex);
#endif
}



LFTypeID LFLockGetTypeID()
{
	return __LFLockTypeID;
}

LFLockRef LFLockGetGlobalLock()
{
	return __LFLockGlobalLock;
}


LFLockRef LFLockCreate()
{
#ifdef LF_TARGET_WIN32
	return LFLockCreateRecursiveLock();
#endif
#ifdef LF_TARGET_OSX
	LFLockRef lock = LFRuntimeCreateInstance(__LFLockTypeID, sizeof(struct __LFLock));
	lock->type = kLFLockTypeNormal;
	
	pthread_mutexattr_init(&lock->attribute);
	pthread_mutexattr_settype(&lock->attribute, PTHREAD_MUTEX_NORMAL);
	pthread_mutex_init(&lock->mutex, &lock->attribute);
	
	return lock;
#endif
}

LFLockRef LFLockCreateRecursiveLock()
{
	LFLockRef lock = LFRuntimeCreateInstance(__LFLockTypeID, sizeof(struct __LFLock));
	lock->type = kLFLockTypeRecursive;

#ifdef LF_TARGET_OSX
	pthread_mutexattr_init(&lock->attribute);
	pthread_mutexattr_settype(&lock->attribute, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&lock->mutex, &lock->attribute);
#endif
#ifdef LF_TARGET_WIN32
	lock->mutex = CreateMutex(NULL, FALSE, NULL);
#endif
	
	return lock;
}

#ifndef NDEBUG
uint8_t LFNoLockZombieScribble(LFTypeRef ref, char *fnct);
#endif

void LFLockLock(LFLockRef lock)
{
	if(LFNoLockZombieScribble(lock, "LFLockLock"))
		return;
	
#ifdef LF_TARGET_OSX
	pthread_mutex_lock(&lock->mutex);
#endif
#ifdef LF_TARGET_WIN32
	WaitForSingleObject(lock->mutex, INFINITE);
#endif
}

void LFLockUnlock(LFLockRef lock)
{	
	if(LFNoLockZombieScribble(lock, "LFLockUnlock"))
		return;
	
#ifdef LF_TARGET_OSX
	pthread_mutex_unlock(&lock->mutex);
#endif
#ifdef LF_TARGET_WIN32
	ReleaseMutex(lock->mutex);
#endif
}

#ifdef LF_TARGET_LITEC
// Source: http://msdn.microsoft.com/en-us/library/ms819773.aspx
#define WAIT_TIMEOUT 258
#endif

uint8_t LFLockTryLock(LFLockRef lock)
{	
	if(LFNoLockZombieScribble(lock, "LFLockTryLock"))
		return 0;
	
#ifdef LF_TARGET_OSX
	int _result = pthread_mutex_trylock(&lock->mutex);
	return !(_result == EBUSY);
#endif
#ifdef LF_TARGET_WIN32
	DWORD _result = WaitForSingleObject(lock->mutex, 2);
	return !(_result == WAIT_TIMEOUT);
#endif
}
