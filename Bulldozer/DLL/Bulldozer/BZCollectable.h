//
//  BZCollectable.h
//  Bulldozer
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

#ifndef BZCOLLECTABLE_H_
#define BZCOLLECTABLE_H_

#include "BZBase.h"

#define BZCollectableMaxStaticCollectables 255

typedef void BZCollectableFreeCallback(void *);

typedef struct
{
    void *pointer;
    int count;
    long size;

    BZCollectableFreeCallback *callback;
} BZCollectable;



BZExtern BZCollectable *BZCollectableInsert(void *pointer, long size, BZCollectableFreeCallback freeCallback);
BZExtern BZCollectable *BZCollectableStatic(void **pointer, long size);
BZExtern BZCollectable *BZCollectableForPointer(void *pointer);

BZExtern void BZCollectableReplace(void *pointer, void *newPointer, long newSize);
BZExtern void BZCollectableRelease(BZCollectable *collectable);

#endif
