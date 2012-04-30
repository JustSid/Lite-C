//
//  LFDictionary.h
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

#ifndef _LFDictionary_H_
#define _LFDictionary_H_

/**
 * @defgroup LFDictionaryRef LFDictionaryRef
 * @{
 * @brief The LFDictionaryRef manages an unorderec collection of objects
 *
 * The LFDictionaryRef is an unordered collection of objects with the same performance and characteristica of an LFSetRef, but instead of using the value as key, a LFDictionaryRe
 * allows you to define your very own key for each value.
 **/

#include "LFBase.h"
#include "LFRuntime.h"
#include "LFEnumerator.h"

typedef struct
{
#ifndef LF_TARGET_LITEC
	/**
	 * Function used to retain a value
	 * @sa LFRetain
	 **/
    void *(*LFDictionaryValueRetainCallback)(void *object);
	/**
	 * Function used to release a value
	 * @sa LFRelease
	 **/
    void (*LFDictionaryValueReleaseCallback)(void *object);
	
	/**
	 * Function used to retain a key
	 * @sa LFRetain
	 **/
	void *(*LFDictionaryKeyRetainCallback)(void *object);
	/**
	 * Function used to release a key
	 * @sa LFRelease
	 **/
    void (*LFDictionaryKeyReleaseCallback)(void *object);
	/**
	 * Function used to identifie two keys as equal
	 * @sa LFEqual
	 **/
	uint8_t (*LFDictionaryKeyEqualCallback)(void *objecta, void *objectb);
	/**
	 * Function used hash a key
	 * @sa LFHash
	 **/
    LFHashCode (*LFDictionaryKeyHashCallback)(void *object);
#else
	void *LFDictionaryValueRetainCallback(void *object);
	void LFDictionaryValueReleaseCallback(void *object);
	
	void *LFDictionaryKeyRetainCallback(void *object);
	void LFDictionaryKeyReleaseCallback(void *object);
	uint8_t LFDictionaryKeyEqualCallback(void *objecta, void *objectb);
	LFHashCode LFDictionaryKeyHashCallback(void *object);
#endif
} LFDictionaryCallbacks;


/**
 * @cond
 **/
struct __LFDictionaryBucket
{
	void *key;
	void *value;
	
	struct __LFDictionaryBucket *next;
};
/**
 * @endcond
 **/

/**
 * The LFDictionary's internal layout
 **/
struct __LFDictionary
{
	LFRuntimeBase base;
	
	/**
	 * Number of objects
	 **/
	LFIndex count;
	/**
	 * Number of buckets
	 **/
	LFIndex bucketCount;
	/**
	 * Internal use for the LFEnumerator
	 **/
	uint32_t mutations;
	/**
	 * Callbacks for the dictionary
	 **/
	LFDictionaryCallbacks callbacks;
	/**
	 * Pointer to the bucket array
	 **/
	struct __LFDictionaryBucket **buckets;
};

/**
 * The dictionary type.
 **/
typedef struct __LFDictionary* LFDictionaryRef;


/**
 * Returns the type ID for the LFDictionary class
 * @remark DON'T use this to create your own instances!
 **/
LF_EXPORT LFTypeID LFDictionaryGetTypeID();


/**
 * Creates a new dictionary with the given callbacks.
 **/
LF_EXPORT LFDictionaryRef LFDictionaryCreate(LFDictionaryCallbacks *callbacks);
/**
 * Sets the given value for the given key.
 * @remark If the value is NULL, the function does the same as calling LFDictionaryRemoveValueForKey() with the key
 **/
LF_EXPORT void LFDictionarySetValueForKey(LFDictionaryRef dict, void *value, void *key);
/**
 * Removes the value associated with the given key from the dictionary
 **/
LF_EXPORT void LFDictionaryRemoveValueForKey(LFDictionaryRef dict, void *key);
/**
 * Removes all values from the dictionary
 **/
LF_EXPORT void LFDictionaryRemoveAllValues(LFDictionaryRef dict);
/**
 * Returns the value associated with the given key, or NULL
 **/
LF_EXPORT void *LFDictionaryValueForKey(LFDictionaryRef dict, void *key);
/**
 * Returns the number of values in the dictionary
 **/
LF_EXPORT uint32_t LFDictionaryCount(LFDictionaryRef dict);

/**
 * Returns an autoreleased enumerator which can be used to enumerate through the keys of the dictionary
 **/
LF_EXPORT LFEnumeratorRef LFDictionaryKeyEnumerator(LFDictionaryRef dict);
/**
 * Returns an autoreleased enumerator which can be used to enumerate through the values of the dictionary
 **/
LF_EXPORT LFEnumeratorRef LFDictionaryValueEnumerator(LFDictionaryRef dict);

/**
 * @}
 **/

#endif
