//
//  LFProtocol.c
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

#include "LFProtocol.h"
#include "LFRuntime.h"
#include "LFInternal.h"
#include "LFDebugInternal.h"

#ifndef LF_TARGET_LITEC
#define __LFProtocolZombieScribbleRet(object, function, ret) if(LFNoLockZombieScribble(object, function)) {return ret;}
#else
#define __LFProtocolZombieScribbleRet(object, function, ret) if(LFNoLockZombieScribble(object, function)) {return ret;} void *__LFFooTempBar
#endif


#define __LFProtocolTableMax 255

static LFProtocolHeader *__LFProtocolTable[__LFProtocolTableMax];
static uint32_t __LFProtocolTableCount = 0;



void __LFRuntimeInitProtocols()
{
	memset(__LFProtocolTable, 0, __LFProtocolTableMax * sizeof(LFProtocolHeader *));
}

LFProtocolID LFRuntimeRegisterProtocol(const LFProtocolHeader *header)
{
	LFIndex selection;
	uint8_t found = 0;
	
	__LFProtocolTableCount ++;
	selection = __LFProtocolTableCount;
	
	if(__LFProtocolTableCount >= __LFProtocolTableMax)
	{
		// Do a cost intensive search for an unregistered protocol which place can be used
		for(selection=1; selection < __LFProtocolTableMax; selection++)
		{
			if(__LFProtocolTable[selection] == 0)
			{
				found = 1;
				break;
			}
		}
		
			
		if(!found)
		{
			printf("Could not register protocol header, no more entries in the table left!");
			return kLFProtocolInvalidID;
		}
	}
	
	__LFProtocolTable[selection] = (LFProtocolHeader *)header;	
	return __LFProtocolTableCount;
}

void LFRuntimeUnregisterProtocol(LFProtocolID pid)
{
	__LFProtocolTable[pid] = NULL;
}



LFProtocolBody *LFRuntimeCreateProtocolBody(LFProtocolID pid)
{
	LFProtocolHeader *header = __LFProtocolTable[pid];
	if(!header)
		return NULL;
	
	LFProtocolBody *body = (LFProtocolBody *)malloc(sizeof(LFProtocolBody));
	if(body)
	{
		body->protocol = header;
		body->imps = (LFProtocolImp *)malloc(header->selectorCount*sizeof(LFProtocolImp));
		
		int i;
		for(i=0; i<header->selectorCount; i++)
		{
			(body->imps)[i].method = NULL;
			(body->imps)[i].selector = (header->selectors)[i];
		}
	}
	
	return body;
}

void LFRuntimeDestroyProtocolBody(LFProtocolBody *body)
{
	if(body)
	{
		free(body->imps);
		free(body);
	}
}

void LFRuntimeProtocolBodySetIMP(LFProtocolBody *body, LFProtocolSel selector, void *method)
{
	LFProtocolHeader *header = body->protocol;
	
	int i;
	for(i=0; i<header->selectorCount; i++)
	{
		if((body->imps)[i].selector == selector)
		{
			(body->imps)[i].method = method;
			break;
		}
	}
}




LFProtocolBag *LFRuntimeGetProtocolBag(LFTypeRef ref, uint8_t nestedSuper);

// forSuper == force only the search beginning with the super class
LFProtocolImp *LFRuntimeProtocolGetImpForSelector(LFTypeRef ref, LFProtocolSel selector, uint8_t forSuper)
{
	if(!ref)
		return NULL;
	
	uint8_t depth = 0;
	
	if(forSuper)
		depth = 1;
	
	while(1)
	{
		LFProtocolBag *bag = LFRuntimeGetProtocolBag(ref, depth);
		if(bag)
		{
			int i, j;
			for(i=0; i<bag->bodyCount; i++)
			{
				LFProtocolBody *body = (bag->bodys)[i];
				LFProtocolHeader *header = body->protocol;
				
				for(j=0; j<header->selectorCount; i++)
				{
					if((header->selectors)[i] == selector)
					{
						return &(body->imps)[i];
					}
				}
			}
			
			depth ++;
		}
		else
			break;
	}
	
	return NULL;
}

