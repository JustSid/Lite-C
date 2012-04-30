//
//  LFUnitTest.h
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

#ifndef _LFUNITTEST_H_
#define _LFUNITTEST_H_

#include "LFBase.h"
#include "LFRuntime.h"
#include "LFString.h"

struct __LFUnitTest;
typedef void (*LFUnitTestFunction)(struct __LFUnitTest *);

struct __LFUnitTest
{
	LFRuntimeBase base;
	
	jmp_buf *jumpBuffer;
	uint32_t ran;
	uint32_t failed;
	
	LFUnitTestFunction testFunction;
	LFStringRef name;
	LFStringRef message;
};

typedef struct __LFUnitTest* LFUnitTestRef;


LF_EXPORT LFUnitTestRef LFUnitTestCreate(LFStringRef name, LFUnitTestFunction testFunction);
LF_EXPORT LFUnitTestRef LFUnitTest(LFStringRef name, LFUnitTestFunction testFunction);

LF_EXPORT void LFUnitTestRun(LFUnitTestRef test);

LF_EXPORT void LFUnitTestAssert(LFUnitTestRef test, LFStringRef message, int condition);
LF_EXPORT void LFUnitAssertCStrEqual(LFUnitTestRef test, char *expected, char *actual, LFStringRef message);
LF_EXPORT void LFUnitAssertStrEqual(LFUnitTestRef test, LFStringRef expected, LFStringRef actual, LFStringRef message);
LF_EXPORT void LFUnitAssertIntEqual(LFUnitTestRef test, int expected, int actual, LFStringRef message);
LF_EXPORT void LFUnitAssertDblEqual(LFUnitTestRef test, double expected, double actual, LFStringRef message);
LF_EXPORT void LFUnitAssertPtrEqual(LFUnitTestRef test, void *expected, void *actual, LFStringRef message);

#define LFUAssert(test, cond, message) LFUnitTestAssert(test, message, (cond))

#define LFUAssertCStrEquals(test, expected, actual, message) LFUnitAssertCStrEqual(test, expected, actual, message)
#define LFUAssertStrEquals(test, expected, actual, message) LFUnitAssertStrEqual(test, expected, actual, message)
#define LFUAssertIntEquals(test, expected, actual, message) LFUnitAssertIntEqual(test, expected, actual, message)
#define LFUAssertDblEquals(test, expected, actual, message) LFUnitAssertDblEqual(test, expected, actual, message)
#define LFUAssertPtrEquals(test, expected, actual, message) LFUnitAssertPtrEqual(test, expected, actual, message)

#define LFUAssertPtrNotNull(test, ptr) LFUnitTestAssert(test, LFSTR("Unexpected NULL pointer!"), ptr != NULL)

#endif
