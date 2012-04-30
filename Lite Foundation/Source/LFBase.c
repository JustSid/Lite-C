//
//  LFBase.c
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

#include "LFBase.h"

#ifdef LF_TARGET_LITEC
#include "LFData.h"
#include "LFAutoreleasePool.h"

LFDataRef LFDataCreateWithBytesNoCopy(uint8_t *bytes, LFIndex size);

LFRange __LFRangeMake(LFIndex loc, LFIndex len) 
{
	LFRange range = (LFRange)malloc(sizeof(struct __LFRange));
	range->location = loc;
	range->length = len;
	
	// Create a wrapping LFDataRef object that gets autoreleased, so we don't leak the memory
	// LFDataCreateWithBytesNoCopy() is a special function that doesn't copy the bytes but instead let the LFDataRef object act as a wrapper for the data
	LFDataRef data = LFDataCreateWithBytesNoCopy(range, sizeof(struct __LFRange));
	LFAutorelease(data);
	
	return range;
}
#else
LFRange __LFRangeMake(LFIndex loc, LFIndex len) 
{
    LFRange range;
    range.location = loc;
    range.length = len;
	
    return range;
}
#endif
