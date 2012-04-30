//
//  LFDebug.c
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

#include "LFDebug.h"
#include "LFRuntime.h"
#include "LFInternal.h"

#include "LFThread.h"

static LFZombieLevel __LFRuntimeZombieLevel = 0;
static FILE *__LFRuntimeLogFile = NULL;

#ifndef NDEBUG
#define __LFDebugGetTypeID(ref) LFBitfieldGetValue(((LFRuntimeBase *)ref)->__info, 8, 0)
#define __LFDebugGetExtraFlags(ref) LFBitfieldGetValue(((LFRuntimeBase *)ref)->__info, 31, 15)
#define __LFDebugSetExtraFlags(ref, flags) LFBitfieldSetValue(((LFRuntimeBase *)ref)->__info, 31, 15, flags)

void __LFRuntimeLockCond();
void __LFRuntimeUnlockCond();

// ---------------------
// Logging
// ---------------------

void LFRuntimeSetLogPath(char *file)
{
    if(__LFRuntimeLogFile)
        fclose(__LFRuntimeLogFile);
    
    if(file)
    {
        __LFRuntimeLogFile = fopen(file, "w");
    }
}

void LFRuntimeSetLogFile(FILE *file)
{
    if(__LFRuntimeLogFile)
        fclose(__LFRuntimeLogFile);
    
    __LFRuntimeLogFile = file;
}

// ----------------------
// Zombies
// ----------------------

LFStringRef LFThreadCopyName(LFThreadRef thread);

void __LFRuntimeScribbleZombie(char *fnct, LFTypeRef object)
{
	const LFRuntimeClass *class = LFRuntimeGetClassWithID(__LFDebugGetTypeID(object));
	if(__LFRuntimeZombieLevel & kLFZombieLevelThreads)
	{
		LFThreadRef thread = LFThreadGetCurrentThread();
		LFStringRef	threadName = LFThreadCopyName(thread);
		
		fprintf(__LFRuntimeLogFile, "Sent %s to already deallocated instance <%s %p> from thread \"%s\"\n", fnct, class->name, object, (char *)threadName->buffer);
		LFRelease(threadName);
	}
	else
	{
		fprintf(__LFRuntimeLogFile, "Sent %s to already deallocated instance <%s %p>\n", fnct, class->name, object);
	}
}

void __LFRuntimeScribbleProtocol(char *fnct, LFTypeRef object, LFProtocolSel selector)
{
	const LFRuntimeClass *class = LFRuntimeGetClassWithID(__LFDebugGetTypeID(object));
	if(__LFRuntimeZombieLevel & kLFZombieLevelThreads)
	{
		LFThreadRef thread = LFThreadGetCurrentThread();
		LFStringRef	threadName = LFThreadCopyName(thread);
		
		fprintf(__LFRuntimeLogFile, "Tried to invoke '%s' in %s on already deallocated instance <%s %p> from thread \"%s\"\n", selector, fnct, class->name, object, (char *)threadName->buffer);
		LFRelease(threadName);
	}
	else
	{
		fprintf(__LFRuntimeLogFile, "Tried to invoke '%s' in %s on already deallocated instance <%s %p>\n", selector, fnct, class->name, object);
	}
}

void __LFRuntimeScribbleAccess(char *fnct, LFTypeRef object)
{
	const LFRuntimeClass *class = LFRuntimeGetClassWithID(__LFDebugGetTypeID(object));
	if(__LFRuntimeZombieLevel & kLFZombieLevelThreads)
	{
		LFThreadRef thread = LFThreadGetCurrentThread();
		LFStringRef	threadName = LFThreadCopyName(thread);
		
		fprintf(__LFRuntimeLogFile, "Sent %s to instance <%s %p> from thread \"%s\"\n", fnct, class->name, object, (char *)threadName->buffer);
		LFRelease(threadName);
	}
	else
	{
		fprintf(__LFRuntimeLogFile, "Sent %s to instance <%s %p>\n", fnct, class->name, object);
	}
}



