//
//  LFString.c
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

#include "LFString.h"
#include "LFInternal.h"
#include "LFData.h"
#include "LFAutoreleasePool.h"
#include "LFArchiver.h"

//------------------------------
// Runtime functions
//------------------------------

void		__LFStringInit(LFTypeRef ref);
LFTypeRef	__LFStringCopy(LFTypeRef ref);
void		__LFStringDealloc(LFTypeRef ref);
uint8_t		__LFStringIsEqual(LFTypeRef refa, LFTypeRef refb);
LFHashCode	__LFStringGetHash(LFTypeRef ref);

void __LFStringArchive(LFStringRef ref, LFArchiverRef archiver);

static LFRuntimeClass __LFStringClass;
static LFTypeID __LFStringTypeID = kLFInvalidTypeID;

void __LFRuntimeInitString()
{
	__LFStringClass.version = 1;
	__LFStringClass.name = "LFString";
	
	__LFStringClass.init = __LFStringInit;
	__LFStringClass.copy = __LFStringCopy;
	__LFStringClass.dealloc = __LFStringDealloc;
	__LFStringClass.equal = __LFStringIsEqual;
	__LFStringClass.hash  = __LFStringGetHash;
	
	__LFStringClass.superClass = kLFInvalidTypeID;
	__LFStringClass.protocolBag = NULL;
	
    
    //LFProtocolBody *archiverBody = LFRuntimeCreateProtocolBody(LFArchiverGetProtocolID());
	//LFRuntimeProtocolBodySetIMP(archiverBody, LFArchiverArchiveSelector(), LFProtocolMethod10(__LFStringArchive));
	
	//LFRuntimeAppendProtocol(__LFStringClass.protocolBag, archiverBody);
	__LFStringTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFStringClass);
}


void __LFStringInit(LFTypeRef ref)
{
	LFStringRef string = (LFStringRef)ref;
	
	string->buffer	= NULL;
	string->length	= 1;
	string->space	= 0;
}

LFTypeRef __LFStringCopy(LFTypeRef ref)
{
	return LFStringCreateWithString(ref);
}

void __LFStringDealloc(LFTypeRef ref)
{
	LFStringRef string = (LFStringRef)ref;
	
	if(string->buffer)
		free(string->buffer);
}


LFHashCode __LFStringGetHash(LFTypeRef ref)
{
	LFStringRef string = (LFStringRef)ref;
	uint8_t *content = (uint8_t *)string->buffer;
	LFIndex length = string->length;
	LFIndex	iterator;
	
	LFHashCode res = length;
	
	if(length <= 16) 
	{
		for(iterator=0; iterator<length; iterator++) 
			res = res * 257 + content[iterator];
	} 
	else 
	{
		// Hash the first and last 8 bytes
		for(iterator = 0; iterator<8; iterator++) 
			res = res * 257 + content[iterator];
		
		for(iterator = length - 8; iterator<length; iterator++) 
			res = res * 257 + content[iterator];
	}
	
	return (res << (length & 31));
}

uint8_t	__LFStringIsEqual(LFTypeRef refa, LFTypeRef refb)
{
	LFStringRef stringa = (LFStringRef)refa;
	LFStringRef stringb = (LFStringRef)refb;
	
	// Try to get unequal values fast and then exit the function
	if(stringa->length != stringb->length)
		return 0;
	
	// Alright, a byte compare... here we go
	int res = memcmp(stringa->buffer, stringb->buffer, stringa->length-1); // No need to compare the NULL byte
	if(res == 0)
		return 1;
	
	return 0;
}


//------------------------------
// Instance functions
//------------------------------

void __LFStringArchive(LFStringRef ref, LFArchiverRef archiver)
{
    LFStringRef string = (LFStringRef)ref;
    LFArchiverArchiveBytes(archiver, string->buffer, string->length, LFSTR("buffer"));
    LFArchiverArchiveBytes(archiver, (uint8_t *)&string->space, sizeof(LFIndex), LFSTR("capacity"));
}



uint8_t LFStringValidateRange(LFStringRef string, LFRange *range)
{
	LFRange _range = *range;
	
	if(_range.location + _range.length > string->length - 1) 
		_range.length = (string->length - 1) - _range.location; 
	
	if(_range.length <= 0 || _range.location < 0)
		return 0;
	
	
	(*range).location = _range.location;
	(*range).length = _range.length;
	
	return 1;
}


