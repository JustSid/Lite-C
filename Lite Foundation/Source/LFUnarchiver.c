//
//  LFUnarchiver.c
//  Lite Foundation
//
//  Created by Sidney Just on 25.06.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "LFUnarchiver.h"
#include "LFArchivingInternal.h"
#include "LFString.h"

void __LFUnarchiverInit(LFTypeRef ref);

static LFRuntimeClass __LFUnarchiverClass;
static LFTypeID __LFUnarchiverTypeID = kLFInvalidTypeID;

void __LFRuntimeInitUnarchiver()
{
	__LFUnarchiverClass.version = 1;
	__LFUnarchiverClass.name = "LFUnarchiver";
	
	__LFUnarchiverClass.init = __LFUnarchiverInit;
	__LFUnarchiverClass.copy = NULL;
	__LFUnarchiverClass.dealloc = NULL;
	__LFUnarchiverClass.equal = NULL;
	__LFUnarchiverClass.hash  = NULL;
	
	__LFUnarchiverClass.superClass = kLFInvalidTypeID;
	__LFUnarchiverClass.protocolBag = NULL;
	__LFUnarchiverTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFUnarchiverClass);
}


void __LFUnarchiverInit(LFTypeRef ref)
{
    LFUnarchiverRef unarchiver = (LFUnarchiverRef)ref;
    unarchiver->objects = LFDictionaryCreate(NULL);
}



void __LFUnarchiverRecreateObjectGraph(LFUnarchiverRef unarchiver, LFDataRef data)
{
    struct __LFArchivedElement element;
    LFIndex offset = 0;
    LFRange range = LFRangeMake(0, sizeof(struct __LFArchivedElement));
    
    while(LFDataGetLength(data) - offset > sizeof(struct __LFArchivedElement))
    {
        LFDataGetBytesInRange(data, range, (uint8_t *)&element);
        
        if(element.type > 0 && element.type <= __LFArchiverElementTypeObject && element.length > 0)
        {
            range.location += range.length;
            range.length = sizeof(LFIndex);
            
            LFIndex keyLength;
            LFIndex dataLength;
            
            // Get the key
            LFDataGetBytesInRange(data, range, (uint8_t *)&keyLength);
            
            range.location += range.length;
            range.length = keyLength;
            
            char *cKey = (char *)malloc((keyLength + 1) * sizeof(char));
            LFDataGetBytesInRange(data, range, (uint8_t *)cKey);
            cKey[keyLength] = '\0';
            
            
            LFStringRef key = LFStringCreateWithCString(cKey);
            free(cKey);
            
            
            // Get the data
            range.location += range.length;
            range.length = sizeof(LFIndex);
            
            LFDataGetBytesInRange(data, range, (uint8_t *)&dataLength);
            
            range.location += range.length;
            range.length = dataLength;
        
            void *buffer = malloc(dataLength);
            LFDataGetBytesInRange(data, range, (uint8_t *)buffer);
            
            LFDataRef data = LFDataCreateWithBytes((uint8_t *)buffer, dataLength);
            free(buffer);
            
            // Append into the dictionary
            LFDictionarySetValueForKey(unarchiver->objects, data, key);
        }
        
        range.location += range.length;
        range.length = sizeof(struct __LFArchivedElement);
    }
}

LFUnarchiverRef LFUnarchiverCreateWithData(LFDataRef data)
{
    LFRuntimeLock(data);
    
    if(LFZombieScribble(data, "LFUnarchiverCreateWithData") || !LFRuntimeValidate(data, LFDataGetTypeID()))
    {
        LFRuntimeUnlock(data);
        return NULL;
    }
    
    
    LFUnarchiverRef unarchiver = LFRuntimeCreateInstance(__LFUnarchiverTypeID, sizeof(struct __LFUnarchiver));
    __LFUnarchiverRecreateObjectGraph(unarchiver, data);
    
    LFRuntimeUnlock(data);
    return unarchiver;
}
