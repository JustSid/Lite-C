//
//  LFUnarchiver.h
//  Lite Foundation
//
//  Created by Sidney Just on 25.06.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef _LFUNARCHIVER_H_
#define _LFUNARCHIVER_H_

#include "LFBase.h"
#include "LFRuntime.h"
#include "LFDictionary.h"
#include "LFData.h"

struct __LFUnarchiver
{
    LFRuntimeBase base;
    
    LFDictionaryRef objects;
};

typedef struct __LFUnarchiver* LFUnarchiverRef;

LFUnarchiverRef LFUnarchiverCreateWithData(LFDataRef data);

#endif
