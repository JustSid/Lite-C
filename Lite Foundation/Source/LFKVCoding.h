//
//  LFKVCoding.h
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

#ifndef _LFKVCODING_H_
#define _LFKVCODING_H_

#include "LFBase.h"
#include "LFRuntime.h"

/**
 * @addtogroup LFRuntime
 * @{
 **/

LF_EXPORT void LFAddObserverForKey(LFTypeRef ref, char *key, LFObserverGenericCallback callback, LFRuntimeObserverType type);
LF_EXPORT void LFRemoveObserverForKey(LFTypeRef ref, char *key, LFObserverGenericCallback callback, LFRuntimeObserverType type);

LF_EXPORT void LFWillAccessValueForKey(LFTypeRef ref, char *key);
LF_EXPORT void LFDidAccessValueForKey(LFTypeRef ref, char *key);
LF_EXPORT void LFWillChangeValueForKey(LFTypeRef ref, char *key);
LF_EXPORT void LFDidChangeValueForKey(LFTypeRef ref, char *key);


LF_EXPORT LFTypeRef LFGetValueForKey(LFTypeRef ref, char *key);
LF_EXPORT LFTypeRef LFGetValueForKeyPath(LFTypeRef ref, char *key);
LF_EXPORT void LFSetValueForKey(LFTypeRef ref, char *key, LFTypeRef value);
LF_EXPORT void LFSetValueForKeyPath(LFTypeRef ref, char *path, LFTypeRef value);

/**
 * @}
 **/

#endif
