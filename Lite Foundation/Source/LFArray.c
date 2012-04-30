//
//  LFArray.c
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

#include "LFArray.h"
#include "LFInternal.h"
#include "LFEnumerator.h"

//------------------------------
// Runtime functions
//------------------------------

void		__LFArrayInit(LFTypeRef ref);
LFTypeRef	__LFArrayCopy(LFTypeRef ref);
void		__LFArrayDealloc(LFTypeRef ref);
uint8_t		__LFArrayEqual(LFTypeRef refa, LFTypeRef refb);
LFHashCode	__LFArrayHash(LFTypeRef ref);

static LFRuntimeClass __LFArrayClass;
static LFTypeID __LFArrayTypeID = kLFInvalidTypeID;


void __LFArrayEnumerate(LFArrayRef array, LFEnumeratorRef enumerator, uint32_t maxCount);

void __LFRuntimeInitArray()
{
	__LFArrayClass.version = 1;
	__LFArrayClass.name = "LFArray";
	
	__LFArrayClass.init = __LFArrayInit;
	__LFArrayClass.copy = __LFArrayCopy;
	__LFArrayClass.dealloc = __LFArrayDealloc;
	__LFArrayClass.equal = __LFArrayEqual;
	__LFArrayClass.hash  = __LFArrayHash;
	
	__LFArrayClass.superClass = kLFInvalidTypeID;
	__LFArrayClass.protocolBag = LFRuntimeCreateProtocolBag();
	
	
	LFProtocolBody *enumeratorBody = LFRuntimeCreateProtocolBody(LFEnumeratorGetProtocolID());
	LFRuntimeProtocolBodySetIMP(enumeratorBody, LFEnumeratorGetSelector(kLFEnumeratorProtocolEnumerateWithObjects), LFProtocolMethod20(__LFArrayEnumerate));
	
	LFRuntimeAppendProtocol(__LFArrayClass.protocolBag, enumeratorBody);
	__LFArrayTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFArrayClass);
}

void __LFArrayInit(LFTypeRef ref)
{
	LFArrayRef array = (LFArrayRef)ref;
	
	array->callbacks.LFArrayEqualCallback	= LFEqual;
	array->callbacks.LFArrayHashCallback	= LFHash;
	array->callbacks.LFArrayRetainCallback	= LFRetain;
	array->callbacks.LFArrayReleaseCallback = LFRelease;
	
	array->count = 0;
	array->capacity = 5;
	array->values = (void **)malloc(array->capacity * sizeof(void *));
	
	memset(&array->cache[0], 0, sizeof(struct __LFArrayCache));
	memset(&array->cache[1], 0, sizeof(struct __LFArrayCache));
	memset(&array->cache[2], 0, sizeof(struct __LFArrayCache));
	memset(&array->cache[3], 0, sizeof(struct __LFArrayCache));
	memset(&array->cache[4], 0, sizeof(struct __LFArrayCache));
}

LFTypeRef __LFArrayCopy(LFTypeRef ref)
{
	LFArrayRef source = (LFArrayRef)ref;
	LFArrayRef copy = LFArrayCreate(&source->callbacks);
	
	if(copy)
	{
		free(copy->values);
		copy->values = (void **)malloc(source->capacity * sizeof(void *));	
		if(!copy->values)
		{
			LFRelease(copy);
			return NULL;
		}
		
		copy->count = source->count;
		copy->capacity = source->capacity;
		copy->mutations = source->mutations;
		
		memcpy(copy->values, source->values, copy->count * sizeof(void *));
		memcpy(&copy->cache, &source->cache, 5 * sizeof(struct __LFArrayCache));
	}
	
	return copy;
}

void __LFArrayDealloc(LFTypeRef ref)
{	
	LFArrayRef array = (LFArrayRef)ref;
	
#ifdef LF_TARGET_LITEC
	void __LFArrayReleaseCallback(void *object);
	__LFArrayReleaseCallback = array->callbacks.LFArrayReleaseCallback;
#endif
	
	LFIndex index;
	for(index=0; index<array->count; index++)
	{
#ifdef LF_TARGET_LITEC
		__LFArrayReleaseCallback((array->values)[index]);
#else
		array->callbacks.LFArrayReleaseCallback((array->values)[index]);
#endif
	}
	
	if(array->values)
		free(array->values);
}

