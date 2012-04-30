//
//  LFNumber.h
//  Lite Foundation
//
//  Created by Sidney Just on 8/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef _LFNUMBER_H_
#define _LFNUMBER_H_

#include "LFBase.h"
#include "LFRuntime.h"

struct __LFNumber
{
    LFRuntimeBase base;
    
    char type;
};

typedef struct __LFNumber* LFNumberRef;


LF_EXPORT LFNumberRef LFNumberCreateWithChar(char val);
LF_EXPORT LFNumberRef LFNumberCreateWithShort(short val);
LF_EXPORT LFNumberRef LFNumberCreateWithInt(int val);
LF_EXPORT LFNumberRef LFNumberCreateWithLong(long val);

LF_EXPORT LFNumberRef LFNumberCreateWithUnsignedChar(unsigned char val);
LF_EXPORT LFNumberRef LFNumberCreateWithUnsignedShort(unsigned short val);
LF_EXPORT LFNumberRef LFNumberCreateWithUnsignedInt(unsigned int val);
LF_EXPORT LFNumberRef LFNumberCreateWithUnsignedLong(unsigned long val);

LF_EXPORT LFNumberRef LFNumberCreateWithFloat(float val);
LF_EXPORT LFNumberRef LFNumberCreateWithDouble(double val);

#ifdef LF_TARGET_LITEC
LF_EXPORT LFNumberRef LFNumberCreateWithVar(var val);
#endif



LF_EXPORT LFNumberRef LFNumberWithChar(char val);
LF_EXPORT LFNumberRef LFNumberWithShort(short val);
LF_EXPORT LFNumberRef LFNumberWithInt(int val);
LF_EXPORT LFNumberRef LFNumberWithLong(long val);

LF_EXPORT LFNumberRef LFNumberWithUnsignedChar(unsigned char val);
LF_EXPORT LFNumberRef LFNumberWithUnsignedShort(unsigned short val);
LF_EXPORT LFNumberRef LFNumberWithUnsignedInt(unsigned int val);
LF_EXPORT LFNumberRef LFNumberWithUnsignedLong(unsigned long val);

LF_EXPORT LFNumberRef LFNumberWithFloat(float val);
LF_EXPORT LFNumberRef LFNumberWithDouble(double val);

#ifdef LF_TARGET_LITEC
LF_EXPORT LFNumberRef LFNumberWithVar(var val);
#endif



LF_EXPORT char LFNumberCharValue(LFNumberRef number);
LF_EXPORT short LFNumberShortValue(LFNumberRef number);
LF_EXPORT int LFNumberIntValue(LFNumberRef number);
LF_EXPORT long LFNumberLongValue(LFNumberRef number);

LF_EXPORT unsigned char LFNumberUnsignedCharValue(LFNumberRef number);
LF_EXPORT unsigned short LFNumberUnsignedShortValue(LFNumberRef number);
LF_EXPORT unsigned int LFNumberUnsignedIntValue(LFNumberRef number);
LF_EXPORT unsigned long LFNumberUnsignedLongValue(LFNumberRef number);

LF_EXPORT float LFNumberFloatValue(LFNumberRef number);
LF_EXPORT double LFNumberDoubleValue(LFNumberRef number);

#ifdef LF_TARGET_LITEC
LF_EXPORT var LFNumberVarValue(LFNumberRef number);
#endif

#endif
