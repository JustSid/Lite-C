//
//  LFArray.h
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

#ifndef _LFARRAY_H_
#define _LFARRAY_H_

/**
 * @defgroup LFArrayRef LFArrayRef
 * @{
 * @brief The LFArrayRef implements a mutable array.
 *
 * A LFArrayRef is a ordered collection of objects. It associates objects with an integer index starting at 0 and going to n-1 where n is the number of objects in the array.
 * When removing an object from an array, the following object(s) will automatically adjust their index so that there is no gap between the objects. You can retrieve objects by their
 * index. <br />
 * The LFArrayRef retains alls added objects and releases them once they get removed from the array (or the array gets deallocated). <br />
 * The LFarrayRef also implements the LFEnumerator protocol to allow fast object enumeration.
 */

#include "LFRuntime.h"
#include "LFBase.h"

/**
 * A list of callbacks that a LFArrayRef uses to handle objects.
 * @remark You must not set any callback to NULL!
 **/
typedef struct
{
#ifndef LF_TARGET_LITEC
	/**
	 * Invoked when the array retains the given object
	 * @sa LFRetain()
	 **/
    void *(*LFArrayRetainCallback)(void *object);
	/**
	 * Invoked when the array releases the given object
	 * @sa LFRelease()
	 **/
    void (*LFArrayReleaseCallback)(void *object);
	/**
	 * Invoked when the array asks if both objects are equal
	 * @sa LFEqual()
	 **/
    uint8_t (*LFArrayEqualCallback)(void *objecta, void *objectb);
	/**
	 * Invoked when the array asks for the hash of the object
	 * @sa LFHash()
	 **/
    LFHashCode (*LFArrayHashCallback)(void *object);
#else
	void *LFArrayRetainCallback(void *object);
	void LFArrayReleaseCallback(void *object);
	uint8_t LFArrayEqualCallback(void *objecta, void *objectb);
	LFHashCode LFArrayHashCallback(void *object);
#endif
} LFArrayCallbacks;

/**
 * A cached value in an array
 **/
struct __LFArrayCache
{
	/**
	 * The index of the object
	 **/
	LFIndex index;
	/**
	 * The associated object
	 **/
	void *value;
};

/**
 * Internal layout of an LFArrayRef
 **/
struct __LFArray
{
	LFRuntimeBase base;
	
	/**
	 * The number of objects in the array.
	 **/
	LFIndex count;
	/**
	 * The maximum number of objects that can be stored in the array without reallocation.
	 **/
	LFIndex capacity;
	/**
	 * The array of objects that the LFArrayRef wraps
	 **/
	void **values;
	
	/**
	 * Internal use only
	 **/
	uint32_t mutations;
	
	/**
	 * The array cache
	 **/
	struct __LFArrayCache cache[5];
	/**
	 * The callbacks of the array
	 **/
	LFArrayCallbacks callbacks;
};

/**
 * This type used as reference to LFArray 
 **/
typedef struct __LFArray* LFArrayRef;

/**
 * Returns the array type ID.
 * @remark The type ID can be used to create new instances.
 **/
LF_EXPORT LFTypeID LFArrayGetTypeID();

/**
 * Creates a new array with the given set of callbacks.
 * @param callbacks A set of callbacks or NULL if the array should use the default callbacks
 **/
LF_EXPORT LFArrayRef LFArrayCreate(LFArrayCallbacks *callbacks);
/**
 * Creates a new array with the given set of callbacks and the given capacity.
 * @param callbacks A set of callbacks or NULL if the array should use the default callbacks.
 * @remark The array will be initialized to hold at least as much objects as specified by capacity. The array will shrink down again to fit the real number of objects when you remove an object from the array!
 **/
LF_EXPORT LFArrayRef LFArrayCreateWithCapacity(LFArrayCallbacks *callbacks, uint32_t capacity);

/**
 * Adds the given object to the array
 * @remark If the array already contains the value, the function does nothing.
 **/
LF_EXPORT void LFArrayAddValue(LFArrayRef array, void *value);

/**
 * Adds all objects from the given array to the receiver.
 * @remark This operation is faster than iterating through the array and calling LFArrayAddValue() repeatedly.
 **/
LF_EXPORT void LFArrayAddValuesFromArray(LFArrayRef array, LFArrayRef otherArray);

/**
 * Inserts the given objcet the given index into the array. The object that was previously at the given index will be moved to make place for the new object.
 * @remark When the index is out of the arrays bounds, the function won't do anything!
 **/
LF_EXPORT void LFArrayInsertValueAtIndex(LFArrayRef array, void *value, LFIndex index);

/**
 * Removes the given object from the array.
 **/
LF_EXPORT void LFArrayRemoveValue(LFArrayRef array, void *value);
/**
 * Removes the object at the given index from the array
 * @remark If the index is out of the arrays bounds, the function won't do anything.
 **/
LF_EXPORT void LFArrayRemoveValueAtIndex(LFArrayRef array, LFIndex index);
/**
 * Removes all objects from the array.
 * @remark This is faster than iterating through the array and removing every object using LFArrayRemoveValue() or LFArrayRemoveValueAtIndex()!
 **/
LF_EXPORT void LFArrayRemoveAllValues(LFArrayRef array);

/**
 * Returns the object with the given index.
 @remark If the index is out of the arrays bounds, the function won't do anything!
 **/
LF_EXPORT void *LFArrayValueAtIndex(LFArrayRef array, LFIndex index);
/**
 * Returns the object with the highest index in the array, or NULL if the array is empty.
 **/
LF_EXPORT void *LFArrayGetLastValue(LFArrayRef array);
/**
 * Exchanges the objects with the given indexes.
 * @remark Both indexes must be inside the arrays bound!
 **/
LF_EXPORT void LFArrayExchangeValueAtIndex(LFArrayRef array, LFIndex indexa, LFIndex indexb);
/**
 * Returns the index of the given object
 * @return The index of the given object or kLFNotFound if the array doesn't contain the object
 **/
LF_EXPORT LFIndex LFArrayIndexOfValue(LFArrayRef array, void *value);
/**
 * Returns true if the array contains the given object, otherwise false
 **/
LF_EXPORT uint8_t LFArrayContainsValue(LFArrayRef array, void *value);
/**
 * Returns the number of objects in the array
 **/
LF_EXPORT uint32_t LFArrayCount(LFArrayRef array);


#ifndef LF_TARGET_LITEC
/**
 * Sorts the array using the given callback function.
 * @param comparisonCallback The callback that should be used by the array to determine the order of the two objects.
 **/
LF_EXPORT void LFArraySortUsingFunction(LFArrayRef array, LFComparisonResult (*comparisonCallback)(void *objecta, void *objectb));
#else
LF_EXPORT void LFArraySortUsingFunction(LFArrayRef array, void *comparisonCallback);
#endif

/**
 * @}
 **/

#endif
