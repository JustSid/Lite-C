//
//  LFDateComponents.c
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

#include "LFDateComponents.h"
#include "LFAutoreleasePool.h"

const uint8_t __LFDateComponentsRAWDaysInMonth[16] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0, 0, 0};
const uint16_t __LFDateComponentsRAWDaysBeforeMonth[16] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365, 0, 0};
const uint16_t __LFDateComponentsRAWDaysAfterMonth[16] = {365, 334, 306, 275, 245, 214, 184, 153, 122, 92, 61, 31, 0, 0, 0, 0};


void		__LFDateComponentsInit(LFTypeRef ref);
LFTypeRef	__LFDateComponentsCopy(LFTypeRef ref);

static LFRuntimeClass __LFDateComponentsClass;
static LFTypeID __LFDateComponentsTypeID = kLFInvalidTypeID;

void __LFRuntimeInitDateComponents()
{
	__LFDateComponentsClass.version = 1;
	__LFDateComponentsClass.name = "LFDateComponents";
	
	__LFDateComponentsClass.init = __LFDateComponentsInit;
	__LFDateComponentsClass.copy = __LFDateComponentsCopy;
	__LFDateComponentsClass.dealloc = NULL;
	__LFDateComponentsClass.equal = NULL;
	__LFDateComponentsClass.hash  = NULL;
	
	__LFDateComponentsClass.superClass = kLFInvalidTypeID;
	__LFDateComponentsClass.protocolBag = NULL;
	__LFDateComponentsTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFDateComponentsClass);
}

void __LFDateComponentsInit(LFTypeRef ref)
{
	LFDateComponentsRef dateComponents = (LFDateComponentsRef)ref;
	
	dateComponents->year = 0;
	dateComponents->month = 0;
	dateComponents->day = 0;
	dateComponents->hour = 0;
	dateComponents->minute = 0;
	dateComponents->second = 0.0;
}

LFTypeRef __LFDateComponentsCopy(LFTypeRef ref)
{
	LFDateComponentsRef source = (LFDateComponentsRef)ref;    
	LFDateComponentsRef copy = LFDateComponentsCreate(source->year, source->month, source->day, source->hour, source->minute, source->second);
	
	return copy;
}


// Helper functions
#ifdef LF_INLINE
	#define LF_DATECOMPONENTS_INLINE LF_INLINE
	#else
	#define LF_DATECOMPONENTS_INLINE 
#endif

LF_DATECOMPONENTS_INLINE uint8_t __LFDateComponentsIsLeapYear(int32_t year)
{
	year = (year + 1) % 400; // correct to nearest multiple of 400 years
	
	if(year < 0) 
		year *= -1;
	
	return (0 == (year & 3) && 100 != year && 200 != year && 300 != year);
}

LF_DATECOMPONENTS_INLINE uint8_t __LFDateComponentsDaysInMonth(int8_t month, int32_t year, uint8_t leap) 
{
	return __LFDateComponentsRAWDaysInMonth[month] + (month == 2 && leap);
}

LF_DATECOMPONENTS_INLINE uint16_t __LFDateComponentsDaysBeforeMonth(int8_t month, int32_t year, uint8_t leap)
{
	return __LFDateComponentsRAWDaysBeforeMonth[month] + (month > 2 && leap);
}

LF_DATECOMPONENTS_INLINE uint16_t __LFDateComponentsDaysAfterMonth(int8_t month, int32_t year, uint8_t leap) 
{
	return __LFDateComponentsRAWDaysAfterMonth[month] + (month < 2 && leap);
}

LF_DATECOMPONENTS_INLINE int32_t __LFDateComponentsDoubleModToInt(double d, int32_t modulus) 
{
	int32_t res = (int32_t)(float)floor(d - floor(d / modulus) * modulus);
	
	if(res < 0) 
		res += modulus;
	
	return res;
}

LF_DATECOMPONENTS_INLINE double __LFDateComponentsDoubleMod(double d, int32_t modulus) 
{
	double res = d - floor(d / modulus) * modulus;
	
	if(res < 0.0) 
		res += (double)modulus;
	
	return res;
}


LFTimeInterval __LFDateComponentsTimeIntervalFromYearMonthDay(int32_t year, int8_t month, int8_t day) 
{
	LFTimeInterval interval = 0.0;
	int32_t index;
	
	if(year < 0) 
	{
		for(index=year; index<0; index++)
		{
			interval -= __LFDateComponentsDaysAfterMonth(0, index, __LFDateComponentsIsLeapYear(index));
		}
	} 
	else 
	{
		for(index=year-1; 0<=index; index--)
		{
			interval += __LFDateComponentsDaysAfterMonth(0, index, __LFDateComponentsIsLeapYear(index));
		}
	}
	
	
	interval += __LFDateComponentsDaysBeforeMonth(month, year, __LFDateComponentsIsLeapYear(year)) + day - 1;
	return interval;
}

