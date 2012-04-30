//
//  LiteFoundation.h
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

#ifndef _LITEFOUNDATION_H_
#define _LITEFOUNDATION_H_

// The LF Runtime
#include "LFBase.h"
#include "LFRuntime.h"
#include "LFDebug.h"
#include "LFInternal.h"
#include "LFProtocol.h"
#include "LFKVCoding.h"

// Classes
#include "LFAutoreleasePool.h"
#include "LFCharacterSet.h"
#include "LFString.h"
#include "LFScanner.h"
#include "LFEnumerator.h"
#include "LFSet.h"
#include "LFArray.h"
#include "LFDictionary.h"
#include "LFData.h"
#include "LFLock.h"
#include "LFCondition.h"
#include "LFRunLoop.h"
#include "LFThread.h"
#include "LFDate.h"
#include "LFTimer.h"
#include "LFTimeZone.h"
#include "LFDateComponents.h"
#include "LFRunLoopSource.h"
#include "LFNumber.h"
#include "LFArchiver.h"
#include "LFUnarchiver.h"


#ifdef LF_TARGET_LITEC
#include "LFBase.c"
#include "LFRuntime.c"
#include "LFDebug.c"
#include "LFInternal.c"
#include "LFProtocol.c"
#include "LFKVCoding.c"

#include "LFAutoreleasePool.c"
#include "LFCharacterSet.c"
#include "LFString.c"
#include "LFScanner.c"
#include "LFEnumerator.c"
#include "LFSet.c"
#include "LFArray.c"
#include "LFDictionary.c"
#include "LFData.c"
#include "LFLock.c"
#include "LFCondition.c"
#include "LFRunLoop.c"
#include "LFThread.c"
#include "LFDate.c"
#include "LFTimer.c"
#include "LFTimeZone.c"
#include "LFDateComponents.c"
#include "LFRunLoopSource.c"
#include "LFNumber.c"
#include "LFArchiver.c"
#include "LFUnarchiver.c"

#include "LiteFoundation.c"
#endif


/**
 * @addtogroup LFRuntime
 * @{
 **/
/**
 * Initializes both, the runtime and the default classes.
 * @remark Call this before doing any other call to the runtime or before creating a class instance!
 * @remark In Lite-C, this function will automatically create a autorelease pool for you at the beginning of every frame which then gets drained at the end.
 * @sa LFAutoreleasePoolRef
 **/
void LFRuntimeInit();
/**
 * Stops the runtime.
 * @remark This function is a stub as it does nothing, thus you can't reinitialize Lite Foundation at a later point. However, this might change in the future.
 **/
void LFRuntimeStop();
/**
 * @}
 **/

#endif


// Doxygen mainpage
/** 
 * @mainpage Lite Foundation reference
 * This is the reference documentation for Lite Foundation, the reference is based on the versions 0.9.5 and 0.1.5 (runtime, library).<br />
 * If you never worked with Lite Foundation, please read the Getting Started guide <a href="http://widerwille.com/litefoundation/started.pdf">here</a>.<br />
 * If you want to know how to write your own Lite Foundation classes, please read the Runtime overview <a href="http://widerwille.com/litefoundation/runtime.pdf">here</a>.<br />
 * <br />
 * The reference is divided into multiple parts, the module tab lists all modules of Lite Foundation (eg. all functions, defines, structs etc that belong to the same group),
 * the classes tab lists all structs with documentation of their members and the files tab lists all header files that were used to generate the documentation. 
 * <br /><br />
 * @subpage include
 * <br /><br />
 * @subpage changelog
 * <br /><br />
 * @subpage license
 **/

/**
 * @page include Including Lite Foundation
 * Lite Foundation can be added to any C code base, just include all .c and .h into your project and then include the LiteFoundation.h file.
 * If your compiler can't build Lite Foundation, you can adjust the host configuration in LFBase.h so that they fit to your compiler and host system.
 * <br /><br />
 * REMARK: Lite Foundation assumes per default to run on an Little Endian machine, if you targeting eg. PPC CPUs, please comment the LFLittleEndianHost and uncomment LFBigEndianHost!
 **/

