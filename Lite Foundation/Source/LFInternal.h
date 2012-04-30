//
//  LFInternal.h
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

#ifndef _LFINTERNAL_H_
#define _LFINTERNAL_H_

/**
 * @addtogroup LFRuntime
 * @{
 **/

#include "LFBase.h"


// -----------------------------
// Assertion
// -----------------------------
/**
 * @cond
 **/
#ifndef NDEBUG
	#ifndef LF_TARGET_LITEC
		#define __LFAssert0(condition, desc, a1, a2, a3, a4) \
			printf("Assertion '%s' failed. Description: ", condition); \
			printf(desc);

		#define __LFAssert1(condition, desc, a1, a2, a3, a4) \
			printf("Assertion '%s' failed. Description: ", condition); \
			printf(desc, a1);

		#define __LFAssert2(condition, desc, a1, a2, a3, a4) \
			printf("Assertion '%s' failed. Description: ", condition); \
			printf(desc, a1, a2);

		#define __LFAssert3(condition, desc, a1, a2, a3, a4) \
			printf("Assertion '%s' failed. Description: ", condition); \
			printf(desc, a1, a2, a3);

		#define __LFAssert4(condition, desc, a1, a2, a3, a4) \
			printf("Assertion '%s' failed. Description: ", condition); \
			printf(desc, a1, a2, a3, a4);

		#define _LFAssert(condition, desc, a1, a2, a3, a4, count) if(!(condition)) {__LFAssert##count(#condition, desc, a1, a2, a3, a4); abort();}
	#else
		#define _LFAssert(condition, desc, a1, a2, a3, a4, ignored) if(!(condition)) {printf(desc, a1, a2, a3, a4); sys_exit(NULL);} void *__LFLFTempPointer
	#endif
#else
	#define _LFAssert
#endif
/**
 * @endcond
 **/

/**
 * Assertion macro. Checks if condition evaluates to false and then prints the description string and exits the program
 **/
#define LFAssert(condition, description) _LFAssert(condition, description, 0, 0, 0, 0, 0)
/**
 * Assertion macro. The description can be used as format string for the additional arguments. 
 * Checks if condition evaluates to false and then prints the description string and exits the program
 **/
#define LFAssert1(condition, description, a1) _LFAssert(condition, description, a1, 0, 0, 0, 1)
/**
 * Assertion macro. The description can be used as format string for the additional arguments. 
 * Checks if condition evaluates to false and then prints the description string and exits the program
 **/
#define LFAssert2(condition, description, a1, a2) _LFAssert(condition, description, a1, a2, 0, 0, 2)
/**
 * Assertion macro. The description can be used as format string for the additional arguments. 
 * Checks if condition evaluates to false and then prints the description string and exits the program
 **/
#define LFAssert3(condition, description, a1, a2, a3) _LFAssert(condition, description, a1, a2, a3, 0, 3)
/**
 * Assertion macro. The description can be used as format string for the additional arguments. 
 * Checks if condition evaluates to false and then prints the description string and exits the program
 **/
#define LFAssert4(condition, description, a1, a2, a3, a4) _LFAssert(condition, description, a1, a2, a3, a4, 4)


// -----------------------------
// Bit manipulation
// -----------------------------
/**
 * Evaluates to true if the given bit is set
 * @remark Bits go from 0 to n-1 where n is the number of bits of the variable
 **/
#define LFBitIsSet(v, bit)	((v & (1 << bit)) != 0)
/**
 * Sets the given bit
 * @remark Bits go from 0 to n-1 where n is the number of bits of the variable
 **/
#define LFBitSet(v, bit)	v |= (1 << bit)
/**
 * Clears the given bit
 * @remark Bits go from 0 to n-1 where n is the number of bits of the variable
 **/
#define LFBitClear(v, bit)	v &= ~(1 << bit)

