//
//  LFEnumerator.h
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

#ifndef _LFENUMERATOR_H_
#define _LFENUMERATOR_H_

/**
 * @defgroup LFEnumeratorRef LFEnumeratorRef
 * @{
 * @brief The LFEnumeratorRef object is a wrapper object for the LFEnumerator protocol.
 *
 * The LFEnumeratorRef can be used to enumerate over any object that implements the LFEnumerator protocol.
 * The class and protocol are mainly for the purpose to enumerate collection objects like the LFSetRef.
 **/

#include "LFBase.h"
#include "LFRuntime.h"
#include "LFProtocol.h"

/**
 * @cond
 **/
#define __kLFEnumeratorMaxItems 10
/**
 * @endcond
 **/

/**
 * The enumerator class layout
 * @remark When you implement the LFEnumerator protocol in one of your classes, you can alter a few member of this struct directly.
 **/
struct __LFEnumerator
{
	LFRuntimeBase base;
	
	/**
	 * The current state of the enumerator
	 * @remark You can read this value when implementing the LFEnumerator protocol to check if this is the first enumeration (the state is 0 then), but not alter it!
	 **/
	uint32_t state;
	/**
	 * Variables that can be used freely by the enumerated object
	 **/
	uint32_t extra[5];
	/**
	 * The number of objects in itemsPtr. Must be set by the enumerated object
	 **/
	uint32_t itemCount;
	/**
	 * Buffer which holds the enumerated objects
	 **/
	LFTypeRef itemsPtr[__kLFEnumeratorMaxItems];
	
	/**
	 * @cond
	 **/
	// INTERNAL STUFF!
	LFTypeRef __object;
	uint32_t __itemIndex;
	/**
	 * @endcond
	 **/
};

/**
 * The enumerator type
 **/
typedef struct __LFEnumerator* LFEnumeratorRef;

/**
 * Returns the type ID for the LFEnumerator class
 * @remark DON'T use this to create your own instances!
 **/
LF_EXPORT LFTypeID LFEnumeratorGetTypeID();

/**
 * Creates a new enumerator that works on the given object
 **/
LF_EXPORT LFEnumeratorRef LFEnumeratorCreate(LFTypeRef ref);
/**
 * Returns the next object from the enumerated object or NULL if all objects are enumerated.
 **/
LF_EXPORT LFTypeRef LFEnumeratorNextObject(LFEnumeratorRef enumerator);

/**
 * @}
 **/


/**
 * @defgroup LFEnumeratorProtocol LFEnumerator protocol
 * @{
 * @brief The LFEnumerator protocol is the protocol that must be implemented by objects that want to be enumeratable by the LFEnumeratorRef
 *
 * The LFEnumerator protocol currently defines only one function that must be implemented. The signature for this function looks like 
 * @code enumerate(LFTypeRef ref, LFEnumeratorRef enumerator, uint32_t maxCount)@endcode
 * and is invoked when the LFEnumeratorRef asks the object for the next objects.
 * You can save the last state of the enumeration in the LFEnumeratorRef's extra members. The maxCount argument is the number of arguments you can enumerate, every enumerated
 * object must be stored in the itemsPtr[] member of the enumerator. When you are done with enumerating, you must set the itemCount member of the enumerator to the number of enumerated
 * objects. If you set this to 0, you signal the enumerator that there are no more objects to enumerate!  
 **/

/**
 * Constant for the enumerate selector
 * @sa LFEnumeratorGetSelector()
 **/
#define kLFEnumeratorProtocolEnumerateWithObjects 1

/**
 * Returns the LFEnumerator protocol ID
 **/
LF_EXPORT LFProtocolID LFEnumeratorGetProtocolID();
/**
 * Returns the selector with the given ID
 * @sa kLFEnumeratorProtocolEnumerateWithObjects
 **/
LF_EXPORT LFProtocolSel LFEnumeratorGetSelector(uint8_t selector);

/**
 * @}
 **/

#endif