LFTypeID LFStringGetTypeID()
{
	return __LFStringTypeID;
}

LFStringRef LFStringCreate()
{
	LFStringRef string = LFRuntimeCreateInstance(__LFStringTypeID, sizeof(struct __LFString));
	
	uint8_t *buffer = (uint8_t *)malloc(10 * sizeof(uint8_t));
	memset(buffer, 0, 10 * sizeof(uint8_t));
	
	string->buffer	= buffer;
	string->length	= 1;
	string->space	= 10;
	
	return string;
}

LFStringRef LFStringCreateWithCString(char *cstring)
{
	if(!cstring)
		return NULL;
	
	LFIndex length = (LFIndex)strlen(cstring) + 1;
	uint8_t *buffer = (uint8_t *)malloc(length * sizeof(uint8_t));
	
	if(buffer)
	{
		LFStringRef string = LFRuntimeCreateInstance(__LFStringTypeID, sizeof(struct __LFString));
		if(!string)
		{
			free(buffer);
			return NULL;
		}
		
		memcpy(buffer, cstring, length);
		
		string->buffer	= buffer;
		string->length	= length;
		string->space	= length;
		
		return string;
	}
	
	return NULL;
}

LFStringRef LFStringWithCString(char *cstring)
{
	return LFAutorelease(LFStringCreateWithCString(cstring));
}

LFStringRef LFStringCreateWithRange(LFRange range, LFStringRef source)
{
	if(range.location < 0 || range.length <= 0)
		return NULL;
	
	LFRuntimeLock(source);
	
	if(LFZombieScribble(source, "LFStringCreateWithRange"))
	{
		LFRuntimeUnlock(source);
		return NULL;
	}
	
	if(LFRuntimeValidate(source, __LFStringTypeID))
	{
		if(!LFStringValidateRange(source, &range))
		{
			LFRuntimeUnlock(source);
			return NULL;
		}
		
		
		LFIndex offset = range.location;
		LFIndex length = range.length + 1; // NULL byte
		
		uint8_t *buffer = (uint8_t *)malloc(length * sizeof(uint8_t));
		if(!buffer)
		{
			LFRuntimeUnlock(source);
			return NULL;
		}
		
		LFStringRef copy = LFStringCreate();
		if(copy)
		{
			memcpy(buffer, &(source->buffer)[offset], length-1);
			buffer[length-1] = '\0';
			
			copy->buffer	= buffer;
			copy->length	= length;
			copy->space		= length;
			
			LFRuntimeUnlock(source);
			return copy;
		}
		
		free(buffer); // The copy couldn't be created so we need to get rid of the buffer
	}
	
	LFRuntimeUnlock(source);	
	return NULL;
}

LFStringRef LFStringWithRange(LFRange range, LFStringRef source)
{
	return LFAutorelease(LFStringCreateWithRange(range, source));
}


LFStringRef LFStringCreateWithString(LFStringRef source)
{
	LFRuntimeLock(source);
	LFStringRef string = NULL;
	
	
	if(LFRuntimeValidate(source, __LFStringTypeID) && !LFZombieScribble(source, "LFStringCreateWithString"))
		string = LFStringCreateWithCString((char *)source->buffer);		
	
	LFRuntimeUnlock(source);
	return string;
}

LFStringRef LFStringWithString(LFStringRef source)
{
	return LFAutorelease(LFStringCreateWithString(source));
}



LFIndex LFStringGetLength(LFStringRef string)
{
	LFRuntimeLock(string);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID)  || LFZombieScribble(string, "LFStringGetLength"))
	{
		LFRuntimeUnlock(string);
		return 0;
	}
	
	
	LFIndex length = string->length - 1;
	LFRuntimeUnlock(string);
	
	return length;
}

char LFStringGetCharacterAtIndex(LFStringRef string, LFIndex index)
{
	LFRuntimeLock(string);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID) || LFZombieScribble(string, "LFStringGetCharacterAtIndex"))
	{
		LFRuntimeUnlock(string);
		return '\0';
	}
	
	if(string->length <= index)
	{
		LFRuntimeUnlock(string);
		return '\0';
	}
		
	char character = (string->buffer)[index];
	LFRuntimeUnlock(string);
	
	return character;
}

