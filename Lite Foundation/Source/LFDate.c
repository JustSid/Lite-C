//
//  LFDate.c
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

#include "LFDate.h"
#include "LFAutoreleasePool.h"

const LFTimeInterval kLFTimeIntervalSince1970 = 978307200.0;

void		__LFDateInit(LFTypeRef ref);
LFTypeRef	__LFDateCopy(LFTypeRef ref);
uint8_t		__LFDateEqual(LFTypeRef refA, LFTypeRef refB);
LFHashCode	__LFDateHash(LFTypeRef ref);

static LFRuntimeClass __LFDateClass;
static LFTypeID __LFDateTypeID = kLFInvalidTypeID;

void __LFRuntimeInitDate()
{
	__LFDateClass.version = 1;
	__LFDateClass.name = "LFDate";
	
	__LFDateClass.init = __LFDateInit;
	__LFDateClass.copy = __LFDateCopy;
	__LFDateClass.dealloc = NULL;
	__LFDateClass.equal = __LFDateEqual;
	__LFDateClass.hash  = __LFDateHash;
	
	__LFDateClass.superClass = kLFInvalidTypeID;
	__LFDateClass.protocolBag = NULL;
	__LFDateTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFDateClass);
}


void __LFDateInit(LFTypeRef ref)
{
	LFDateRef date = (LFDateRef)ref;
	date->time = LFTimeIntervalGetCurrent();
}

LFTypeRef __LFDateCopy(LFTypeRef ref)
{
	LFDateRef source = (LFDateRef)ref;
	LFDateRef copy = LFDateCreateWithTimeInterval(source->time);
	
	return copy;
}

uint8_t __LFDateEqual(LFTypeRef refA, LFTypeRef refB)
{
	LFDateRef dateA = (LFDateRef)refA;
	LFDateRef dateB = (LFDateRef)refB;
	
	return (dateA->time == dateB->time);
}

LFHashCode __LFDateHash(LFTypeRef ref)
{
   LFDateRef date = (LFDateRef)ref;
    return (LFHashCode)((float)floor(date->time));
}



LFTypeID LFDateGetTypeID()
{
	return __LFDateTypeID;
}


LFDateRef LFDateCreate()
{
	LFDateRef date = LFRuntimeCreateInstance(__LFDateTypeID, sizeof(struct __LFDate));
	return date;
}

LFDateRef LFDate()
{
	LFDateRef date = LFRuntimeCreateInstance(__LFDateTypeID, sizeof(struct __LFDate));
	return LFAutorelease(date);
}



LFDateRef LFDateCreateWithTimeInterval(LFTimeInterval interval)
{
	LFDateRef date = LFRuntimeCreateInstance(__LFDateTypeID, sizeof(struct __LFDate));
	date->time = interval;
	return date;
}

LFDateRef LFDateWithTimeInterval(LFTimeInterval interval)
{
	return LFAutorelease(LFDateCreateWithTimeInterval(interval));
}



LFDateRef LFDateCreateWithTimeIntervalSinceNow(LFTimeInterval interval)
{
	LFDateRef date = LFRuntimeCreateInstance(__LFDateTypeID, sizeof(struct __LFDate));
	date->time += interval;
	return date;
}

LFDateRef LFDateWithTimeIntervalSinceNow(LFTimeInterval interval)
{
	return LFAutorelease(LFDateCreateWithTimeIntervalSinceNow(interval));
}



LFDateRef LFDateCreateWithTimeIntervalSinceReferenceDate(LFTimeInterval interval)
{
	LFDateRef date = LFRuntimeCreateInstance(__LFDateTypeID, sizeof(struct __LFDate));
	date->time = kLFTimeIntervalSince1970 + interval;
	return date;
}

LFDateRef LFDateWithTimeIntervalSinceReferenceDate(LFTimeInterval interval)
{
	return LFAutorelease(LFDateCreateWithTimeIntervalSinceReferenceDate(interval));
}



