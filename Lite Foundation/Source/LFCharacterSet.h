//
//  LFCharacterSet.h
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

#ifndef _LFCHARACTERSET_H_
#define _LFCHARACTERSET_H_

/**
 * @defgroup LFCharacterSetRef LFCharacterSetRef
 * @{
 * @brief The LFCharacterSetRef represents a set of character
 *
 * A LFCharacterSetRef is a container for a set of characters. Unlike a string, a set can contain a character only once and stores them unordered.
 * Character sets are used by LFStringRef and LFScannerRef to allow scanning up to a character from the set.
 * <br />
 * There is a set of default character sets with common used characters, like punctuation and whitespaces.
 **/

#include "LFBase.h"
#include "LFRuntime.h"

/**
 * Internal structure of the set
 **/
struct __LFCharacterSet
{
	LFRuntimeBase base;
	
	/**
	 * Bucket which is used to store the character sets characters
	 **/
	uint8_t bucket[256];
};

/**
 * Character set type
 **/
typedef struct __LFCharacterSet* LFCharacterSetRef;

/**
 * Constant for the alphanumeric set. Contains the letters a to z (lowercase and uppercase) plus the numbers 0 to 9
 **/
#define kLFCharacterSetAlphanumeric 0
/**
 * Constant for the lowercase letter set. Contains the letters a to z
 **/
#define kLFCharacterSetLowercase	1
/**
 * Constant for the uppercase letter set. Contains the letters A to Z
 **/
#define kLFCharacterSetUppercase	2
/**
 * Constant for the punctuation set. Contains ".,-_(){}[]"
 **/
#define kLFCharacterSetPunctuation	3
/**
 * Constant for the whitespace set. Contains a whitespace and the newline character, but not the carriage return character.
 **/
#define kLFCharacterSetWhitespace	4

/**
 * Returns the character set ID, can be used to create new instances.
 **/
LF_EXPORT LFTypeID LFCharacterSetGetTypeID();

/**
 * Returns a new character set containing all the characters found in the given string
 **/
LF_EXPORT LFCharacterSetRef LFCharacterSetCreateWithString(LFTypeRef string);
/**
 * Returns one of the predefined set.
 * @remark As per ownership rule defined, you don't need to release the returned object!
 * @sa kLFCharacterSetAlphanumeric
 * @sa kLFCharacterSetLowercase
 * @sa kLFCharacterSetUppercase
 * @sa kLFCharacterSetPunctuation
 * @sa kLFCharacterSetWhitespace
 **/
LF_EXPORT LFCharacterSetRef LFCharacterSetPredefinedSet(uint8_t cset);
/**
 * Returns a copy of the given character set, but completely inverted.
 * @remark The returned set is autoreleased
 **/
LF_EXPORT LFCharacterSetRef LFCharacterSetInvertedSet(LFCharacterSetRef cset);


/**
 * Returns true if the given character is in the set
 **/
LF_EXPORT uint8_t LFCharacterSetIsMember(LFCharacterSetRef cset, char character);

/**
 * @}
 **/

#endif