uint8_t LFZombieScribble(LFTypeRef ref, char *fnct)
{
	if(!ref)
		return 0;
	
	LFRuntimeLock(ref);
	
	short flags = __LFDebugGetExtraFlags(ref);
	if(LFBitIsSet(flags, kLFRuntimeZombieBit)) 
	{
		__LFRuntimeScribbleZombie(fnct, ref); 
		
		LFRuntimeUnlock(ref);
		return 1;
	}
	
	if(LFBitIsSet(flags, kLFRuntimeTrackedBit))
	{
		__LFRuntimeScribbleAccess(fnct, ref);
	}
	
	LFRuntimeUnlock(ref);
	return 0;
}

// Only used by the locking classes themself to avoid a deadlock
uint8_t LFNoLockZombieScribble(LFTypeRef ref, char *fnct)
{	
	if(!ref)
		return 0;
	
	short flags = __LFDebugGetExtraFlags(ref);
	if(LFBitIsSet(flags, kLFRuntimeZombieBit)) 
	{
		__LFRuntimeScribbleZombie(fnct, ref);
		return 1;
	}
	
	if(LFBitIsSet(flags, kLFRuntimeTrackedBit))
	{
		__LFRuntimeScribbleAccess(fnct, ref);
	}
	
	return 0;
}

uint8_t LFProtocolZombieScribble(LFTypeRef ref, char *fnct, LFProtocolSel selector)
{
    if(!ref)
		return 0;
	
	LFRuntimeLock(ref);
	
	short flags = __LFDebugGetExtraFlags(ref);
	if(LFBitIsSet(flags, kLFRuntimeZombieBit)) 
	{
		__LFRuntimeScribbleProtocol(fnct, ref, selector);
        
		LFRuntimeUnlock(ref);
		return 1;
	}
	
	if(LFBitIsSet(flags, kLFRuntimeTrackedBit))
	{
		__LFRuntimeScribbleAccess(fnct, ref);
	}
	
	LFRuntimeUnlock(ref);
	return 0;
}



void LFRuntimeSetZombieLevel(LFZombieLevel lvl)
{
	__LFRuntimeLockCond();
	__LFRuntimeZombieLevel = lvl;
	__LFRuntimeUnlockCond();
}

LFZombieLevel LFRuntimeGetZombieLevel()
{
	__LFRuntimeLockCond();
	LFZombieLevel lvl = __LFRuntimeZombieLevel;
	__LFRuntimeUnlockCond();
	
	return lvl;
}



// ----------------------
// Tracking
// ----------------------

void LFRuntimeStartTracking(LFTypeRef ref)
{
	LFRuntimeLock(ref);
	if(LFZombieScribble(ref, "LFRuntimeStartTracking"))
	{
		LFRuntimeUnlock(ref);
		return;
	}
	
	short flags = __LFDebugGetExtraFlags(ref);
	LFBitSet(flags, kLFRuntimeTrackedBit);
	
	__LFDebugSetExtraFlags(ref, flags);
	LFRuntimeUnlock(ref);
}

void LFRuntimeStopTracking(LFTypeRef ref)
{
	LFRuntimeLock(ref);
	if(LFZombieScribble(ref, "LFRuntimeStopTracking"))
	{
		LFRuntimeUnlock(ref);
		return;
	}
	
	short flags = __LFDebugGetExtraFlags(ref);
	LFBitClear(flags, kLFRuntimeTrackedBit);
	
	__LFDebugSetExtraFlags(ref, flags);
	LFRuntimeUnlock(ref);
}


// ----------------------
// Misc
// ----------------------

uint8_t LFRuntimeValidate(LFTypeRef ref, LFTypeID tid)
{
	if(!ref)
		return 0;
	
	LFRuntimeLock(ref);
	
	if(__LFDebugGetTypeID(ref) == tid)
	{
		LFRuntimeUnlock(ref);
		return 1;
	}
	
	LFRuntimeUnlock(ref);
	return 0;
}

#endif