LFDataRef LFDataCreateWithBytesNoCopy(uint8_t *bytes, LFIndex size);

char *LFStringGetCString(LFStringRef string)
{
	LFRuntimeLock(string);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID) || LFZombieScribble(string, "LFStringGetCString"))
	{
		LFRuntimeUnlock(string);
		return NULL;
	}
	
	
	char *copy = (char *)malloc(string->length * sizeof(char));
	if(copy)
	{
		strcpy(copy, (char *)string->buffer);
		
		// Make sure the copy is released
		LFDataRef data = LFDataCreateWithBytesNoCopy((uint8_t *)copy, string->length);
		LFAutorelease(data);
	}
	
	LFRuntimeUnlock(string);
	return copy;
}




LFComparisonResult LFStringCompareWithRange(LFStringRef stringa, LFStringRef stringb, LFRange range, LFStringCompareFlags options)
{
	LFRuntimeLock(stringa);
	LFRuntimeLock(stringb);
	
	if(!LFRuntimeValidate(stringa, __LFStringTypeID) || !LFRuntimeValidate(stringb, __LFStringTypeID) ||
	   LFZombieScribble(stringa, "LFStringCompareWithRange") || LFZombieScribble(stringb, "LFStringCompareWithRange"))
	{
		LFRuntimeUnlock(stringa);
		LFRuntimeUnlock(stringb);
		
		return kLFCompareLessThan;
	}
	
	if(range.location < 0 || range.length <= 0)
	{
		LFRuntimeUnlock(stringa);
		LFRuntimeUnlock(stringb);
		
		return kLFCompareLessThan;
	}

	
	
	LFIndex lengtha = stringa->length - 1;
	LFIndex lengthb = stringb->length - 1;
		
	if(!LFStringValidateRange(stringa, &range) || !LFStringValidateRange(stringb, &range))
	{
		if(lengtha > lengthb)
		{
			LFRuntimeUnlock(stringa);
			LFRuntimeUnlock(stringb);
			
			return kLFCompareGreaterThan;
		}
			
		LFRuntimeUnlock(stringa);
		LFRuntimeUnlock(stringb);
		
		return kLFCompareLessThan;
	}
		
		
		
#define __LFStringCharacterConvertToLowercase(c) if(c >= 'A' && c <= 'Z') c += 32
		
	LFIndex offset;
	for(offset=0; offset < range.length; offset++)
	{
		uint8_t chara = (stringa->buffer)[range.location + offset];
		uint8_t charb = (stringb->buffer)[range.location + offset];
		
		if((options & kLFCompareNumerically))
		{
			// Check if we really deal with numbers
			if(chara <= '9' && chara >= '0' && charb <= '9' && charb >= '0')
			{
				uint32_t numa = 0;
				uint32_t numb = 0;
				
				LFIndex poffset = offset;
				
				// Get the numeric value of the stringa
				do 
				{
					numa = numa * 10 + (chara - '0');
					offset ++;
					
					if(offset >= range.length)
						break;
					
					chara = (stringa->buffer)[range.location + offset];
				} while(chara <= '9' && chara >= '0');
				
				
				// Get the numeric value of the stringb
				offset = poffset;
				do 
				{
					numb = numb * 10 + (charb - '0');
					offset ++;
					
					if(offset >= range.length)
						break;
					
					charb = (stringb->buffer)[range.location + offset];
				} while(charb <= '9' && charb >= '0');
				
				
				
				if(numa > numb)
				{
					LFRuntimeUnlock(stringa);
					LFRuntimeUnlock(stringb);
					
					return kLFCompareGreaterThan;
				}
				
				if(numa < numb)
				{
					LFRuntimeUnlock(stringa);
					LFRuntimeUnlock(stringb);
					
					return kLFCompareLessThan;
				}
				
				offset = poffset; // We don't know where both numbers end (they may end at different places) so we reset the offset
				continue; // If both numbers are equal, we need to compare the rest of the string
			}
		}
		
		
		if((options & kLFCompareCaseInsensitive))
		{
			// If the user wants a case insensitive search, we need to convert both characters to lowercase
			__LFStringCharacterConvertToLowercase(chara);
			__LFStringCharacterConvertToLowercase(charb);
		}
		
		
		if(chara != charb)
		{
			uint8_t lwca = chara;
			uint8_t lwcb = charb;
			
			__LFStringCharacterConvertToLowercase(lwca);
			__LFStringCharacterConvertToLowercase(lwcb);
			
			if(lwca != lwcb)
			{
				chara = lwca;
				charb = lwcb;
			}
			else
			{
				if(chara > charb)
				{
					LFRuntimeUnlock(stringa);
					LFRuntimeUnlock(stringb);
					
					return kLFCompareLessThan;
				}
				
				if(chara < charb)
				{
					LFRuntimeUnlock(stringa);
					LFRuntimeUnlock(stringb);
					
					return kLFCompareGreaterThan;
				}
			}
		}
		
		
		if(chara > charb)
		{	
			LFRuntimeUnlock(stringa);
			LFRuntimeUnlock(stringb);
			
			return kLFCompareGreaterThan;
		}
		
		if(chara < charb)
		{
			LFRuntimeUnlock(stringa);
			LFRuntimeUnlock(stringb);
			
			return kLFCompareLessThan;
		}
	}
	
	LFRuntimeUnlock(stringa);
	LFRuntimeUnlock(stringb);
	
	return kLFCompareEqualTo;
#undef __LFStringCharacterConvertToLowercase
}


