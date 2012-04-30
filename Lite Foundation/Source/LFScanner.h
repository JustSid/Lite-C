//
//  LFScanner.h
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

#ifndef _LFSCANNER_H_
#define _LFSCANNER_H_

/**
 * @defgroup LFScannerRef LFScannerRef
 * @{
 * @brief The LFScannerRef is a object that is able to scan through strings
 *
 * A LFScannerRef object consists out of a string and a location which is used to scan through the scan.
 * A scanner can scan throuh a string by using the LFScannerScanUpToString() function which increases the scanners location. The location is the number of scanned
 * characters from the string and is flexible, thus you are allowed to mutate the string during a scan. This is useful to eg. parse out every occurrences of a certain string pattern.
 *
 * @sa LFStringRef
 **/

#include "LFRuntime.h"
#include "LFBase.h"
#include "LFString.h"
#include "LFCharacterSet.h"

/**
 * The scanners layout
 **/
struct __LFScanner
{
	LFRuntimeBase base;

	/**
	 * True or false, if true, the scanner will scan through the string case sensitive
	 **/
	uint8_t caseSensitive;
	
	/**
	 * The string (retained)
	 * @sa LFStringRef
	 **/
	LFStringRef string;
	/**
	 * The current location
	 **/
	LFIndex index;
	/**
	 * The last known length of the string. This is -1 until the first string. Is used to calculate needed location changes when the string got mutated.
	 **/
	LFIndex lastLength;
};

/**
 * The scanner type
 **/
typedef struct __LFScanner* LFScannerRef;

/**
 * Returns the scanners type ID.
 * @remark Please use the LFScannerCreateWithString() or LFSCannerWithString() function to create a new scanner
 * @sa LFScannerCreateWithString()
 * @sa LFSCannerWithString()
 **/
LF_EXPORT LFTypeID LFScannerGetTypeID();

/**
 * Creates a new scanner with the given string
 * @param string A object of type LFStringRef. Must not be NULL
 * @remark The scanner will retain the string
 **/
LF_EXPORT LFScannerRef LFScannerCreateWithString(LFStringRef string);
/**
 * Same as LFScannerCreateWithString() but returns a autoreleased scanner
 * @sa LFScannerCreateWithString()
 **/
LF_EXPORT LFScannerRef LFScannerWithString(LFStringRef string);

/**
 * Sets the scanners case sensitivity
 * @param caseSensitive true to let the scanner respect the case, otherwise false
 * @remark The default value is true, changing this value will affect all following scans
 **/
LF_EXPORT void LFScannerSetCaseSensitive(LFScannerRef scanner, uint8_t caseSensitive);
/**
 * Sets the scanners new location
 * @param location The location can be out of the strings bound, thus the next scan will return false and LFScannerIsAtEnd() will return true
 **/
LF_EXPORT void LFScannerSetLocation(LFScannerRef scanner, LFIndex location);

/**
 * Returns true if the scanner is case sensitive, otherwise false
 **/
LF_EXPORT uint8_t LFScannerIsCaseSensitive(LFScannerRef scanner);
/**
 * Returns the scanners current location inside the string
 **/
LF_EXPORT LFIndex LFScannerGetLocation(LFScannerRef scanner);


/**
 * Returns true if the scanner scanned through the whole string, otherwise false. You can use this in a loop to scane one time through the whole string
 **/
LF_EXPORT uint8_t LFScannerIsAtEnd(LFScannerRef scanner);

/**
 * Scans in the string up to the next occurrence of stopString.
 * @param stopString The string where the scanner should stop. The scanner will set its location right before the found string
 * @param string If this is set to non-NULL, the scanner will set the value to an autoreleased string containing the scanned characters, or NULL if nothing was scanned
 * @return true If the string was found, false if the scanner reached the end of the string or the first string at the scanners location is the stopString
 * @sa LFScannerScanUpToCharacterFromSet()
 * @sa LFScannerSetCaseSensitive()
 * @sa LFScannerGetLocation()
 **/
LF_EXPORT uint8_t LFScannerScanUpToString(LFScannerRef scanner, LFStringRef stopString, LFStringRef *string);

/**
 * Scans in the string up to the first character that is part of the given character set.
 * @remark The returned value and string is set by the same rules like in LFScannerScanUpToString()
 * @sa LFScannerScanUpToString()
 **/
LF_EXPORT uint8_t LFScannerScanUpToCharacterFromSet(LFScannerRef scanner, LFCharacterSetRef characterSet, LFStringRef *string);

/**
 * @}
 **/

#endif
