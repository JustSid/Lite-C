//
//  LFArchiver.c
//  Lite Foundation
//
//  Created by Sidney Just on 22.06.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "LFArchiver.h"
#include "LFArchivingInternal.h"
#include "LFEnumerator.h"
#include "LFAutoreleasePool.h"

void __LFArchiverInit(LFTypeRef ref);
void __LFArchiverDealloc(LFTypeRef ref);

static LFRuntimeClass __LFArchiverClass;
static LFTypeID __LFArchiverTypeID = kLFInvalidTypeID;

static LFProtocolHeader __LFArchiverProtocolHeader;
static LFProtocolSel __LFArchiverProtocolSel[1];

static LFProtocolID  __LFArchiverProtocolID	= kLFProtocolInvalidID;
static LFProtocolSel __LFArchiverArchiveSel	= "LFArchiverArchiveSel";

void __LFRuntimeInitArchiver()
{
	__LFArchiverClass.version = 1;
	__LFArchiverClass.name = "LFArchiver";
	
	__LFArchiverClass.init = __LFArchiverInit;
	__LFArchiverClass.copy = NULL;
	__LFArchiverClass.dealloc = __LFArchiverDealloc;
	__LFArchiverClass.equal = NULL;
	__LFArchiverClass.hash  = NULL;
	
	__LFArchiverClass.superClass = kLFInvalidTypeID;
	__LFArchiverClass.protocolBag = NULL;
	__LFArchiverTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFArchiverClass);
    
    
    __LFArchiverProtocolSel[0] = __LFArchiverArchiveSel;
	
	__LFArchiverProtocolHeader.version = 0;
	__LFArchiverProtocolHeader.name = "LFArchiverProtocol";
	__LFArchiverProtocolHeader.selectorCount = 1;
	__LFArchiverProtocolHeader.selectors = __LFArchiverProtocolSel;
	
	__LFArchiverProtocolID = LFRuntimeRegisterProtocol((const LFProtocolHeader *)&__LFArchiverProtocolHeader);
}


void __LFArchiverInit(LFTypeRef ref)
{
    LFArchiverRef archiver = (LFArchiverRef)ref;
 
    archiver->finished = 0;
    archiver->data = NULL;
    archiver->objects = LFDictionaryCreate(NULL);
}

void __LFArchiverDealloc(LFTypeRef ref)
{
    LFArchiverRef archiver = (LFArchiverRef)ref;
    
    LFRelease(archiver->data);
    LFRelease(archiver->objects);
}


LFProtocolSel LFArchiverArchiveSelector()
{
    return __LFArchiverArchiveSel;
}

LFProtocolID LFArchiverGetProtocolID()
{
    return __LFArchiverTypeID;
}


LFArchiverRef LFArchiverCreateWithData(LFDataRef data)
{
    LFRuntimeLock(data);
    
    if(LFZombieScribble(data, "LFArchiverCreateWithData") || !LFRuntimeValidate(data, LFDataGetTypeID()))
    {
        LFRuntimeUnlock(data);
        return NULL;
    }
    
    
    LFArchiverRef archiver = LFRuntimeCreateInstance(__LFArchiverTypeID, sizeof(struct __LFArchiver));
    archiver->data = LFRetain(data);
    
    LFRuntimeUnlock(data);
    return archiver;
}

LFDataRef LFArchiverArchivedObject(LFTypeRef object)
{
    LFRuntimeLock(object);
    
    if(LFZombieScribble(object, "LFArchiverArchivedObject") || !LFRespondsToSelector(object, __LFArchiverArchiveSel))
    {
        LFRuntimeUnlock(object);
        return NULL;
    }

    LFDataRef data = LFDataCreate();
    LFArchiverRef archiver = LFArchiverCreateWithData(data);
    LFArchiverArchiveObject(archiver, object, LFSTR("ROOT"));
    LFArchiverFinish(archiver);
    
    LFRuntimeUnlock(object);
    return LFAutorelease(data);
}


void __LFArchiverArchiveBytes(LFArchiverRef archiver, uint8_t *bytes, uint32_t length, uint32_t type, LFStringRef key)
{
    if(!bytes || length == 0)
        return;
    
    LFRuntimeLock(key);

    if(LFZombieScribble(key, "__LFArchiverArchiveBytes") || !LFRuntimeValidate(key, LFStringGetTypeID()))
    {
        LFRuntimeUnlock(key);
        return;
    }
    
    struct __LFArchivedElement element;
    element.type = type;
    element.length = length;
    
    LFDataRef data = LFDataCreate();
    LFDataAppendBytes(data, (uint8_t *)&element, sizeof(struct __LFArchivedElement));
    LFDataAppendBytes(data, bytes, length);
    
    LFDictionarySetValueForKey(archiver->objects, data, key);
    LFRelease(data);
    
    LFRuntimeUnlock(key);
}


void LFArchiverArchiveBytes(LFArchiverRef archiver, uint8_t *bytes, uint32_t length, LFStringRef key)
{
    LFRuntimeLock(archiver);
    if(LFZombieScribble(archiver, "LFArchiverArchiveBytes") || !LFRuntimeValidate(archiver, __LFArchiverTypeID))
    {
        LFRuntimeUnlock(archiver);
        return;
    }
    
    if(archiver->finished)
    {
        LFRuntimeUnlock(archiver);
        return;
    }
    
    __LFArchiverArchiveBytes(archiver, bytes, length, __LFArchiverElementTypeBytes, key);
    LFRuntimeUnlock(archiver);
}

