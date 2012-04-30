//
//  LFRuntime.c
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

#include "LFRuntime.h"
#include "LFInternal.h"
#include "LFDebug.h"
#include "LFDebugInternal.h"

#include "LFThread.h"
#include "LFLock.h"
#include "LFCondition.h"

// Runtime locking
static LFLockRef __LFRuntimeLock = NULL;

#ifdef LF_TARGET_OSX
#define __LFRuntimeLockConditional(lock) if(LFThreadIsMultithreaded()) pthread_mutex_lock(&lock->mutex)
#define __LFRuntimeUnlockConditional(lock) if(LFThreadIsMultithreaded()) pthread_mutex_unlock(&lock->mutex)
#endif

#ifdef LF_TARGET_WIN32
#define __LFRuntimeLockConditional(lock) if(LFThreadIsMultithreaded()) WaitForSingleObject(lock->mutex, INFINITE)
#define __LFRuntimeUnlockConditional(lock) if(LFThreadIsMultithreaded()) ReleaseMutex(lock->mutex)
#endif

void __LFRuntimeLockCond()
{
	__LFRuntimeLockConditional(__LFRuntimeLock);
}

void __LFRuntimeUnlockCond()
{
	__LFRuntimeUnlockConditional(__LFRuntimeLock);
}


// Class handling
// DON'T Forget to update LFDebug.c!
#define __LFRuntimeGetTypeID_inline(ref) LFBitfieldGetValue(((LFRuntimeBase *)ref)->__info, 8, 0)
#define __LFRuntimeGetExtraFlags(ref) LFBitfieldGetValue(((LFRuntimeBase *)ref)->__info, 31, 15)
#define __LFRuntimeSetExtraFlags(ref, flags) LFBitfieldSetValue(((LFRuntimeBase *)ref)->__info, 31, 15, flags)
#define __LFRuntimeGetLocks(ref) ((LFRuntimeBase *)ref)->__locks
#define __LFRuntimeSetLocks(ref, locks) ((LFRuntimeBase *)ref)->__locks = locks

#define __LFRuntimeClassTableMax 255

static LFRuntimeClass *__LFRuntimeClassTable[__LFRuntimeClassTableMax];
static uint32_t __LFRuntimeClassTableCount = 0;



// Protocol helper methods
LFProtocolBag *LFRuntimeCreateProtocolBag()
{
	LFProtocolBag *bag = (LFProtocolBag *)malloc(sizeof(LFProtocolBag));
	if(bag)
	{
		bag->bodyCount = 0;
		bag->bodys = (LFProtocolBody **)malloc(sizeof(LFProtocolBody *));
	}
	
	return bag;
}

void LFRuntimeDestroyProtocolBag(LFProtocolBag *bag)
{
	if(bag)
	{
		free(bag->bodys);
		free(bag);
	}
}

void LFRuntimeAppendProtocol(LFProtocolBag *bag, LFProtocolBody *body)
{
	int i;
	for(i=0; i<bag->bodyCount; i++)
	{
		if((bag->bodys)[i] == body)
			return;
	}
	
	LFProtocolBody **temp = (LFProtocolBody **)realloc(bag->bodys, (bag->bodyCount + 1) * sizeof(LFProtocolBody *));
	if(temp)
	{
		bag->bodys = temp;
		(bag->bodys)[bag->bodyCount] = body;
		bag->bodyCount ++;
	}
}

LFProtocolBag *LFRuntimeGetProtocolBag(LFTypeRef ref, uint8_t nestedSuper)
{
	if(!ref)
		return NULL;
	
	// No zombie check needed because the function is only used internally
	if(nestedSuper == 0)
	{
		LFRuntimeClass *cls = (LFRuntimeClass *)__LFRuntimeClassTable[__LFRuntimeGetTypeID_inline(ref)];
		return cls->protocolBag;
	}
	else
	{
		LFRuntimeClass *cls = (LFRuntimeClass *)__LFRuntimeClassTable[__LFRuntimeGetTypeID_inline(ref)];
		LFIndex index;
		
		for(index=0; index<nestedSuper; index++)
		{
			if(!cls)
				return NULL;
			
			cls = __LFRuntimeClassTable[cls->superClass];
		}
		
		return cls->protocolBag;
	}
	
	return NULL;
}


