//
//  LFSet.c
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

#include "LFSet.h"
#include "LFInternal.h"
#include "LFEnumerator.h"

uint32_t __LFSetBucketCount[42] = 
{
    5, 11, 23, 41, 67, 113, 199, 317, 521, 839, 1361, 2207, 3571, 5779, 9349, 15121,
    24473, 39607, 64081, 103681, 167759, 271429, 439199, 710641, 1149857, 1860503, 3010349,
    4870843, 7881193, 12752029, 20633237, 33385273, 54018521, 87403763, 141422317, 228826121,
    370248451, 599074561, 969323023, 1568397599, 2537720629, 4106118251
};

uint32_t __LFSetMaxCapacities[42] = 
{
    4, 8, 17, 29, 47, 76, 123, 199, 322, 521, 843, 1364, 2207, 3571, 5778, 9349,
    15127, 24476, 39603, 64079, 103682, 167761, 271443, 439204, 710647, 1149851, 1860498,
    3010349, 4870847, 7881196, 12752043, 20633239, 33385282, 54018521, 87403803, 141422324,
    228826127, 370248451, 599074578, 969323029, 1568397607, 2537720636
}; 


//------------------------------
// Runtime functions
//------------------------------

void		__LFSetInit(LFTypeRef ref);
LFTypeRef	__LFSetCopy(LFTypeRef ref);
void		__LFSetDealloc(LFTypeRef ref);
LFHashCode	__LFSetHash(LFTypeRef ref);
uint8_t		__LFSetEqual(LFTypeRef refa, LFTypeRef refb);

void __LFSetEnumerate(LFSetRef _set, LFEnumeratorRef enumerator, uint32_t maxCount);

static LFRuntimeClass __LFSetClass;
static LFTypeID __LFSetTypeID = kLFInvalidTypeID;


void __LFRuntimeInitSet()
{
	__LFSetClass.version = 1;
	__LFSetClass.name = "LFSet";
	
	__LFSetClass.init = __LFSetInit;
	__LFSetClass.copy = __LFSetCopy;
	__LFSetClass.dealloc = __LFSetDealloc;
	__LFSetClass.equal = __LFSetEqual;
	__LFSetClass.hash  = __LFSetHash;
	
	__LFSetClass.superClass = kLFInvalidTypeID;
	__LFSetClass.protocolBag = LFRuntimeCreateProtocolBag();
	
	
	LFProtocolBody *enumeratorBody = LFRuntimeCreateProtocolBody(LFEnumeratorGetProtocolID());
	LFRuntimeProtocolBodySetIMP(enumeratorBody, LFEnumeratorGetSelector(kLFEnumeratorProtocolEnumerateWithObjects), LFProtocolMethod20(__LFSetEnumerate));
	
	LFRuntimeAppendProtocol(__LFSetClass.protocolBag, enumeratorBody);
	
	__LFSetTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFSetClass);
}


void __LFSetInit(LFTypeRef ref)
{
	LFSetRef _set = (LFSetRef)ref;
	
	uintptr_t callbacks = (uintptr_t)_set;
	callbacks += sizeof(struct __LFSet);
	
	_set->callbacks = (void *)callbacks;
	_set->callbacks->LFSetEqualCallback	= LFEqual;
	_set->callbacks->LFSetHashCallback	= LFHash;
	_set->callbacks->LFSetRetainCallback	= LFRetain;
	_set->callbacks->LFSetReleaseCallback = LFRelease;
	
	_set->count = 0;
	_set->bucketCount = __LFSetBucketCount[0];
	_set->buckets = (struct __LFSetBucket **)malloc(_set->bucketCount * sizeof(struct __LFSetBucket *));
	
	if(_set->buckets)
	{
		memset(_set->buckets, 0, _set->bucketCount * sizeof(struct __LFSetBucket *));
	}
}

