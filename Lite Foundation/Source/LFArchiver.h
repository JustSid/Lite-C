//
//  LFArchiver.h
//  Lite Foundation
//
//  Created by Sidney Just on 22.06.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef _LFARCHIVER_H_
#define _LFARCHIVER_H_

#include "LFBase.h"
#include "LFRuntime.h"
#include "LFProtocol.h"
#include "LFString.h"
#include "LFData.h"
#include "LFDictionary.h"

struct __LFArchiver
{
    LFRuntimeBase base;
    
    LFDictionaryRef objects;
    LFDataRef data;
    
    uint8_t finished;
};

typedef struct __LFArchiver* LFArchiverRef;

LF_EXPORT LFProtocolSel LFArchiverArchiveSelector();
LF_EXPORT LFProtocolID LFArchiverGetProtocolID();

LF_EXPORT LFArchiverRef LFArchiverCreateWithData(LFDataRef data);
LF_EXPORT LFDataRef LFArchiverArchivedObject(LFTypeRef object);

LF_EXPORT void LFArchiverArchiveBytes(LFArchiverRef archiver, uint8_t *bytes, uint32_t length, LFStringRef key);
LF_EXPORT void LFArchiverArchiveInt(LFArchiverRef archiver, int value, LFStringRef key);
LF_EXPORT void LFArchiverArchiveShort(LFArchiverRef archiver, short value, LFStringRef key);
LF_EXPORT void LFArchiverArchiveChar(LFArchiverRef archiver, char value, LFStringRef key);
LF_EXPORT void LFArchiverArchiveFloat(LFArchiverRef archiver, float value, LFStringRef key);
LF_EXPORT void LFArchiverArchiveDouble(LFArchiverRef archiver, double value, LFStringRef key);
LF_EXPORT void LFArchiverArchiveObject(LFArchiverRef archiver, LFTypeRef value, LFStringRef key);

LF_EXPORT void LFArchiverFinish(LFArchiverRef archiver);

#endif