// Class functions
LFTypeID LFRuntimeRegisterClass(const LFRuntimeClass *cls)
{
	LFIndex selection;
	uint8_t found = 0;
	
	__LFRuntimeClassTableCount ++;
	selection = __LFRuntimeClassTableCount;
	
	if(__LFRuntimeClassTableCount >= __LFRuntimeClassTableMax)
	{
		// Do a cost intensive search for an unregistered protocol which place can be used
		for(selection=1; selection < __LFRuntimeClassTableMax; selection++)
		{
			if(__LFRuntimeClassTable[selection] == 0)
			{
				found = 1;
				break;
			}
		}
		
		
		if(!found)
		{
			printf("Could not register class, no more entries in the class table left!");
			return kLFInvalidTypeID;
		}
	}
	
	
	// Version legacy stuff
	LFRuntimeClass *class = (LFRuntimeClass *)cls;
	if(cls->version == 0)
	{
		class->superClass = kLFInvalidTypeID;
	}
	
	
	__LFRuntimeClassTable[selection] = (LFRuntimeClass *)cls;	
	return __LFRuntimeClassTableCount;
}

const LFRuntimeClass *LFRuntimeGetClassWithID(LFTypeID tid)
{
	return __LFRuntimeClassTable[tid];
}

void LFRuntimeUnregisterClass(LFTypeID tid)
{
	__LFRuntimeClassTable[tid] = NULL;
}



void __LFRuntimeInit1()
{
	memset(__LFRuntimeClassTable, 0, __LFRuntimeClassTableMax);
}

void __LFRuntimeInit2()
{
	__LFRuntimeLock = LFLockCreate();
}


__LFRuntimeTable *__LFKVOCreateTable();
void __LFKVODeleteTable(__LFRuntimeTable *table);


void __LFRuntimeInvokeInit(LFTypeRef base, LFTypeID tid)
{
	LFRuntimeClass *cls = __LFRuntimeClassTable[tid];
	
	if(cls->superClass != kLFInvalidTypeID)
		__LFRuntimeInvokeInit(base, cls->superClass);
	
	if(cls->init)
		cls->init(base);
}

LFTypeRef LFRuntimeCreateInstance(LFTypeID tid, size_t bytes)
{
	LFRuntimeClass *cls = __LFRuntimeClassTable[tid];
	if(!cls)
		return NULL;
	
	if(bytes < sizeof(LFRuntimeBase))
		return NULL;
	
	
	LFRuntimeBase *base = (LFRuntimeBase *)malloc(bytes);
	if(base)
	{
        memset(base, 0, bytes);
        
		base->__rc = 1;
		base->__info = 0;
        base->__size = (uint32_t)bytes; // TODO: 64 bit!
        base->__owner = NULL;
        base->__kvoTable = __LFKVOCreateTable();
        base->__kvoCount = 0;
        base->__kvoList = NULL;
		
        __LFRuntimeSetLocks(base, 0);
		LFBitfieldSetValue(base->__info, 8, 0, (char)tid);
		__LFRuntimeInvokeInit((LFTypeRef)base, tid);
	}
	
	return (LFTypeRef *)base;	
}

void LFRuntimeSetInstanceID(LFTypeRef ref, LFTypeID tid)
{
	if(!ref)
		return;
	
	LFRuntimeLock(ref);
    if(LFZombieScribble(ref, "LFRuntimeSetInstanceID"))
    {
        LFRuntimeUnlock(ref);
        return;
    }
    
	LFRuntimeClass *cls = (LFRuntimeClass *)__LFRuntimeClassTable[tid];
	if(!cls)
    {
        LFRuntimeUnlock(ref);
		return;
    }
	
	LFRuntimeBase *base = (LFRuntimeBase *)ref;
	LFBitfieldSetValue(base->__info, 8, 0, (char)tid);
	
    LFRuntimeUnlock(ref);
}

char *LFRuntimeGetClassName(LFTypeRef ref)
{
    LFRuntimeLock(ref);
    
    if(LFZombieScribble(ref, "LFRuntimeGetClassName"))
    {
        LFRuntimeUnlock(ref);
        return NULL;
    }
        
    const LFRuntimeClass *class = __LFRuntimeClassTable[__LFRuntimeGetTypeID_inline(ref)];
    LFRuntimeUnlock(ref);
    
    return (char *)class->name;
}

const LFRuntimeClass *LFRuntimeGetClassWithName(char *name)
{
    if(!name)
        return NULL;
    
    int i;
    for(i=1; i<__LFRuntimeClassTableCount; i++)
    {
        const LFRuntimeClass *class = __LFRuntimeClassTable[i];
        
        if(strcmp(name, class->name) == 0)
            return class;
    }
    
    return NULL;
}

LFTypeID LFGetTypeID(LFTypeRef ref)
{
	if(!ref)
		return kLFInvalidTypeID;
	
    LFRuntimeLock(ref);
    if(LFZombieScribble(ref, "LFGetTypeID"))
    {
        LFRuntimeUnlock(ref);
        return kLFInvalidTypeID;
    }
    

	LFTypeID typeID = __LFRuntimeGetTypeID_inline(ref);
	
	LFRuntimeUnlock(ref);
	return typeID;
}


