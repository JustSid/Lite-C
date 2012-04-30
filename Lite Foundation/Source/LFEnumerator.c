//
//  LFEnumerator.c
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

#include "LFEnumerator.h"

//------------------------------
// Runtime functions
//------------------------------

void __LFEnumeratorInit(LFTypeRef ref);
void __LFEnumeratorDealloc(LFTypeRef ref);

static LFRuntimeClass __LFEnumeratorClass;
static LFTypeID __LFEnumeratorTypeID = kLFInvalidTypeID;


static LFProtocolHeader __LFEnumeratorProtocolHeader;
static LFProtocolSel __LFEnumeratorProtocolSel[1];

static LFProtocolID  __LFEnumeratorProtocolID	= kLFProtocolInvalidID;
static LFProtocolSel __LFEnumeratorEnumerateSel	= "LFEnumeratorEnumerateSel";


void __LFRuntimeInitEnumerator()
{
	// Enumerator protocol
	__LFEnumeratorProtocolSel[0] = __LFEnumeratorEnumerateSel;
	
	__LFEnumeratorProtocolHeader.version = 0;
	__LFEnumeratorProtocolHeader.name = "LFEnumeratorProtocol";
	__LFEnumeratorProtocolHeader.selectorCount = 1;
	__LFEnumeratorProtocolHeader.selectors = __LFEnumeratorProtocolSel;
	
	__LFEnumeratorProtocolID = LFRuntimeRegisterProtocol((const LFProtocolHeader *)&__LFEnumeratorProtocolHeader);
	
	
	// Enumerator class
	__LFEnumeratorClass.version = 1;
	__LFEnumeratorClass.name = "LFEnumerator";
	
	__LFEnumeratorClass.init = __LFEnumeratorInit;
	__LFEnumeratorClass.copy = NULL;
	__LFEnumeratorClass.dealloc = __LFEnumeratorDealloc;
	__LFEnumeratorClass.equal = NULL;
	__LFEnumeratorClass.hash  = NULL;
	
	__LFEnumeratorClass.superClass = kLFInvalidTypeID;
	__LFEnumeratorClass.protocolBag = NULL;
	__LFEnumeratorTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFEnumeratorClass);
}

void __LFEnumeratorInit(LFTypeRef ref)
{
	LFEnumeratorRef enumerator = (LFEnumeratorRef)ref;
	
	enumerator->state = 0;
	enumerator->itemCount = 0;
	enumerator->__itemIndex = 0;
}

void __LFEnumeratorDealloc(LFTypeRef ref)
{
	LFEnumeratorRef enumerator = (LFEnumeratorRef)ref;
	LFRelease(enumerator->__object);
}


LFTypeID LFEnumeratorGetTypeID()
{
	return __LFEnumeratorTypeID;
}


LFEnumeratorRef LFEnumeratorCreate(LFTypeRef ref)
{
	LFRuntimeLock(ref);
	
	if(LFZombieScribble(ref, "LFEnumeratorCreate") || !LFRespondsToSelector(ref, __LFEnumeratorEnumerateSel))
	{
		LFRuntimeUnlock(ref);
		return NULL;
	}
	
    
	LFEnumeratorRef enumerator = LFRuntimeCreateInstance(__LFEnumeratorTypeID, sizeof(struct __LFEnumerator));
	enumerator->__object = LFRetain(ref);
	
	LFRuntimeUnlock(ref);
	return enumerator;
}

LFTypeRef LFEnumeratorNextObject(LFEnumeratorRef enumerator)
{
    if(!enumerator)
        return NULL;
    
	LFRuntimeLock(enumerator);
	LFRuntimeLock(enumerator->__object);
	
	if(LFZombieScribble(enumerator, "LFEnumeratorNextObject") || !LFRuntimeValidate(enumerator, __LFEnumeratorTypeID))
	{
		LFRuntimeUnlock(enumerator->__object);
		LFRuntimeUnlock(enumerator);
		
		return NULL;
	}
	
	
	if(enumerator->state == 2)
	{
		LFRuntimeUnlock(enumerator->__object);
		LFRuntimeUnlock(enumerator);
		
		return NULL;
	}
	
	if(enumerator->state == 0)
	{
		LFRuntimeInvoke2(enumerator->__object, __LFEnumeratorEnumerateSel, enumerator, (LFTypeRef)__kLFEnumeratorMaxItems);
		enumerator->state = 1;
		
		if(enumerator->itemCount > 0)
		{
			enumerator->__itemIndex = 1;
			
			LFTypeRef object = enumerator->itemsPtr[0];
			
			LFRuntimeUnlock(enumerator->__object);
			LFRuntimeUnlock(enumerator);
			
			return object;
		}
		else
		{
			enumerator->state = 2;
			
			LFRuntimeUnlock(enumerator->__object);
			LFRuntimeUnlock(enumerator);
			
			return NULL;
		}
	}
	
	
	if(enumerator->__itemIndex < enumerator->itemCount)
	{
		enumerator->__itemIndex ++;
		
		LFTypeRef object = enumerator->itemsPtr[enumerator->__itemIndex-1];
		
		LFRuntimeUnlock(enumerator->__object);
		LFRuntimeUnlock(enumerator);
		
		return object;
	}
	else
	{
		LFRuntimeInvoke2(enumerator->__object, __LFEnumeratorEnumerateSel, enumerator, (LFTypeRef)__kLFEnumeratorMaxItems);
		
		if(enumerator->itemCount > 0)
		{
			enumerator->__itemIndex = 1;
			
			LFTypeRef object = enumerator->itemsPtr[0];
			
			LFRuntimeUnlock(enumerator->__object);
			LFRuntimeUnlock(enumerator);
			
			return object;
		}
		else
		{
			enumerator->state = 2;
			
			LFRuntimeUnlock(enumerator->__object);
			LFRuntimeUnlock(enumerator);
			return NULL;
		}
	}	
	
	LFRuntimeUnlock(enumerator->__object);
	LFRuntimeUnlock(enumerator);
	return NULL;
}


// Enumerator protocol
LFProtocolID LFEnumeratorGetProtocolID()
{
	return __LFEnumeratorProtocolID;
}

LFProtocolSel LFEnumeratorGetSelector(uint8_t selector)
{
	switch(selector)
	{
		case kLFEnumeratorProtocolEnumerateWithObjects:
			return __LFEnumeratorEnumerateSel;
			
		default:
			break;
	}
	
	return NULL;
}