uint8_t __LFArrayEqual(LFTypeRef refa, LFTypeRef refb)
{
	LFArrayRef arraya = (LFArrayRef)refa;
	LFArrayRef arrayb = (LFArrayRef)refb;
	
	if(arraya->callbacks.LFArrayEqualCallback != arrayb->callbacks.LFArrayEqualCallback)
		return 0;
	
	if(arraya->callbacks.LFArrayHashCallback != arrayb->callbacks.LFArrayHashCallback)
		return 0;
	
	if(arraya->count != arrayb->count)
		return 0;
	
#ifdef LF_TARGET_LITEC
	uint8_t 	__LFArrayEqualCallback(void *objecta, void *objectb);
	LFHashCode 	__LFArrayHashCallback(void *object);
	
	__LFArrayEqualCallback	= arraya->callbacks.LFArrayEqualCallback;
	__LFArrayHashCallback	= arraya->callbacks.LFArrayHashCallback;
#endif
	
	
	LFIndex index;
	for(index=0; index<arraya->count; index++)
	{
		void *valuea = (arraya->values)[index];
		void *valueb = (arrayb->values)[index];
		
#ifdef LF_TARGET_LITEC
		if(__LFArrayHashCallback(valuea) != __LFArrayHashCallback(valueb))
			return 0;
		
		if(!__LFArrayEqualCallback(valuea, valueb))
			return 0;
#else
		if(arraya->callbacks.LFArrayHashCallback(valuea) != arraya->callbacks.LFArrayHashCallback(valueb))
			return 0;
		
		if(!arraya->callbacks.LFArrayEqualCallback(valuea, valueb))
			return 0;
#endif
	}
	
	return 0;
}

LFHashCode __LFArrayHash(LFTypeRef ref)
{
	LFArrayRef array = (LFArrayRef)ref;
	return (LFHashCode)array->count;
}

//------------------------------
// Implementation
//------------------------------

LFTypeID LFArrayGetTypeID()
{
	return __LFArrayTypeID;
}

LFArrayRef LFArrayCreate(LFArrayCallbacks *callbacks)
{
	LFArrayRef array = LFRuntimeCreateInstance(__LFArrayTypeID, sizeof(struct __LFArray));
	
	if(callbacks)
	{
		array->callbacks.LFArrayEqualCallback	= callbacks->LFArrayEqualCallback;
		array->callbacks.LFArrayHashCallback	= callbacks->LFArrayHashCallback;
		array->callbacks.LFArrayRetainCallback	= callbacks->LFArrayRetainCallback;
		array->callbacks.LFArrayReleaseCallback = callbacks->LFArrayReleaseCallback;
	}
	
	return array;
}

LFArrayRef LFArrayCreateWithCapacity(LFArrayCallbacks *callbacks, uint32_t capacity)
{
	if(capacity == 0)
		return NULL;
	
	LFArrayRef array = LFRuntimeCreateInstance(__LFArrayTypeID, sizeof(struct __LFArray));
	free(array->values);
	
	array->capacity = capacity;
	array->values = (void **)malloc(array->capacity * sizeof(void *));
	
	if(!array->values)
	{
		LFRelease(array);
		return NULL;
	}
	
	if(callbacks)
	{
		array->callbacks.LFArrayEqualCallback	= callbacks->LFArrayEqualCallback;
		array->callbacks.LFArrayHashCallback	= callbacks->LFArrayHashCallback;
		array->callbacks.LFArrayRetainCallback	= callbacks->LFArrayRetainCallback;
		array->callbacks.LFArrayReleaseCallback = callbacks->LFArrayReleaseCallback;
	}
	
	return array;
}



void __LFArrayEnumerate(LFArrayRef array, LFEnumeratorRef enumerator, uint32_t maxCount)
{
	LFRuntimeLock(array);
	
	if(enumerator->state == 0)
	{
		enumerator->extra[0] = array->mutations;
		enumerator->extra[1] = 0;
	}
	else
	{
		LFAssert(enumerator->extra[0] == array->mutations, "Enumerating an mutated array is not supported!");
	}
	
	
	LFIndex index;
	for(index=0; index<maxCount; index++)
	{
		if(index + enumerator->extra[1] >= array->count)
		{
			enumerator->itemCount = index;
			enumerator->extra[1] += index;
			
			return;
		}
		
		(enumerator->itemsPtr)[index] = (array->values)[index + enumerator->extra[1]];
	}
	
	enumerator->itemCount = index;
	enumerator->extra[1] += index;
	
	LFRuntimeUnlock(array);
}