LFComparisonResult LFStringCompare(LFStringRef refa, LFStringRef refb, LFStringCompareFlags options)
{
	LFRuntimeLock(refa);
	LFRuntimeLock(refb);
	
	if(!LFRuntimeValidate(refa, __LFStringTypeID) || !LFRuntimeValidate(refb, __LFStringTypeID) ||
	   LFZombieScribble(refa, "LFStringCompare") || LFZombieScribble(refb, "LFStringCompare"))
	{
		LFRuntimeUnlock(refa);
		LFRuntimeUnlock(refb);
		return kLFCompareLessThan;
	}
	
	
	LFIndex lengtha = LFStringGetLength(refa);
	LFIndex lengthb = LFStringGetLength(refb);
	
	if(lengtha > lengthb)
		lengtha = lengthb;
	
	LFComparisonResult comparisonResult = LFStringCompareWithRange(refa, refb, LFRangeMake(0, lengtha), options);
	
	LFRuntimeUnlock(refa);
	LFRuntimeUnlock(refb);
	
	return comparisonResult;
}


LFRange LFStringFindWithRange(LFStringRef stringa, LFStringRef stringb, LFRange range, LFStringCompareFlags options)
{	
	LFRuntimeLock(stringa);
	LFRuntimeLock(stringb);
	
	if(!LFRuntimeValidate(stringa, __LFStringTypeID) || !LFRuntimeValidate(stringb, __LFStringTypeID) ||
	   LFZombieScribble(stringa, "LFStringFindWithRange") || LFZombieScribble(stringb, "LFStringFindWithRange"))
	{
		LFRuntimeUnlock(stringa);
		LFRuntimeUnlock(stringb);
		
		return LFRangeMake(kLFNotFound, 0);
	}
	
	
	LFRange resultRange = LFRangeMake(kLFNotFound, 0);
	if(!LFStringValidateRange(stringa, &range))
	{
		LFRuntimeUnlock(stringa);
		LFRuntimeUnlock(stringb);
		
		return resultRange;
	}
	
	
    LFIndex length = stringb->length - 1;
	LFIndex offset;
	LFIndex matching = 0;
	
#define __LFStringCharacterConvertToLowercase(c) if(c >= 'A' && c <= 'Z') c += 32
	
	if(!(options & kLFCompareBackwards))
	{
		// Forward search
		for(offset=0; offset < range.length; offset++)
		{
			uint8_t chara = (stringa->buffer)[range.location + offset];
			uint8_t charb = (stringb->buffer)[matching];
			
			if((options & kLFCompareCaseInsensitive))
			{
				__LFStringCharacterConvertToLowercase(chara);
				__LFStringCharacterConvertToLowercase(charb);
			}
			
			// Check if both string matches
			if(charb == chara)
			{
				if(matching == 0)
					resultRange.location = range.location + offset; // If this is the very first matching character, store its position
				
				matching ++;
				if(matching >= length) // We found the whole string
				{
					resultRange.length = matching;
					
					LFRuntimeUnlock(stringa);
					LFRuntimeUnlock(stringb);
					return resultRange;
				}
			}
			else
			{
				// There is a difference, reset the range and the matching length
				resultRange.location = kLFNotFound;
				matching = 0;
			}
		}
	}
	else
	{			
		// Backward search, same as forward search
		for(offset=length; offset >= 0; offset--)
		{
			uint8_t chara = (stringa->buffer)[range.location + offset];
			uint8_t charb = (stringb->buffer)[(stringb->length - 1) - matching]; // As this is a backward search, we also need to crawl the string to find in backwards order

			if((options & kLFCompareCaseInsensitive))
			{
				__LFStringCharacterConvertToLowercase(chara);
				__LFStringCharacterConvertToLowercase(charb);
			}
			
			
			if(charb == chara)
			{
				matching ++;
				if(matching >= range.length) // We found the whole string
				{
					resultRange.location = range.location + offset;
					resultRange.length	= matching;
					
					LFRuntimeUnlock(stringa);
					LFRuntimeUnlock(stringb);
					
					return resultRange;
				}
			}
			else
			{
				resultRange.location = kLFNotFound;
				matching = 0;
			}
		}
	}
	
#undef __LFStringCharacterConvertToLowercase
	
	LFRuntimeUnlock(stringa);
	LFRuntimeUnlock(stringb);
	return resultRange;
}

