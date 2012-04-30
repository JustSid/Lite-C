//
//  LFInternal.c
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

#include "LFInternal.h"

// The functions here are the very same as their inline siblings defined in LFInternal.h
short _LFGetCurrentByteOrder()
{
    unsigned int x = (LFByteOrderBigEndian << 24) | LFByteOrderLittleEndian;
    return (short)*((unsigned char *)&x);
}

uint16_t _LFSwapInt16(uint16_t arg) 
{
    uint16_t res;
    res = ((arg << 8) & 0xFF00) | ((arg >> 8) & 0xFF);
	
    return res;
}

uint32_t _LFSwapInt32(uint32_t arg)
{
    uint32_t res;
    res = ((arg & 0xFF) << 24) | ((arg & 0xFF00) << 8) | ((arg >> 8) & 0xFF00) | ((arg >> 24) & 0xFF);
    return res;
}

uint64_t _LFSwapInt64(uint64_t arg) 
{
	uint32_t ul[2];
	memcpy(&ul[0], &arg, sizeof(uint64_t));
	
    ul[0] = LFSwapInt32(ul[1]); 
    ul[1] = LFSwapInt32(ul[0]);
	
	uint64_t res;
	memcpy(&res, &ul[0], sizeof(uint64_t));
	
    return res;
}


uint16_t _LFSwapBigInt16ToHost(uint16_t arg)
{
#ifdef LFBigEndianHost
	return arg;
#else
	return LFSwapInt16(arg);
#endif
}

uint32_t _LFSwapBigInt32ToHost(uint32_t arg)
{
#ifdef LFBigEndianHost
	return arg;
#else
	return LFSwapInt32(arg);
#endif
}

uint64_t _LFSwapBigInt64ToHost(uint64_t arg)
{
#ifdef LFBigEndianHost
	return arg;
#else
	return LFSwapInt64(arg);
#endif
}

// Little endian
uint16_t _LFSwapLittleInt16ToHost(uint16_t arg)
{
#ifdef LFLittleEndianHost
	return arg;
#else
	return LFSwapInt16(arg);
#endif
}

uint32_t _LFSwapLittleInt32ToHost(uint32_t arg)
{
#ifdef LFLittleEndianHost
	return arg;
#else
	return LFSwapInt32(arg);
#endif
}

uint64_t _LFSwapLittleInt64ToHost(uint64_t arg)
{
#ifdef LFLittleEndianHost
	return arg;
#else
	return LFSwapInt64(arg);
#endif
}