LFTypeRef __LFSetCopy(LFTypeRef ref)
{
	LFSetRef source = (LFSetRef)ref;
	LFSetRef copy = LFSetCreate(source->callbacks);
	
    if(!copy)
        return NULL;
    
	copy->bucketCount = source->bucketCount;
	copy->count = source->count;
	
    free(copy->buckets);
	copy->buckets = (struct __LFSetBucket **)malloc(copy->bucketCount * sizeof(struct __LFSetBucket *));
	
	if(copy->buckets)
	{
		memset(copy->buckets, 0, copy->bucketCount * sizeof(struct __LFSetBucket *));
		LFIndex index;
		
		for(index=0; index<copy->bucketCount; index++)
		{
			struct __LFSetBucket *sbucket = (source->buckets)[index];
			if(sbucket)
			{
				struct __LFSetBucket *cbucket = (struct __LFSetBucket *)malloc(sizeof(struct __LFSetBucket));
				if(!cbucket)
				{
					LFRelease(copy);
					return NULL;
				}
				
				memcpy(cbucket, sbucket, sizeof(struct __LFSetBucket));
				
				if(cbucket->key)
					cbucket->key = copy->callbacks->LFSetRetainCallback(sbucket->key);
				
				(copy->buckets)[index] = cbucket;
				
				
				// Copy the overflow buckets...
				while(sbucket->next)
				{
					struct __LFSetBucket *nbucket = (struct __LFSetBucket *)malloc(sizeof(struct __LFSetBucket));
					if(!nbucket)
					{
						cbucket->next = NULL; // Avoid havin false buckets
						LFRelease(copy);
						
						return NULL;
					}
					
					memcpy(nbucket, sbucket->next, sizeof(struct __LFSetBucket));
					
					if(nbucket->key)
						nbucket->key = copy->callbacks->LFSetRetainCallback(sbucket->next->key);
					
					cbucket->next = nbucket;
					cbucket = nbucket;
					sbucket = sbucket->next;
				}
			}
		}
	}
	else
	{
		free(copy);
		return NULL;
	}
	
	return copy;
}

void __LFSetDealloc(LFTypeRef ref)
{
	LFSetRef _set = (LFSetRef)ref;
	
	LFIndex index;
	for(index=0; index<_set->bucketCount; index++)
	{
		struct __LFSetBucket *bucket = (_set->buckets)[index];
		struct __LFSetBucket *tbucket;
		
		if(bucket)
		{
			while(bucket)
			{
				tbucket = bucket;		
				
				_set->callbacks->LFSetReleaseCallback(bucket->key);
				
				bucket = bucket->next;
				free(tbucket);
			}
		}
	}
	
	free(_set->buckets);
}

LFHashCode __LFSetHash(LFTypeRef ref)
{
	LFSetRef _set = (LFSetRef)ref;
	return (LFHashCode)_set->count;
}

uint8_t __LFSetEqual(LFTypeRef refa, LFTypeRef refb)
{
	LFSetRef _seta = (LFSetRef)refa;
	LFSetRef _setb = (LFSetRef)refb;
	
	if(_seta->count != _setb->count)
		return 0;
	
	LFEnumeratorRef enumerator = LFEnumeratorCreate(_seta);
	LFTypeRef object;
	
	while((object = LFEnumeratorNextObject(enumerator)))
	{
		if(!LFSetContainsValue(_setb, object))
		{
			LFRelease(enumerator);
			return 0;
		}
	}
	
	LFRelease(enumerator);
	return 1;
}



LFTypeID LFSetGetTypeID()
{
	return __LFSetTypeID;
}

LFSetRef LFSetCreate(LFSetCallbacks *callbacks)
{
	LFSetRef _set = (LFSetRef)LFRuntimeCreateInstance(__LFSetTypeID, sizeof(struct __LFSet) + sizeof(LFSetCallbacks));
	
	if(callbacks)
	{
		_set->callbacks->LFSetEqualCallback		= callbacks->LFSetEqualCallback;
		_set->callbacks->LFSetHashCallback		= callbacks->LFSetHashCallback;
		_set->callbacks->LFSetRetainCallback	= callbacks->LFSetRetainCallback;
		_set->callbacks->LFSetReleaseCallback	= callbacks->LFSetReleaseCallback;
	}
	
	return _set;
}