void LFArchiverArchiveInt(LFArchiverRef archiver, int value, LFStringRef key)
{
    LFRuntimeLock(archiver);
    if(LFZombieScribble(archiver, "LFArchiverArchiveInt") || !LFRuntimeValidate(archiver, __LFArchiverTypeID))
    {
        LFRuntimeUnlock(archiver);
        return;
    }
    
    if(archiver->finished)
    {
        LFRuntimeUnlock(archiver);
        return;
    }
    
    __LFArchiverArchiveBytes(archiver, (uint8_t *)&value, sizeof(int), __LFArchiverElementTypeInt, key);
    LFRuntimeUnlock(archiver);
}

void LFArchiverArchiveShort(LFArchiverRef archiver, short value, LFStringRef key)
{
    LFRuntimeLock(archiver);
    if(LFZombieScribble(archiver, "LFArchiverArchiveShort") || !LFRuntimeValidate(archiver, __LFArchiverTypeID))
    {
        LFRuntimeUnlock(archiver);
        return;
    }
    
    if(archiver->finished)
    {
        LFRuntimeUnlock(archiver);
        return;
    }
    
    __LFArchiverArchiveBytes(archiver, (uint8_t *)&value, sizeof(short), __LFArchiverElementTypeShort, key);
    LFRuntimeUnlock(archiver);
}

void LFArchiverArchiveChar(LFArchiverRef archiver, char value, LFStringRef key)
{
    LFRuntimeLock(archiver);
    if(LFZombieScribble(archiver, "LFArchiverArchiveChar") || !LFRuntimeValidate(archiver, __LFArchiverTypeID))
    {
        LFRuntimeUnlock(archiver);
        return;
    }
    
    if(archiver->finished)
    {
        LFRuntimeUnlock(archiver);
        return;
    }
    
    __LFArchiverArchiveBytes(archiver, (uint8_t *)&value, sizeof(char), __LFArchiverElementTypeChar, key);
    LFRuntimeUnlock(archiver);
}

void LFArchiverArchiveFloat(LFArchiverRef archiver, float value, LFStringRef key)
{
    LFRuntimeLock(archiver);
    if(LFZombieScribble(archiver, "LFArchiverArchiveFloat") || !LFRuntimeValidate(archiver, __LFArchiverTypeID))
    {
        LFRuntimeUnlock(archiver);
        return;
    }
    
    if(archiver->finished)
    {
        LFRuntimeUnlock(archiver);
        return;
    }
    
    __LFArchiverArchiveBytes(archiver, (uint8_t *)&value, sizeof(float), __LFArchiverElementTypeFloat, key);
    LFRuntimeUnlock(archiver);
}

void LFArchiverArchiveDouble(LFArchiverRef archiver, double value, LFStringRef key)
{
    LFRuntimeLock(archiver);
    if(LFZombieScribble(archiver, "LFArchiverArchiveDouble") || !LFRuntimeValidate(archiver, __LFArchiverTypeID))
    {
        LFRuntimeUnlock(archiver);
        return;
    }
    
    if(archiver->finished)
    {
        LFRuntimeUnlock(archiver);
        return;
    }
    
    __LFArchiverArchiveBytes(archiver, (uint8_t *)&value, sizeof(double), __LFArchiverElementTypeDouble, key);
    LFRuntimeUnlock(archiver);
}

void LFArchiverArchiveObject(LFArchiverRef archiver, LFTypeRef value, LFStringRef key)
{
    LFRuntimeLock(archiver);
    LFRuntimeLock(value);
    
    if(LFZombieScribble(archiver, "LFArchiverArchiveObject") || LFZombieScribble(value, "LFArchiverArchiveObject") || 
       !LFRuntimeValidate(archiver, __LFArchiverTypeID) || !LFRespondsToSelector(value, __LFArchiverArchiveSel))
    {
        LFRuntimeUnlock(archiver);
        return;
    }
    
    
    LFRuntimeBase *base = (LFRuntimeBase *)value;
    
    char *name = LFRuntimeGetClassName(value);
    LFIndex length = (LFIndex)strlen(name) + 1;
    
    LFDataRef data = LFDataCreate();
    LFDataAppendBytes(data, (uint8_t *)&length, sizeof(LFIndex));
    LFDataAppendBytes(data, (uint8_t *)name, length);
    LFDataAppendBytes(data, (uint8_t *)base->__size, sizeof(uint32_t));
    
    LFRuntimeInvoke1(value, __LFArchiverArchiveSel, archiver);
    __LFArchiverArchiveBytes(archiver, data->bytes, data->length, __LFArchiverElementTypeObject, key);
    
    LFRelease(data);
    LFRuntimeUnlock(archiver);
    LFRuntimeUnlock(value);
}


void LFArchiverFinish(LFArchiverRef archiver)
{
    LFRuntimeLock(archiver);
    if(LFZombieScribble(archiver, "LFArchiverFinish") || !LFRuntimeValidate(archiver, __LFArchiverTypeID))
    {
        LFRuntimeUnlock(archiver);
        return;
    }
    
    if(!archiver->finished)
    {
        LFEnumeratorRef enumerator = LFDictionaryKeyEnumerator(archiver->objects);
        LFStringRef key;
        
        while((key = LFEnumeratorNextObject(enumerator)))
        {
            LFDataRef data = LFDictionaryValueForKey(archiver->objects, key);
            
            LFIndex keyLength = LFStringGetLength(key);
            LFIndex dataLength = LFDataGetLength(data);
            
            LFDataAppendBytes(archiver->data, (uint8_t *)&keyLength, sizeof(LFIndex));
            LFDataAppendBytes(archiver->data, (uint8_t *)LFStringGetCString(key), keyLength);
            LFDataAppendBytes(archiver->data, (uint8_t *)&dataLength, sizeof(LFIndex));
            LFDataAppendData(archiver->data, data);
        }
        
        archiver->finished = 1;
    }
    
    LFRuntimeUnlock(archiver);
}
