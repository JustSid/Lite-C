//
//  LFAutoreleasePool.c
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

#include "LFAutoreleasePool.h"
#include "LFInternal.h"
#include "LFThread.h"

//------------------------------
// Runtime functions
//------------------------------

#define LFAutoreleasePoolGetExtraFlags(ref) LFBitfieldGetValue(((LFRuntimeBase *)ref)->__info, 31, 15)
#define LFAutoreleasePoolSetExtraFlags(ref, flags) LFBitfieldSetValue(((LFRuntimeBase *)ref)->__info, 31, 15, flags)

void __LFAutoreleasePoolInit(LFTypeRef ref);
void __LFAutoreleasePoolDealloc(LFTypeRef ref);

static LFRuntimeClass __LFAutoreleasePoolClass;
static LFTypeID __LFAutoreleasePoolTypeID = kLFInvalidTypeID;


void __LFRuntimeInitAutoreleasePool()
{
	__LFAutoreleasePoolClass.version = 1;
	__LFAutoreleasePoolClass.name = "LFAutoreleasePool";
	
	__LFAutoreleasePoolClass.init = __LFAutoreleasePoolInit;
	__LFAutoreleasePoolClass.copy = NULL;
	__LFAutoreleasePoolClass.dealloc = __LFAutoreleasePoolDealloc;
	__LFAutoreleasePoolClass.equal = NULL;
	__LFAutoreleasePoolClass.hash  = NULL;
	
	__LFAutoreleasePoolClass.superClass = kLFInvalidTypeID;
	__LFAutoreleasePoolClass.protocolBag = NULL;
	__LFAutoreleasePoolTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFAutoreleasePoolClass);
}


void __LFAutoreleasePoolInit(LFTypeRef ref)
{
	LFThreadRef thread = LFThreadGetCurrentThread();
	
	LFAutoreleasePoolRef pool = (LFAutoreleasePoolRef)ref;
	pool->prev		= NULL;
	pool->thread	= NULL;
	pool->count		= 0;
	pool->capacity	= 10; // Initialize the internal array to store 10 objects. Its no problem if this fails now, but will raise an assertion later
	pool->objects	= (LFTypeRef *)malloc(pool->capacity * sizeof(LFTypeRef));
	
	if(thread)
	{
		pool->prev = thread->pool;
		pool->thread = thread;
		
		thread->pool = pool; // We are now the topmost pool
	}
	
	short flags = LFAutoreleasePoolGetExtraFlags(pool);
	
	LFBitSet(flags, kLFRuntimeNoRetainBit); // Mark the object as not retainable
	LFAutoreleasePoolSetExtraFlags(pool, flags);
}

void __LFAutoreleasePoolDealloc(LFTypeRef ref)
{
	LFAutoreleasePoolRef pool = (LFAutoreleasePoolRef)ref;
	LFThreadRef thread = (LFThreadRef)pool->thread;
	
	if(thread)	
	{
		// When starting up the LFThreads, there is no thread associated with the very first pools so we skip pools without a thread
		LFAssert(thread->pool == pool, "Trying to release a non topmost autorelease pool is not supported!");
		thread->pool = pool->prev;
	}
	
	if(pool->count > 0)
	{
		LFIndex index;
		
		for(index=0; index<pool->count; index++)
			LFRelease((pool->objects)[index]);
	}
	
	free(pool->objects);
}


//------------------------------
// Implementation
//------------------------------

LFTypeID LFAutoreleasePoolGetTypeID()
{
	return __LFAutoreleasePoolTypeID;
}


LFAutoreleasePoolRef LFAutoreleasePoolCreate()
{
	LFAutoreleasePoolRef pool = LFRuntimeCreateInstance(__LFAutoreleasePoolTypeID, sizeof(struct __LFAutoreleasePool));
	return pool;
}

LFAutoreleasePoolRef LFAutoreleasePoolGetCurrentPool()
{
	LFThreadRef thread = LFThreadGetCurrentThread();
	return (LFAutoreleasePoolRef)thread->pool;
}



uint8_t __LFAutoreleasePoolTryResize(LFAutoreleasePoolRef pool, LFIndex newSize)
{
	LFAssert(pool->objects != NULL, "Trying to resize an unallocated buffer!");
	
	LFTypeRef *tempObjects = (LFTypeRef *)realloc(pool->objects, newSize * sizeof(LFTypeRef));
	if(tempObjects)
	{
		pool->objects = tempObjects;
		return 1;
	}
	
	return 0;
}

void LFAutoreleasePoolDrain(LFAutoreleasePoolRef pool)
{
	if(LFZombieScribble(pool, "LFAutoreleasePoolDrain") || !LFRuntimeValidate(pool, __LFAutoreleasePoolTypeID))
		return;
	
	LFIndex index;
	for(index=0; index<pool->count; index++)
		LFRelease((pool->objects)[index]); 
	
	pool->count = 0;
	pool->capacity = 10;
	
	LFAssert(__LFAutoreleasePoolTryResize(pool, pool->capacity), "Failed to resize autorelease pool!");
}

void LFAutoreleasePoolAddObject(LFAutoreleasePoolRef pool, LFTypeRef object)
{
	if(!object)
		return;
	
	LFAssert(LFGetTypeID(object) != __LFAutoreleasePoolTypeID, "Adding autorelease pools to autorelease pools is unsupported!");
	
	if(LFZombieScribble(pool, "LFAutoreleasePoolAddObject") || !LFRuntimeValidate(pool, __LFAutoreleasePoolTypeID))
		return;
	
	if(pool->count >= pool->capacity)
	{
		pool->capacity += 15;
		LFAssert(__LFAutoreleasePoolTryResize(pool, pool->capacity), "Failed to resize autorelease pool!");
	}
	
	(pool->objects)[pool->count] = object;
	pool->count ++;
}



LFTypeRef LFAutorelease(LFTypeRef object)
{
	LFThreadRef thread = LFThreadGetCurrentThread();
	LFAssert(thread->pool, "Calling LFAutorelease with no pool in place. The object will be leaked!");
	
	if(!object)
		return NULL;
	
	LFAutoreleasePoolAddObject(thread->pool, object);
	return object;
}
