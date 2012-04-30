//
//  LFDate.h
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

#ifndef _LFDATE_H_
#define _LFDATE_H_

/**
 * @defgroup LFDateRef LFDateRef
 * @{
 * @brief The LFDateRef is an abstract wrapper for time
 *
 * A LFDateRef wraps a single point in time, using a double precision floating point. The time is represented using an absolute point in time, the 1st January 2001 GMT.
 * Every passed second is represented by 1.0, thus a time of 1.0 is the 1st January 2001 00:00:01 GMT. A negative time represents a date before 1 January 2001 GMT. <br />
 * The time is stored in an LFTimeInterval variable which yields a sub milisecond precision for a range of ~10k years. A LFDateRef is always in GMT, but one can use the LFDateComponents and
 * LFTimeZone class to transform a LFDateRef into any other time zone.
 **/

#include "LFBase.h"
#include "LFRuntime.h"

/**
 * Type used to hold time. A LFTimerInterval represents time always in seconds!
 **/
typedef double LFTimeInterval;

/**
 * LFDate layout
 **/
struct __LFDate 
{
	LFRuntimeBase base;

	/**
	 * The wrapped time
	 **/
	LFTimeInterval time;
};

/**
 * The date type
 **/
typedef struct __LFDate* LFDateRef;

LF_EXPORT LFTypeID LFDateGetTypeID();

/**
 * Returns the current time interval since the reference date (1st January 2001 GMT).
 **/
LF_EXPORT LFTimeInterval LFTimeIntervalGetCurrent();

/**
 * Creates a new LFDateRef object which holds the current time.
 **/
LF_EXPORT LFDateRef LFDateCreate();
/**
 * Creates a new, autoreleased LFDateRef object which holds the current time.
 **/
LF_EXPORT LFDateRef LFDate();
/**
 * Creates a new LFDateRef object holds the the given time.
 **/
LF_EXPORT LFDateRef LFDateCreateWithTimeInterval(LFTimeInterval interval);
/**
 * Creates a new, autoreleased, LFDateRef object holds the the given time.
 **/
LF_EXPORT LFDateRef LFDateWithTimerInterval(LFTimeInterval interval);
/**
 * Creates a new LFDateRef object which holds the current time plus the given time.
 **/
LF_EXPORT LFDateRef LFDateCreateWithTimeIntervalSinceNow(LFTimeInterval interval);
/**
 * Creates a new, autoreleased, LFDateRef object which holds the current time plus the given time.
 **/
LF_EXPORT LFDateRef LFDateWithTimeIntervalSinceNow(LFTimeInterval interval);
/**
 * Creates a new LFDateRef object based on the reference date (1st Januray 2001) plus the given time.
 **/
LF_EXPORT LFDateRef LFDateCreateWithTimeIntervalSinceReferenceDate(LFTimeInterval interval);
/**
 * Creates a new, autorleased, LFDateRef object based on the reference date (1st Januray 2001) plus the given time.
 **/
LF_EXPORT LFDateRef LFDateWithTimeIntervalSinceReferenceDate(LFTimeInterval interval);

/**
 * Updates the LFDateRef to represent the current time.
 **/
LF_EXPORT void LFDateUpdateDate(LFDateRef date);
/**
 * Adds the given time interval to the date.
 **/
LF_EXPORT void LFDateAddTimeInterval(LFDateRef date, LFTimeInterval interval);

/**
 * Returns the time that passed between the two dates.
 **/
LF_EXPORT LFTimeInterval LFDateGetTimeIntervalSinceDate(LFDateRef date, LFDateRef otherDate);
/**
 * Returns the time that passed since the 1st January 1970.
 **/
LF_EXPORT LFTimeInterval LFDateGetTimeIntervalSince1970(LFDateRef date);

/**
 * Compares both dates.
 **/
LF_EXPORT LFComparisonResult LFDateCompare(LFDateRef date, LFDateRef otherDate);

/**
 * @}
 **/

#endif
