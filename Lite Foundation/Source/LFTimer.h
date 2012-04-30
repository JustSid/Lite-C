//
//  LFTimer.h
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

#ifndef _LFTIMER_H_
#define _LFTIMER_H_

/**
 * @defgroup LFTimerRef LFTimerRef
 * @{
 * @brief The LFTimerRef object is a object which can either fire a callback once or periodically.
 *
 * A timer must be associated with a LFRunLoopRef in order to fire its callback, the run loop must also run or at least do steps regulary, otherwise the time won't fire its callback.
 **/

#include "LFRuntime.h"
#include "LFBase.h"
#include "LFDate.h"

/**
 * The timer layout
 **/
struct __LFTimer
{
	LFRuntimeBase base;
	
	/**
	 * True if the timer should repeat, otherwise false
	 **/
	uint8_t repeats;
	
	/**
	 * The date when the timer should fire
	 **/
	LFDateRef fireDate;
	/**
	 * The fire interval of the timer, only used when the timer is a repeating timer.
	 **/
	LFTimeInterval fireInterval;
	
	/**
	 * The run loop of the timer
	 **/
	void *scheduledRunLoop;
	
	/**
	 * The callback of the timer
	 **/
	void *callback;
};

/**
 * The timer type.
 **/
typedef struct __LFTimer* LFTimerRef;
#ifndef LF_TARGET_LITEC
/**
 * A callback type for the timer.
 **/
typedef void (*LFTimerCallback)(LFTimerRef timer);
#else
typedef void* LFTimerCallback;
#endif

/**
 * Returns the type ID of the timer class. Don't use this to create your own instances!
 **/
LF_EXPORT LFTypeID LFTimerGetTypeID();

/**
 * Creates a new timer and returns it.
 * @remark The timer is not scheduled in any run loop!
 **/
LF_EXPORT LFTimerRef LFTimerCreate(LFTimeInterval fire, LFTimerCallback callback, uint8_t repeats);
/**
 * Creates a new timer and returns it. The returned timer is autoreleased and already scheduled in the current run loop.
 **/
LF_EXPORT LFTimerRef LFTimerScheduledTimer(LFTimeInterval fire, LFTimerCallback callback, uint8_t repeats);

/**
 * @}
 **/

#endif
