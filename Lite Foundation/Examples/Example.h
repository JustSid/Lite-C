//
//  Example.h
//  Lite Foundation
//
//  Created by Sidney Just on 14.04.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef _EXAMPLE_H_
#define _EXAMPLE_H_

#include "LFRuntime.h"
#include "LFBase.h"

struct EXExample
{
	LFRuntimeBase base;
	
	int a;
	int b;
};

typedef struct EXExample* EXExampleRef;

LF_EXPORT void __EXInit();



LF_EXPORT EXExampleRef EXInit();
LF_EXPORT EXExampleRef EXInitWithValues(int a, int b);

LF_EXPORT int EXAddValues(EXExampleRef ref);
LF_EXPORT int EXGetValueA(EXExampleRef ref);
LF_EXPORT int EXGetValueB(EXExampleRef ref);

#endif