void __LFDateComponentsYearMonthDayFromTimeInterval(LFTimeInterval absolute, int32_t *year, int8_t *month, int8_t *day)
{
	int32_t b = absolute / 146097;
	int32_t _year = b * 400;
	
	uint16_t ydays;
	absolute -= b * 146097;
	
	while(absolute < 0) 
	{
		_year -= 1;
		absolute += __LFDateComponentsDaysAfterMonth(0, _year, __LFDateComponentsIsLeapYear(_year));
	}
	
	
	ydays = __LFDateComponentsDaysAfterMonth(0, _year, __LFDateComponentsIsLeapYear(_year));
	
	while(ydays <= absolute) 
	{
		_year += 1;
		absolute -= ydays;
		ydays = __LFDateComponentsDaysAfterMonth(0, _year, __LFDateComponentsIsLeapYear(_year));
	}
	
	
	if(year)
		*year = _year;
	
	if(month != NULL || day != NULL)
	{
		int8_t _month = absolute / 33 + 1;
		uint8_t leap = __LFDateComponentsIsLeapYear(_year);
		
		while(__LFDateComponentsDaysBeforeMonth(_month + 1, _year, leap) <= absolute) 
			_month++;
		
		if(month) 
			*month = _month;
		
		if(day) 
			*day = absolute - __LFDateComponentsDaysBeforeMonth(_month, _year, leap) + 1;
	}
}




// Actual implementation
LFDateComponentsRef LFDateComponentsCreate(int32_t year, int8_t month, int8_t day, int8_t hour, int8_t minute, double second)
{
	LFDateComponentsRef components = LFRuntimeCreateInstance(__LFDateComponentsTypeID, sizeof(struct __LFDateComponents));
	
	components->year = year;
	components->month = month;
	components->day = day;
	components->hour = hour;
	components->minute = minute;
	components->second = second;
	
	return components;
}

LFDateComponentsRef LFDateComponentsGetDifferenceFromDates(LFDateRef date1, LFDateRef date2, LFTimeZoneRef timeZone, LFDateComponentsFlags flags)
{
	LFRuntimeLock(date1);
	LFRuntimeLock(date2);
    LFRuntimeUnlock(timeZone);
	
	if(LFZombieScribble(date1, "LFDateComponentsGetDifferenceFromDates") || LFZombieScribble(date2, "LFDateComponentsGetDifferenceFromDates") ||
       !LFRuntimeValidate(date1, LFDateGetTypeID()) || !LFRuntimeValidate(date2, LFDateGetTypeID()))
	{
		LFRuntimeUnlock(date1);
		LFRuntimeUnlock(date2);
        LFRuntimeUnlock(timeZone);
    
		return NULL;
	}
	
	if(LFZombieScribble(timeZone, "LFDateComponentsGetDifferenceFromDates") || !LFRuntimeValidate(timeZone, LFTimeZoneGetTypeID()))
	{
		LFRuntimeUnlock(date1);
		LFRuntimeUnlock(date2);
        LFRuntimeUnlock(timeZone);
        
		return NULL;
	}
	
	int32_t seconds[5];
	seconds[0] = 366 * 24 * 3600;
	seconds[1] = 31 * 24 * 3600;
	seconds[2] = 24 * 3600;
	seconds[3] = 3600;
	seconds[4] = 60;
	
	
	LFDateComponentsRef components = LFRuntimeCreateInstance(__LFDateComponentsTypeID, sizeof(struct __LFDateComponents));
	
	LFTimeInterval oldTime, newTime = date2->time;
	LFIndex index, increment = -1;

	if(date2->time < date1->time)
	increment = 1;
	
	
	LFAutoreleasePoolRef pool = LFAutoreleasePoolCreate();
	
	int32_t *units = (int32_t *)&components->year;
	for(index=0; index<5; index++) 
	{
		if(flags & (1 << index)) 
		{
			units[index] = -3 * increment + (date1->time - newTime) / seconds[index];
			
			do 
			{
				oldTime = newTime;
				units[index] += increment;
				
				LFDateRef newDate = LFDateComponentsDateByAddingDateComponents(components, date2, timeZone);
				newTime = newDate->time;
			} while((increment == 1 && newTime <= date1->time) || (increment == -1 && date1->time <= newTime));
			
			units[index] -= increment;
			newTime = oldTime;
			
			LFAutoreleasePoolDrain(pool);
		}
	}
	
	if(flags & kLFSecondsComponent)
	{
		components->second = date1->time - newTime;
	}
	
	LFRelease(pool);
	
	LFRuntimeUnlock(date1);
	LFRuntimeUnlock(date2);
    LFRuntimeUnlock(timeZone);
    
	return LFAutorelease(components);
}