#define LFBitfieldMask(n1, n2)				(((uint32_t)~0) << (31 - n1 + n2)) >> (31 - n1)
#define LFBitfieldGetValue(v, n1, n2)		((v & LFBitfieldMask(n1, n2)) >> n2)
#define LFBitfieldSetValue(v, n1, n2, x)	v = (v & ~LFBitfieldMask(n1, n2)) | ((x << n2) & LFBitfieldMask(n1, n2))
#define LFBitfieldMaxValue(n1, n2)			LFBitfieldGetValue(0xFFFFFFFF, n1, n2)


// -----------------------------
// Byte order
// -----------------------------
/**
 * Used to represent a unknown byte order
 * @remark Getting this value means that you are on some obscure architecture that no one cares about anyway. There be some huge dragons!
 **/
#define LFByteOrderUnknown		0
/**
 * Used to represent a little endian byte order. This is the most usual result on i368 and x86-64 CPUs
 **/
#define LFByteOrderLittleEndian 1
/**
 * Used to represent a big endian byte order. This is the most usual result on PPC CPUs
 **/
#define LFByteOrderBigEndian	2

/**
 * Type used to store a byte order
 * @sa LFByteOrderUnknown
 * @sa LFByteOrderLittleEndian
 * @sa LFByteOrderBigEndian
 **/
typedef uint8_t LFByteOrder;

/**
 * MUST be defined on little endian hosts (i368, x86-64)
 * If you are on a host with other byte order, please undefine this value and define LFBigEndianHost instead
 **/
#define LFLittleEndianHost
//#define LFBigEndianHost


#ifdef LF_INLINE
/**
 * Returns the current byte order. This function does not use the hosts byte order but uses an uint32_t to compute the result
 * @remark This function is inlined on compilers with support for inline function
 **/
LF_INLINE LFByteOrder LFGetCurrentByteOrder()
{
    uint32_t x = (LFByteOrderBigEndian << 24) | LFByteOrderLittleEndian;
    return (LFByteOrder)*((uint8_t *)&x);
}

// -----------------------------
// Variable swapping
// -----------------------------
/**
 * Swaps the given 16 bit ints byte order
 * @remark This function is inlined on compilers with support for inline function
 * @remark When compiling with the GNU compiler, the function will use the xchgb assembler instrunction to swap the int
 **/
LF_INLINE uint16_t LFSwapInt16(uint16_t arg) 
{
#ifdef __GNUC__
    __asm__("xchgb %b0, %h0" : "+q" (arg));
    return arg;
#else
    uint16_t result;
    result = ((arg << 8) & 0xFF00) | ((arg >> 8) & 0xFF);
	
    return result;
#endif
}

/**
 * Swaps the given 32 bit ints byte order
 * @remark This function is inlined on compilers with support for inline function
 * @remark When compiling with the GNU compiler, the function will use the bswap assembler instrunction to swap the int
 **/
LF_INLINE uint32_t LFSwapInt32(uint32_t arg)
{
#ifdef __GNUC__
    __asm__("bswap %0" : "+r" (arg));
    return arg;
#else
    uint32_t result;
    result = ((arg & 0xFF) << 24) | ((arg & 0xFF00) << 8) | ((arg >> 8) & 0xFF00) | ((arg >> 24) & 0xFF);
    return result;
#endif
}

/**
 * Swaps the given 64 bit ints byte order
 * @remark This function is inlined on compilers with support for inline function
 * @remark This function uses two 32 bit integers that both get swapped using two LFSwapInt32() instructions
 * @sa LFSwapInt32()
 **/
LF_INLINE uint64_t LFSwapInt64(uint64_t arg) 
{
	// 64 bit ints are swapped by using two 32 bit integers, swapping them and then writing them out as one 64 bit
	uint32_t ul[2];
	memcpy(&ul[0], &arg, sizeof(uint64_t));
	
    ul[0] = LFSwapInt32(ul[1]); 
    ul[1] = LFSwapInt32(ul[0]);
	
	uint64_t result;
	memcpy(&result, &ul[0], sizeof(uint64_t));
	
    return result;
}


