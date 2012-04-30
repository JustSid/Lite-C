//
//  LFCharacterSet.c
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

#include "LFCharacterSet.h"
#include "LFAutoreleasePool.h"
#include "LFString.h"
#include "LFLock.h"

// Defined in LFLock.c
LFLockRef LFLockGetGlobalLock();

void		__LFCharacterSetInit(LFTypeRef ref);
LFTypeRef	__LFCharacterSetCopy(LFTypeRef ref);

static LFRuntimeClass __LFCharacterSetClass;
static LFTypeID __LFCharacterSetTypeID = kLFInvalidTypeID;

#define __LFCharacterSetPredefinedSets 5
static LFCharacterSetRef __LFCharacterSetPredefinedSetRef[__LFCharacterSetPredefinedSets];

void __LFRuntimeInitCharacterSet()
{
	__LFCharacterSetClass.version = 1;
	__LFCharacterSetClass.name = "LFCharacterSet";
	
	__LFCharacterSetClass.init = __LFCharacterSetInit;
	__LFCharacterSetClass.copy = __LFCharacterSetCopy;
	__LFCharacterSetClass.dealloc = NULL;
	__LFCharacterSetClass.equal = NULL;
	__LFCharacterSetClass.hash  = NULL;
	
	__LFCharacterSetClass.superClass = kLFInvalidTypeID;
	__LFCharacterSetClass.protocolBag = NULL;
	__LFCharacterSetTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFCharacterSetClass);
	
	memset(__LFCharacterSetPredefinedSetRef, 0, __LFCharacterSetPredefinedSets * sizeof(LFCharacterSetRef));
}

void __LFCharacterSetInit(LFTypeRef ref)
{
	LFCharacterSetRef cset = (LFCharacterSetRef)ref;
	memset(cset->bucket, 0, 255);
}

LFTypeRef __LFCharacterSetCopy(LFTypeRef ref)
{
	LFCharacterSetRef cset = (LFCharacterSetRef)ref;
	LFCharacterSetRef copy = LFRuntimeCreateInstance(__LFCharacterSetTypeID, sizeof(struct __LFCharacterSet));
	if(copy)
	{
		LFIndex index;
		for(index=0; index<255; index++)
		{
			(copy->bucket)[index] = (cset->bucket)[index];
		}
	}
	
	return copy;
}




LFTypeID LFCharacterSetGetTypeID()
{
	return __LFCharacterSetTypeID;
}


LFCharacterSetRef LFCharacterSetCreateWithString(LFTypeRef string)
{
	if(!LFRuntimeValidate(string, LFStringGetTypeID()))
	   return NULL;
	   
	LFStringRef str = (LFStringRef)string;
	
	LFCharacterSetRef cset = LFRuntimeCreateInstance(__LFCharacterSetTypeID, sizeof(struct __LFCharacterSet));
	if(cset)
	{
		LFIndex index;
		uint8_t *bucket = cset->bucket;
		
		for(index=0; index<str->length; index++)
		{
			bucket[(str->buffer)[index]] = 1;
		}
	}
	
	return cset;
}

LFCharacterSetRef LFCharacterSetPredefinedSet(uint8_t setNum)
{
	if(setNum >= __LFCharacterSetPredefinedSets)
		return NULL;
	
	LFLockRef lock = LFLockGetGlobalLock();
	LFLockLock(lock);
	
	LFCharacterSetRef cset = __LFCharacterSetPredefinedSetRef[setNum];
	if(!cset)
	{
		switch(setNum)
		{
			case kLFCharacterSetAlphanumeric:	
				cset = LFCharacterSetCreateWithString(LFStringWithCString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"));
				break;
				
			case kLFCharacterSetLowercase:
				cset = LFCharacterSetCreateWithString(LFStringWithCString("abcdefghijklmnopqrstuvwxyz"));
				break;
				
			case kLFCharacterSetUppercase:
				cset = LFCharacterSetCreateWithString(LFStringWithCString("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
				break;
				
			case kLFCharacterSetPunctuation:
				cset = LFCharacterSetCreateWithString(LFStringWithCString(".,-_(){}[]"));
				break;
				
			case kLFCharacterSetWhitespace:
				cset = LFCharacterSetCreateWithString(LFStringWithCString(" \n"));
				break;
		}
		
		__LFCharacterSetPredefinedSetRef[setNum] = cset;
	}
	
	LFLockUnlock(lock);
	return cset;
}

LFCharacterSetRef LFCharacterSetInvertedSet(LFCharacterSetRef cset)
{
	LFRuntimeLock(cset);
	if(LFZombieScribble(cset, "LFCharacterSetInvertedSet") || !LFRuntimeValidate(cset, __LFCharacterSetTypeID))
	{
		LFRuntimeUnlock(cset);
		return NULL;
	}
	
	
	LFCharacterSetRef copy = LFCopy(cset);
	if(copy)
	{
		LFIndex index;
		for(index=0; index<255; index++)
		{
			(copy->bucket)[index] = !(copy->bucket)[index];
		}
	}
	
	LFRuntimeUnlock(cset);
	return LFAutorelease(copy);
}



uint8_t LFCharacterSetIsMember(LFCharacterSetRef cset, char character)
{
	LFRuntimeLock(cset);
	
	if(LFZombieScribble(cset, "LFCharacterSetIsMember") || !LFRuntimeValidate(cset, __LFCharacterSetTypeID))
	{
		LFRuntimeUnlock(cset);
		return 0;
	}
	
	
	
	uint8_t isMember = (cset->bucket)[character];
	LFRuntimeUnlock(cset);
	
	return isMember;
}
