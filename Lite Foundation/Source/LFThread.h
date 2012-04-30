//
//  LFThread.h
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

#ifndef _LFTHREAD_H_
#define _LFTHREAD_H_

/**
 * @defgroup LFThreadRef LFThreadRef
 * @{
 * @brief The LFThreadRef is an wrapper for the OS native thread object
 *
 * A basic LFThreadRef acts as a wrapper for the native OS thread object, but because it is designed as high level thread, a LFThreadRef can do more tasks than a normal thread.
 * Eg. one can delegate function calls to other threads.
 * <br /><br />
 * Every thread manages its own LFRunLoopRef instance and its own LFAutoreleasePoolRef stack, creating a new LFAutoreleasePoolRef will push the new pool to the current threads stack.
 **/

#include "LFBase.h"
#include "LFRuntime.h"
#include "LFLock.h"
#include "LFRunLoop.h"
#include "LFString.h"
#include "LFDictionary.h"
#include "LFAutoreleasePool.h"

/**
 * The threads internal layout
 **/
struct __LFThread
{
	LFRuntimeBase base;
	
	/**
	 * @cond
	 **/
#ifdef LF_TARGET_OSX
	pthread_t thread;
#endif
#ifdef LF_TARGET_WIN32
	HANDLE thread;
	DWORD threadID;
#endif
	/**
	 * @endcond
	 **/
	
	/**
	 * The state of the thread
	 * @sa kLFThreadStatePending
	 * @sa kLFThreadStateExecuting
	 * @sa kLFThreadStateFinished
	 * @sa kLFThreadStateCancelled
	 **/
	uint8_t state;
    
    /**
     * 1 if the thread waits for an action to be performed, otherwise 0
     **/
    uint8_t idle;
	
	/**
	 * Entry function of the thread
	 **/
	void *entry;
	/**
	 * Callback function of the thread
	 **/
	void *callback;
	/**
	 * Thread that should execute the callback function
	 **/
	struct __LFThread *callbackThread;
	
	/**
	 * The run loop that is associated with the thread
	 **/
	LFRunLoopRef runLoop;
	
	/**
	 * A dictionary containing user info
	 **/
	LFDictionaryRef info;
	/**
	 * The name of the thread
	 **/
	LFStringRef name;
	
	/**
	 * The top most autorelease pool of the thread
	 **/
	LFAutoreleasePoolRef pool;
};

/**
 * A thread state signaling that the thread is yet waiting to begin its execution
 **/
#define kLFThreadStatePending	0
/**
 * A thread state signaling that the thread is currently executing
 **/
#define kLFThreadStateExecuting 1
/**
 * A thread state signaling that the thread has finished its executing
 **/
#define kLFThreadStateFinished	2
/**
 * A thread state signaling that the thread was cancelled before it could finish its executing.
 **/
#define kLFThreadStateCancelled	3

/**
 * The thread object.
 **/
typedef struct __LFThread* LFThreadRef;

#ifndef LF_TARGET_LITEC
/**
 * Entry point for the thread.
 **/
typedef void (*LFThreadEntry)(LFThreadRef thread);
/**
 * Callback that should be called when the thread finishes.
 **/
typedef void (*LFThreadCallback)(LFThreadRef thread);
/**
 * Generic callback type.
 **/
typedef void (*LFThreadGenericCallback)(void *param);
#else
typedef void* LFThreadGenericCallback;
typedef void* LFThreadEntry;
typedef void* LFThreadCallback;
#endif

/**
 * Returns true if Lite Foundation operates in multithreaded environment, otherwise false.
 * @remark The function returns always true once you dispatched your first thread
 **/
LF_EXPORT uint8_t LFThreadIsMultithreaded();

/**
 * Creates a new thread with the given entry
 * @remark The thread is not automatically started!
 **/
LF_EXPORT LFThreadRef LFThreadCreate(LFThreadEntry entry);
/**
 * Creates a new thread with the given entry point and also starts it.
 * @return The returned thread is autoreleased.
 **/
LF_EXPORT LFThreadRef LFThread(LFThreadEntry entry);

/**
 * Returns the current thread.
 **/
LF_EXPORT LFThreadRef LFThreadGetCurrentThread();
/**
 * Returns the main thread.
 **/
LF_EXPORT LFThreadRef LFThreadGetMainThread();

/**
 * Starts the given thread
 * @remark A thread can only be started once!
 **/
LF_EXPORT void LFThreadStart(LFThreadRef thread);
/**
 * Lets the calling thread join the given thread. This lets the calling thread wait until the given thread finished executing.
 **/
LF_EXPORT void LFThreadJoin(LFThreadRef thread);

/**
 * Sets the threads callback
 * @param callbackThread The thread that should execute the callback.
 **/
LF_EXPORT void LFThreadSetCallback(LFThreadRef thread, LFThreadCallback callback, LFThreadRef callbackThread);

/**
 * Sets the given threads name
 **/
LF_EXPORT void LFThreadSetName(LFThreadRef thread, LFStringRef name);
/**
 * Returns the name of the given thread.
 * @remark A new thread will be named automatically as "Thread n" where n is the number of the thread. The main thread automatically gets the name "Main Thread".
 **/
LF_EXPORT LFStringRef LFThreadGetName(LFThreadRef thread);

/**
 * Sets the threads informational dictionary. This can be used to pass additional parameters to the thread
 **/
LF_EXPORT void LFThreadSetDictionary(LFThreadRef thread, LFDictionaryRef dictionary);
/**
 * Returns the threads informational dictonary.
 * @remark The returned value is a copy of the actual informational dictionary to guarantee atomical access.
 **/
LF_EXPORT LFDictionaryRef LFThreadGetDictionary(LFThreadRef thread);

/**
 * Returns true if the thread was cancelled, otherwise false.
 * @remark You should periodically poll this variable in your threads function and return from it if it returns true.
 **/
LF_EXPORT uint8_t LFThreadIsCancelled(LFThreadRef thread);

/**
 * Performs the given function in the context of the given thread.
 * @param waitForInvocation True if you want the thread to halt until the function got invoked
 * @remark If the receiving thread is the current thread, the function will be executed immediately, otherwise it will be added to the threads run loop
 * @remark If the thread isn't running, the function will do nothing.
 **/
LF_EXPORT void LFThreadInvokeFunction(LFThreadRef thread, LFThreadGenericCallback fnct, void *parameter, uint8_t waitForInvocation);

/**
 * Returns the run loop of the given thread
 * @remark The function returns NULL if the thread isn't running!
 **/
LF_EXPORT LFRunLoopRef LFThreadGetRunLoop(LFThreadRef thread);

/**
 * @}
 **/

#endif