// Big endian
/**
 * Swaps the given big endian 16 bit integer to the host byte order
 * @remark On big endian hosts, this function will do nothing. The function uses LFSwapInt16() on little endian hosts to swap the int
 * @remark This function is inlined on compilers with support for inline function
 * @sa LFSwapInt16()
 **/
LF_INLINE uint16_t LFSwapBigInt16ToHost(uint16_t arg)
{
#ifdef LFBigEndianHost
	return arg;
#else
	return LFSwapInt16(arg);
#endif
}

/**
 * Swaps the given big endian 32 bit integer to the host byte order
 * @remark On big endian hosts, this function will do nothing. The function uses LFSwapInt32() on little endian hosts to swap the int
 * @remark This function is inlined on compilers with support for inline function
 * @sa LFSwapInt32()
 **/
LF_INLINE uint32_t LFSwapBigInt32ToHost(uint32_t arg)
{
#ifdef LFBigEndianHost
	return arg;
#else
	return LFSwapInt32(arg);
#endif
}

/**
 * Swaps the given big endian 64 bit integer to the host byte order
 * @remark On big endian hosts, this function will do nothing. The function uses LFSwapInt64() on little endian hosts to swap the int
 * @remark This function is inlined on compilers with support for inline function
 * @sa LFSwapInt64()
 **/
LF_INLINE uint64_t LFSwapBigInt64ToHost(uint64_t arg)
{
#ifdef LFBigEndianHost
	return arg;
#else
	return LFSwapInt64(arg);
#endif
}


// Little endian
/**
 * Swaps the given little endian 16 bit integer to the host byte order
 * @remark On little endian hosts, this function will do nothing. The function uses LFSwapInt16() on big endian hosts to swap the int
 * @remark This function is inlined on compilers with support for inline function
 * @sa LFSwapInt16()
 **/
LF_INLINE uint16_t LFSwapLittleInt16ToHost(uint16_t arg)
{
#ifdef LFLittleEndianHost
	return arg;
#else
	return LFSwapInt16(arg);
#endif
}

/**
 * Swaps the given little endian 32 bit integer to the host byte order
 * @remark On little endian hosts, this function will do nothing. The function uses LFSwapInt32() on big endian hosts to swap the int
 * @remark This function is inlined on compilers with support for inline function
 * @sa LFSwapInt32()
 **/
LF_INLINE uint32_t LFSwapLittleInt32ToHost(uint32_t arg)
{
#ifdef LFLittleEndianHost
	return arg;
#else
	return LFSwapInt32(arg);
#endif
}

/**
 * Swaps the given little endian 64 bit integer to the host byte order
 * @remark On little endian hosts, this function will do nothing. The function uses LFSwapInt64() on big endian hosts to swap the int
 * @remark This function is inlined on compilers with support for inline function
 * @sa LFSwapInt64()
 **/
LF_INLINE uint64_t LFSwapLittleInt64ToHost(uint64_t arg)
{
#ifdef LFLittleEndianHost
	return arg;
#else
	return LFSwapInt64(arg);
#endif
}

#else
#define LFGetCurrentByteOrder _LFGetCurrentByteOrder

#define LFSwapInt16 _LFSwapInt16
#define LFSwapInt32 _LFSwapInt32
#define LFSwapInt64 _LFSwapInt64

#define LFSwapBigInt16ToHost _LFSwapBigInt16ToHost
#define LFSwapBigInt32ToHost _LFSwapBigInt32ToHost
#define LFSwapBigInt64ToHost _LFSwapBigInt64ToHost

#define LFSwapLittleInt16ToHost _LFSwapLittleInt16ToHost
#define LFSwapLittleInt32ToHost _LFSwapLittleInt32ToHost
#define LFSwapLittleInt64ToHost _LFSwapLittleInt64ToHost
#endif

/**
 * @}
 **/

#endif
