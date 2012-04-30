//
//  LFTimeZone.c
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
#include "LFTimeZone.h"
#include "LFAutoreleasePool.h"

#define __LFTimeZoneHour(hours) hours * 60 * 60
#define __LFTimeZoneCount 32

char *__LFTimeZoneAbbreviations[__LFTimeZoneCount];
char *__LFTimeZoneNames[__LFTimeZoneCount];
int32_t __LFTimeZoneOffsets[__LFTimeZoneCount];


void __LFTimeZoneInit(LFTypeRef ref);
void __LFTimeZoneDealloc(LFTypeRef ref);

static LFRuntimeClass __LFTimeZoneClass;
static LFTypeID __LFTimeZoneTypeID = kLFInvalidTypeID;

void __LFRuntimeInitTimeZone()
{
	__LFTimeZoneClass.version = 1;
	__LFTimeZoneClass.name = "LFTimeZone";
	
	__LFTimeZoneClass.init = __LFTimeZoneInit;
	__LFTimeZoneClass.copy = NULL;
	__LFTimeZoneClass.dealloc = __LFTimeZoneDealloc;
	__LFTimeZoneClass.equal = NULL;
	__LFTimeZoneClass.hash  = NULL;
	
	__LFTimeZoneClass.superClass = kLFInvalidTypeID;
	__LFTimeZoneClass.protocolBag = NULL;
	__LFTimeZoneTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFTimeZoneClass);
	
	
	__LFTimeZoneAbbreviations[0] = "ADT";
	__LFTimeZoneAbbreviations[1] = "AFT";
	__LFTimeZoneAbbreviations[2] = "AKDT";
	__LFTimeZoneAbbreviations[3] = "AKST";
	__LFTimeZoneAbbreviations[4] = "AST";
	__LFTimeZoneAbbreviations[5] = "CDT";
	__LFTimeZoneAbbreviations[6] = "CEST";
	__LFTimeZoneAbbreviations[7] = "CET";
	__LFTimeZoneAbbreviations[8] = "CST";
	__LFTimeZoneAbbreviations[9] = "EDT";
	__LFTimeZoneAbbreviations[10] = "EEST";
	__LFTimeZoneAbbreviations[11] = "EET";
	__LFTimeZoneAbbreviations[12] = "EST";
	__LFTimeZoneAbbreviations[13] = "GMT";
	__LFTimeZoneAbbreviations[14] = "HKST";
	__LFTimeZoneAbbreviations[15] = "HST";
	__LFTimeZoneAbbreviations[16] = "JST";
	__LFTimeZoneAbbreviations[17] = "MDT";
	__LFTimeZoneAbbreviations[18] = "MSD";
	__LFTimeZoneAbbreviations[19] = "MSK";
	__LFTimeZoneAbbreviations[20] = "MST";
	__LFTimeZoneAbbreviations[21] = "NZDT";
	__LFTimeZoneAbbreviations[22] = "NZST";
	__LFTimeZoneAbbreviations[23] = "PDT";
	__LFTimeZoneAbbreviations[24] = "PST";
	__LFTimeZoneAbbreviations[25] = "UTC";
	__LFTimeZoneAbbreviations[26] = "WEST";
	__LFTimeZoneAbbreviations[27] = "WET";
	__LFTimeZoneAbbreviations[28] = "YDT";
	__LFTimeZoneAbbreviations[29] = "YST";
	
	__LFTimeZoneNames[0] = "America/Halifax";
	__LFTimeZoneNames[1] = "Asia/Kabul";
	__LFTimeZoneNames[2] = "America/Juneau";
	__LFTimeZoneNames[3] = "America/Juneau";
	__LFTimeZoneNames[4] = "America/Halifax";
	__LFTimeZoneNames[5] = "America/Chicago";
	__LFTimeZoneNames[6] = "Europe/Rome";
	__LFTimeZoneNames[7] = "Europe/Rome";
	__LFTimeZoneNames[8] = "America/Chicago";
	__LFTimeZoneNames[9] = "America/New York";
	__LFTimeZoneNames[10] = "Europe/Warsaw";
	__LFTimeZoneNames[11] = "Europe/Warsaw";
	__LFTimeZoneNames[12] = "America/New York";
	__LFTimeZoneNames[13] = "GMT";
	__LFTimeZoneNames[14] = "Asia/Hong Kong";
	__LFTimeZoneNames[15] = "Pacific/Honolulu";
	__LFTimeZoneNames[16] = "Asia/Tokyo";
	__LFTimeZoneNames[17] = "America/Denver";
	__LFTimeZoneNames[18] = "Europe/Moscow";
	__LFTimeZoneNames[19] = "Europe/Moscow";
	__LFTimeZoneNames[20] = "America/Denver";
	__LFTimeZoneNames[21] = "Pacific/Auckland";
	__LFTimeZoneNames[22] = "Pacific/Auckland";
	__LFTimeZoneNames[23] = "America/Los Angeles";
	__LFTimeZoneNames[24] = "America/Los Angeles";
	__LFTimeZoneNames[25] = "UTC";
	__LFTimeZoneNames[26] = "Europe/Paris";
	__LFTimeZoneNames[27] = "Europe/Paris";
	__LFTimeZoneNames[28] = "America/Yakutat";
	__LFTimeZoneNames[29] = "America/Yakutat";
	
	__LFTimeZoneOffsets[0] = __LFTimeZoneHour(-3.0); // "ADT"
	__LFTimeZoneOffsets[1] = __LFTimeZoneHour(4.5); // "AFT"
	__LFTimeZoneOffsets[2] = __LFTimeZoneHour(-8.0); // "AKDT"
	__LFTimeZoneOffsets[3] = __LFTimeZoneHour(-9.0); // "AKST"	
	__LFTimeZoneOffsets[4] = __LFTimeZoneHour(-4.0); // "AST"
	__LFTimeZoneOffsets[5] = __LFTimeZoneHour(10.50); // "CDT"
	__LFTimeZoneOffsets[6] = __LFTimeZoneHour(2.0); // "CEST"
	__LFTimeZoneOffsets[7] = __LFTimeZoneHour(1.0); // "CET"
	__LFTimeZoneOffsets[8] = __LFTimeZoneHour(-6.0); // "CST"
	__LFTimeZoneOffsets[9] = __LFTimeZoneHour(-4.0); // "EDT"
	__LFTimeZoneOffsets[10] = __LFTimeZoneHour(3.0); // "EEST"
	__LFTimeZoneOffsets[11] = __LFTimeZoneHour(2.0); // "EET"
	__LFTimeZoneOffsets[12] = __LFTimeZoneHour(-5.0); // "EST"
	__LFTimeZoneOffsets[13] = __LFTimeZoneHour(0.0); // "GMT"
	__LFTimeZoneOffsets[14] = __LFTimeZoneHour(8.0); // "HKT"
	__LFTimeZoneOffsets[15] = __LFTimeZoneHour(-10.0); // "HST"
	__LFTimeZoneOffsets[16] = __LFTimeZoneHour(9.0); // "JST"
	__LFTimeZoneOffsets[17] = __LFTimeZoneHour(-6.0); // "MDT"
	__LFTimeZoneOffsets[18] = __LFTimeZoneHour(4.0); // "MSD"
	__LFTimeZoneOffsets[19] = __LFTimeZoneHour(3.0); // "MSK"
	__LFTimeZoneOffsets[20] = __LFTimeZoneHour(-7.0); // "MST"
	__LFTimeZoneOffsets[21] = __LFTimeZoneHour(13.0); // "NZDT"
	__LFTimeZoneOffsets[22] = __LFTimeZoneHour(12.0); // "NZST"
	__LFTimeZoneOffsets[23] = __LFTimeZoneHour(-7.0); // "PDT"
	__LFTimeZoneOffsets[24] = __LFTimeZoneHour(-8.0); // "PST"
	__LFTimeZoneOffsets[25] = __LFTimeZoneHour(0.0); // "UTC"
	__LFTimeZoneOffsets[26] = __LFTimeZoneHour(1.0); // "WEST"
	__LFTimeZoneOffsets[27] = __LFTimeZoneHour(0.0); // "WET"
	__LFTimeZoneOffsets[28] = __LFTimeZoneHour(-8.0); // "YDT"
	__LFTimeZoneOffsets[29] = __LFTimeZoneHour(-9.0); // "YST"
}


