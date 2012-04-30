//
//  LFNumber.m
//  Lite Foundation
//
//  Created by Sidney Just on 8/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "LFNumber.h"
#include "LFAutoreleasePool.h"

#define kLFNumberTypeSInt8      1
#define kLFNumberTypeUInt8      2
#define kLFNumberTypeSInt16     3
#define kLFNumberTypeUInt16     4
#define kLFNumberTypeSInt32     5
#define kLFNumberTypeUInt32     6
#define kLFNumberTypeFloat32    7
#define kLFNumberTypeFloat64    8
#define kLFNumberTypeSInt64     9
#define kLFNumberTypeUInt64     10
#define kLFNumberTypeFixed32    11


size_t __LFnumberTypeSize[] =
{
 /* kLFNumberTypeSInt8 */ 1,
 /* kLFNumberTypeUInt8 */ 1,
 /* kLFNumberTypeSInt16 */ 2,
 /* kLFNumberTypeUInt16 */ 2,
 /* kLFNumberTypeSInt32 */ 4,
 /* kLFNumberTypeUInt32 */ 4,
 /* kLFNumberTypeFloat32 */ 4,
 /* kLFNumberTypeFloat64 */ 8,
 /* kLFNumberTypeSInt64 */ 8,
 /* kLFNumberTypeUInt64 */ 8,
 /* kLFNumberTypeFixed32 */ 4
};


LFTypeRef	__LFNumberCopy(LFTypeRef ref);
//uint8_t		__LFNumberEqual(LFTypeRef refA, LFTypeRef refB);
//LFHashCode	__LFNumberHash(LFTypeRef ref);

static LFRuntimeClass __LFNumberClass;
static LFTypeID __LFNumberTypeID = kLFInvalidTypeID;

void __LFRuntimeInitNumber()
{
	__LFNumberClass.version = 1;
	__LFNumberClass.name = "LFNumber";
	
	__LFNumberClass.init = NULL;
	__LFNumberClass.copy = __LFNumberCopy;
	__LFNumberClass.dealloc = NULL;
	__LFNumberClass.equal = NULL;//__LFNumberEqual;
	__LFNumberClass.hash  = NULL;//__LFNumberHash;
	
	__LFNumberClass.superClass = kLFInvalidTypeID;
	__LFNumberClass.protocolBag = NULL;
	__LFNumberTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFNumberClass);
}


LFTypeRef __LFNumberCopy(LFTypeRef ref)
{
    LFNumberRef source = (LFNumberRef)ref;
    LFNumberRef copy = LFRuntimeCreateInstance(__LFNumberTypeID, sizeof(struct __LFNumber) + __LFnumberTypeSize[source->type]);
    
    copy->type = source->type;
    memcpy((void *)((uintptr_t)(copy))+sizeof(struct __LFNumber), (void *)((uintptr_t)(source))+sizeof(struct __LFNumber), __LFnumberTypeSize[source->type]);
    
    return copy;
    
}

/*uint8_t __LFNumberEqual(LFTypeRef refA, LFTypeRef refB)
{
}

LFHashCode __LFNumberHash(LFTypeRef ref)
{
}*/


#define LFNumberSetValue(number, value) memcpy((void *)((uintptr_t)(number))+sizeof(struct __LFNumber), value, __LFnumberTypeSize[number->type])
#define LFNumberGetValue(number, value) memcpy(value, (void *)((uintptr_t)(number))+sizeof(struct __LFNumber), __LFnumberTypeSize[number->type])

LFNumberRef LFNumberCreateWithValue(void *value, char type)
{
    LFNumberRef number = LFRuntimeCreateInstance(__LFNumberTypeID, sizeof(struct __LFNumber) + __LFnumberTypeSize[type]);
    number->type = type;

    LFNumberSetValue(number, value);
    return number;
}


// Signed

LFNumberRef LFNumberCreateWithChar(char val)
{
    return LFNumberCreateWithValue(&val, kLFNumberTypeSInt8);
}

LFNumberRef LFNumberCreateWithShort(short val)
{
    return LFNumberCreateWithValue(&val, kLFNumberTypeSInt16);
}

