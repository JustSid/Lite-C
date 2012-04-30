//
//  LFRunLoopSource.h
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

#ifndef _LFRUNLOOPSOURCE_H_
#define _LFRUNLOOPSOURCE_H_

/**
 * @defgroup LFRunLoopSourceRef LFRunLoopSourceRef
 * @{
 * @brief The LFRunLoopSourceRef object acts as source for a LFRunLoopRef
 *
 * A LFRunLoopSourceRef contains out of two dates and a callback, when added to a LFRunLoopRef, the run loop will automatically call the callback as long as the current date
 * is greater or equal to the start date and smaller than the end date. When the current date is past the end date, the run loop will automatically remove the source.
 **/

#include "LFRuntime.h"
#include "LFBase.h"
#include "LFDate.h"

/**
 * The LFRunLoopSource layout
 **/
struct __LFRunLoopSource
{
	LFRuntimeBase base;
	
	/**
	 * The callback.
	 **/
	void *callback;
	/**
	 * The run loop associated with the object.
	 **/
	void *runLoop;
	
	/**
	 * The start date.
	 **/
	LFDateRef startDate;
	/**
	 * The end date.
	 **/
	LFDateRef endDate;
	
	/**
	 * Extra user data.
	 **/
	void *data;
};

/**
 * The run loop source type
 **/
typedef struct __LFRunLoopSource* LFRunLoopSourceRef;
#ifndef LF_TARGET_LITEC
/**
 * Type used as callback for a run loop source.
 **/
typedef void (*LFRunLoopSourceCallback)(LFRunLoopSourceRef source);
#else
typedef void* LFRunLoopSourceCallback;
#endif

LF_EXPORT LFTypeID LFRunLoopSourceGetTypeID();

/**
 * Creates a new and infenitely running run loop source
 **/
LF_EXPORT LFRunLoopSourceRef LFRunLoopSourceCreate(LFRunLoopSourceCallback callback, void *data);
/**
 * Creates a new run loop source using the callback and the given dates.
 * @param start The start date or NULL.
 * @param end The end date or NULL.
 **/
LF_EXPORT LFRunLoopSourceRef LFRunLoopSourceCreateWithDate(LFRunLoopSourceCallback callback, LFDateRef start, LFDateRef end, void *data);

/**
 * @}
 **/

#endif