#ifdef LF_INLINE
#define LF_ARRAY_INLINE LF_INLINE
#else
#define LF_ARRAY_INLINE
#endif


LF_ARRAY_INLINE void __LFArrayTryResize(LFArrayRef array, LFIndex capacity)
{
	void **tarray = (void **)realloc(array->values, capacity * sizeof(void *));
	LFAssert(tarray, "Failed to resize LFArray!");
	
	if(tarray)
	{
		array->values = tarray;
		array->capacity = capacity;
	}
}


LF_ARRAY_INLINE void __LFArrayPushCache(LFArrayRef array, void *value, LFIndex index)
{
	memcpy(&array->cache[0], &array->cache[1], 4 * sizeof(struct __LFArrayCache));
	(array->cache)[0].value = value;
	(array->cache)[0].index = index;
}

LF_ARRAY_INLINE void __LFArrayPopCache(LFArrayRef array, LFIndex index)
{
	LFIndex i;
	for(i=0; i<5; i++)
	{
		if((array->cache)[i].index == index)
		{
			(array->cache)[i].value = NULL;
			(array->cache)[i].index = -1;
			
			return;
		}
	}
}

LF_ARRAY_INLINE void __LFArrayMoveCache(LFArrayRef array, LFIndex index, LFIndex offset)
{
	LFIndex i;
	for(i=0; i<5; i++)
	{
		if((array->cache)[i].index >= index)
			(array->cache)[i].index -= offset;
	}
}

LF_ARRAY_INLINE LFIndex __LFArrayIndexOfValue(LFArrayRef array, void *value)
{
#ifdef LF_TARGET_LITEC
	uint8_t __LFArrayEqualCallback(void *objecta, void *objectb);
	__LFArrayEqualCallback = array->callbacks.LFArrayEqualCallback;
#endif
	
	// Try the cache first
	LFIndex index;
	for(index=0; index<5; index++)
	{
#ifdef LF_TARGET_LITEC
		if(__LFArrayEqualCallback((array->cache)[index].value, value))
			return (array->cache)[index].index;
#else
		if(array->callbacks.LFArrayEqualCallback((array->cache)[index].value, value))
			return (array->cache)[index].index;
#endif
	}

	// If the cache doesn't contain the searched value, search the whole array
	for(index=0; index<array->count; index++)
	{
#ifdef LF_TARGET_LITEC
		if(__LFArrayEqualCallback((array->values)[index], value))
			return index;
#else
		if(array->callbacks.LFArrayEqualCallback((array->values)[index], value))
			return index;
#endif
	}
	
	return -1;
}


// REMARK: Internal use only, no lock, no zombie check, nothing!
LF_ARRAY_INLINE void __LFArrayAddValue(LFArrayRef array, void *value)
{	
#ifdef LF_TARGET_LITEC
	void *__LFArrayRetainCallback(void *object);
	__LFArrayRetainCallback = array->callbacks.LFArrayRetainCallback;
#endif
	
	LFIndex index = __LFArrayIndexOfValue(array, value);
	if(index != -1)
		return;
	
	
	if(array->count >= array->capacity)
		__LFArrayTryResize(array, array->capacity + 20);
	
#ifdef LF_TARGET_LITEC
	(array->values)[array->count] = __LFArrayRetainCallback(value);
#else
	(array->values)[array->count] = array->callbacks.LFArrayRetainCallback(value);
#endif
	array->count ++;
	array->mutations ++;
	
	__LFArrayPushCache(array, value, array->count-1);
}





void LFArrayAddValue(LFArrayRef array, void *value)
{
	LFRuntimeLock(array);
	
	if(LFZombieScribble(array, "LFArrayAddValue") || !LFRuntimeValidate(array, __LFArrayTypeID))
	{
		LFRuntimeUnlock(array);
		return;
	}
	
	__LFArrayAddValue(array, value);
	LFRuntimeUnlock(array);
}


