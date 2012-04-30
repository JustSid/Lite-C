//
//  LFBase.h
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

#define LF_TARGET_OSX
//#define LF_TARGET_WIN32
//#define LF_TARGET_LITEC // Needs to be combined with LF_TARGET_WIN32!

#define LF_BUILD_LITEFOUNDATION
//#define LF_ANSI_TYPES_ARE_DEFINED // Only valid for LF_TARGET_LITEC, define this to signal that you have implemented the posix integer types like uint32_t

#define LF_BUILD_32BIT // 32 Bit build
//#define LF_BUILD_64BIT // 64 Bit build

#ifdef LF_BUILD_32BIT
    #ifdef LF_BUILD_64BIT

        #ifdef LF_TARGET_LITEC
            #define PRAGMA_PRINT "ERROR!!! Can't build Lite Foundation in 32 and 64 bit at the same time!"
        #else
            #error "Can't build Lite Foundation in 32 and 64 bit at the same time!"
        #endif

    #endif
#endif

#ifndef _LFBASE_H_
#define _LFBASE_H_

/**
 * @addtogroup LFRuntime
 * @{
 **/

// -----------------------------
// Platform specific stuff
// -----------------------------
#ifdef LF_TARGET_OSX
	#include <string.h>
	#include <stdint.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <math.h>
	#include <setjmp.h>
	#include <pthread.h>
	#include <sys/time.h>
	#include <errno.h>

	/**
	 * Exports the function, this is "extern" on OS X, "__declspec(dllexport) extern" on Windows and "__declspec(dllimport) extern" on Windows without LF_BUILD_LITEFOUNDATION
	 **/
	#define LF_EXPORT  
	/**
	 * Defined when the compiler supports function inlining by keyword. When it is defined, it is defined as "static __inline__"
	 **/
	#define LF_INLINE static inline
#endif

#ifdef LF_TARGET_WIN32
#include <windows.h>

#define LF_EXPORT


#ifndef LF_TARGET_LITEC
	#include <string.h>
	#include <stdint.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <math.h>
	#include <setjmp.h>

	#ifdef LF_BUILD_LITEFOUNDATION
		#define LF_EXPORT __declspec(dllexport) extern
	#else
		#define LF_EXPORT __declspec(dllimport) extern
	#endif


	#define LF_INLINE static inline
	#endif
#endif


#ifdef LF_TARGET_LITEC
#ifndef LF_ANSI_TYPES_ARE_DEFINED
// The 64 bit integer is defined as 32 bit one, Lite-C doesn't support greater types anyway
// However, if the switch to 64 bit is made, long is most probably the type that becomes 64 bit.
typedef unsigned long			uint64_t;
typedef unsigned int			uint32_t;
typedef unsigned short			uint16_t;
typedef unsigned char			uint8_t;

typedef signed long				int64_t;
typedef signed int				int32_t;
typedef signed short			int16_t;
typedef signed char				int8_t;

typedef uint32_t				uintptr_t;
typedef uint32_t				size_t;
#endif

double floor(double val)
{
	int res = (int)val;
	return (double)res;
}
#endif



// -----------------------------
// Version handling
// -----------------------------
/**
 * Defined when the Lite Foundation runtime version is 0.9.5 or higher
 **/
#define kLFRuntimeVersionNumber0_9_5
/**
 * Defined when the Lite Foundation runtime version is 0.9.3 or higher
 **/
#define kLFRuntimeVersionNumber0_9_3
/**
 * Defined when the Lite Foundation runtime version is 0.9.1 or higher
 **/
#define kLFRuntimeVersionNumber0_9_1
/**
 * Defined when the Lite Foundation runtime version is 0.9.0 or higher
 **/
#define kLFRuntimeVersionNumber0_9_0

/**
 * Defined when the Lite Foundation default library version is 0.1.5 or higher
 **/
#define kLFLibraryVersionNumber0_1_5
/**
 * Defined when the Lite Foundation default library version is 0.1.0 or higher
 **/
#define kLFLibraryVersionNumber0_1_0
/**
 * Defined when the Lite Foundation default library version is 0.0.7 or higher
 **/
#define kLFLibraryVersionNumber0_0_7
/**
 * Defined when the Lite Foundation default library version is 0.0.6 or higher
 **/
#define kLFLibraryVersionNumber0_0_6
/**
 * Defined when the Lite Foundation default library version is 0.0.5 or higher
 **/
#define kLFLibraryVersionNumber0_0_5
/**
 * Defined when the Lite Foundation default library version is 0.0.2 or higher
 **/
#define kLFLibraryVersionNumber0_0_2


// -----------------------------
// Types and predefined values
// -----------------------------
/**
 * Type used to store class IDs
 **/
typedef uint32_t LFTypeID;
/**
 * Type used to hold bit fields
 **/
typedef uint32_t LFOptionFlags;
/**
 * Type used to hold a hash
 **/
typedef uint32_t LFHashCode;
/**
 * Type used to hold a index
 **/
typedef int32_t	LFIndex;
/**
 * Type used to hold a comparision result
 * @sa kLFCompareLessThan
 * @sa kLFCompareEqualTo
 * @sa kLFCompareGreaterThan
 **/
typedef int32_t	LFComparisonResult;

/**
 * A "typeless" type that can hold a reference to any Lite Foundation object
 **/
typedef void* LFTypeRef;

/**
 * The left value is less/smaller than the right value. Returned by comparision functions
 **/
#define kLFCompareLessThan -1
/**
 * The left value is equal right value. Returned by comparision functions
 **/
#define kLFCompareEqualTo 0
/**
 * The left value is greater than the right value. Returned by comparision functions
 **/
#define kLFCompareGreaterThan 1

/**
 * Constant used to signal that nothing was found. Can be used in eg. LFRange like {kLFNotFound, 0}
 **/
#define kLFNotFound -1

#define kLFObserverTypeWillAccess   1
#define kLFObserverTypeDidAccess    2
#define kLFObserverTypeWillChange   4
#define kLFObserverTypeDidChange    8

typedef char LFRuntimeObserverType;
typedef void (*LFObserverGenericCallback)(LFTypeRef ref, char *key, LFTypeRef value, LFRuntimeObserverType type);


#ifndef LF_TARGET_LITEC
/**
 * Struct that is used to represent a range
 **/
typedef struct
{
	/**
	 * The location of the range, eg. a index of a string
	 **/
	LFIndex location;
	/**
	 * The length of the range
	 **/
	LFIndex length;
} LFRange;
#else
struct __LFRange
{
	LFIndex location;
	LFIndex length;
};
#endif

#ifdef LF_INLINE
/**
 * Creates a new range with the given location and length and returns it
 * @remark LITE-C: As Lite-C isn't able to return structs on the struct, Lite Foundation uses pointer for them. Thus LFRange is on Lite-C defined to be a pointer. This also means that you can't use LFRange range; range.lengt = x; without allocating space for the range
 **/
LF_INLINE LFRange LFRangeMake(LFIndex loc, LFIndex len) 
{
    LFRange range;
    range.location = loc;
    range.length = len;
	
    return range;
}
#else
	#ifdef LF_TARGET_LITEC
		typedef struct __LFRange* LFRange;
	#endif
	#define LFRangeMake(loc, len) __LFRangeMake(loc, len)
#endif

/**
 * @}
 **/

#endif
