//
//  LFString.h
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

#ifndef _LFSTRING_H_
#define _LFSTRING_H_

/**
 * @defgroup LFStringRef LFStringRef
 * @{
 * @brief The LFStringRef is Lite Foundations string class
 *
 * A LFStringRef is basically an array of characters which also stores some additional informations.
 * 
 * The LFStringRef's hashing function has naturally a few collisions as strings can contain a lot more than 4294967295 states.
 * The function takes into account the first 8 and last 8 bytes to generate the hash (or if the string is smaller than 16 bytes it will take the whole strings length).
 * Thus "12345678a1234568" and "12345678d1234568" generate the same hash, although their content is not the same.
 * 
 **/

#include "LFRuntime.h"
#include "LFBase.h"
#include "LFCharacterSet.h"
#include "LFArray.h"

/**
 * @brief Internal string Layout
 *
 * While the layout of the class is open, its highly discouraged to alter any of its members!
 **/
struct __LFString
{
	LFRuntimeBase base;
	
	/**
	 * The buffer which holds the string.
	 * @remark The buffers layout is not defined, although it currently holds only primitive C strings, it might become support for eg. UTF16 in the future
	 **/
	uint8_t *buffer;
	/**
	 * The length of the string, inclusive NULL byte
	 **/
	LFIndex length;
	/**
	 * The number of bytes allocated for the buffer
	 **/
	LFIndex space;
};

/**
 * The string type
 **/
typedef struct __LFString* LFStringRef;
/**
 * Type that is able to hold comparision flags as bit field
 **/
typedef uint8_t LFStringCompareFlags;

/**
 * Flag used to indicate a case insensitive comparision.
 * This means that the string will tread eg. 'a' the same as an 'A'
 **/
#define kLFCompareCaseInsensitive	1
/**
 * Flag used to indicate a backward comparision.
 * Backward comparision isn't supported by all string functions, as they might make no sense for them. 
 * See the description of the string functions that take an option bit field for more detail
 **/
#define kLFCompareBackwards			4
/**
 * Flag used to indicate a numerical search. A numerical search takes the integer value of the number in the string instead of its char value
 **/
#define kLFCompareNumerically		8



#define LFSTR(str) LFStringWithCString(str)

// ------------------------
// Basic implementation
// ------------------------

/**
 * Returns the strings type ID
 * @remark Please create strings only using the LFStringCreate... functions!
 **/
LF_EXPORT LFTypeID LFStringGetTypeID();

/**
 * Creates a empty string and returns int
 **/
LF_EXPORT LFStringRef LFStringCreate();
/**
 * Creates a string containing the given C String
 * @param cstring A NULL terminated C String
 * @remark The C string will be copied into the strings internal buffer
 **/
LF_EXPORT LFStringRef LFStringCreateWithCString(char *cstring);
/**
 * Same as calling LFStringCreateWithCString() but returns an autoreleased string.
 **/
LF_EXPORT LFStringRef LFStringWithCString(char *cstring);
/**
 * Creates a new string with the content of the given source string at the given range
 * @param range The range of the new string inside the source string. This can be partially out of bounds, however, the string will only allocate enough memory for the string inside the boundaries
 * @remark If the range is completely out of bounds (eg. because the source is empty), the function will fail and return NULL
 **/
LF_EXPORT LFStringRef LFStringCreateWithRange(LFRange range, LFStringRef source);
/**
 * Sames as calling LFStringCreateWithRange() but returns an autoreleased string
 **/
LF_EXPORT LFStringRef LFStringWithRange(LFRange range, LFStringRef source);
/**
 * Creates a new string containing the given source string
 **/
LF_EXPORT LFStringRef LFStringCreateWithString(LFStringRef source);
/**
 * Sames as calling LFStringCreateWithString() but returns an autoreleased string
 **/
LF_EXPORT LFStringRef LFStringWithString(LFStringRef source);

/**
 * Returns the length of the given string
 * @return The length of the string, counting only characters.
 **/
LF_EXPORT LFIndex LFStringGetLength(LFStringRef string);
/**
 * Returns the character at the givens strings index
 * @return The character at the given index or '\0' when the index is out of bounds
 * @sa LFStringGetCString()
 **/
LF_EXPORT char LFStringGetCharacterAtIndex(LFStringRef string, LFIndex index);
/**
 * Retunrs the C string representation of the string
 * @remark Currently this will return the strings internal buffer, you must not alter this! This also might change in the future
 **/
LF_EXPORT char *LFStringGetCString(LFStringRef string);


// ------------------------
// Comparison
// ------------------------

/**
 * Compares string A with string B, taking into account only the given range.
 * @param range The range that should be compared. This can be slightly out of bounds in both strings, the function will then only compare the most possible comparable values. If the range is completely out of bound, the function will return a value != kLFCompareEqual
 * @param options The options for the comparision. Please remark that the function won't do a backward search
 * @sa LFStringCompare()
 **/
LF_EXPORT LFComparisonResult LFStringCompareWithRange(LFStringRef stringA, LFStringRef stringB, LFRange range, LFStringCompareFlags options);
/**
 * Same as calling LFStringCompareWithRange() on the whole string
 * @sa LFStringCompareWithRange()
 **/