void LFArrayAddValuesFromArray(LFArrayRef array, LFArrayRef otherArray)
{
	LFRuntimeLock(array);
    LFRuntimeLock(otherArray);
	
	if(LFZombieScribble(array, "LFArrayAddValuesFromArray") || LFZombieScribble(otherArray, "LFArrayAddValuesFromArray") || 
       !LFRuntimeValidate(array, __LFArrayTypeID) || !LFRuntimeValidate(array, __LFArrayTypeID))
	{
		LFRuntimeUnlock(array);
        LFRuntimeUnlock(otherArray);
        
		return;
	}
	
#ifdef LF_TARGET_LITEC
	void *__LFArrayRetainCallback(void *object);
	__LFArrayRetainCallback = array->callbacks.LFArrayRetainCallback;
#endif
	
	if(array->capacity < otherArray->count)
		__LFArrayTryResize(array, array->capacity + otherArray->count);
	
	LFIndex i;
	for(i=0; i<otherArray->count; i++)
		__LFArrayAddValue(array, (otherArray->values)[i]);
	
	LFRuntimeUnlock(array);
    LFRuntimeUnlock(otherArray);
}


void LFArrayInsertValueAtIndex(LFArrayRef array, void *value, LFIndex index)
{
	LFRuntimeLock(array);
	
	if(LFZombieScribble(array, "LFArrayInsertValueAtIndex") || !LFRuntimeValidate(array, __LFArrayTypeID))
	{
		LFRuntimeUnlock(array);
		return;
	}
	
	LFAssert(index < array->count, "LFArrayInsertValueAtIndex(), index out of bounds!");
#ifdef LF_TARGET_LITEC
	void *__LFArrayRetainCallback(void *object);
	__LFArrayRetainCallback = array->callbacks.LFArrayRetainCallback;
#endif
	
	LFIndex oindex = __LFArrayIndexOfValue(array, value);
	if(oindex != -1)
	{
		LFRuntimeUnlock(array);
		return;
	}
	
	if(array->count >= array->capacity)
		__LFArrayTryResize(array, array->capacity + 20);
	
	
	LFIndex i;
	for(i=array->count; i>index; i--)
	{
		(array->values)[i] = (array->values)[i-1];
	}
	
	__LFArrayPushCache(array, value, index);
	__LFArrayMoveCache(array, index, 1);
	
#ifdef LF_TARGET_LITEC
	(array->values)[index] = __LFArrayRetainCallback(value);
#else
	(array->values)[index] = array->callbacks.LFArrayRetainCallback(value);
#endif
	
	array->count ++;
	array->mutations ++;
	
	LFRuntimeUnlock(array);
}


// REMARK: Internal use only, no lock, no bound checking, inline!
LF_ARRAY_INLINE void __LFArrayRemoveValueAtIndex(LFArrayRef array, LFIndex index)
{
#ifdef LF_TARGET_LITEC
	void __LFArrayReleaseCallback(void *object);
	__LFArrayReleaseCallback = array->callbacks.LFArrayReleaseCallback;
#endif
	
	void *value = (array->values)[index];
	
	__LFArrayPopCache(array, index);
	__LFArrayMoveCache(array, index, -1);
	
#ifdef LF_TARGET_LITEC
	__LFArrayReleaseCallback(value);
#else
	array->callbacks.LFArrayReleaseCallback(value);
#endif
	
	
	for(; index<array->count-1; index++)
	{
		(array->values)[index] = (array->values)[index+1];
	}
	
	array->count --;
	array->mutations ++;
	
	if(array->capacity - 20 > array->count)
		__LFArrayTryResize(array, array->capacity - 20);
}


void LFArrayRemoveValue(LFArrayRef array, void *value)
{
	LFRuntimeLock(array);
	if(LFZombieScribble(array, "LFArrayRemoveValue") || !LFRuntimeValidate(array, __LFArrayTypeID))
	{
		LFRuntimeUnlock(array);
		return;
	}
	
	LFIndex index = __LFArrayIndexOfValue(array, value);
	if(index != -1)
	{
		__LFArrayRemoveValueAtIndex(array, index);
	}
	
	LFRuntimeUnlock(array);
}

