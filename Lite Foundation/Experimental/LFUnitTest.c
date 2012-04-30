//
//  LFUnitTest.c
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

#include "LFUnitTest.h"
#include "LFAutoreleasePool.h"

void __LFUnitTestInit(LFTypeRef ref);
void __LFUnitTestDealloc(LFTypeRef ref);

static LFRuntimeClass __LFUnitTestClass;
static LFTypeID __LFUnitTestTypeID = kLFInvalidTypeID;


void __LFRuntimeInitUnitTest()
{
	__LFUnitTestClass.version = 1;
	__LFUnitTestClass.name = "LFUnitTest";
	
	__LFUnitTestClass.init = __LFUnitTestInit;
	__LFUnitTestClass.copy = NULL;
	__LFUnitTestClass.dealloc = __LFUnitTestDealloc;
	__LFUnitTestClass.equal = NULL;
	__LFUnitTestClass.hash  = NULL;
	
	__LFUnitTestClass.superClass = kLFInvalidTypeID;
	__LFUnitTestClass.protocolBag = NULL;
	__LFUnitTestTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFUnitTestClass);
}

void __LFUnitTestInit(LFTypeRef ref)
{
	LFUnitTestRef unitTest = (LFUnitTestRef)ref;
	
	unitTest->ran = 0;
	unitTest->failed = 0;
	unitTest->jumpBuffer = NULL;
	unitTest->testFunction = NULL;
	unitTest->name = NULL;
	unitTest->message = NULL;
}

void __LFUnitTestDealloc(LFTypeRef ref)
{
	LFUnitTestRef unitTest = (LFUnitTestRef)ref;
	
	LFRelease(unitTest->name);
	LFRelease(unitTest->message);
}




LFUnitTestRef LFUnitTestCreate(LFStringRef name, LFUnitTestFunction testFunction)
{
	LFUnitTestRef unitTest = LFRuntimeCreateInstance(__LFUnitTestTypeID, sizeof(struct __LFUnitTest));
	
	if(unitTest)
	{
		unitTest->name = LFRetain(name);
		unitTest->testFunction = testFunction;
	}
	
	return unitTest;
}

LFUnitTestRef LFUnitTest(LFStringRef name, LFUnitTestFunction testFunction)
{
	return LFAutorelease(LFUnitTestCreate(name, testFunction));
}


void LFUnitTestRun(LFUnitTestRef test)
{	
	jmp_buf buffer;
	test->jumpBuffer = &buffer;
	
	if(setjmp(buffer) == 0)
	{
		test->ran = 1;
		test->testFunction(test);
	}
	
	test->jumpBuffer = NULL;
}

void LFUnitTestFail(LFUnitTestRef test, LFStringRef message1, LFStringRef message2)
{
	test->failed = 1;
	test->message = LFStringCreate();
	
	if(message1)
	{
		LFStringAppend(test->message, message1);
		
		if(message2)
			LFStringAppendCString(test->message, ": ");
	}
	
	if(message2)
		LFStringAppend(test->message, message2);
	
	
	if(test->jumpBuffer)
		longjmp(*test->jumpBuffer, 0);
}

void LFUnitTestAssert(LFUnitTestRef test, LFStringRef message, int condition)
{
	if(condition)
		return;
	
	LFStringRef msg = LFSTR("Assertion failed");
	LFUnitTestFail(test, msg, message);
}


void LFUnitAssertCStrEqual(LFUnitTestRef test, char *expected, char *actual, LFStringRef message)
{
	if(expected == NULL && actual == NULL)
		return;
	
	if(expected != NULL && actual != NULL)
	{
		if(strcmp(expected, actual) == 0)
			return;
	}
	

	LFStringRef msg = LFSTR("Expected '");
	LFStringAppendCString(msg, expected);
	LFStringAppendCString(msg, "' but was '");
	LFStringAppendCString(msg, actual);
	LFStringAppendCString(msg, "'");
	
	LFUnitTestFail(test, msg, message);
}

void LFUnitAssertStrEqual(LFUnitTestRef test, LFStringRef expected, LFStringRef actual, LFStringRef message)
{
	LFUnitAssertCStrEqual(test, LFStringGetCString(expected), LFStringGetCString(actual), message);
}

void LFUnitAssertIntEqual(LFUnitTestRef test, int expected, int actual, LFStringRef message)
{
	if(expected == actual)
		return;
	
	char buffer[256];
	sprintf(buffer, "Expected %i but was %i", expected, actual);
	
	LFUnitTestFail(test, LFSTR(buffer), message);
}

void LFUnitAssertDblEqual(LFUnitTestRef test, double expected, double actual, LFStringRef message)
{
	if(fabs(expected - actual) <= 0.000001)
		return;
	
	char buffer[256];
	sprintf(buffer, "Expected %f but was %f", expected, actual);
	
	LFUnitTestFail(test, LFSTR(buffer), message);
}

void LFUnitAssertPtrEqual(LFUnitTestRef test, void *expected, void *actual, LFStringRef message)
{
	if(expected == actual)
		return;
	
	char buffer[256];
	sprintf(buffer, "Expected pointer <0x%p> but was <0x%p>", expected, actual);
	
	LFUnitTestFail(test, LFSTR(buffer), message);
}

