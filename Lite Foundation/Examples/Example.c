//
//  Example.c
//  Lite Foundation
//
//  Created by Sidney Just on 14.04.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Example.h"
#include "Example_Protocol.h"


// Function prototypes for the runtime callbacks
// REMARK: The functions aren't called if the runtime got called with invalid data, so there is no need to check the validity of the instances again
void __EXInitInstance(LFTypeRef ref);
LFTypeRef __EXCopyInstance(LFTypeRef ref);
void __EXDeallocInstance(LFTypeRef ref);
uint8_t __EXEqualInstance(LFTypeRef refa, LFTypeRef refb);
LFHashCode __EXHashInstance(LFTypeRef ref);

void __EXProtocolImpHello(LFTypeRef ref);
void __EXProtocolImpBye(LFTypeRef ref);

static LFRuntimeClass __EXClass;
static LFTypeID __EXTypeID = kLFInvalidTypeID;

void __EXInit()
{	
	__EXClass.version = 0;
	__EXClass.name = "EXExampleRef";
	
	__EXClass.init = __EXInitInstance;
	__EXClass.copy = __EXCopyInstance;
	__EXClass.dealloc = __EXDeallocInstance;
	__EXClass.equal = __EXEqualInstance;
	__EXClass.hash  = __EXHashInstance;
	
	// Protocol implementation
	LFProtocolBody *pbody = LFRuntimeCreateProtocolBody(EXExampleProtocolGetID());
	LFRuntimeProtocolBodySetIMP(pbody, EXExampleProtocolGetHelloSel(), LFProtocolMethod10(__EXProtocolImpHello));
	LFRuntimeProtocolBodySetIMP(pbody, EXExampleProtocolGetByeSel(), LFProtocolMethod10(__EXProtocolImpBye));
	
	__EXClass.protocolBag = LFRuntimeCreateProtocolBag(); // If you are not intending to implement a protocol, set the value to NULL!
	LFRuntimeAppendProtocol(__EXClass.protocolBag, pbody);
	
	// Tell the runtime that we have a new class, so that it can be instanced from now on
	__EXTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__EXClass);
}

void __EXInitInstance(LFTypeRef ref)
{
	// This function is called when the runtime creates a new instance of the class
	// Here we are supposed to initialize all default values.
	// REMARK: The runtime doesn't clean the memory, so set everything to NULL that needs to be NULL!
	
	EXExampleRef _ref = (EXExampleRef)ref;
	_ref->a = 0;
	_ref->b = 0;
}

LFTypeRef __EXCopyInstance(LFTypeRef ref)
{
	// This is called when something called LFCopy(), we are supposed to return an equal copy (deep or shallow doesn't matter and is up to you).
	// REMARK: The retain count is not meant to be copied!
	
	EXExampleRef source = (EXExampleRef)ref;
	EXExampleRef copy = EXInitWithValues(source->a, source->b);
	
	return copy;
}

void __EXDeallocInstance(LFTypeRef ref)
{
	// This is the point were we are supposed to clean up allocated memory or do some other operations before the instance is pureged from the memory
	// However, this class only has some basic types in it and thus don't need to clean up anything...
}

uint8_t __EXEqualInstance(LFTypeRef refa, LFTypeRef refb)
{
	// This function is called when something called LFEqual(). We return true if both instances are equal, otherwise false.
	
	EXExampleRef _a = (EXExampleRef)refa;
	EXExampleRef _b = (EXExampleRef)refb;
	
	return (_a->a == _b->a && _a->b == _b->b);
}

LFHashCode __EXHashInstance(LFTypeRef ref)
{
	// Remark: As the LFHash function might be called very often, its better to create a function that uses only fast operations like xor to generate the hash!
	
	EXExampleRef _ref = (EXExampleRef)ref;
	return (LFHashCode)(_ref->a + _ref->b);
}




EXExampleRef EXInit()
{
	return LFRuntimeCreateInstance(__EXTypeID, sizeof(struct EXExample));
}

EXExampleRef EXInitWithValues(int a, int b)
{
	EXExampleRef instance = LFRuntimeCreateInstance(__EXTypeID, sizeof(struct EXExample));
	instance->a = a;
	instance->b = b;
	
	return instance;
}


int EXAddValues(EXExampleRef ref)
{
	if(!ref)
		return -1;
	
	return ref->a + ref->b;
}

int EXGetValueA(EXExampleRef ref)
{
	if(!ref)
		return -1;
	
	return ref->a;
}

int EXGetValueB(EXExampleRef ref)
{
	if(!ref)
		return -1;
	
	return ref->b;
}


// Protocol implementation
void __EXProtocolImpHello(LFTypeRef ref)
{
	printf("HI, I'm %p\n", ref);
}

void __EXProtocolImpBye(LFTypeRef ref)
{
	printf("Cya");
}
