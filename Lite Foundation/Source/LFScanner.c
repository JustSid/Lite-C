//
//  LFScanner.c
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

#include "LFScanner.h"
#include "LFAutoreleasePool.h"

void		__LFScannerInit(LFTypeRef ref);
LFTypeRef	__LFScannerCopy(LFTypeRef ref);
void		__LFScannerDealloc(LFTypeRef ref);


static LFRuntimeClass __LFScannerClass;
static LFTypeID __LFScannerTypeID = kLFInvalidTypeID;


void __LFRuntimeInitScannner()
{
	__LFScannerClass.version = 1;
	__LFScannerClass.name = "LFScanner";
	
	__LFScannerClass.init = __LFScannerInit;
	__LFScannerClass.copy = __LFScannerCopy;
	__LFScannerClass.dealloc = __LFScannerDealloc;
	__LFScannerClass.equal = NULL;
	__LFScannerClass.hash  = NULL;
	
	__LFScannerClass.superClass = kLFInvalidTypeID;
	__LFScannerClass.protocolBag = NULL;
	__LFScannerTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFScannerClass);
}


void __LFScannerInit(LFTypeRef ref)
{
	LFScannerRef scanner = (LFScannerRef)ref;
	scanner->string = NULL;
	scanner->index = 0;
	scanner->caseSensitive = 1;
	scanner->lastLength = -1;
}

LFTypeRef __LFScannerCopy(LFTypeRef ref)
{
	LFScannerRef scanner = (LFScannerRef)ref;
	LFScannerRef copy = LFScannerCreateWithString(scanner->string);
	
	if(copy)
	{
		copy->lastLength = scanner->lastLength;
		copy->index = scanner->index;
		copy->caseSensitive = scanner->caseSensitive;
	}
	
	return copy;
}

void __LFScannerDealloc(LFTypeRef ref)
{
	LFScannerRef scanner = (LFScannerRef)ref;
	LFRelease(scanner->string);
}

void __LFScannerUpdateLastLength(LFScannerRef scanner)
{
	LFRuntimeLock(scanner->string);
	LFRuntimeLock(scanner);
	
	if(scanner->lastLength != -1 && scanner->lastLength != scanner->string->length)
	{
		LFIndex offset = scanner->string->length - scanner->lastLength;
		scanner->index += (offset + 1);
		scanner->lastLength = scanner->string->length;
	}
	
	if(scanner->lastLength == -1)
		scanner->lastLength = scanner->string->length;
	
	if(scanner->index < 0)
		scanner->index = 0;

	LFRuntimeUnlock(scanner);
	LFRuntimeUnlock(scanner->string);
}



LFTypeID LFScannerGetTypeID()
{
	return __LFScannerTypeID;
}

LFScannerRef LFScannerCreateWithString(LFStringRef string)
{
	LFRuntimeLock(string);
	
	if(!LFZombieScribble(string, "LFScannerCreateWithString") && LFRuntimeValidate(string, LFStringGetTypeID()))
	{
		LFScannerRef scanner = LFRuntimeCreateInstance(__LFScannerTypeID, sizeof(struct __LFScanner));
		scanner->string = LFRetain(string);
		
		LFRuntimeUnlock(string);
		return scanner;
	}
	
	LFRuntimeUnlock(string);
	return NULL;
}

LFScannerRef LFScannerWithString(LFStringRef string)
{
	return LFAutorelease(LFScannerCreateWithString(string));
}



void LFScannerSetCaseSensitive(LFScannerRef scanner, uint8_t caseSensitive)
{
	LFRuntimeLock(scanner);
	
	if(LFZombieScribble(scanner, "LFScannerSetCaseSensitive") || !LFRuntimeValidate(scanner, __LFScannerTypeID))
	{
		LFRuntimeUnlock(scanner);
		return;
	}
	
	
	scanner->caseSensitive = caseSensitive;
	LFRuntimeUnlock(scanner);
}

void LFScannerSetLocation(LFScannerRef scanner, LFIndex location)
{
	LFRuntimeLock(scanner);
	
	if(LFZombieScribble(scanner, "LFScannerSetLocation") || !LFRuntimeValidate(scanner, __LFScannerTypeID))
	{
		LFRuntimeUnlock(scanner);
		return;
	}
	
    scanner->index = location;
	LFRuntimeUnlock(scanner);
}


uint8_t LFScannerIsCaseSensitive(LFScannerRef scanner)
{
	LFRuntimeLock(scanner);
	
	if(LFZombieScribble(scanner, "LFScannerIsCaseSensitive") || !LFRuntimeValidate(scanner, __LFScannerTypeID))
	{
		LFRuntimeUnlock(scanner);
		return 0;
	}
	

    uint8_t caseSensitive = scanner->caseSensitive;
    LFRuntimeUnlock(scanner);
    
    return caseSensitive;
}

LFIndex LFScannerGetLocation(LFScannerRef scanner)
{
	LFRuntimeLock(scanner);
	
	if(LFZombieScribble(scanner, "LFScannerGetLocation") || !LFRuntimeValidate(scanner, __LFScannerTypeID))
	{
		LFRuntimeUnlock(scanner);
		return -1;
	}
	
	
    __LFScannerUpdateLastLength(scanner);
    LFIndex index = scanner->index;
    
    LFRuntimeUnlock(scanner);
    return index;
}



