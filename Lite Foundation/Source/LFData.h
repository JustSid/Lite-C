//
//  LFData.h
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

#ifndef _LFDATA_H_
#define _LFDATA_H_

/**
 * @defgroup LFDataRef LFDataRef
 * @{
 * @brief The LFDataRef object acts as wrapper for binary data.
 *
 * A LFDataRef object can hold and mutate binary data.
 **/

#include "LFRuntime.h"
#include "LFBase.h"

/**
 * The LFData layout
 **/
struct __LFData
{
	LFRuntimeBase base;
	
	/**
	 * Number of bytes
	 **/
	LFIndex length;
	/**
	 * Number of allocated bytes
	 **/
	LFIndex capacity;
	
	/**
	 * Buffer for the data
	 **/
	uint8_t *bytes;
};

/**
 * The data type
 **/
typedef struct __LFData* LFDataRef;

LF_EXPORT LFTypeID LFDataGetTypeID();

/**
 * Creates a new LFDataRef object
 **/
LF_EXPORT LFDataRef LFDataCreate();
/**
 * Creates a new LFDataRef object containing the given bytes.
 * @param bytes A valid pointer, must not be NULL
 **/
LF_EXPORT LFDataRef LFDataCreateWithBytes(uint8_t *bytes, LFIndex size);

/**
 * Appends the given data to the LFDataRef object
 **/
LF_EXPORT void LFDataAppendData(LFDataRef data, LFDataRef appendData);
/**
 * Appends the given bytes to the LFDataRef object
 **/
LF_EXPORT void LFDataAppendBytes(LFDataRef data, uint8_t *bytes, LFIndex size);

/**
 * Writes the bytes of the given range of the LFDataRef object into the given buffer.
 * @param buffer Must point to an allocated memory area that can hold atleas range.length bytes.
 **/
LF_EXPORT void LFDataGetBytesInRange(LFDataRef data, LFRange range, uint8_t *buffer);
/**
 * Replaces the bytes in the given range with the given buffer. If the length is unequal to the range length, the bytes of the data object will be shifted so
 * that the buffer fits.
 **/
LF_EXPORT void LFDataReplaceBytesInRange(LFDataRef data, LFRange range, uint8_t *buffer, LFIndex length);

/**
 * Returns the number of bytes in the LFDataRef object
 **/
LF_EXPORT LFIndex LFDataGetLength(LFDataRef data);
/**
 *  Sets the length of the receiver. If the length is greater than the current length, the data will be extended and filled with zeroes.
 **/
LF_EXPORT void LFDataSetLength(LFDataRef data, LFIndex length);

/**
 * @}
 **/

#endif