void LFDateUpdateDate(LFDateRef date)
{
	LFRuntimeLock(date);
	if(LFZombieScribble(date, "LFDateUpdateDate") || !LFRuntimeValidate(date, __LFDateTypeID))
	{
		LFRuntimeUnlock(date);
		return;
	}
	
	date->time = LFTimeIntervalGetCurrent();
	LFRuntimeUnlock(date);
}

void LFDateAddTimeInterval(LFDateRef date, LFTimeInterval interval)
{
	LFRuntimeLock(date);
	if(LFZombieScribble(date, "LFDateAddTimeInterval") || !LFRuntimeValidate(date, __LFDateTypeID))
	{
		LFRuntimeUnlock(date);
		return;
	}
	
	date->time += interval;
	LFRuntimeUnlock(date);
}



LFTimeInterval LFDateGetTimeIntervalSinceDate(LFDateRef date, LFDateRef otherDate) 
{
	LFRuntimeLock(date);
	LFRuntimeLock(otherDate);
	
	if(LFZombieScribble(date, "LFDateGetTimeIntervalSinceDate") || LFZombieScribble(otherDate, "LFDateGetTimeIntervalSinceDate") ||
	   !LFRuntimeValidate(date, __LFDateTypeID) || !LFRuntimeValidate(otherDate, __LFDateTypeID))
	{
		LFRuntimeUnlock(date);
		LFRuntimeUnlock(otherDate);
		
		return 0.0;
	}
	
    LFTimeInterval interval = date->time - otherDate->time;
	
	LFRuntimeUnlock(date);
	LFRuntimeUnlock(otherDate);
	
	return interval;
}

LFTimeInterval LFDateGetTimeIntervalSince1970(LFDateRef date)
{
	LFRuntimeLock(date);
	
	if(LFZombieScribble(date, "LFDateGetTimeIntervalSince1970") || !LFRuntimeValidate(date, __LFDateTypeID))
	{
		LFRuntimeUnlock(date);
		return 0.0;
	}
	
	
	LFTimeInterval interval = date->time + kLFTimeIntervalSince1970;
	LFRuntimeUnlock(date);
	
	return interval;
}

LFComparisonResult LFDateCompare(LFDateRef date, LFDateRef otherDate)
{
	LFRuntimeLock(date);
	LFRuntimeLock(otherDate);
	
	if(LFZombieScribble(date, "LFDateCompare") || LFZombieScribble(otherDate, "LFDateCompare") ||
	   !LFRuntimeValidate(date, __LFDateTypeID) || !LFRuntimeValidate(otherDate, __LFDateTypeID))
	{
		LFRuntimeUnlock(date);
		LFRuntimeUnlock(otherDate);
		
		return kLFCompareLessThan;
	}
	
	LFComparisonResult compResult = kLFCompareEqualTo;
	
    if(date->time < otherDate->time) compResult = kLFCompareLessThan;
    if(date->time > otherDate->time) compResult = kLFCompareGreaterThan;
	
	LFRuntimeUnlock(date);
	LFRuntimeUnlock(otherDate);
	
    return compResult;
}


LFTimeInterval LFTimeIntervalGetCurrent()
{
	LFTimeInterval ret;
	
#ifdef LF_TARGET_OSX
	struct timeval tv;
    gettimeofday(&tv, NULL);
	
    ret = (LFTimeInterval)tv.tv_sec - kLFTimeIntervalSince1970;
    ret += (1.0E-6 * (LFTimeInterval)tv.tv_usec);
#endif
#ifdef LF_TARGET_WIN32
	FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
	
    ret = (LFTimeInterval)ft.dwHighDateTime * 429.49672960;
	
    ret += (LFTimeInterval)ft.dwLowDateTime / 10000000.0;
    ret -= (11644473600.0 + kLFTimeIntervalSince1970);
#endif
	
	return ret;
}