uint8_t LFScannerIsAtEnd(LFScannerRef scanner)
{
    if(!scanner)
        return 1; // No scanner == scanner is exhausted
    
	LFRuntimeLock(scanner);
	LFRuntimeLock(scanner->string);
	
	if(LFZombieScribble(scanner, "LFScannerIsAtEnd") || !LFRuntimeValidate(scanner, __LFScannerTypeID))
	{
		LFRuntimeUnlock(scanner->string);
		LFRuntimeUnlock(scanner);
		return 1;
	}
	
	if(scanner)
	{
		__LFScannerUpdateLastLength(scanner);
		
		
		if(scanner->index >= scanner->string->length)
		{
			LFRuntimeUnlock(scanner->string);
			LFRuntimeUnlock(scanner);
			return 1;
		}
		
		LFRuntimeUnlock(scanner->string);
		LFRuntimeUnlock(scanner);
		return 0;
	}
	
	LFRuntimeUnlock(scanner->string);
	LFRuntimeUnlock(scanner);
	return 1;
}


uint8_t LFScannerScanUpToString(LFScannerRef scanner, LFStringRef stopString, LFStringRef *string)
{
	if(!stopString || !scanner)
		return 0;
	
	LFRuntimeLock(scanner);
	LFRuntimeLock(scanner->string);
    LFRuntimeLock(stopString);
	
	if(LFZombieScribble(scanner, "LFScannerScanUpToString") || !LFRuntimeValidate(scanner, __LFScannerTypeID) ||
       !LFRuntimeValidate(stopString, LFStringGetTypeID()))
	{
		LFRuntimeUnlock(scanner);
		LFRuntimeUnlock(scanner->string);
        LFRuntimeUnlock(stopString);
        
		return 0;
	}
	
	if(scanner)
	{
		__LFScannerUpdateLastLength(scanner);
		LFStringCompareFlags flags = 0;
		
		if(scanner->caseSensitive)
			flags |= kLFCompareCaseInsensitive;

		
		
		LFRange range = LFStringFindWithRange(scanner->string, stopString, LFRangeMake(scanner->index, scanner->lastLength - scanner->index), flags);
		if(range.location != kLFNotFound)
		{
			if(scanner->index == range.location)
			{
				if(string)
					*string = NULL;
				
				LFRuntimeUnlock(scanner->string);
				LFRuntimeUnlock(scanner);
                LFRuntimeUnlock(stopString);
                
				return 0;
			}
			
			
			if(string)
			{
				LFRange __range = LFRangeMake(scanner->index, range.location - scanner->index);
				LFStringRef copy = LFStringWithRange(__range, scanner->string);
				
				*string = copy;
			}
			
			scanner->index = range.location;
			
			LFRuntimeUnlock(scanner->string);
			LFRuntimeUnlock(scanner);
            LFRuntimeUnlock(stopString);
            
			return 1;
		}
		else
		{
			if(string)
				*string = NULL;
			
			scanner->index = scanner->string->length;
			
			LFRuntimeUnlock(scanner->string);
			LFRuntimeUnlock(scanner);
            LFRuntimeUnlock(stopString);
            
			return 0;
		}
	}
	
	LFRuntimeUnlock(scanner->string);
	LFRuntimeUnlock(scanner);
    LFRuntimeUnlock(stopString);
    
	return 0;
}

uint8_t LFScannerScanUpToCharacterFromSet(LFScannerRef scanner, LFCharacterSetRef characterSet, LFStringRef *string)
{
	if(!characterSet || !scanner)
		return 0;
	
	LFRuntimeLock(scanner);
	LFRuntimeLock(scanner->string);
	LFRuntimeLock(characterSet);
	
	if(LFZombieScribble(scanner, "LFScannerScanUpToCharacterFromSet") || !LFRuntimeValidate(scanner, __LFScannerTypeID) ||
       !LFRuntimeValidate(characterSet, LFCharacterSetGetTypeID()))
	{
		LFRuntimeUnlock(characterSet);
		LFRuntimeUnlock(scanner->string);
		LFRuntimeUnlock(scanner);
		return 0;
	}
	
	if(scanner)
	{
		__LFScannerUpdateLastLength(scanner);
		
		LFStringCompareFlags flags = 0;
		
		if(scanner->caseSensitive)
			flags |= kLFCompareCaseInsensitive;
		
		
		
		LFRange range = LFStringFindCharacterFromSet(scanner->string, characterSet, LFRangeMake(scanner->index, scanner->lastLength - scanner->index), flags);
		if(range.location != kLFNotFound)
		{
			if(scanner->index == range.location)
			{
				if(string)
					*string = NULL;
				
				LFRuntimeUnlock(characterSet);
				LFRuntimeUnlock(scanner->string);
				LFRuntimeUnlock(scanner);
				return 0;
			}
			
			
			if(string)
			{
				LFRange __range = LFRangeMake(scanner->index, range.location - scanner->index);
				LFStringRef copy = LFStringWithRange(__range, scanner->string);
				
				*string = copy;
			}
			
			scanner->index = range.location;
			
			LFRuntimeUnlock(characterSet);
			LFRuntimeUnlock(scanner->string);
			LFRuntimeUnlock(scanner);
			return 1;
		}
		else
		{
			if(string)
				*string = NULL;
			
			scanner->index = scanner->string->length;
			
			LFRuntimeUnlock(characterSet);
			LFRuntimeUnlock(scanner->string);
			LFRuntimeUnlock(scanner);
			return 0;
		}
	}
	
	LFRuntimeUnlock(characterSet);
	LFRuntimeUnlock(scanner->string);
	LFRuntimeUnlock(scanner);
	return 0;
}
