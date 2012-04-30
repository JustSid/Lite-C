//
//  LFDateComponents.h
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

#ifndef _LFDATECOMPONENTS_H_
#define _LFDATECOMPONENTS_H_

/**
 * @defgroup LFDateComponentsRef LFDateComponentsRef
 * @{
 * @brief The LFDateComponentsRef is an object that can calculate with dates
 *
 * A LFDateComponentsRef can be used to transform a LFDateRef into a human readable format (with years, months, days etc) or to calculate with multiple dates.
 * The LFDateComponentsRef also takes a time zone into account, allowing one to transform a LFDateRef from its fixed GMT time zone into another time zone.
 **/

#include "LFRuntime.h"
#include "LFBase.h"
#include "LFDate.h"
#include "LFTimeZone.h"

/**
 * LFDateComponentsRef layout
 **/
struct __LFDateComponents
{
	LFRuntimeBase base;
	
	/**
	 * The wrapped year(s)
	 **/
	int32_t year;
	/**
	 * The wrapped month(s)
	 **/
    int32_t month;
	/**
	 * The wrapped day(s)
	 **/
    int32_t day;
	/**
	 * The wrapped hour(s)
	 **/
    int32_t hour;
	/**
	 * The wrapped minute(s)
	 **/
    int32_t minute;
	/**
	 * The wrapped second(s) and milisecond(s)
	 **/
    double second;
};

/**
 * The date component type
 **/
typedef struct __LFDateComponents* LFDateComponentsRef;

/**
 * Flag to represent the year component
 **/
#define kLFYearsComponent	(1 << 0)
/**
 * Flag to represent the month component
 **/
#define kLFMonthsComponent	(1 << 1)
/**
 * Flag to represent the day component
 **/
#define kLFDaysComponent	(1 << 2)
/**
 * Flag to represent the hour component
 **/
#define kLFHoursComponent	(1 << 3)
/**
 * Flag to represent the minute component
 **/
#define kLFMinutesComponent (1 << 4)
/**
 * Flag to represent the second component
 **/
#define kLFSecondsComponent (1 << 5)
/**
 * Flag that represents all components
 **/
#define kLFAllComponents kLFYearsComponent | kLFMonthsComponent | kLFDaysComponent | kLFHoursComponent | kLFMinutesComponent | kLFSecondsComponent

/**
 * Type to hold date component flags
 **/
typedef uint8_t LFDateComponentsFlags;


/**
 * Returns a new LFDateComponentsRef using the given year, month, day, hour, minute and second.
 **/
LF_EXPORT LFDateComponentsRef LFDateComponentsCreate(int32_t year, int8_t month, int8_t day, int8_t hour, int8_t minute, double second);
/**
 * Returns a new LFDateComponentsRef by converting the given date into its components using the given time zone and by respecting leap years.
 * @param timeZone A valid LFTimeZoneRef object or NULL for GMT
 **/
LF_EXPORT LFDateComponentsRef LFDateComponentsCreateFromDate(LFDateRef date, LFTimeZoneRef timeZone);
/**
 * Returns a new LFDateComponentsRef containing the difference between date1 and date2.
 * @param timeZone A valid LFTimeZoneRef object or NULL for GMT
 * @param flags Bit field containing which components should be extracted. If you pass eg. only kLFMinutesComponent, the LFDateComponentRef will only contain the difference between both dates in minutes
 **/
LF_EXPORT LFDateComponentsRef LFDateComponentsGetDifferenceFromDates(LFDateRef date1, LFDateRef date2, LFTimeZoneRef timeZone, LFDateComponentsFlags flags);

/**
 * Returns an autoreleased LFDateRef object which represents the given LFDateComponentsRef object
 * @param timeZone A valid LFTimeZoneRef object or NULL for GMT
 **/
LF_EXPORT LFDateRef LFDateComponentsGetDate(LFDateComponentsRef dateComponents, LFTimeZoneRef timeZone);
/**
 * Returns a LFDateRef object containing the given date components plus the given date.
 * @param timeZone A valid LFTimeZoneRef object or NULL for GMT
 **/
LF_EXPORT LFDateRef LFDateComponentsDateByAddingDateComponents(LFDateComponentsRef dateComponents, LFDateRef date, LFTimeZoneRef timeZone);

/**
 * Returns the day of the week of the given date component
 * @return 1 for monday, 2 for tuesday etc.
 **/
LF_EXPORT int32_t LFDateComponentsGetDayOfWeek(LFDateComponentsRef dateComponents, LFTimeZoneRef timeZone);
/**
 * Returns the day of the year of the given date component
 **/
LF_EXPORT int32_t LFDateComponentsGetDayOfYear(LFDateComponentsRef dateComponents, LFTimeZoneRef timeZone);

/**
 * @}
 **/

#endif
