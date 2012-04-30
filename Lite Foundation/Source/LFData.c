//
//  LFData.c
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

#include "LFData.h"
#include "LFInternal.h"

void __LFDataInit(LFTypeRef ref);
void __LFDataDealloc(LFTypeRef ref);

static LFRuntimeClass __LFDataClass;
static LFTypeID __LFDataTypeID = kLFInvalidTypeID;

void __LFRuntimeInitData()
{
	__LFDataClass.version = 1;
	__LFDataClass.name = "LFData";
	
	__LFDataClass.init = __LFDataInit;
	__LFDataClass.copy = NULL;
	__LFDataClass.dealloc = __LFDataDealloc;
	__LFDataClass.equal = NULL;
	__LFDataClass.hash  = NULL;
	
	__LFDataClass.superClass = kLFInvalidTypeID;
	__LFDataClass.protocolBag = NULL;
	__LFDataTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFDataClass);
}


void __LFDataInit(LFTypeRef ref)
{
	LFDataRef data = (LFDataRef)ref;
	
	data->length = 0;
	data->capacity = 5;
	data->bytes = (uint8_t *)malloc(data->capacity * sizeof(uint8_t));
}

void __LFDataDealloc(LFTypeRef ref)
{
	LFDataRef data = (LFDataRef)ref;
	
	if(data->bytes)
		free(data->bytes);
}

LFTypeID LFDataGetTypeID()
{
	return __LFDataTypeID;
}


LFDataRef LFDataCreate()
{
	LFDataRef data = LFRuntimeCreateInstance(__LFDataTypeID, sizeof(struct __LFData));
	return data;
}

LFDataRef LFDataCreateWithBytes(uint8_t *bytes, LFIndex size)
{
	LFDataRef data = LFRuntimeCreateInstance(__LFDataTypeID, sizeof(struct __LFData));
	if(data)
	{
		if(data->capacity < size)
		{
			data->capacity = size + 5;
			uint8_t *tData = (uint8_t *)realloc(data->bytes, data->capacity * sizeof(uint8_t));
			
			if(!tData)
			{
				LFRelease(data);
				return NULL;
			}
			
			data->bytes = tData;
		}
		
		data->length = size;
		memcpy(data->bytes, bytes, size);
	}
	
	return data;
}

LFDataRef LFDataCreateWithBytesNoCopy(uint8_t *bytes, LFIndex size)
{
	LFDataRef data = LFRuntimeCreateInstance(__LFDataTypeID, sizeof(struct __LFData));
	if(data)
	{
		free(data->bytes);
		
		data->bytes = bytes;
		data->length = size;
		data->capacity = size;
	}
	
	return data;
}



#ifdef LF_INLINE
#define LF_DATA_INLINE LF_INLINE
#else
#define LF_DATA_INLINE
#endif

LF_DATA_INLINE void __LFDataResize(LFDataRef data, LFIndex size)
{
	uint8_t *bytes = (uint8_t *)realloc(data->bytes, size);
	LFAssert1(bytes, "Failed to resize LFData %p", data);
	
	data->capacity = size;
	data->bytes	= bytes;
}



void LFDataAppendBytes(LFDataRef data, uint8_t *bytes, LFIndex size)
{
	LFRuntimeLock(data);
	
	if(LFZombieScribble(data, "LFDataAppendBytes") || !LFRuntimeValidate(data, __LFDataTypeID))
	{		
		LFRuntimeUnlock(data);
		return;
	}
	
	
	if(data->length + size >= data->capacity)
		__LFDataResize(data, data->length + size + 20);
	
	memcpy(&(data->bytes)[data->length], bytes, size);
	data->length += size;
	
	LFRuntimeUnlock(data);
}

void LFDataAppendData(LFDataRef data, LFDataRef appendData)
{
	LFRuntimeLock(appendData);
	LFRuntimeLock(data);
	
	if(LFZombieScribble(data, "LFDataAppendData") ||  LFZombieScribble(appendData, "LFDataAppendData") ||
		!LFRuntimeValidate(data, __LFDataTypeID) || !LFRuntimeValidate(appendData, __LFDataTypeID))
	{
		LFRuntimeUnlock(appendData);
		LFRuntimeUnlock(data);
		return;
	}
	
	
	
	LFDataAppendBytes(data, appendData->bytes, appendData->length);
	LFRuntimeUnlock(appendData);
	LFRuntimeUnlock(data);
}

void LFDataGetBytesInRange(LFDataRef data, LFRange range, uint8_t *buffer)
{
	LFRuntimeLock(data);
	if(LFZombieScribble(data, "LFDataGetBytesInRange") || !LFRuntimeValidate(data, __LFDataTypeID))
	{
		LFRuntimeUnlock(data);
		return;
	}
	
	if(range.location > data->length)
	{
		LFRuntimeUnlock(data);
		return;
	}
	
	if(range.location + range.length > data->length)
		range.length = data->length - range.location;
	
	memcpy(buffer, &(data->bytes)[range.location], range.length);
	LFRuntimeUnlock(data);
}

void LFDataReplaceBytesInRange(LFDataRef data, LFRange range, uint8_t *buffer, LFIndex length)
{
    LFRuntimeLock(data);
	if(LFZombieScribble(data, "LFDataReplaceBytesInRange") || !LFRuntimeValidate(data, __LFDataTypeID))
	{
		LFRuntimeUnlock(data);
		return;
	}
    
    if(data->length < range.location + range.length)
        range.length = data->length - range.location;
    
    if(range.location >= data->length)
    {
        LFRuntimeUnlock(data);
        return;
    }
    
    
    if(length == range.length)
    {
        memcpy(&(data->bytes)[range.location], buffer, length);
    }
    else
    {
        if(length > range.length)
        {
            LFIndex diff = length - range.length;
            
            if(data->capacity <= data->length + diff)
                __LFDataResize(data, data->length + diff + 10);
            
            LFIndex rLength = data->length - range.location;
            void *tBuffer = (void *)malloc(rLength * sizeof(uint8_t));
            
            memcpy(tBuffer, &(data->bytes)[range.location], rLength);
            memcpy(&(data->bytes)[range.location + range.length], tBuffer, rLength);
            memcpy(&(data->bytes)[range.location], buffer, length);
            
            free(tBuffer);
            
            data->length += diff;
        }
        else
        {
            memcpy(&(data->bytes)[range.location], buffer, length);
            memcpy(&(data->bytes)[range.location + length], &(data->bytes)[range.location + range.length], data->length - (range.location + range.length));
            
            data->length += length - range.length;
        }
    }
    
    LFRuntimeUnlock(data);
}

LFIndex LFDataGetLength(LFDataRef data)
{
	LFRuntimeLock(data);
	if(LFZombieScribble(data, "LFDataGetLength") || !LFRuntimeValidate(data, __LFDataTypeID))
	{
		LFRuntimeUnlock(data);
		return 0;
	}
	
	LFIndex length = data->length;
	LFRuntimeUnlock(data);
	
	return length;
}

void LFDataSetLength(LFDataRef data, LFIndex length)
{
    LFRuntimeLock(data);
	if(LFZombieScribble(data, "LFDataSetLength") || !LFRuntimeValidate(data, __LFDataTypeID))
	{
		LFRuntimeUnlock(data);
		return;
	}

    if(data->length > length)
    {
        __LFDataResize(data, length + 10);
        data->length = length;
    }
    else if(data->length < length)
    {
        if(data->capacity <= length)
            __LFDataResize(data, length + 5);
        
        
        memset(&(data->bytes)[data->length], 0, length - data->length);
        data->length = length;
    }
    
    LFRuntimeUnlock(data);
}
