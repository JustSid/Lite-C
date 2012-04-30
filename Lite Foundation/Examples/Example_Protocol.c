//
//  Example_Protocol.c
//  Lite Foundation
//
//  Created by Sidney Just on 14.04.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Example_Protocol.h"

LFProtocolID  kEXExampleProtocolID	= kLFProtocolInvalidID;
LFProtocolSel kEXExampleSayHello	= "kEXExampleSayHello";
LFProtocolSel kEXExampleSayBye		= "kEXExampleSayBye";

static LFProtocolHeader __EXExampleProtocolHeader;
static LFProtocolSel __EXExampleProtocolSelector[2];

void __EXExampleProtocolInit()
{
	__EXExampleProtocolSelector[0] = kEXExampleSayHello;
	__EXExampleProtocolSelector[1] = kEXExampleSayBye;
	
	
	__EXExampleProtocolHeader.version = 0;
	__EXExampleProtocolHeader.name = "Example Protocol";
	__EXExampleProtocolHeader.selectorCount = 2;
	__EXExampleProtocolHeader.selectors = __EXExampleProtocolSelector;
	
	kEXExampleProtocolID = LFRuntimeRegisterProtocol((const LFProtocolHeader *)&__EXExampleProtocolHeader);
}



LFProtocolID EXExampleProtocolGetID()
{
	return kEXExampleProtocolID;
}

LFProtocolSel EXExampleProtocolGetHelloSel()
{
	return kEXExampleSayHello;
}

LFProtocolSel EXExampleProtocolGetByeSel()
{
	return kEXExampleSayBye;
}


void EXProtocolHello(LFTypeRef ref)
{
	LFRuntimeInvoke0(ref, kEXExampleSayHello);
}

void EXProtocolBye(LFTypeRef ref)
{
	LFRuntimeInvoke0(ref, kEXExampleSayBye);
}