uint8_t LFImplementsProtocol(LFTypeRef ref, LFProtocolID pid)
{
	if(pid == kLFProtocolInvalidID)
		return 0;
	
	__LFProtocolZombieScribbleRet(ref, "LFImplementsProtocol", 0);
	
	LFProtocolHeader *header = __LFProtocolTable[pid];
	LFProtocolBag *bag;
	uint32_t index = 0;
	
	while((bag = LFRuntimeGetProtocolBag(ref, index)))
	{
		if(bag)
		{
			int i;
			for(i=0; i<bag->bodyCount; i++)
			{
				if((bag->bodys)[i]->protocol == header)
					return 1;
			}
		}
		
		index ++;
	}
		
	return 0;
}

uint8_t LFRespondsToSelector(LFTypeRef ref, LFProtocolSel selector)
{
	if(!selector)
		return 0;
	
	__LFProtocolZombieScribbleRet(ref, "LFImplementsProtocol", 0);
	
	LFProtocolBag *bag;
	uint32_t index = 0;
	
	while((bag = LFRuntimeGetProtocolBag(ref, index)))
	{
		if(bag)
		{
			int i, j;
			for(i=0; i<bag->bodyCount; i++)
			{
				LFProtocolBody *protocol = (bag->bodys)[i];
				
				for(j=0; j<protocol->protocol->selectorCount; j++)
				{
					if((protocol->imps)[j].selector == selector)
					{
						return 1;
					}
				}					
			}
		}
		
		index ++;
	}
	
	return 0;
}



// Simple invoke
void LFRuntimeInvoke0(LFTypeRef ref, LFProtocolSel selector)
{
    LFRuntimeLock(ref);
    
    if(LFProtocolZombieScribble(ref, "LFRuntimeInvoke0", selector))
    {
        LFRuntimeUnlock(ref);
        return;
    }
	
	LFProtocolImp *imp = LFRuntimeProtocolGetImpForSelector(ref, selector, 0);
	
#ifdef LF_TARGET_LITEC
	void __LFProtocolSignature1_0(LFTypeRef arg1);
	
	__LFProtocolSignature1_0 = imp->method;
	__LFProtocolSignature1_0(ref);
#else
	__LFProtocolSignature1_0 signature = (__LFProtocolSignature1_0)imp->method;
	signature(ref);
#endif
    
    LFRuntimeUnlock(ref);
}

void LFRuntimeInvoke1(LFTypeRef ref, LFProtocolSel selector, LFTypeRef arg1)
{
    LFRuntimeLock(ref);
    
	if(LFProtocolZombieScribble(ref, "LFRuntimeInvoke1", selector))
    {
        LFRuntimeUnlock(ref);
        return;
    }
	
	LFProtocolImp *imp = LFRuntimeProtocolGetImpForSelector(ref, selector, 0);	
	
#ifdef LF_TARGET_LITEC
	void __LFProtocolSignature2_0(LFTypeRef arg1, LFTypeRef arg2);
	
	__LFProtocolSignature2_0 = imp->method;
	__LFProtocolSignature2_0(ref, arg1);
#else
	__LFProtocolSignature2_0 signature = (__LFProtocolSignature2_0)imp->method;
	signature(ref, arg1);
#endif
    
    LFRuntimeUnlock(ref);
}

void LFRuntimeInvoke2(LFTypeRef ref, LFProtocolSel selector, LFTypeRef arg1, LFTypeRef arg2)
{
	LFRuntimeLock(ref);
	
	if(LFProtocolZombieScribble(ref, "LFRuntimeInvoke2", selector))
    {
        LFRuntimeUnlock(ref);
        return;
    }
	
	LFProtocolImp *imp = LFRuntimeProtocolGetImpForSelector(ref, selector, 0);
	
#ifdef LF_TARGET_LITEC
	void __LFProtocolSignature3_0(LFTypeRef arg1, LFTypeRef arg2, LFTypeRef arg3);
	
	__LFProtocolSignature3_0 = imp->method;
	__LFProtocolSignature3_0(ref, arg1, arg2);
#else
	__LFProtocolSignature3_0 signature = (__LFProtocolSignature3_0)imp->method;
	signature(ref, arg1, arg2);
#endif
	
	LFRuntimeUnlock(ref);
}