void __LFTimeZoneInit(LFTypeRef ref)
{
	LFTimeZoneRef zone = (LFTimeZoneRef)ref;
	
	zone->offset = 0.0;
	zone->abbreviation = NULL;
	zone->name = NULL;
}

void __LFTimeZoneDealloc(LFTypeRef ref)
{
	LFTimeZoneRef zone = (LFTimeZoneRef)ref;
	
	LFRelease(zone->abbreviation);
	LFRelease(zone->name);
}


LFTypeID LFTimeZoneGetTypeID()
{
	return __LFTimeZoneTypeID;
}


LFTimeZoneRef LFTimeZoneCreate(LFTimeInterval interval, LFStringRef name, LFStringRef abbreviation)
{
	LFRuntimeLock(name);
	LFRuntimeLock(abbreviation);
	
	if(!LFRuntimeValidate(name, LFStringGetTypeID()) || !LFRuntimeValidate(abbreviation, LFStringGetTypeID()) ||
	   LFZombieScribble(name, "LFTimeZoneCreate") || LFZombieScribble(abbreviation, "LFTimeZoneCreate"))
	{
		LFRuntimeUnlock(name);
		LFRuntimeUnlock(abbreviation);
		
		return NULL;
	}
	
	LFTimeZoneRef zone = LFRuntimeCreateInstance(__LFTimeZoneTypeID, sizeof(struct __LFTimeZone));
	
	zone->name = LFCopy(name);
	zone->abbreviation = LFCopy(abbreviation);
	zone->offset = interval;
	
	LFRuntimeUnlock(name);
	LFRuntimeUnlock(abbreviation);
	return zone;
}