LFTypeRef LFRetain(LFTypeRef ref)
{
	if(!ref)
		return NULL;
	
    LFRuntimeLock(ref);
    if(LFZombieScribble(ref, "LFRetain"))
    {
        LFRuntimeUnlock(ref);
        return NULL;
    }
  
	if(LFBitIsSet(__LFRuntimeGetExtraFlags(ref), kLFRuntimeNoRetainBit))
	{
		LFRuntimeUnlock(ref);
		return ref; 
	}
	
	((LFRuntimeBase *)ref)->__rc ++;	
	
	LFRuntimeUnlock(ref);
	return ref;
}

void LFRelease(LFTypeRef ref)
{
	if(!ref)
		return;
	
	LFRuntimeLock(ref);
    if(LFZombieScribble(ref, "LFRelease"))
    {
        LFRuntimeUnlock(ref);
        return;
    }
    
	((LFRuntimeBase *)ref)->__rc --;
	
	if(((LFRuntimeBase *)ref)->__rc == 0)
	{
		// The retain count is zero, so we have to remove the object from memory
		LFRuntimeClass *cls = __LFRuntimeClassTable[__LFRuntimeGetTypeID_inline(ref)];

		while(cls)
		{
			if(cls->dealloc) // Notify the callback that the instance is about to be removed
				cls->dealloc(ref);
			
			cls = __LFRuntimeClassTable[cls->superClass];
		}
        
        // Cleanup the kv observer
        LFRuntimeBase *base = (LFRuntimeBase *)ref;
        __LFKVODeleteTable(base->__kvoTable);
        
        LFIndex index;
        for(index=0; index<base->__kvoCount; index++)
        {
            __LFRuntimeKVObserver *observer = base->__kvoList[index];
            if(observer)
            {
                free(observer->key);
                free(observer);
            }
        }
        
        if(base->__kvoList)
            free(base->__kvoList);
		
#ifndef NDEBUG
		if(LFRuntimeGetZombieLevel() > 0)
		{
			short flags = __LFRuntimeGetExtraFlags(ref);
			LFBitSet(flags, kLFRuntimeZombieBit); // Mark the object as Zombie
			
			__LFRuntimeSetExtraFlags(ref, flags);
            __LFRuntimeSetLocks(ref, __LFRuntimeGetLocks(ref) - 1);
            
            return;
		}
		else
        {            
			free(ref);
            return;
        }
#else
		free(ref);
        return;
#endif
	}
	
    LFRuntimeUnlock(ref);
}

LFIndex LFGetRetainCount(LFTypeRef ref)
{
	if(!ref)
		return 0;
	
    LFRuntimeLock(ref);
    if(LFZombieScribble(ref, "LFGetRetainCount"))
    {
        LFRuntimeUnlock(ref);
        return 0;
    }
    

	uint32_t retainCount = ((LFRuntimeBase *)ref)->__rc;
	
	LFRuntimeUnlock(ref);
	return retainCount;
}


uint8_t LFEqual(LFTypeRef refa, LFTypeRef refb)
{
	if(!refa || !refb)
		return 0;
	
    LFRuntimeLock(refa);
    LFRuntimeLock(refb);
    
    if(LFZombieScribble(refa, "LFEqual") || LFZombieScribble(refb, "LFEqual"))
    {
        LFRuntimeUnlock(refa);
        LFRuntimeUnlock(refb);
    }
    

	if(__LFRuntimeGetTypeID_inline(refa) != __LFRuntimeGetTypeID_inline(refb))
	{
        LFRuntimeUnlock(refa);
        LFRuntimeUnlock(refb);
		return 0;
	}
	
	
	
	LFRuntimeClass *cls = (LFRuntimeClass *)__LFRuntimeClassTable[__LFRuntimeGetTypeID_inline(refa)];
	while(cls)
	{
		if(cls->equal)
        {
			uint8_t equal = cls->equal(refa, refb);
            LFRuntimeUnlock(refa);
            LFRuntimeUnlock(refb);
            
            return equal;
        }

		cls = __LFRuntimeClassTable[cls->superClass];
	}
	
    LFRuntimeUnlock(refa);
    LFRuntimeUnlock(refb);
	return (refa == refb);
}

