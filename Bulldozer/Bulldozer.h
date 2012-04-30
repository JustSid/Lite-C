//
//  Bulldozer.h
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

#ifndef BULLDOZER_H_
#define BULLDOZER_H_

#include <windows.h>

#define BZExtern

// DLL exported
typedef struct
{
    void *pointer;
    int count;

    void *callback;
} BZCollectable;


BZExtern BZCollectable *BZCollectableInsert(void *pointer, long size, void *freeCallback);
BZExtern BZCollectable *BZCollectableStatic(void **pointer, long size);
BZExtern BZCollectable *BZCollectableForPointer(void *pointer);

BZExtern void BZCollectableReplace(void *pointer, void *newPointer, long size);
BZExtern void BZCollectableRelease(BZCollectable *collectable);

BZExtern void BZCollectorInit(void *address);
BZExtern void BZCollectorCollect(int maxCollections);


// Convinient functions
void *malloc_gc(long size)
{
	void *pointer = malloc(size);
	if(pointer)
		BZCollectableInsert(pointer, size, free);
	
	return pointer;
}

void *realloc_gc(void *pointer, long size)
{
	void *temp = realloc(pointer, size);
	if(temp)
		BZCollectableReplace(pointer, temp, size);
	
	return temp;
}

// Convinient Gamestudio functions
STRING *str_create_gc(char *content)
{
	STRING *string = str_create(content);
	if(string)
		BZCollectableInsert(string, 0, ptr_remove);
	
	return string;
}

STRING *str_create_gc(STRING *content)
{
	STRING *string = str_create(content);
	if(string)
		BZCollectableInsert(string, 0, ptr_remove);
	
	return string;
}


// Helper
static long BZHandle = 0;

void __BZInit()
{
	if(BZHandle == 0)
	{
		BZHandle = LoadLibrary("Bulldozer.dll");
		
		BZCollectableInsert = GetProcAddress(BZHandle, "BZCollectableInsert");
		BZCollectableStatic = GetProcAddress(BZHandle, "BZCollectableStatic");
		BZCollectableForPointer = GetProcAddress(BZHandle, "BZCollectableForPointer");
		
		BZCollectableReplace = GetProcAddress(BZHandle, "BZCollectableReplace");
		BZCollectableRelease = GetProcAddress(BZHandle, "BZCollectableRelease");
		
		BZCollectorInit = GetProcAddress(BZHandle, "BZCollectorInit");
		BZCollectorCollect = GetProcAddress(BZHandle, "BZCollectorCollect");
	}
}

void __BZStub()
{}

#define BZInit do{int t; __BZInit(); BZCollectorInit(&t);}while(0); __BZStub

#endif