void LFArrayRemoveValueAtIndex(LFArrayRef array, LFIndex index)
{
	LFRuntimeLock(array);
	
	if(LFZombieScribble(array, "LFArrayRemoveValueAtIndex") || !LFRuntimeValidate(array, __LFArrayTypeID))
	{
		LFRuntimeUnlock(array);
		return;
	}
	
	LFAssert(index < array->count, "LFArrayRemoveValueAtIndex(), index out of bounds!");
	__LFArrayRemoveValueAtIndex(array, index);
	
	LFRuntimeUnlock(array);
}


void LFArrayRemoveAllValues(LFArrayRef array)
{
	LFRuntimeLock(array);
	if(LFZombieScribble(array, "LFArrayRemoveAllValues") || !LFRuntimeValidate(array, __LFArrayTypeID))
	{
		LFRuntimeUnlock(array);
		return;
	}
	
#ifdef LF_TARGET_LITEC
	void __LFArrayReleaseCallback(void *object);
	__LFArrayReleaseCallback = array->callbacks.LFArrayReleaseCallback;
#endif
	
	LFIndex index;
	for(index=0; index<array->count; index++)
	{
#ifdef LF_TARGET_LITEC
		__LFArrayReleaseCallback((array->values)[index]);
#else
		array->callbacks.LFArrayReleaseCallback((array->values)[index]);
#endif
	}
	
	
	array->mutations ++;
	array->count = 0;
	array->capacity = 5;
	array->values = (void **)malloc(array->capacity * sizeof(void *));
	
	memset(&array->cache[0], 0, sizeof(struct __LFArrayCache));
	memset(&array->cache[1], 0, sizeof(struct __LFArrayCache));
	memset(&array->cache[2], 0, sizeof(struct __LFArrayCache));
	memset(&array->cache[3], 0, sizeof(struct __LFArrayCache));
	memset(&array->cache[4], 0, sizeof(struct __LFArrayCache));
	
	LFRuntimeUnlock(array);
}


void *LFArrayValueAtIndex(LFArrayRef array, LFIndex index)
{
	LFRuntimeLock(array);
	
	if(LFZombieScribble(array, "LFArrayValueAtIndex") || !LFRuntimeValidate(array, __LFArrayTypeID))
	{
		LFRuntimeUnlock(array);
		return NULL;
	}
	
	LFAssert(index < array->count, "LFArrayValueAtIndex(), index out of bounds!");
	void *value = (array->values)[index];
	
	__LFArrayPushCache(array, value, index);
	LFRuntimeUnlock(array);
	
	return value;
}

void *LFArrayGetLastValue(LFArrayRef array)
{
	LFRuntimeLock(array);
	if(LFZombieScribble(array, "LFArrayGetLastValue") || !LFRuntimeValidate(array, __LFArrayTypeID))
	{
		LFRuntimeUnlock(array);
		return NULL;
	}
	
	if(array->count == 0)
	{
		LFRuntimeUnlock(array);
		return NULL;
	}
	
	void *value = (array->values)[array->count - 1];
	__LFArrayPushCache(array, value, array->count - 1);
	
	LFRuntimeUnlock(array);
	return value;
}

void LFArrayExchangeValueAtIndex(LFArrayRef array, LFIndex indexa, LFIndex indexb)
{
	LFRuntimeLock(array);
	
	if(LFZombieScribble(array, "LFArrayExchangeValueAtIndex") || !LFRuntimeValidate(array, __LFArrayTypeID))
	{
		LFRuntimeUnlock(array);
		return;
	}
	
	LFAssert(indexa < array->count, "LFArrayExchangeValueAtIndex(), indexa out of bounds!");
	LFAssert(indexb < array->count, "LFArrayExchangeValueAtIndex(), indexb out of bounds!");
	
	__LFArrayPopCache(array, indexa);
	__LFArrayPopCache(array, indexb);
	
	void *tvalue = (array->values)[indexa];
	
	(array->values)[indexa] = (array->values)[indexb];
	(array->values)[indexb] = tvalue;
	
	__LFArrayPushCache(array, (array->values)[indexa], indexa);
	__LFArrayPushCache(array, (array->values)[indexb], indexb);
	
	LFRuntimeUnlock(array);
}