/**
 * @page changelog Changelog
 * <b>Runtime version 0.9.5</b><br />
 * Added new tracking features for objects, LFRuntimeStartTracking() and LFRuntimeStopTracking()<br />
 * Changed the way the runtime create objects, the memory is now initialized with zeros which makes almost every init function obsolete<br />
 * Fixed a bug with the runtime locking functions. Switching to multithreaded mode during an object function resulted in undefined behavior<br />
 * <br />
 * <br />
 * <b>Library version 0.1.5</b><br />
 * Audited all classes and fixed many bugs<br />
 * Added multithreading support for Lite-C<br />
 * Added the LFThread, LFLock, LFDictionary, LFDate, LFDateComponents, LFTimeZone, LFTimer, LFRunLoop, LFRunLoopSource and LFData classes<br />
 * <br />
 * <br />
 * <b>Runtime version 0.9.3</b><br />
 * Made the whole runtime multithreadable <br />
 * Added zombies. Zombies can be turned on and off via LFRuntimeSetZombieLevel()<br />
 * Added object locking functions LFRuntimeLock() and LFRuntimeUnlock()<br /> 
 * <br />
 * <br />
 * <b>Library version 0.1.0</b><br />
 * Adapted the multithreaded runtime environment<br />
 * Adapted the zombie concept<br />
 * Added the LFThread, LFLock, LFDictionary, LFDate, LFDateComponents, LFTimeZone, LFTimer, LFRunLoop, LFRunLoopSource and LFData classes<br />
 * Fixed a bug in the LFStringCreate() function which returned a bufferless string<br />
 * Fixed a bug in the LFStringCompareWithRange() function which compared the first strings number against the very first number of the second string (now it compares both numbers completely)<br />
 * Fixed a bug in the LFStringByAppendingString() function which returned most of the time NULL, crashed or leaked memory.<br />
 * <br />
 * <br />
 * <b>Library version 0.0.7</b><br />
 * Added a sorting function to the LFArray class<br />
 * Added the possibility to compare string numerical<br />
 * Changed how the LFString class compares strings (previously, lowercase letters were always greater than uppercase letters)<br />
 * Fixed a bug with the LFStringCompare() function which skipped the last character of the string<br />
 * <br />
 * <br />
 * <b>Library version 0.0.6</b><br />
 * Added the LFArrayRef class<br />
 * Added a new function to the LFStringRef class called LFStringComponentsSeperatedByString() <br />
 * <br />
 * <br />
 * <b>Library version 0.0.5</b><br />
 * Added the LFSetRef class which can store objects in a hash table like structure. <br />
 * Added the LFEnumeratorRef class and the LFEnumerator protocol which allows the enumeration of objects. <br />
 * Lite-C: Fixed a bug which made the hash and equal function of LFStrings pretty random<br />
 * <br />
 * <br />
 * <b>Runtime version 0.9.1</b><br />
 * Added better support for inheritance by allowing classes to define a super class and protocol functions to invoke on the superclass.
 * Most runtime functions like LFHash() will now fallback to the super class if the base class doesn't implement the callback. <br />
 * LFRuntimeSuperInvoke0();<br />
 * LFRuntimeSuperInvoke1();<br />
 * LFRuntimeSuperInvoke2();<br />
 * LFRuntimeSuperRInvoke0();<br />
 * LFRuntimeSuperRInvoke1();<br />
 * LFRuntimeSuperRInvoke2();<br />
 * <br />
 * <b>REMARK:</b> When you create a instance of a class, the runtime will invoke the init callback on all superclasses in inheritance order (base class -> first subclass -> second sublcass -> ... -> instance class). Likewise the dealloc callback will be invoked on all superclasses but in different order (instance class -> super class -> super class -> ... -> base class).
 **/

/**
 * @page license License
 * Copyright (c) 2011 by Sidney Just<br />
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated <br />
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation <br />
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, <br />
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:<br />
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.<br />
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, <br />
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR <br />
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE <br />
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, <br />
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.<br />
 **/