LFRange LFStringFind(LFStringRef refa, LFStringRef refb, LFStringCompareFlags options)
{
	LFRuntimeLock(refa);
	LFRuntimeLock(refb);
	
	if(!LFRuntimeValidate(refa, __LFStringTypeID) || !LFRuntimeValidate(refb, __LFStringTypeID) ||
	   LFZombieScribble(refa, "LFStringFind") || LFZombieScribble(refb, "LFStringFind"))
	{
		LFRuntimeUnlock(refa);
		LFRuntimeUnlock(refb);
		return LFRangeMake(kLFNotFound, 0);
	}
	
	
	LFIndex length = refa->length - 1;
	LFRange foundRange = LFStringFindWithRange(refa, refb, LFRangeMake(0, length), options);
	
	LFRuntimeUnlock(refa);
	LFRuntimeUnlock(refb);
	return foundRange;
}



LFRange LFStringFindCharacterFromSet(LFStringRef string, LFCharacterSetRef characterSet, LFRange range, LFStringCompareFlags options)
{
	LFRuntimeLock(string);
	LFRuntimeLock(characterSet);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID) || !LFRuntimeValidate(characterSet, LFCharacterSetGetTypeID()) ||
	   LFZombieScribble(string, "LFStringFindCharacterFromSet") || LFZombieScribble(characterSet, "LFStringFindCharacterFromSet"))
	{
		LFRuntimeUnlock(characterSet);
		LFRuntimeUnlock(string);
		
		return LFRangeMake(kLFNotFound, 0);
	}
	
	
	
	LFIndex offset;
	uint8_t *buffer = characterSet->bucket;
	
	// Bound check
	if(!LFStringValidateRange(string, &range))
	{
		LFRuntimeUnlock(characterSet);
		LFRuntimeUnlock(string);
		return LFRangeMake(kLFNotFound, 0);
	}
	
	
	
	// Search the string
	if(!(options & kLFCompareBackwards))
	{
		for(offset=0; offset<range.length; offset++)
		{
			uint8_t character = (string->buffer)[range.location + offset];
			
			if(buffer[character] == 1)
			{
				LFRuntimeUnlock(characterSet);
				LFRuntimeUnlock(string);
				
				return LFRangeMake(range.location + offset, 1);
			}
		}
	}
	else
	{
		for(offset=range.length-1; offset>=0; offset--)
		{
			uint8_t character = (string->buffer)[range.location + offset];
			
			if(buffer[character] == 1)
			{
				LFRuntimeUnlock(characterSet);
				LFRuntimeUnlock(string);
				
				return LFRangeMake(range.location + offset, 1);
			}
		}
	}
	
	LFRuntimeUnlock(characterSet);
	LFRuntimeUnlock(string);
	
	return LFRangeMake(kLFNotFound, 0);
}