LFTimeZoneRef LFTimeZoneCreateWithTimeIntervalSinceGMT(LFTimeInterval interval)
{
	LFIndex index, mostFitting = 0;
	for(index=0; index<__LFTimeZoneCount; index++)
	{
		if(fabs(__LFTimeZoneOffsets[index] - interval) < fabs(__LFTimeZoneOffsets[mostFitting] - interval))
			mostFitting = index;
	}
	
	return LFTimeZoneCreate(__LFTimeZoneOffsets[mostFitting], LFSTR(__LFTimeZoneNames[mostFitting]), LFSTR(__LFTimeZoneAbbreviations[mostFitting]));
}

LFTimeZoneRef LFTimeZoneCreateWithAbbreviation(LFStringRef abbreviation)
{
	LFRuntimeLock(abbreviation);
	
	if(!LFRuntimeValidate(abbreviation, LFStringGetTypeID()) || LFZombieScribble(abbreviation, "LFTimeZoneCreateWithAbbreviation"))
	{
		LFRuntimeUnlock(abbreviation);
		return NULL;
	}
	
	LFIndex index;
	for(index=0; index<__LFTimeZoneCount; index++)
	{
		if(LFEqual(LFSTR(__LFTimeZoneAbbreviations[index]), abbreviation))
		{
			LFRuntimeUnlock(abbreviation);
			return LFTimeZoneCreate(__LFTimeZoneOffsets[index], LFSTR(__LFTimeZoneNames[index]), LFSTR(__LFTimeZoneAbbreviations[index]));
		}
	}
	
	LFRuntimeUnlock(abbreviation);
	return NULL;
}

LFTimeZoneRef LFTimeZoneWithAbbreviation(LFStringRef abbreviation)
{
	return LFAutorelease(LFTimeZoneCreateWithAbbreviation(abbreviation));
}


LFTimeInterval LFTimeZoneGetTimeIntervalFromGMT(LFTimeZoneRef timeZone, LFTimeInterval time)
{
	LFRuntimeLock(timeZone);
	
	if(!LFRuntimeValidate(timeZone, __LFTimeZoneTypeID) || LFZombieScribble(timeZone, "LFTimeZoneGetTimeIntervalFromGMT"))
	{
		LFRuntimeUnlock(timeZone);
		return 0.0;
	}
	
	
	LFTimeInterval timeZoneTime = time - timeZone->offset;
	LFRuntimeUnlock(timeZone);
	
	return time - timeZoneTime;
}

