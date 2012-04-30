//
//  LFArchivingInternal.h
//  Lite Foundation
//
//  Created by Sidney Just on 23.06.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef _LFARCHIVINGINTERNAL_H_
#define _LFARCHIVINGINTERNAL_H_

#define __LFArchiverElementTypeInt      1
#define __LFArchiverElementTypeShort    2
#define __LFArchiverElementTypeChar     3
#define __LFArchiverElementTypeFloat    4
#define __LFArchiverElementTypeDouble   5
#define __LFArchiverElementTypeBytes    6
#define __LFArchiverElementTypeObject   7

struct __LFArchivedElement 
{
    uint32_t type, length;
};

#endif