uint8_t LFStringHasPrefix(LFStringRef string, LFStringRef prefix)
{
	LFRuntimeLock(string);
	LFRuntimeLock(prefix);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID) || !LFRuntimeValidate(prefix, __LFStringTypeID) ||
	   LFZombieScribble(string, "LFStringHasPrefix") || LFZombieScribble(prefix, "LFStringHasPrefix"))
	{
		LFRuntimeUnlock(string);
		LFRuntimeUnlock(prefix);
		return 0;
	}
	
	
	LFRange range = LFRangeMake(0, LFStringGetLength(prefix));
	LFRange resultRange = LFStringFindWithRange(string, prefix, range, 0);
	
	if(resultRange.location != kLFNotFound)
	{
		LFRuntimeUnlock(string);
		LFRuntimeUnlock(prefix);
		
		return 1;
	}
	
	LFRuntimeUnlock(string);
	LFRuntimeUnlock(prefix);
	
	return 0;
}

uint8_t LFStringHasSuffix(LFStringRef string, LFStringRef suffix)
{
	LFRuntimeLock(string);
	LFRuntimeLock(suffix);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID) || !LFRuntimeValidate(suffix, __LFStringTypeID) ||
	   LFZombieScribble(string, "LFStringHasSuffix") || LFZombieScribble(suffix, "LFStringHasSuffix"))
	{
		LFRuntimeUnlock(string);
		LFRuntimeUnlock(suffix);
		return 0;
	}
	
	
	LFRange range = LFRangeMake(LFStringGetLength(string), LFStringGetLength(suffix));
	range.location -= range.length;
	
	LFRange resultRange = LFStringFindWithRange(string, suffix, range, 0);
	
	if(resultRange.location != kLFNotFound)
	{
		LFRuntimeUnlock(string);
		LFRuntimeUnlock(suffix);
		
		return 1;
	}
	
	LFRuntimeUnlock(string);
	LFRuntimeUnlock(suffix);
	
	return 0;
}



LFArrayRef LFStringComponentsSeperatedByString(LFStringRef string, LFStringRef seperator)
{
	LFRuntimeLock(string);
	LFRuntimeLock(seperator);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID) || !LFRuntimeValidate(seperator, __LFStringTypeID) ||
	   LFZombieScribble(string, "LFStringComponentsSeperatedByString") || LFZombieScribble(seperator, "LFStringComponentsSeperatedByString"))
	{
		LFRuntimeUnlock(string);
		LFRuntimeUnlock(seperator);
		return 0;
	}
	

	LFArrayRef array = LFArrayCreate(NULL);
	
	LFIndex length = string->length - 1;
	LFRange range  = LFRangeMake(0, length);
	
	while(1)
	{
		LFRange resultRange = LFStringFindWithRange(string, seperator, range, 0);
		if(resultRange.location == kLFNotFound)
		{
			// Append the remainder of the string
			LFStringRef substring = LFStringWithRange(range, string);
			LFArrayAddValue(array, substring);
			
			break;
		}
		
		range.length = resultRange.location - range.location;
		
		LFStringRef substring = LFStringWithRange(range, string);
		LFArrayAddValue(array, substring);
		
		range.location = resultRange.location + resultRange.length;
		range.length -= range.location;
	}
	
	LFRuntimeUnlock(string);
	LFRuntimeUnlock(seperator);
	
	return LFAutorelease(array);
}


// Does not set a new NULL byte!
uint8_t __LFStringTryResize(LFStringRef string, LFIndex newSize)
{
	uint8_t *tBuffer = (uint8_t	*)realloc(string->buffer, newSize);
	if(tBuffer)
	{
		string->space = newSize;
		string->buffer = tBuffer;
		
		return 1;
	}
	
	return 0;
}

#define __LFStringResizeIfNeeded(str, len) if((str->length + len) >= str->space) {LFAssert(__LFStringTryResize(str, str->space + len + 20), "Failed to mutate LFString, not enough memory!");}


void LFStringAppend(LFStringRef string, LFStringRef appendString)
{
	LFRuntimeLock(string);
	LFRuntimeLock(appendString);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID) || !LFRuntimeValidate(appendString, __LFStringTypeID) ||
	   LFZombieScribble(string, "LFStringAppend") || LFZombieScribble(appendString, "LFStringAppend"))
	{
		LFRuntimeUnlock(string);
		LFRuntimeUnlock(appendString);
		return;
	}
	

	LFStringAppendCString(string, (char *)appendString->buffer);
	
	LFRuntimeUnlock(string);
	LFRuntimeUnlock(appendString);
}