LFNumberRef LFNumberCreateWithInt(int val)
{
    return LFNumberCreateWithValue(&val, kLFNumberTypeSInt32);
}

LFNumberRef LFNumberCreateWithLong(long val)
{
#ifdef LF_BUILD_32BIT
    return LFNumberCreateWithValue(&val, kLFNumberTypeSInt32);
#else
    return LFNumberCreateWithValue(&val, kLFNumberTypeSInt64);
#endif
}

// Unsigned

LFNumberRef LFNumberCreateWithUnsignedChar(unsigned char val)
{
    return LFNumberCreateWithValue(&val, kLFNumberTypeUInt8);
}

LFNumberRef LFNumberCreateWithUnsignedShort(unsigned short val)
{
    return LFNumberCreateWithValue(&val, kLFNumberTypeUInt16);
}

LFNumberRef LFNumberCreateWithUnsignedInt(unsigned int val)
{
    return LFNumberCreateWithValue(&val, kLFNumberTypeUInt32);
}

LFNumberRef LFNumberCreateWithUnsignedLong(unsigned long val)
{
#ifdef LF_BUILD_32BIT
    return LFNumberCreateWithValue(&val, kLFNumberTypeUInt32);
#else
    return LFNumberCreateWithValue(&val, kLFNumberTypeUInt64);
#endif
}

// Floating point

LFNumberRef LFNumberCreateWithFloat(float val)
{
    return LFNumberCreateWithValue(&val, kLFNumberTypeFloat32);
}

LFNumberRef LFNumberCreateWithDouble(double val)
{
    return LFNumberCreateWithValue(&val, kLFNumberTypeFloat64);
}

#ifdef LF_TARGET_LITEC
LFNumberRef LFNumberCreateWithVar(var val)
{
    return LFNumberCreateWithValue(&val, kLFNumberTypeFixed32);
}
#endif



LFNumberRef LFNumberWithChar(char val)
{
    return LFAutorelease(LFNumberCreateWithChar(val));
}

LFNumberRef LFNumberWithShort(short val)
{
    return LFAutorelease(LFNumberCreateWithShort(val));
}

LFNumberRef LFNumberWithInt(int val)
{
    return LFAutorelease(LFNumberCreateWithInt(val));
}

LFNumberRef LFNumberWithLong(long val)
{
    return LFAutorelease(LFNumberCreateWithLong(val));
}

LFNumberRef LFNumberWithUnsignedChar(unsigned char val)
{
    return LFAutorelease(LFNumberCreateWithUnsignedChar(val));
}

LFNumberRef LFNumberWithUnsignedShort(unsigned short val)
{
    return LFAutorelease(LFNumberCreateWithUnsignedShort(val));
}

LFNumberRef LFNumberWithUnsignedInt(unsigned int val)
{
    return LFAutorelease(LFNumberCreateWithUnsignedInt(val));
}

LFNumberRef LFNumberWithUnsignedLong(unsigned long val)
{
    return LFAutorelease(LFNumberCreateWithUnsignedLong(val));
}

LFNumberRef LFNumberWithFloat(float val)
{
    return LFAutorelease(LFNumberCreateWithFloat(val));
}

LFNumberRef LFNumberWithDouble(double val)
{
    return LFAutorelease(LFNumberCreateWithDouble(val));
}

#ifdef LF_TARGET_LITEC
LFNumberRef LFNumberWithVar(var val)
{
    return LFAutorelease(LFNumberCreateWithVar(val));
}
#endif



#define LFNumberConvertAndReturn(number, src, dst) src val; LFNumberGetValue(number, &val); return (dst)val;
#define LFNumberConvertTargeted(n, d) switch(n->type){case kLFNumberTypeSInt8:{LFNumberConvertAndReturn(n, char, d)}break;case kLFNumberTypeUInt8:{LFNumberConvertAndReturn(n, unsigned char, d)}break;case kLFNumberTypeSInt16:{LFNumberConvertAndReturn(n, short, d)}break;case kLFNumberTypeUInt16:{LFNumberConvertAndReturn(n, unsigned short, d)}break;case kLFNumberTypeSInt32:{LFNumberConvertAndReturn(n, int, d)}break;case kLFNumberTypeUInt32:{LFNumberConvertAndReturn(n, unsigned int, d)}break;case kLFNumberTypeSInt64:{LFNumberConvertAndReturn(n, long, d)}break;case kLFNumberTypeUInt64:{LFNumberConvertAndReturn(n, unsigned long, d)}break;case kLFNumberTypeFloat32:{printf("FOOO");LFNumberConvertAndReturn(n, float, d)}break;case kLFNumberTypeFloat64:{LFNumberConvertAndReturn(n, double, d)}break;default:break;}
    