// Invoke on super class
void LFRuntimeSuperInvoke0(LFTypeRef ref, LFProtocolSel selector)
{
    LFRuntimeLock(ref);
    
    if(LFProtocolZombieScribble(ref, "LFRuntimeSuperInvoke0", selector))
    {
        LFRuntimeUnlock(ref);
        return;
    }

	
	LFProtocolImp *imp = LFRuntimeProtocolGetImpForSelector(ref, selector, 1);
	
#ifdef LF_TARGET_LITEC
	void __LFProtocolSignature1_0(LFTypeRef arg1);
	
	__LFProtocolSignature1_0 = imp->method;
	__LFProtocolSignature1_0(ref);
#else
	__LFProtocolSignature1_0 signature = (__LFProtocolSignature1_0)imp->method;
	signature(ref);
#endif
    
    LFRuntimeUnlock(ref);
}

void LFRuntimeSuperInvoke1(LFTypeRef ref, LFProtocolSel selector, LFTypeRef arg1)
{
    LFRuntimeLock(ref);
    
    if(LFProtocolZombieScribble(ref, "LFRuntimeSuperInvoke1", selector))
    {
        LFRuntimeUnlock(ref);
        return;
    }
	
	LFProtocolImp *imp = LFRuntimeProtocolGetImpForSelector(ref, selector, 1);	
	
#ifdef LF_TARGET_LITEC
	void __LFProtocolSignature2_0(LFTypeRef arg1, LFTypeRef arg2);
	
	__LFProtocolSignature2_0 = imp->method;
	__LFProtocolSignature2_0(ref, arg1);
#else
	__LFProtocolSignature2_0 signature = (__LFProtocolSignature2_0)imp->method;
	signature(ref, arg1);
#endif
    
    LFRuntimeUnlock(ref);
}

void LFRuntimeSuperInvoke2(LFTypeRef ref, LFProtocolSel selector, LFTypeRef arg1, LFTypeRef arg2)
{
    LFRuntimeLock(ref);
    
    if(LFProtocolZombieScribble(ref, "LFRuntimeSuperInvoke2", selector))
    {
        LFRuntimeUnlock(ref);
        return;
    }
	
	LFProtocolImp *imp = LFRuntimeProtocolGetImpForSelector(ref, selector, 1);
	
#ifdef LF_TARGET_LITEC
	void __LFProtocolSignature3_0(LFTypeRef arg1, LFTypeRef arg2, LFTypeRef arg3);
	
	__LFProtocolSignature3_0 = imp->method;
	__LFProtocolSignature3_0(ref, arg1, arg2);
#else
	__LFProtocolSignature3_0 signature = (__LFProtocolSignature3_0)imp->method;
	signature(ref, arg1, arg2);
#endif
    
    LFRuntimeUnlock(ref);
}



// Simple invoke with return
LFTypeRef LFRuntimeRInvoke0(LFTypeRef ref, LFProtocolSel selector)
{
    LFRuntimeLock(ref);
    
    if(LFProtocolZombieScribble(ref, "LFRuntimeRInvoke0", selector))
    {
        LFRuntimeUnlock(ref);
        return NULL;
    }
	
	LFProtocolImp *imp = LFRuntimeProtocolGetImpForSelector(ref, selector, 0);
	LFTypeRef retVal;
    
#ifdef LF_TARGET_LITEC
	LFTypeRef __LFProtocolSignature1_1(LFTypeRef arg1);
	
	__LFProtocolSignature1_1 = imp->method;
	retVal = __LFProtocolSignature1_1(ref);
#else
	__LFProtocolSignature1_1 signature = (__LFProtocolSignature1_1)imp->method;
	retVal = signature(ref);
#endif
    
    LFRuntimeUnlock(ref);
    return retVal;
}

LFTypeRef LFRuntimeRInvoke1(LFTypeRef ref, LFProtocolSel selector, LFTypeRef arg1)
{
    LFRuntimeLock(ref);
    
    if(LFProtocolZombieScribble(ref, "LFRuntimeRInvoke1", selector))
    {
        LFRuntimeUnlock(ref);
        return NULL;
    }
	
	LFProtocolImp *imp = LFRuntimeProtocolGetImpForSelector(ref, selector, 0);
	LFTypeRef retVal;
    
#ifdef LF_TARGET_LITEC
	LFTypeRef __LFProtocolSignature2_1(LFTypeRef arg1, LFTypeRef arg2);
	
	__LFProtocolSignature2_1 = imp->method;
	retVal = __LFProtocolSignature2_1(ref, arg1);
#else
	__LFProtocolSignature2_1 signature = (__LFProtocolSignature2_1)imp->method;
	retVal = signature(ref, arg1);
#endif
    
    LFRuntimeUnlock(ref);
    return retVal;
}