void LFStringAppendCString(LFStringRef string, char *appendCString)
{
	LFRuntimeLock(string);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID) || LFZombieScribble(string, "LFStringAppendCString"))
	{
		LFRuntimeUnlock(string);
		return;
	}
	
	if(!appendCString)
	{
		LFRuntimeUnlock(string);
		return;
	}
	
	
	
	LFIndex appendLength = (LFIndex)strlen(appendCString);		
	__LFStringResizeIfNeeded(string, appendLength)
	
	memcpy(&(string->buffer)[string->length-1], appendCString, appendLength + 1);
	string->length += appendLength;
	
	LFRuntimeUnlock(string);
}


void LFStringInsertAtIndex(LFStringRef string, LFIndex index, LFStringRef insertString)
{
	LFRuntimeLock(string);
	LFRuntimeLock(insertString);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID) || !LFRuntimeValidate(insertString, __LFStringTypeID) ||
	   LFZombieScribble(string, "LFStringInsertAtIndex") || LFZombieScribble(insertString, "LFStringInsertAtIndex"))
	{
		LFRuntimeUnlock(string);
		LFRuntimeUnlock(insertString);
		return;
	}
	
	
	LFStringInsertCStringAtIndex(string, index, (char *)insertString->buffer);
	LFRuntimeUnlock(string);
	LFRuntimeUnlock(insertString);
}

void LFStringInsertCStringAtIndex(LFStringRef string, LFIndex index, char *insertCString)
{
	LFRuntimeLock(string);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID) || LFZombieScribble(string, "LFStringInsertCStringAtIndex"))
	{
		LFRuntimeUnlock(string);
		return;
	}
	
	if(!insertCString || index > string->length - 1)
	{
		LFRuntimeUnlock(string);
		return;
	}
	
	
	LFIndex shiftLength  = string->length - index;
	LFIndex insertLength = (LFIndex)strlen(insertCString);
	
	__LFStringResizeIfNeeded(string, insertLength)
	
	memcpy(&(string->buffer)[index+insertLength], &(string->buffer)[index], shiftLength);
	memcpy(&(string->buffer)[index], insertCString, insertLength);
	
	string->length += insertLength;
	
	LFRuntimeUnlock(string);
}



void LFStringDeleteRange(LFStringRef string, LFRange range)
{
	LFRuntimeLock(string);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID) || LFZombieScribble(string, "LFStringDeleteRange"))
	{
		LFRuntimeUnlock(string);
		return;
	}
	
	if(!LFStringValidateRange(string, &range))
	{
		LFRuntimeUnlock(string);
		return;
	}
	
	
	
	LFIndex startingIndex = range.location + range.length;
	LFIndex copyLength = string->length - startingIndex;
	
	memcpy(&(string->buffer)[range.location], &(string->buffer)[startingIndex], copyLength);
	
	string->length -= range.length;
	(string->buffer)[string->length] = '\0';
	
	if(string->length < (string->space - 20))
	{
		uint8_t *tBuffer = (uint8_t	*)realloc(string->buffer, string->space - 20);
		if(tBuffer)
		{
			string->buffer = tBuffer;
			string->space -= 20;
		}
	}
	
	LFRuntimeUnlock(string);
}