LFDateComponentsRef LFDateComponentsCreateFromDate(LFDateRef date, LFTimeZoneRef timeZone)
{
	LFRuntimeLock(date);
	LFRuntimeLock(timeZone);
	
	if(LFZombieScribble(date, "LFDateComponentsCreateFromDate") || LFZombieScribble(timeZone, "LFDateComponentsCreateFromDate") ||
       !LFRuntimeValidate(date, LFDateGetTypeID()) || !LFRuntimeValidate(timeZone, LFTimeZoneGetTypeID()))
	{
		LFRuntimeUnlock(date);
		LFRuntimeUnlock(timeZone);
		return NULL;
	}
	
	
	LFDateComponentsRef dateComponents = LFRuntimeCreateInstance(__LFDateComponentsTypeID, sizeof(struct __LFDateComponents));
	LFTimeInterval timeInterval = date->time; 
	
	int32_t absolute, year;
	int8_t month, day;
	
	if(timeZone)
	{
		timeInterval += LFTimeZoneGetTimeIntervalFromGMT(timeZone, timeInterval);
	}
	
	absolute = (int32_t)(float)floor(timeInterval / 86400.0);
	__LFDateComponentsYearMonthDayFromTimeInterval(absolute, &year, &month, &day);
	
	
	dateComponents->year = year + 2001;
	dateComponents->month = month;
	dateComponents->day = day;
	dateComponents->hour = __LFDateComponentsDoubleModToInt(floor(timeInterval / 3600.0), 24);
	dateComponents->minute = __LFDateComponentsDoubleModToInt(floor(timeInterval / 60.0), 60);
	dateComponents->second = __LFDateComponentsDoubleMod(timeInterval, 60);
	
	LFRuntimeUnlock(date);
	LFRuntimeUnlock(timeZone);
	
	return dateComponents;
}

LFDateRef LFDateComponentsGetDate(LFDateComponentsRef dateComponents, LFTimeZoneRef timeZone)
{
	LFRuntimeLock(dateComponents);
	LFRuntimeLock(timeZone);
	
	if(LFZombieScribble(dateComponents, "LFDateComponentsGetDate") || LFZombieScribble(timeZone, "LFDateComponentsGetDate") ||
       !LFRuntimeValidate(dateComponents, __LFDateComponentsTypeID) || !LFRuntimeValidate(timeZone, LFTimeZoneGetTypeID()))
	{
		LFRuntimeUnlock(dateComponents);
		LFRuntimeUnlock(timeZone);
		return NULL;
	}
	
	
	LFTimeInterval time, offset0, offset1;
	
	time = 86400.0 * __LFDateComponentsTimeIntervalFromYearMonthDay(dateComponents->year - 2001, dateComponents->month, dateComponents->day);
	time += (3600.0 * dateComponents->hour) + (60.0 * dateComponents->minute) + dateComponents->second;
	
	if(timeZone) 
	{
		offset0 = LFTimeZoneGetTimeIntervalFromGMT(timeZone, time);
		offset1 = LFTimeZoneGetTimeIntervalFromGMT(timeZone, time - offset0);
		
		time -= offset1;
	}
	
	
	LFRuntimeUnlock(dateComponents);
	LFRuntimeUnlock(timeZone);
	return LFAutorelease(LFDateCreateWithTimeInterval(time));
}

