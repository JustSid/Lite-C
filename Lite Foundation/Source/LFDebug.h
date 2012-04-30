//
//  LFDebug.h
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

#ifndef _LFDEBUG_H_
#define _LFDEBUG_H_

/**
 * @addtogroup LFRuntime
 * @{
 **/

#include "LFBase.h"

/**
 * No zombie logging
 **/
#define kLFZombieLevelNone			0
/**
 * Access of dead objects will be logged into the runtime log file
 **/
#define kLFZombieLevelScribble		(1 << 1)
/**
 * Access of dead objects will be logged together with the thread that made access.
 **/
#define kLFZombieLevelThreads		(1 << 2)

/**
 * Data type that holds a zombie level
 **/
typedef char LFZombieLevel;

/**
 * The runtime will try to open the file at the given path and will write all new messages into this file.
 * @param file Path to a log file or NULL to close the current log file.
 **/
LF_EXPORT void LFRuntimeSetLogPath(char *file);
/**
 * Sets the runtimes log file. The runtime will write all new messages into this file.
 * @remark The runtime will close the file via fclose() at its exit or if a new path/file is set!
 **/
LF_EXPORT void LFRuntimeSetLogFile(FILE *file);

#ifndef NDEBUG

/**
 * Sets the runtimes zombie level
 * @param level Possible values are kLFZombieLevelNone, kLFZombieLevelScribble and kLFZombieLevelThreads
 * @remark If set to kLFZombieLevelScribble, the runtime won't purge the objects from memory when they are deallocated and will also log every access made to this object.
 **/
LF_EXPORT void LFRuntimeSetZombieLevel(LFZombieLevel lvl);
/**
 * Returns the current zombie level
 **/
LF_EXPORT LFZombieLevel LFRuntimeGetZombieLevel();

/**
 * Scribbles a zombie note if zombies are enabled.
 * @return true of the object is a zombie, otherwise false
 **/
LF_EXPORT uint8_t LFZombieScribble(LFTypeRef ref, char *funct);
/**
 * Returns true if the object is not nil and instance of the given type ID or one of its subclasses.
 **/
LF_EXPORT uint8_t LFRuntimeValidate(LFTypeRef ref, LFTypeID tid);

/**
 * Starts tracking the given object. A tracked object will log all access made to it into the runtime log file.
 **/
LF_EXPORT void LFRuntimeStartTracking(LFTypeRef ref);
/**
 * Stops the tracking of the given object.
 **/
LF_EXPORT void LFRuntimeStopTracking(LFTypeRef ref);

#else

#define LFRuntimeSetZombieLevel(ignored)
#define LFRuntimeGetZombieLevel 0
#define LFZombieScribble(ignored1, ignored2) 0
#define LFRuntimeValidate(ignored1, ignored2) 1
#define LFRuntimeStartTracking(ignored)
#define LFRuntimeStartTracking(ignored)

#endif

/**
 * @}
 **/
#endif