struct __LFSetBucket *__LFSetFindBucket1(LFSetRef _set, void *key)
{
	LFHashCode hash = _set->callbacks->LFSetHashCallback(key);
	LFIndex index	= hash % _set->bucketCount;
	
	struct __LFSetBucket *bucket = (_set->buckets)[index];
	if(bucket)
	{
		if(bucket->key)
		{
			if(_set->callbacks->LFSetEqualCallback(bucket->key, key))
				return bucket;
		}
		
		
		if(bucket->next)
		{
			bucket = bucket->next;
			
			while(bucket)
			{
				if(bucket->key)
				{
					if(_set->callbacks->LFSetEqualCallback(bucket->key, key))
						return bucket;
				}

				if(!bucket->next)
					break;
				
				bucket = bucket->next;
			}
		}
	}
	
	return NULL;
}

struct __LFSetBucket *__LFSetFindBucket2(LFSetRef _set, void *key)
{
	LFHashCode hash = _set->callbacks->LFSetHashCallback(key);
	LFIndex index	= hash % _set->bucketCount;
	
	struct __LFSetBucket *bucket = (_set->buckets)[index];
	struct __LFSetBucket *freeBucket = NULL;
	
	if(bucket)
	{
		if(bucket->key)
		{
			if(_set->callbacks->LFSetEqualCallback(bucket->key, key))
				return bucket;
		}
		else
			freeBucket = bucket;
		
		
		if(bucket->next)
		{
			bucket = bucket->next;
			
			while(bucket)
			{
				if(bucket->key)
				{
					if(_set->callbacks->LFSetEqualCallback(key, bucket->key))
						return bucket;
				}
				else
					freeBucket = bucket;
				
				if(!bucket->next)
					break;
				
				
				bucket = bucket->next;
			}
		}
		

		if(freeBucket)
			return freeBucket;
		
		
		bucket->next = (struct __LFSetBucket *)malloc(sizeof(struct __LFSetBucket));
		
		if(bucket->next)
		{
			bucket = bucket->next;
			bucket->key  = NULL;
			bucket->next = NULL;
			
			return bucket;
		}
	}
	else
	{
		bucket = (struct __LFSetBucket *)malloc(sizeof(struct __LFSetBucket));
		
		if(bucket)
		{
			bucket->key = NULL;
			bucket->next = NULL;
		}
		
		(_set->buckets)[index] = bucket;
		return bucket;
	}
	
	return NULL;
}




void __LFSetRehashBuckets(LFSetRef _set, struct __LFSetBucket **buckets, uint32_t count)
{
	LFIndex index;
	for(index=0; index<count; index++)
	{
		struct __LFSetBucket *bucket = buckets[index];
		struct __LFSetBucket *tbucket;
		
		if(bucket)
		{
			while(bucket)
			{
				tbucket = bucket;			
				
				if(bucket->key)
				{
					struct __LFSetBucket *nbucket = __LFSetFindBucket2(_set, bucket->key);
					nbucket->key = bucket->key;
				}
				
				bucket = bucket->next;
				free(tbucket);
			}
			
			free(bucket);
		}
	}
	
	free(buckets);
}



void __LFSetExpandIfNeeded(LFSetRef _set)
{		
	uint32_t bucketCount = 0;
	LFIndex index;
	
	for(index=0; index<42; index++)
	{
		if(_set->bucketCount == __LFSetBucketCount[index])
		{
			if(_set->count > __LFSetMaxCapacities[index])
			{
				bucketCount = __LFSetBucketCount[index+1];
			}
			
			break;
		}
	}
	
	if(bucketCount > 0 && bucketCount != _set->bucketCount)
	{
		LFIndex oldCount = _set->bucketCount;
		
		struct __LFSetBucket **buckets = _set->buckets;
		struct __LFSetBucket **tbuckets = (struct __LFSetBucket **)malloc(bucketCount * sizeof(struct __LFSetBucket *));
		
		if(tbuckets)
		{
			_set->bucketCount = bucketCount;
			_set->buckets = tbuckets;
			
			memset(_set->buckets, 0, _set->bucketCount * sizeof(struct __LFSetBucket *));
			__LFSetRehashBuckets(_set, buckets, oldCount);
		}
	}
}