void LFStringReplaceOccurrencesOfString(LFStringRef string, LFStringRef lookFor, LFStringRef replacement)
{
	LFRuntimeLock(string);
	LFRuntimeLock(lookFor);
	LFRuntimeLock(replacement);
	
	char *fnct = "LFStringReplaceOccurrencesOfString";
	if(!LFRuntimeValidate(string, __LFStringTypeID) || !LFRuntimeValidate(lookFor, __LFStringTypeID) || !LFRuntimeValidate(replacement, __LFStringTypeID) ||
	   LFZombieScribble(string, fnct) || LFZombieScribble(lookFor, fnct) || LFZombieScribble(replacement, fnct))
	{
		LFRuntimeUnlock(string);
		LFRuntimeUnlock(lookFor);
		LFRuntimeUnlock(replacement);
		return;
	}
	
	
	
	LFRange range  = LFRangeMake(0, string->length - 1);
	LFRange lastRange;
	
	LFIndex shiftLength = (lookFor->length - 1) - (replacement->length - 1);
	if(shiftLength < 0)
		shiftLength *= -1;
	
	while(1)
	{
		lastRange = LFStringFindWithRange(string, lookFor, range, 0);
		
		if(lastRange.location != kLFNotFound)
		{
			LFStringDeleteRange(string, lastRange);
			LFStringInsertAtIndex(string, lastRange.location, replacement);
			
			
			range.location = lastRange.location;
			range.length -= shiftLength;
		}
		else
			break;
	}
	
	LFRuntimeUnlock(string);
	LFRuntimeUnlock(lookFor);
	LFRuntimeUnlock(replacement);
}


#define __LFStringCharacterConvertToLowercase(c) if(c >= 'A' && c <= 'Z') c += 32
#define __LFStringCharacterConvertToUppercase(c) if(c >= 'a' && c <= 'z') c -= 32

void LFStringLowercase(LFStringRef string)
{
	LFRuntimeLock(string);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID) || LFZombieScribble(string, "LFStringLowercase"))
	{
		LFRuntimeUnlock(string);
		return;
	}
	
	if(string)
	{
		LFIndex index;
		LFIndex length = string->length - 1;
		
		for(index = 0; index < length; index++)
			__LFStringCharacterConvertToLowercase((string->buffer)[index]);
	}
	
	LFRuntimeUnlock(string);
}

void LFStringUppercase(LFStringRef string)
{
	LFRuntimeLock(string);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID) || LFZombieScribble(string, "LFStringUppercase"))
	{
		LFRuntimeUnlock(string);
		return;
	}
	
	if(string)
	{
		LFIndex index;
		LFIndex length = string->length - 1;
		
		for(index = 0; index < length; index++)
			__LFStringCharacterConvertToUppercase((string->buffer)[index]);
	}
	
	LFRuntimeUnlock(string);
}

#undef __LFStringCharacterConvertToLowercase
#undef __LFStringCharacterConvertToUppercase
#undef __LFStringResizeIfNeeded



LFStringRef LFStringSubstringFromIndex(LFStringRef string, LFIndex index)
{
	LFRuntimeLock(string);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID) || LFZombieScribble(string, "LFStringSubstringFromIndex"))
		return NULL;
	

	LFRange range = LFRangeMake(index, (string->length - 1) - index);
	LFStringRef newString = LFStringWithRange(range, string);
	
	LFRuntimeUnlock(string);
	return newString;
}

LFStringRef LFStringSubstringToIndex(LFStringRef string, LFIndex index)
{
	LFRuntimeLock(string);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID) || LFZombieScribble(string, "LFStringSubstringToIndex"))
	{
		LFRuntimeUnlock(string);
		return NULL;
	}
	

	LFRange range = LFRangeMake(0, index);
	LFStringRef newString = LFStringWithRange(range, string);
	LFRuntimeUnlock(string);
	
	return newString;
}

LFStringRef LFStringStringByAppendingString(LFStringRef string, LFStringRef stringToAppend)
{
	LFRuntimeLock(string);
	LFRuntimeLock(stringToAppend);
	
	if(!LFRuntimeValidate(string, __LFStringTypeID) || !LFRuntimeValidate(stringToAppend, __LFStringTypeID) || 
	   LFZombieScribble(string, "LFStringStringByAppendingString") || LFZombieScribble(stringToAppend, "LFStringStringByAppendingString"))
	{
		LFRuntimeUnlock(string);
		LFRuntimeUnlock(stringToAppend);
		return NULL;
	}
	

	LFStringRef newString = LFStringCreateWithString(string);
	LFStringAppend(newString, stringToAppend);
	
	if(newString->length + 1 < (string->length - 1) + stringToAppend->length)
	{
		// Looks like something went wrong...
		LFRelease(newString);
		LFRuntimeUnlock(string);
		LFRuntimeUnlock(stringToAppend);
		
		return NULL;
	}
	
	LFRuntimeUnlock(string);
	LFRuntimeUnlock(stringToAppend);
	
	return LFAutorelease(newString);
}
