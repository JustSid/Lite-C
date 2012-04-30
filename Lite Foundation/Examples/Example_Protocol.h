//
//  Example_Protocol.h
//  Lite Foundation
//
//  Created by Sidney Just on 14.04.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef _EXAMPLE_PROTOCOL_H_
#define _EXAMPLE_PROTOCOL_H_

#include "LFProtocol.h"

void __EXExampleProtocolInit();

LFProtocolID EXExampleProtocolGetID();
LFProtocolSel EXExampleProtocolGetHelloSel();
LFProtocolSel EXExampleProtocolGetByeSel();


void EXProtocolHello(LFTypeRef ref);
void EXProtocolBye(LFTypeRef ref);

#endif