char LFNumberCharValue(LFNumberRef number)
{
    LFNumberConvertTargeted(number, char)
    
#ifdef LF_TARGET_LITEC
    if(number->type == kLFNumberTypeFixed32)
    {
        LFNumberConvertAndReturn(number, char, var);
    }
#endif
    
    return 0;
}

short LFNumberShortValue(LFNumberRef number)
{
    LFNumberConvertTargeted(number, short)
    
#ifdef LF_TARGET_LITEC
    if(number->type == kLFNumberTypeFixed32)
    {
        LFNumberConvertAndReturn(number, short, var);
    }
#endif
    
    return 0;
}

int LFNumberIntValue(LFNumberRef number)
{
    LFNumberConvertTargeted(number, int)
    
#ifdef LF_TARGET_LITEC
    if(number->type == kLFNumberTypeFixed32)
    {
        LFNumberConvertAndReturn(number, int, var);
    }
#endif
    
    return 0;
}

long LFNumberLongValue(LFNumberRef number)
{
    LFNumberConvertTargeted(number, long)
    
#ifdef LF_TARGET_LITEC
    if(number->type == kLFNumberTypeFixed32)
    {
        LFNumberConvertAndReturn(number, long, var);
    }
#endif
    
    return 0;
}



unsigned char LFNumberUnsignedCharValue(LFNumberRef number)
{
    LFNumberConvertTargeted(number, unsigned char)
    
#ifdef LF_TARGET_LITEC
    if(number->type == kLFNumberTypeFixed32)
    {
        LFNumberConvertAndReturn(number, unsigned char, var);
    }
#endif
    
    return 0;
}

unsigned short LFNumberUnsignedShortValue(LFNumberRef number)
{
    LFNumberConvertTargeted(number, unsigned short)
    
#ifdef LF_TARGET_LITEC
    if(number->type == kLFNumberTypeFixed32)
    {
        LFNumberConvertAndReturn(number, unsigned short, var);
    }
#endif
    
    return 0;
}

unsigned int LFNumberUnsignedIntValue(LFNumberRef number)
{
    LFNumberConvertTargeted(number, unsigned int)
    
#ifdef LF_TARGET_LITEC
    if(number->type == kLFNumberTypeFixed32)
    {
        LFNumberConvertAndReturn(number, unsigned int, var);
    }
#endif
    
    return 0;
}

unsigned long LFNumberUnsignedLongValue(LFNumberRef number)
{
    LFNumberConvertTargeted(number, unsigned long)
    
#ifdef LF_TARGET_LITEC
    if(number->type == kLFNumberTypeFixed32)
    {
        LFNumberConvertAndReturn(number, unsigned long, var);
    }
#endif
    
    return 0;
}



float LFNumberFloatValue(LFNumberRef number)
{
    LFNumberConvertTargeted(number, float)
    
#ifdef LF_TARGET_LITEC
    if(number->type == kLFNumberTypeFixed32)
    {
        LFNumberConvertAndReturn(number, float, var);
    }
#endif
    
    return 0.0;
}

double LFNumberDoubleValue(LFNumberRef number)
{
    LFNumberConvertTargeted(number, double)
    
#ifdef LF_TARGET_LITEC
    if(number->type == kLFNumberTypeFixed32)
    {
        LFNumberConvertAndReturn(number, double, var);
    }
#endif
    
    return 0.0;
}

#ifdef LF_TARGET_LITEC
var LFNumberVarValue(LFNumberRef number)
{
    LFNumberConvertTargeted(number, var)
    
#ifdef LF_TARGET_LITEC
    if(number->type == kLFNumberTypeFixed32)
    {
        LFNumberConvertAndReturn(number, var, var);
    }
#endif
    
    return 0.0;
}
#endif