LFTypeRef LFRuntimeRInvoke2(LFTypeRef ref, LFProtocolSel selector, LFTypeRef arg1, LFTypeRef arg2)
{
    LFRuntimeLock(ref);
    
    if(LFProtocolZombieScribble(ref, "LFRuntimeRInvoke2", selector))
    {
        LFRuntimeUnlock(ref);
        return NULL;
    }
	
	LFProtocolImp *imp = LFRuntimeProtocolGetImpForSelector(ref, selector, 0);
	LFTypeRef retVal;
    
#ifdef LF_TARGET_LITEC
	LFTypeRef __LFProtocolSignature3_1(LFTypeRef arg1, LFTypeRef arg2, LFTypeRef arg3);
	
	__LFProtocolSignature3_1 = imp->method;
	retVal = __LFProtocolSignature3_1(ref, arg1, arg2);
#else
	__LFProtocolSignature3_1 signature = (__LFProtocolSignature3_1)imp->method;
	retVal = signature(ref, arg1, arg2);
#endif
    
    LFRuntimeUnlock(ref);
    return retVal;
}

// Invoke on superclass with return
LFTypeRef LFRuntimeSuperRInvoke0(LFTypeRef ref, LFProtocolSel selector)
{
    LFRuntimeLock(ref);
    
    if(LFProtocolZombieScribble(ref, "LFRuntimeSuperRInvoke0", selector))
    {
        LFRuntimeUnlock(ref);
        return NULL;
    }
	
	LFProtocolImp *imp = LFRuntimeProtocolGetImpForSelector(ref, selector, 1);
	LFTypeRef retVal;
    
#ifdef LF_TARGET_LITEC
	LFTypeRef __LFProtocolSignature1_1(LFTypeRef arg1);
	
	__LFProtocolSignature1_1 = imp->method;
	retVal = __LFProtocolSignature1_1(ref);
#else
	__LFProtocolSignature1_1 signature = (__LFProtocolSignature1_1)imp->method;
	retVal = signature(ref);
#endif
    
    LFRuntimeUnlock(ref);
    return retVal;
}

LFTypeRef LFRuntimeSuperRInvoke1(LFTypeRef ref, LFProtocolSel selector, LFTypeRef arg1)
{
    LFRuntimeLock(ref);
    
    if(LFProtocolZombieScribble(ref, "LFRuntimeSuperRInvoke1", selector))
    {
        LFRuntimeUnlock(ref);
        return NULL;
    }
	
	LFProtocolImp *imp = LFRuntimeProtocolGetImpForSelector(ref, selector, 1);
	LFTypeRef retVal;
    
#ifdef LF_TARGET_LITEC
	LFTypeRef __LFProtocolSignature2_1(LFTypeRef arg1, LFTypeRef arg2);
	
	__LFProtocolSignature2_1 = imp->method;
	retVal = __LFProtocolSignature2_1(ref, arg1);
#else
	__LFProtocolSignature2_1 signature = (__LFProtocolSignature2_1)imp->method;
	retVal = signature(ref, arg1);
#endif
    
    LFRuntimeUnlock(ref);
    return retVal;
}

LFTypeRef LFRuntimeSuperRInvoke2(LFTypeRef ref, LFProtocolSel selector, LFTypeRef arg1, LFTypeRef arg2)
{
    LFRuntimeLock(ref);
    
    if(LFProtocolZombieScribble(ref, "LFRuntimeSuperRInvoke2", selector))
    {
        LFRuntimeUnlock(ref);
        return NULL;
    }
	
	LFProtocolImp *imp = LFRuntimeProtocolGetImpForSelector(ref, selector, 1);
	LFTypeRef retVal;
    
#ifdef LF_TARGET_LITEC
	LFTypeRef __LFProtocolSignature3_1(LFTypeRef arg1, LFTypeRef arg2, LFTypeRef arg3);
	
	__LFProtocolSignature3_1 = imp->method;
	retVal = __LFProtocolSignature3_1(ref, arg1, arg2);
#else
	__LFProtocolSignature3_1 signature = (__LFProtocolSignature3_1)imp->method;
	retVal = signature(ref, arg1, arg2);
#endif
    
    LFRuntimeUnlock(ref);
    return retVal;
}
