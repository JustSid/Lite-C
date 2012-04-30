//
//  LFAutoreleasePool.h
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

#ifndef _LFAUTORELEASEPOOL_H_
#define _LFAUTORELEASEPOOL_H_

/**
 * @defgroup LFAutoreleasePoolRef LFAutoreleasePoolRef
 * @{
 * @brief The LFAutoreleasePoolRef is a special class that collects objects and releases them at a later point
 *
 * The LFAutoreleasePoolRef stores objects and sends them at a later time a release message. It is useful to relinquish the ownership of an object without risking
 * to shorten its lifetime. Usually the opposite is the case, in which a autorelease pool extends an objects lifetime.
 * <br />
 * <br />
 * You can add objects to an LFAutoreleasePool multiple times, for every time the object is added to the pool, it will receive an release call when the pool gets drained.
 * You usually use the LFAutorelease() function to add an object to the topmost pool, this relinqusihes your ownership of the object like a release call but keeps the object alive.
 * When the pool receives a LFAutoreleasePoolDrain() call or gets deallocated, it will send a release method to every stored object.
 * @sa LFAutoreleasePoolDrain()
 * @sa LFAutorelease()
 */

#include "LFBase.h"
#include "LFRuntime.h"

/**
 * @brief The autorelease pools layout
 *
 * While the internal layout of the LFAutoreleasePool class is open, you should neither depend on it nor alter it!
 * The layout might change at any time.
 **/
struct __LFAutoreleasePool
{
	LFRuntimeBase base;
	
	/**
	 * The previous top most pool
	 **/
	struct __LFAutoreleasePool *prev;	
	void *thread;
	
	/**
	 * The buffer holding the objects
	 **/
	LFTypeRef *objects;
	/**
	 * The number of objects in the buffer
	 **/
	LFIndex count;
	/**
	 * The capacity of the buffer
	 **/
	LFIndex capacity;
};

/**
 * The autorelease pool type
 **/
typedef struct __LFAutoreleasePool* LFAutoreleasePoolRef;

/**
 * Returns the type ID of the autorelease pool class. Can be used to create new instances.
 **/
LF_EXPORT LFTypeID LFAutoreleasePoolGetTypeID();

/**
 * Creates a new autorelease pool and makes it the topmost one
 **/
LF_EXPORT LFAutoreleasePoolRef LFAutoreleasePoolCreate();
/**
 * Returns the topmost autorelease pool.
 **/
LF_EXPORT LFAutoreleasePoolRef LFAutoreleasePoolGetCurrentPool();

/**
 * Sends a release message to every stored object and then clears the buffer.
 * @remark When you are doing heavy memory allocations repeatly in a loop, its often a good idea to wrap the loop in an pool and draining it at the end of every iteration to avoid memory spikes
 * @remark The pool is automatically drained on its deallocation.
 **/
LF_EXPORT void LFAutoreleasePoolDrain(LFAutoreleasePoolRef pool);
/**
 * Adds an object to the autorelease pool. For every time you add the object into the pool, it will receive a release message when the pool is drained.
 * @remark Usually you don't need to call this method but use LFAutorelease() which is slightly faster
 * @remark Adding an autorelease pool will raise in an assertion.
 * @sa LFAutoreleasePoolDrain()
 * @sa LFAutorelease()
 **/
LF_EXPORT void LFAutoreleasePoolAddObject(LFAutoreleasePoolRef pool, LFTypeRef object);

/**
 * Adds the object to the topmost autorelease pool
 * @return Returns the same object you passed, this is useful in nested function calls.
 **/
LF_EXPORT LFTypeRef LFAutorelease(LFTypeRef object);

/**
 * @}
 **/

#endif