LFIndex LFArrayIndexOfValue(LFArrayRef array, void *value)
{
	LFRuntimeLock(array);
	if(LFZombieScribble(array, "LFArrayIndexOfValue") || !LFRuntimeValidate(array, __LFArrayTypeID))
	{
		LFRuntimeUnlock(array);
		return kLFNotFound;
	}
	
	uint32_t index = __LFArrayIndexOfValue(array, value);
	if(index != -1)
	{
		void *value = (array->values)[index];
		__LFArrayPushCache(array, value, index);
		
		LFRuntimeUnlock(array);
		return index;
	}
	
	LFRuntimeUnlock(array);
	return kLFNotFound;
}

uint8_t LFArrayContainsValue(LFArrayRef array, void *value)
{
	LFRuntimeLock(array);
	if(LFZombieScribble(array, "LFArrayContainsValue") || !LFRuntimeValidate(array, __LFArrayTypeID))
	{
		LFRuntimeUnlock(array);
		return 0;
	}
	
	LFIndex index = __LFArrayIndexOfValue(array, value);
	LFRuntimeUnlock(array);
	
	return (index != -1);
}

uint32_t LFArrayCount(LFArrayRef array)
{
	LFRuntimeLock(array);
	if(LFZombieScribble(array, "LFArrayCount") || !LFRuntimeValidate(array, __LFArrayTypeID))
	{
		LFRuntimeUnlock(array);
		return 0;
	}
	
	uint32_t count = array->count;
	LFRuntimeUnlock(array);
	
	return count;
}

// Sorting helper functions
#ifdef LF_TARGET_LITEC
LFComparisonResult __LFArrayComparisonCallback(void *objecta, void *objectb);
#endif

LF_ARRAY_INLINE uint8_t __LFArrayIsSorted(LFArrayRef array, LFIndex begin, LFIndex end, void *callback)
{
#ifdef LF_TARGET_LITEC
	LFComparisonResult __LFArrayComparisonCallback(void *objecta, void *objectb);
	__LFArrayComparisonCallback = callback;
#else
	LFComparisonResult (*__LFArrayComparisonCallback)(void *objecta, void *objectb) = callback;
#endif
	
	for(; begin<end-1; begin++)
	{
		if(__LFArrayComparisonCallback((array->values)[begin], (array->values)[begin+1]) < kLFCompareEqualTo)
			return 0;
	}
	
	return 1;
}

// No bound check performed, inline
// REMARK: Clear the cache after performing this function!
LF_ARRAY_INLINE void __LFArrayExchangeValueInline(LFArrayRef array, LFIndex indexa, LFIndex indexb)
{
	void *tvalue = (array->values)[indexa];
	
	(array->values)[indexa] = (array->values)[indexb];
	(array->values)[indexb] = tvalue;
}

// Sorting functions
LF_ARRAY_INLINE void __LFArrayHeapSortEx(LFArrayRef array, LFIndex index, LFIndex length, void *callback)
{
#ifdef LF_TARGET_LITEC
	LFComparisonResult __LFArrayComparisonCallback(void *objecta, void *objectb);
	__LFArrayComparisonCallback = callback;
#else
	LFComparisonResult (*__LFArrayComparisonCallback)(void *objecta, void *objectb) = callback;
#endif
	
	LFIndex index2 = index * 2 + 1;
	void *tvalue = (array->values)[index];

	while(index2 < length)
	{
		if(index2 + 1 < length)
		{
			if(__LFArrayComparisonCallback((array->values)[index2], (array->values)[index2 + 1]) < kLFCompareEqualTo)
				index2++;
		}
	
		if(__LFArrayComparisonCallback(tvalue, (array->values)[index2]) >= kLFCompareEqualTo)
			break;


		(array->values)[index] = (array->values)[index2];
		index = index2;
		index2 = index * 2 + 1;
	}

	(array->values)[index] = tvalue;
}

LF_ARRAY_INLINE void __LFArrayHeapSort(LFArrayRef array, LFIndex length, void *callback)
{
	LFIndex index = length / 2;
	
	while(index > 0)
	{
		index --;
		__LFArrayHeapSortEx(array, index, length, callback);
	}
	 
	while(length - 1 > 0)
	{
		length--;
		
		__LFArrayExchangeValueInline(array, 0, length);
		__LFArrayHeapSortEx(array, 0, length, callback);
	}
}

