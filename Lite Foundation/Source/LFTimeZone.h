//
//  LFTimeZone.h
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

#ifndef _LFTIMEZONE_H_
#define _LFTIMEZONE_H_

/**
 * @defgroup LFTimeZoneRef LFTimeZoneRef
 * @{
 * @brief The LFTimeZoneRef is an abstract wrapper for a time zone
 *
 * A LFTimeZoneRef object wraps a time zone by saving its offset in seconds to GMT. Lite Foundation knows by default 30 time zones;
 * ADT, AFT, AKDT, AKST, AST, CDT, CEST, CET, CST, EDT, EEST, EET, EST, GMT, HKST, HST, JST, MDT, MSD, MSK, MST, NZDT, NZST, PDT,
 * PST, UTC, WEST, WET, YDT and YST.
 **/


#include "LFBase.h"
#include "LFRuntime.h"
#include "LFString.h"
#include "LFDate.h"

/**
 * The LFTimeZone layout
 **/
struct __LFTimeZone
{
	LFRuntimeBase base;
	
	/**
	 * The abbreviation of the time zone, eg. "AFT"
	 **/
	LFStringRef abbreviation;
	/**
	 * The long name of the time zone eg. "Asia/Kabul"
	 **/
	LFStringRef name;
	/**
	 * The offset to GMT
	 **/
	LFTimeInterval offset;
};

/**
 * The time zone type
 **/
typedef struct __LFTimeZone* LFTimeZoneRef;

/**
 * Returns the type ID of the time zone class. Don't use this to create your own instances!
 **/
LF_EXPORT LFTypeID LFTimeZoneGetTypeID();

/**
 * Creates a new time zone with the given GMT offset, name and abbreviation. You can use this function to create time zones that aren't supported by default.
 **/
LF_EXPORT LFTimeZoneRef LFTimeZoneCreate(LFTimeInterval interval, LFStringRef name, LFStringRef abbreviation);
/**
 * Creates a new time zone by selecting a known time zone that matches the given GMT offset.
 **/
LF_EXPORT LFTimeZoneRef LFTimeZoneCreateWithTimeIntervalSinceGMT(LFTimeInterval interval);
/**
 * Creates a new time zone by selecting a known time zone which matches the given abbreviation (eg. "UTC").
 * @return A new LFTimeZoneRef object if the time zone is known, otherwise NULL
 **/
LF_EXPORT LFTimeZoneRef LFTimeZoneCreateWithAbbreviation(LFStringRef abbreviation);
/**
 * Returns an autoreleased instance of the time zone that matches the given abbreviation.
 **/
LF_EXPORT LFTimeZoneRef LFTimeZoneWithAbbreviation(LFStringRef abbreviation);

/**
 * Returns the difference between the time zone and GMT at the specified time.
 **/
LF_EXPORT LFTimeInterval LFTimeZoneGetTimeIntervalFromGMT(LFTimeZoneRef timeZone, LFTimeInterval time);

/**
 * @}
 **/

#endif
