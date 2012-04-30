//
//  LFUnitTestSuite.c
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

#include "LFUnitTestSuite.h"
#include "LFEnumerator.h"
#include "LFAutoreleasePool.h"

void __LFUnitTestSuiteInit(LFTypeRef ref);
void __LFUnitTestSuiteDealloc(LFTypeRef ref);

static LFRuntimeClass __LFUnitTestSuiteClass;
static LFTypeID __LFUnitTestSuiteTypeID = kLFInvalidTypeID;


void __LFRuntimeInitUnitTestSuite()
{
	__LFUnitTestSuiteClass.version = 1;
	__LFUnitTestSuiteClass.name = "LFUnitTestSuite";
	
	__LFUnitTestSuiteClass.init = __LFUnitTestSuiteInit;
	__LFUnitTestSuiteClass.copy = NULL;
	__LFUnitTestSuiteClass.dealloc = __LFUnitTestSuiteDealloc;
	__LFUnitTestSuiteClass.equal = NULL;
	__LFUnitTestSuiteClass.hash  = NULL;
	
	__LFUnitTestSuiteClass.superClass = kLFInvalidTypeID;
	__LFUnitTestSuiteClass.protocolBag = NULL;
	__LFUnitTestSuiteTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFUnitTestSuiteClass);
}


void __LFUnitTestSuiteInit(LFTypeRef ref)
{
	LFUnitTestSuiteRef testSuite = (LFUnitTestSuiteRef)ref;
	
	testSuite->tests = LFArrayCreate(NULL);
	testSuite->failed = 0;
}

void __LFUnitTestSuiteDealloc(LFTypeRef ref)
{
	LFUnitTestSuiteRef testSuite = (LFUnitTestSuiteRef)ref;
	
	LFRelease(testSuite->tests);
}


LFUnitTestSuiteRef LFUnitTestSuiteCreate()
{
	LFUnitTestSuiteRef testSuite = LFRuntimeCreateInstance(__LFUnitTestSuiteTypeID, sizeof(struct __LFUnitTestSuite));
	return testSuite;
}

void LFUnitTestSuiteAddTest(LFUnitTestSuiteRef testSuite, LFUnitTestRef test)
{	
	LFArrayAddValue(testSuite->tests, test);
}

void LFUnitTestSuiteAddSuite(LFUnitTestSuiteRef testSuite, LFUnitTestSuiteRef otherTestSuite)
{
	LFArrayAddValuesFromArray(testSuite->tests, otherTestSuite->tests);
}



void LFUnitTestSuiteRun(LFUnitTestSuiteRef testSuite)
{
	LFEnumeratorRef enumerator = LFEnumeratorCreate(testSuite->tests);
	LFUnitTestRef test;
	
	while((test = LFEnumeratorNextObject(enumerator)))
	{
		LFUnitTestRun(test);
		
		if(test->failed)
			testSuite->failed ++;
	}
	
	LFRelease(enumerator);
}

LFStringRef LFUnitTestSuiteSummary(LFUnitTestSuiteRef testSuite)
{
	char buffer[256];
	sprintf(buffer, "%i of %i tests failed:\n", testSuite->failed, LFArrayCount(testSuite->tests));
	
	LFStringRef testSummary = LFSTR(buffer);
	
	LFEnumeratorRef enumerator = LFEnumeratorCreate(testSuite->tests);
	LFUnitTestRef test;
	
	while((test = LFEnumeratorNextObject(enumerator)))
	{
		if(test->failed)
			LFStringAppendCString(testSummary, "F");
		else
			LFStringAppendCString(testSummary, ".");
	}
	
	LFStringAppendCString(testSummary, "\n");
	LFRelease(enumerator);
	
	return testSummary;
}

LFStringRef LFUnitTestSuiteDetails(LFUnitTestSuiteRef testSuite)
{
	if(testSuite->failed == 0)
	{
		return LFSTR("All tests passed!");
	}
	
	LFStringRef detailString = LFStringCreate();
	
	if(testSuite->failed > 1)
	{
		char buffer[256];
		sprintf(buffer, "There were %i failed tests\n", testSuite->failed);
		LFStringAppendCString(detailString, buffer);
	}
	else
		LFStringAppendCString(detailString, "There was 1 failed test\n");
	
	
	LFEnumeratorRef enumerator = LFEnumeratorCreate(testSuite->tests);
	LFUnitTestRef test;
	
	while((test = LFEnumeratorNextObject(enumerator)))
	{
		if(test->failed)
		{
			if(test->name)
			{
				LFStringAppend(detailString, test->name);
				LFStringAppendCString(detailString, ": ");
			}
			
			if(test->message)
			{
				LFStringAppend(detailString, test->message);
				LFStringAppendCString(detailString, "\n");
			}
		}
	}
	
	LFRelease(enumerator);
	return LFAutorelease(detailString);
}