LF_EXPORT LFComparisonResult LFStringCompare(LFStringRef stringA, LFStringRef stringB, LFStringCompareFlags options);

/**
 * Returns the range of the first occurence of the given string inside the given string starting at the given range.
 * @param range The range where the functions should lookf for the string. This can be slightly out of bounds, if its too far out of bound to find any occurence of the searched string, it will return {kLFNotFound, 0}
 * @param options The options for the search as bit field. The search can be a backwards search.
 * @return The range of the string or {kLFNotFound, 0}
 * @sa LFStringFind()
 **/
LF_EXPORT LFRange LFStringFindWithRange(LFStringRef string, LFStringRef stringToFind, LFRange range, LFStringCompareFlags options);
/**
 * Same as calling LFStringFindWithRange() with the whole range of the string
 * @sa LFStringFindWithRange()
 **/
LF_EXPORT LFRange LFStringFind(LFStringRef string, LFStringRef stringToFind, LFStringCompareFlags options);
/**
 * Returns the range of the first found character in the string that is also present in the given character set.
 * @remark This function does not support case insensitive search
 **/
LF_EXPORT LFRange LFStringFindCharacterFromSet(LFStringRef string, LFCharacterSetRef characterSet, LFRange range, LFStringCompareFlags options);

/**
 * Returns true if the string has the given prefix, otherwise false
 * @sa LFStringFind()
 **/
LF_EXPORT uint8_t LFStringHasPrefix(LFStringRef string, LFStringRef prefix);
/**
 * Returns true if the string has the given suffix, otherwise false
 * @sa LFStringFind()
 **/
LF_EXPORT uint8_t LFStringHasSuffix(LFStringRef string, LFStringRef suffix);
/**
 * Returns an array containing LFStringRef objects with the contents of the string seperated by the seperator string.
 * When the string contains "Michael, Julius, Clara, Markus, Daniela, Hummel" and you call this function with ", " as seperator, the returned array contains
 * "Michael", "Julius", "Clara", "Markus", "Daniela" and "Hummel".
 **/
LF_EXPORT LFArrayRef LFStringComponentsSeperatedByString(LFStringRef string, LFStringRef seperator);

// ------------------------
// Mutating
// ------------------------

/**
 * Appends the given string to the receive
 **/
LF_EXPORT void LFStringAppend(LFStringRef string, LFStringRef appendString);
/**
 * Appends the given c string to the receive
 **/
LF_EXPORT void LFStringAppendCString(LFStringRef string, char *appendCString);

/**
 * Inserts the given string at the given index into the receiver, the remainding characters of the receiver are shifted by the length of the insert string
 * @Remark The index must be inside the receivers bounds, otherwise the method will fail
 **/
LF_EXPORT void LFStringInsertAtIndex(LFStringRef string, LFIndex index, LFStringRef insertString);
/**
 * Inserts the given c string at the given index into the receiver, the remainding characters of the receiver are shifted by the length of the insert string
 * @Remark The index must be inside the receivers bounds, otherwise the method will fail
 **/
LF_EXPORT void LFStringInsertCStringAtIndex(LFStringRef string, LFIndex index, char *insertCString);

/**
 * Deletes all characters inside the given range from the receiver
 * @remark If the range is outside the receivers bounds, it will be trimmed to match it again. If its totally out of bound, the receiver won't change
 **/
LF_EXPORT void LFStringDeleteRange(LFStringRef string, LFRange range);

/**
 * Replaces all occurrences of the given string inside the receiver into the given string.
 * @param lookFor The string that should be changed. Must not be NULL
 * @param replacement The replacement for the lookFor string. Must not be NULL.
 **/
LF_EXPORT void LFStringReplaceOccurrencesOfString(LFStringRef string, LFStringRef lookFor, LFStringRef replacement);

// ------------------------
// Mutating without size changes
// ------------------------

/**
 * Converts the string into a lowercase string
 * @remark Only the characters from A to Z are matched, german umlauts like Ä or other special characters won't be changed!
 **/
LF_EXPORT void LFStringLowercase(LFStringRef string);
/**
 * Converts the string into a uppercase string
 * @remark Only the characters from a to z are matched, german umlauts like ü or other special characters won't be changed!
 **/
LF_EXPORT void LFStringUppercase(LFStringRef string);


// ------------------------
// Substrings
// ------------------------

/**
 * Returns a new, autoreleased, string containing the receivers string from the given index to its end
 * @remark The index must be inside the receivers bound!
 **/
LF_EXPORT LFStringRef LFStringSubstringFromIndex(LFStringRef string, LFIndex index);
/**
 * Returns a new, autoreleased, string containing the receivers string up to the given index
 * @remark The index must be inside the receivers bound!
 **/
LF_EXPORT LFStringRef LFStringSubstringToIndex(LFStringRef string, LFIndex index);
/**
 * Returns a new, autoreleased, string containing the receiver + the append string
 **/
LF_EXPORT LFStringRef LFStringStringByAppendingString(LFStringRef string, LFStringRef stringToAppend);

/**
 * @}
 **/

#endif