LF_ARRAY_INLINE LFIndex __LFArrayPartition(LFArrayRef array, LFIndex begin, LFIndex end, void *callback)
{
	LFIndex pivot = begin;
	LFIndex middle = (begin + end) / 2;
#ifdef LF_TARGET_LITEC
	LFComparisonResult __LFArrayComparisonCallback(void *objecta, void *objectb);
	__LFArrayComparisonCallback = callback;
#else
	LFComparisonResult (*__LFArrayComparisonCallback)(void *objecta, void *objectb) = callback;
#endif
	
	// Avoid of hitting a degenerated pivot
	if(__LFArrayComparisonCallback((array->values)[middle], (array->values)[begin]) >= kLFCompareEqualTo)
		pivot = middle;
	
	if(__LFArrayComparisonCallback((array->values)[pivot], (array->values)[end]) >= kLFCompareEqualTo)
		pivot = end;
	
	
	// Swap the pivot with the first value to avoid running over the end of the array
	__LFArrayExchangeValueInline(array, pivot, begin);
	pivot = begin;
	
	
	while(begin < end)
	{
		if(__LFArrayComparisonCallback((array->values)[begin], (array->values)[end]) <= kLFCompareEqualTo)
		{
			__LFArrayExchangeValueInline(array, pivot, begin);
			pivot++;
		}
		
		begin ++;
	}
	
	
	__LFArrayExchangeValueInline(array, pivot, end);
	return pivot;
}

void __LFArrayQuickSort(LFArrayRef array, LFIndex begin, LFIndex end, LFIndex depth, void *callback)
{
	if(begin < end)
	{
		if(depth == 0)
		{
			__LFArrayHeapSort(array, end - begin + 1, callback);
		}
		else
		{
			if(__LFArrayIsSorted(array, begin, end, callback))
				return;
			
			LFIndex pivot = __LFArrayPartition(array, begin, end, callback);
			
			__LFArrayQuickSort(array, begin, pivot-1, depth-1, callback);
			__LFArrayQuickSort(array, pivot+1, end, depth-1, callback);
		}
	}
}

LF_ARRAY_INLINE void __LFArrayInsertionSort(LFArrayRef array, void *callback)
{
#ifdef LF_TARGET_LITEC
	LFComparisonResult __LFArrayComparisonCallback(void *objecta, void *objectb);
	__LFArrayComparisonCallback = callback;
#else
	LFComparisonResult (*__LFArrayComparisonCallback)(void *objecta, void *objectb) = callback;
#endif
	LFIndex i, j;
	 
	for(i=1; i<array->count; i++) 
	{
	   void *tvalue = (array->values)[i];
	   for(j=i; j>=1; j--)
		{
			if(__LFArrayComparisonCallback((array->values)[j-1], tvalue) <= kLFCompareEqualTo)
				break;
			
			(array->values)[j] = (array->values)[j-1];
		}
		
		(array->values)[j] = tvalue;
	}
}


#ifndef LF_TARGET_LITEC
void LFArraySortUsingFunction(LFArrayRef array, LFComparisonResult (*comparisonCallback)(void *objecta, void *objectb))
#else
void LFArraySortUsingFunction(LFArrayRef array, void *comparisonCallback)
#endif
{
	LFRuntimeLock(array);
	if(LFZombieScribble(array, "LFArraySortUsingFunction") || !LFRuntimeValidate(array, __LFArrayTypeID))
	{
		LFRuntimeUnlock(array);
		return;
	}

	__LFArrayQuickSort(array, 0, array->count-1, (LFIndex)(2*log(array->count)), comparisonCallback);
	__LFArrayInsertionSort(array, comparisonCallback);

	memset(&array->cache[0], 0, sizeof(struct __LFArrayCache)); // Clear the cache because it probably became invalid during sorting
	memset(&array->cache[1], 0, sizeof(struct __LFArrayCache));
	memset(&array->cache[2], 0, sizeof(struct __LFArrayCache));
	memset(&array->cache[3], 0, sizeof(struct __LFArrayCache));
	memset(&array->cache[4], 0, sizeof(struct __LFArrayCache));
	
	LFRuntimeUnlock(array);
}
