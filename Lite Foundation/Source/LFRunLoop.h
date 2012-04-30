//
//  LFRunLoop.h
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

#ifndef _LFRUNLOOP_H_
#define _LFRUNLOOP_H_

/**
 * @defgroup LFRunLoopRef LFRunLoopRef
 * @{
 * @brief The LFRunLoopRef object is a object which runs certain tasks and timers.
 *
 * Every thread has its very own singleton LFRunLoopRef object. A run loop manages sources and timers, which callbacks are fired on the run loops thread.
 * This allows one to delegate function calls to different thread.
 **/

#include "LFRuntime.h"
#include "LFBase.h"
#include "LFSet.h"
#include "LFString.h"
#include "LFTimer.h"
#include "LFRunLoopSource.h"
#include "LFDate.h"

/**
 * The run loop layout
 **/
struct __LFRunLoop
{
	LFRuntimeBase base;
	
	/**
	 * All sources managed by the run loop.
	 **/
	LFSetRef sources;
	/**
	 * All timers managed by the run loop.
	 **/
	LFSetRef timers;
	
	/**
	 * A flag stating wether the run loop should stop or not.
	 **/
	uint8_t stopped;
};

/**
 * The run loop type
 **/
typedef struct __LFRunLoop* LFRunLoopRef;

/**
 * Returns the run loop of the current thread.
 **/
LF_EXPORT LFRunLoopRef LFRunLoopCurrentRunLoop();

/**
 * Lets the run loop do one step. The run loop will fire all timers and sources that are due.
 **/
LF_EXPORT void LFRunLoopStep(LFRunLoopRef runLoop);
/**
 * Lets the run loop run in an endless loop, until it is stopped by calling LFRunLoopStop()
 **/
LF_EXPORT void LFRunLoopRun(LFRunLoopRef runLoop);
/**
 * Lets the run loop run in an endless loop until the given date passed.
 **/
LF_EXPORT void LFRunLoopRunUntilDate(LFRunLoopRef runLoop, LFDateRef date);
/**
 * Stops the run loop.
 **/
LF_EXPORT void LFRunLoopStop(LFRunLoopRef runLoop);

/**
 * Adds the given timer to the run loop.
 * @remark A timer can only be added to one run loop at a time!
 **/
LF_EXPORT void LFRunLoopAddTimer(LFRunLoopRef runLoop, LFTimerRef timer);
/**
 * Removes the given timer from the run loop.
 **/
LF_EXPORT void LFRunLoopRemoveTimer(LFRunLoopRef runLoop, LFTimerRef timer);

/**
 * Adds the given source to the run loop.
 * @remark A source can only be added to one run loop at a time!
 **/
LF_EXPORT void LFRunLoopAddSource(LFRunLoopRef runLoop, LFRunLoopSourceRef source);
/**
 * Removes teh given source from the run loop.
 **/
LF_EXPORT void LFRunLoopRemoveSource(LFRunLoopRef runLoop, LFRunLoopSourceRef source);

/**
 * @}
 **/

#endif