LFHashCode LFHash(LFTypeRef ref)
{
	if(!ref)
		return 0;
	
    LFRuntimeLock(ref);
    
    if(LFZombieScribble(ref, "LFHash"))
    {
        LFRuntimeUnlock(ref);
        return 0;
    }
    
	
	LFRuntimeClass *cls = (LFRuntimeClass *)__LFRuntimeClassTable[__LFRuntimeGetTypeID_inline(ref)];
	while(cls)
	{
		if(cls->hash)
        {
            LFHashCode hash = cls->hash(ref);
            LFRuntimeUnlock(ref);
            
			return hash;
        }
		
		cls = __LFRuntimeClassTable[cls->superClass];
	}
	
    
	LFRuntimeUnlock(ref);
	return (LFHashCode)ref;
}


LFTypeRef LFCopy(LFTypeRef ref)
{
	if(!ref)
		return NULL;
	
    LFRuntimeLock(ref);
    if(LFZombieScribble(ref, "LFCopy"))
    {
        LFRuntimeUnlock(ref);
        return NULL;
    }
    
	LFRuntimeClass *cls = (LFRuntimeClass *)__LFRuntimeClassTable[__LFRuntimeGetTypeID_inline(ref)];
	while(cls)
	{
		if(cls->copy)
        {
            LFTypeRef copy = cls->copy(ref);
            LFRuntimeUnlock(ref);
            
			return copy;
        }
		
		cls = __LFRuntimeClassTable[cls->superClass];
	}
	
	LFRuntimeUnlock(ref);
	return NULL;
}

uint8_t LFIsKindOf(LFTypeRef ref, LFTypeID tid)
{
	if(!ref)
		return 0;
	
	LFRuntimeLock(ref);
    if(LFZombieScribble(ref, "LFIsKindOf"))
    {
        LFRuntimeUnlock(ref);
        return 0;
    }

	LFTypeID oid = __LFRuntimeGetTypeID_inline(ref);
	uint8_t kindOf = (oid == tid);
	
	if(kindOf)
	{
		LFRuntimeUnlock(ref);
		return 1;
	}
	
	
	LFRuntimeClass *class = __LFRuntimeClassTable[oid];
	while(class)
	{
		if(class->superClass == tid)
        {
            LFRuntimeUnlock(ref);
			return 1;
        }
        
		class = __LFRuntimeClassTable[class->superClass];
	}
	
    LFRuntimeUnlock(ref);
	return 0;
}


// TODO: A real spinlock would probably fit better...
void LFRuntimeLock(LFTypeRef ref)
{
    LFRuntimeBase *base = (LFRuntimeBase *)ref;
    uint8_t locks;
    
	if(LFThreadIsMultithreaded())
	{
		if(!ref || LFNoLockZombieScribble(ref, "LFRuntimeLock"))
			return;
		
		
	CHECK_LOCK:
        __LFRuntimeLockConditional(__LFRuntimeLock);
		locks = __LFRuntimeGetLocks(base);
        
		if(locks > 0)
		{
#ifdef LF_TARGET_OSX
			if(pthread_equal(base->__owner, pthread_self()) || base->__owner == NULL)
#else
			if(base->__owner == GetCurrentThreadId() || base->__owner == NULL)
#endif
			{
				__LFRuntimeSetLocks(base, locks + 1);
				__LFRuntimeUnlockConditional(__LFRuntimeLock);
			}
			else
			{
				__LFRuntimeUnlockConditional(__LFRuntimeLock);
				goto CHECK_LOCK;
			}
		}
		else
		{
			__LFRuntimeSetLocks(base, 1);
#ifdef LF_TARGET_OSX
			base->__owner = pthread_self();
#else
			base->__owner = GetCurrentThreadId();
#endif
			__LFRuntimeUnlockConditional(__LFRuntimeLock);
		}
	}
    else
    {
        locks = __LFRuntimeGetLocks(base);
        __LFRuntimeSetLocks(base, locks + 1);
    }
}

void LFRuntimeUnlock(LFTypeRef ref)
{
    LFRuntimeBase *base = (LFRuntimeBase *)ref;
    uint8_t locks;
    
	if(LFThreadIsMultithreaded())
	{
		if(!ref || LFNoLockZombieScribble(ref, "LFRuntimeUnlock"))
			return;
		
		__LFRuntimeLockConditional(__LFRuntimeLock);
        
        locks = __LFRuntimeGetLocks(base) - 1;
		__LFRuntimeSetLocks(base, locks);
		
		if(locks == 0)
		{
			uint32_t flags = __LFRuntimeGetExtraFlags(ref);
			if(LFBitIsSet(flags, kLFRuntimeMissedDeallocBit))
			{
				__LFRuntimeUnlockConditional(__LFRuntimeLock);
				LFRelease(ref);
				
				return;
            }
		}		
		
		__LFRuntimeUnlockConditional(__LFRuntimeLock);
	}
    else
    {
        locks = __LFRuntimeGetLocks(base);
        __LFRuntimeSetLocks(base, locks - 1);
    }
}
