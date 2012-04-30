//
//  LFSet.h
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

#ifndef _LFSET_H_
#define _LFSET_H_

/**
 * @defgroup LFSetRef LFSetRef
 * @{
 * @brief The LFSetRef can collect objects in unordered order
 *
 * A LFSetRef is internally implemented as hash table based on primes and can story Lite Foundation objects or any other object inside of it.
 * You can enumerate through an LFSet by using an LFEnumeratorRef object, otherwise you can access any object from a set by using the LFSetGetAnyObject() function.
 * Sets can be mutated using the LFSetAddValue(), LFSetRemoveValue() and LFSetRemoveAllValues() functions, however, you must not mutate a set while enumerating it!
 * <br />
 * A set will invoke the retain callback defined for the set on any added object, and the release callback for every remove object.
 **/

#include "LFBase.h"
#include "LFRuntime.h"

/**
 * A list of callbacks that a LFSetRef can use
 * @remark You must not set any callback to NULL!
 **/
typedef struct
{
#ifndef LF_TARGET_LITEC
	/**
	 * Invoked when the set retains the given object
	 * @sa LFRetain()
	 **/
    void *(*LFSetRetainCallback)(void *object);
	/**
	 * Invoked when the set releases the given object
	 * @sa LFRelease()
	 **/
    void (*LFSetReleaseCallback)(void *object);
	/**
	 * Invoked when the set asks if both objects are equal
	 * @sa LFEqual()
	 **/
    uint8_t (*LFSetEqualCallback)(void *objecta, void *objectb);
	/**
	 * Invoked when the set asks for the hash of the object
	 * @sa LFHash()
	 **/
    LFHashCode (*LFSetHashCallback)(void *object);
#else
	void *LFSetRetainCallback(void *object);
   void LFSetReleaseCallback(void *object);
   uint8_t LFSetEqualCallback(void *objecta, void *objectb);
   LFHashCode LFSetHashCallback(void *object);
#endif
} LFSetCallbacks;


/**
 * @cond
 **/
struct __LFSetBucket
{
	void *key;
	
	struct __LFSetBucket *next;
};
/**
 * @endcond
 **/

/**
 * The LFSets internal layout
 * @remark DO NOT alter a set directly!
 **/
struct __LFSet
{
	LFRuntimeBase base;
	
	/**
	 * Number of objects
	 **/
	LFIndex count;
	/**
	 * Number of buckets in the set
	 **/
	LFIndex bucketCount;
	/**
	 * Internal use for enumeration
	 **/
	uint32_t mutations;
	
	/**
	 * The callbacks defined for the set
	 **/
	LFSetCallbacks *callbacks;
	/**
	 * The buckets
	 **/
	struct __LFSetBucket **buckets;
};

/**
 * The set type
 **/
typedef struct __LFSet* LFSetRef;


/**
 * Returns the type ID for the LFSet class
 * @remark DON'T use this to create your own instances!
 **/
LF_EXPORT LFTypeID LFSetGetTypeID();


/**
 * Creates a new set using the given callbacks
 * @param callbacks You can either pass your own callbacks or pass NULL to force the set to use the LFRetain(), LFRelease(), LFHash() and LFEqual() functions as callbacks
 * @remark If you want to add non Lite Foundation types into the set, you MUST pass your own callbacks!
 **/
LF_EXPORT LFSetRef LFSetCreate(LFSetCallbacks *callbacks);

/**
 * Adds a value to the set
 * @remark A set can hold a value only one time!
 * @remark The set will retain the value
 **/
LF_EXPORT void LFSetAddValue(LFSetRef _set, void *value);
/**
 * Removes the value from the set
 * @remark If the set contained the value, it will release it before removing it from the set
 **/
LF_EXPORT void LFSetRemoveValue(LFSetRef _set, void *value);
/**
 * Removes all values from the set
 * @remark The set will release all values before removing them.
 **/
LF_EXPORT void LFSetRemoveAllValues(LFSetRef _set);

/**
 * Returns any object from the set
 * @remark The object will not be random and calling the function multiple times on a non mutated set will always return the same object!
 **/
LF_EXPORT void *LFSetGetAnyObject(LFSetRef _set);

/**
 * Returns true if the set contains the given value, otherwise false.
 **/
LF_EXPORT uint8_t LFSetContainsValue(LFSetRef _set, void *value);

/**
 * Returns the number of objects in the set
 **/
LF_EXPORT uint32_t LFSetCount(LFSetRef _set);

/**
 * @}
 **/

#endif