void __LFSetCollapseIfNeeded(LFSetRef _set)
{		
	uint32_t bucketCount = 0;
	LFIndex index;
	
	for(index=1; index<42; index++)
	{
		if(_set->count < __LFSetMaxCapacities[index])
		{
			bucketCount = __LFSetBucketCount[index-1];
			break;
		}
	}
	
	if(bucketCount > 0 && bucketCount != _set->bucketCount)
	{
		LFIndex oldCount = _set->bucketCount;
		
		struct __LFSetBucket **buckets = _set->buckets;
		struct __LFSetBucket **tbuckets = (struct __LFSetBucket **)malloc(bucketCount * sizeof(struct __LFSetBucket *));
		
		if(tbuckets)
		{
			_set->bucketCount = bucketCount;
			_set->buckets = tbuckets;
			
			memset(_set->buckets, 0, _set->bucketCount * sizeof(struct __LFSetBucket *));
			__LFSetRehashBuckets(_set, buckets, oldCount);
		}
	}
}


void *__LFSetGetNextEntry(LFSetRef _set, uint32_t *__bucket, uint32_t *__overflow)
{	
	LFIndex index1 = (LFIndex)*__bucket;
	LFIndex index2;
	
    if(index1 >= _set->bucketCount)
        return NULL;
    
	struct __LFSetBucket *bucket = (_set->buckets)[index1];
	
	while(!bucket)
	{
		index1 ++;		
		if(index1 >= _set->bucketCount)
			return NULL;
		
		bucket = (_set->buckets)[index1];
	}
	
	for(index2=0; index2<*__overflow; index2++)
	{
		if(!bucket->next)
		{
			*__overflow = 0;
			bucket = NULL;
			
			break;
		}
		
		bucket = bucket->next;
	}
		
	if(bucket)
	{
		*__overflow = *__overflow + 1;
		*__bucket = (uint32_t)index1;
	
		if(!bucket->key)
			return __LFSetGetNextEntry(_set, __bucket, __overflow);
		
		return bucket->key;
	}
	else
	{
		*__bucket = *__bucket + 1;
		return __LFSetGetNextEntry(_set, __bucket, __overflow);
	}
	
	return NULL;
}

void __LFSetEnumerate(LFSetRef _set, LFEnumeratorRef enumerator, uint32_t maxCount)
{
	if(enumerator->state == 0)
	{
		enumerator->extra[0] = _set->mutations;
		enumerator->extra[1] = 0; // Bucket index
		enumerator->extra[2] = 0; // Overflow bucket
		enumerator->itemCount = 0;
	}
	else
	{
		LFAssert(enumerator->extra[0] == _set->mutations, "Enumerating a mutated LFSet is not supported!");
		enumerator->itemCount = 0;
	}
	
	
	LFIndex index;
	for(index=0; index<maxCount; index++)
	{
		void *object = __LFSetGetNextEntry(_set, &enumerator->extra[1], &enumerator->extra[2]);
		if(!object)
		{
			enumerator->itemCount = index;
			return;
		}
		
		enumerator->itemsPtr[index] = object;
	}
	
	enumerator->itemCount = maxCount;
}



void LFSetAddValue(LFSetRef _set, void *value)
{
	LFRuntimeLock(_set);
	
	if(LFZombieScribble(_set, "LFSetAddValue") || !LFRuntimeValidate(_set, __LFSetTypeID))
	{
		LFRuntimeUnlock(_set);
		return;
	}
	
	struct __LFSetBucket *bucket = __LFSetFindBucket2(_set, value);
	
	if(!bucket)
	{
		LFRuntimeUnlock(_set);
		return;
	}
	
	if(bucket->key == NULL)
	{
		bucket->key = _set->callbacks->LFSetRetainCallback(value);
		
		_set->count ++;
		_set->mutations ++;
		
		__LFSetExpandIfNeeded(_set);
	}
	
	LFRuntimeUnlock(_set);
}

