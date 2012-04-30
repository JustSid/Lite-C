//
//  LiteFoundation.c
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

#include "LiteFoundation.h"
#include "LFAutoreleasePool.h"

void __LFRuntimeInit1();
void __LFRuntimeInit2();
void __LFRuntimeInitProtocols();

void __LFRuntimeInitAutoreleasePool();
void __LFRuntimeInitArchiver();
void __LFRuntimeInitCharacterSet();
void __LFRuntimeInitString();
void __LFRuntimeInitScannner();
void __LFRuntimeInitEnumerator();
void __LFRuntimeInitSet();
void __LFRuntimeInitArray();
void __LFRuntimeInitDictionary();
void __LFRuntimeInitData();
void __LFRuntimeInitLock();
void __LFRuntimeInitCondition();
void __LFRuntimeInitRunLoop();
void __LFRuntimeInitThread();
void __LFRuntimeInitDate();
void __LFRuntimeInitTimer();
void __LFRuntimeInitTimeZone();
void __LFRuntimeInitDateComponents();
void __LFRuntimeInitRunLoopSource();
void __LFRuntimeInitNumber();


void LFRuntimeInit()
{
#ifdef LF_TARGET_LITEC
    STRING *logfileLocation = str_create(work_dir);
    str_cat(logfileLocation, "\\LFLog.txt");
    
    LFRuntimeSetLogPath(_chr(logfileLocation));
    
    ptr_remove(logfileLocation);
#else
    LFRuntimeSetLogFile(stdout);
#endif
	__LFRuntimeInit1();
	__LFRuntimeInitProtocols();
	
	__LFRuntimeInitAutoreleasePool();
    __LFRuntimeInitArchiver();
	__LFRuntimeInitCharacterSet();
	__LFRuntimeInitString();
	__LFRuntimeInitScannner();
	__LFRuntimeInitEnumerator();
	__LFRuntimeInitSet();
	__LFRuntimeInitArray();
	__LFRuntimeInitDictionary();
	__LFRuntimeInitData();
	__LFRuntimeInitLock();
	__LFRuntimeInitCondition();
	__LFRuntimeInit2(); // At this point, LFRuntimeLock works!
	__LFRuntimeInitRunLoop();
	__LFRuntimeInitThread();
	__LFRuntimeInitDate();
	__LFRuntimeInitTimer();
	__LFRuntimeInitTimeZone();
	__LFRuntimeInitDateComponents();
	__LFRuntimeInitRunLoopSource();
    __LFRuntimeInitNumber();
	
#ifdef LF_TARGET_LITEC
	while(1)
	{
        proc_mode = PROC_EARLY | PROC_NOFREEZE;
        
		LFAutoreleasePoolRef pool = LFAutoreleasePoolCreate();
		LFRunLoopStep(LFRunLoopCurrentRunLoop());
		
		wait(1);
		LFRelease(pool);
	}
#endif
}

void LFRuntimeStop()
{
    LFRuntimeSetLogPath(NULL); // Close the open log file.
}