LFDateRef LFDateComponentsDateByAddingDateComponents(LFDateComponentsRef dateComponents, LFDateRef date, LFTimeZoneRef timeZone)
{
	LFRuntimeLock(dateComponents);
	LFRuntimeLock(date);
	LFRuntimeLock(timeZone);
	
    static char *fnctName = "LFDateComponentsDateByAddingDateComponents";
    
	if(LFZombieScribble(dateComponents, fnctName) || LFZombieScribble(date, fnctName) || LFZombieScribble(timeZone, fnctName) ||
       !LFRuntimeValidate(dateComponents, __LFDateComponentsTypeID) || !LFRuntimeValidate(date, LFDateGetTypeID()) ||
       !LFRuntimeValidate(timeZone, LFTimeZoneGetTypeID()))
	{
		LFRuntimeUnlock(dateComponents);
		LFRuntimeUnlock(date);
		LFRuntimeUnlock(timeZone);
        
		return NULL;
	}
	
	
	LFDateComponentsRef components;
	LFTimeInterval interval;
	uint8_t monthdays;
	
	components = LFDateComponentsCreateFromDate(date, timeZone);
	components->year += dateComponents->year;
	components->month += dateComponents->month;
	
	while(components->month >= 12)
	{
		components->month -= 12;
		components->year ++;
	}
	
	while(components->month < 1)
	{
		components->month += 12;
		components->year --;
	}
	
	
	monthdays = __LFDateComponentsDaysInMonth(components->month, components->year - 2001, __LFDateComponentsIsLeapYear(components->year - 2001));
	if(monthdays < components->day)
	{
		// Clamp to new month
		components->day = monthdays;
	}
	
	components->day += dateComponents->day;
	
	while(components->day >= monthdays)
	{
		components->month ++;
		
		if(components->month >= 12)
		{
			components->month -= 12;
			components->year ++;
		}
		
		components->day -= monthdays;
		monthdays = __LFDateComponentsDaysInMonth(components->month, components->year - 2001, __LFDateComponentsIsLeapYear(components->year - 2001));
	}
	
	while(components->day < 1)
	{
		components->month --;
		
		if(components->month < 1)
		{
			components->month += 12;
			components->year --;
		}
		
		monthdays = __LFDateComponentsDaysInMonth(components->month, components->year - 2001, __LFDateComponentsIsLeapYear(components->year - 2001));
		components->day -= monthdays;
	}
	
	
	LFDateRef newDate = LFDateComponentsGetDate(components, timeZone); 
	interval = newDate->time;
	interval = interval + (3600.0 * dateComponents->hour) + (60 * dateComponents->minute) + dateComponents->second;

	LFRuntimeUnlock(dateComponents);
	LFRuntimeUnlock(date);
	LFRuntimeUnlock(timeZone);
	
	return LFAutorelease(LFDateCreateWithTimeInterval(interval));
}

int32_t LFDateComponentsGetDayOfWeek(LFDateComponentsRef dateComponents, LFTimeZoneRef timeZone) 
{
	LFRuntimeLock(dateComponents);
	LFRuntimeLock(timeZone);
	
	if(LFZombieScribble(dateComponents, "LFDateComponentsGetDayOfWeek") || LFZombieScribble(timeZone, "LFDateComponentsGetDayOfWeek") ||
       !LFRuntimeValidate(dateComponents, __LFDateComponentsTypeID) || !LFRuntimeValidate(timeZone, LFTimeZoneGetTypeID()))
	{
		LFRuntimeUnlock(dateComponents);
		LFRuntimeUnlock(timeZone);
		return 0;
	}

	

	LFDateRef newDate = LFDateComponentsGetDate(dateComponents, timeZone);
	LFTimeInterval interval = newDate->time;
	int32_t absolute = (int32_t)(float)floor(interval / 86400.0);
	
	if(absolute < 0)
	{
		return (absolute + 1) % 7 + 7;
	}
	
	LFRuntimeUnlock(dateComponents);
	LFRuntimeUnlock(timeZone);
	
	return absolute % 7 + 1;
}

int32_t LFDateComponentsGetDayOfYear(LFDateComponentsRef dateComponents, LFTimeZoneRef timeZone) 
{
	LFRuntimeLock(dateComponents);
	LFRuntimeLock(timeZone);
	
	if(LFZombieScribble(dateComponents, "LFDateComponentsGetDayOfWeek") || LFZombieScribble(timeZone, "LFDateComponentsGetDayOfWeek") ||
       !LFRuntimeValidate(dateComponents, __LFDateComponentsTypeID) || !LFRuntimeValidate(timeZone, LFTimeZoneGetTypeID()))
	{
		LFRuntimeUnlock(dateComponents);
		LFRuntimeUnlock(timeZone);
		return 0;
	}
	
	
	int32_t absolute, year;
	int8_t month, day;
	
	LFDateRef newDate = LFDateComponentsGetDate(dateComponents, timeZone);
	LFTimeInterval interval = newDate->time;
	
	absolute = (int32_t)(float)floor(interval / 86400.0);
	__LFDateComponentsYearMonthDayFromTimeInterval(absolute, &year, &month, &day);
	
	int32_t dayOfYear =  __LFDateComponentsDaysBeforeMonth(month, year, __LFDateComponentsIsLeapYear(year)) + day;
	
	LFRuntimeUnlock(dateComponents);
	LFRuntimeUnlock(timeZone);
	
	return dayOfYear;
}