void LFSetRemoveValue(LFSetRef _set, void *value)
{
	LFRuntimeLock(_set);
	
	if(LFZombieScribble(_set, "LFSetRemoveValue") || !LFRuntimeValidate(_set, __LFSetTypeID))
	{
		LFRuntimeUnlock(_set);
		return;
	}
	
	struct __LFSetBucket *bucket = __LFSetFindBucket1(_set, value);
	
	#ifdef LF_TARGET_LITEC
	void __LFSetReleaseCallback(void *object);
	__LFSetReleaseCallback = _set->callbacks.LFSetReleaseCallback;
	#endif

	if(!bucket)
	{
		LFRuntimeUnlock(_set);
		return;
	}
	
	if(bucket->key)
	{
		_set->callbacks->LFSetReleaseCallback(value);
		
		bucket->key = NULL;
		_set->count --;
		_set->mutations ++;
		
		__LFSetCollapseIfNeeded(_set);
	}
	
	LFRuntimeUnlock(_set);
}

void LFSetRemoveAllValues(LFSetRef _set)
{
	LFRuntimeLock(_set);
	
	if(LFZombieScribble(_set, "LFSetRemoveAllValues") || !LFRuntimeValidate(_set, __LFSetTypeID))
	{
		LFRuntimeUnlock(_set);
		return;
	}
	
	LFIndex index;
	for(index=0; index<_set->bucketCount; index++)
	{
		struct __LFSetBucket *bucket = (_set->buckets)[index];
		struct __LFSetBucket *tbucket;
		
		if(bucket)
		{
			while(bucket)
			{
				tbucket = bucket;		
				
				_set->callbacks->LFSetReleaseCallback(bucket->key);
				bucket = bucket->next;
				
				free(tbucket);
			}
		}
	}
	
	free(_set->buckets);
	
	_set->count = 0;
	_set->bucketCount = __LFSetBucketCount[0];
	_set->buckets = (struct __LFSetBucket **)malloc(_set->bucketCount * sizeof(struct __LFSetBucket *));
	_set->mutations ++;
	
	if(_set->buckets)
		memset(_set->buckets, 0, _set->bucketCount * sizeof(struct __LFSetBucket *));
	
	LFRuntimeUnlock(_set);
}


void *LFSetGetAnyObject(LFSetRef _set)
{
	LFRuntimeLock(_set);
	
	if(LFZombieScribble(_set, "LFSetGetAnyObject") || !LFRuntimeValidate(_set, __LFSetTypeID))
	{
		LFRuntimeUnlock(_set);
		return NULL;
	}
	
	uint32_t bucket = 0;
	uint32_t overflow = 0;
	
	if(_set->count == 0)
	{
		LFRuntimeUnlock(_set);
		return NULL;
	}
	
	void *entry = __LFSetGetNextEntry(_set, &bucket, &overflow);
	LFRuntimeUnlock(_set);
	
	return entry;
}

uint8_t LFSetContainsValue(LFSetRef _set, void *value)
{
	LFRuntimeLock(_set);
	
	if(LFZombieScribble(_set, "LFSetContainsValue") || !LFRuntimeValidate(_set, __LFSetTypeID))
	{
		LFRuntimeUnlock(_set);
		return 0;
	}
	
	struct __LFSetBucket *bucket = __LFSetFindBucket1(_set, value);
	if(bucket)
	{
		LFRuntimeUnlock(_set);
		return (bucket->key != NULL);
	}
	
	LFRuntimeUnlock(_set);
	return 0;
}

uint32_t LFSetCount(LFSetRef _set)
{
	LFRuntimeLock(_set);
	
	if(LFZombieScribble(_set, "LFSetCount") || !LFRuntimeValidate(_set, __LFSetTypeID))
	{
		LFRuntimeUnlock(_set);
		return 0;
	}
	
	uint32_t count = _set->count;
	LFRuntimeUnlock(_set);
	
	return count;
}
