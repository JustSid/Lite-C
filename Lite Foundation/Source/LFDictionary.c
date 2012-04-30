//
//  LFDictionary.c
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

#include "LFDictionary.h"
#include "LFInternal.h"
#include "LFAutoreleasePool.h"

uint32_t __LFDictionaryBucketCount[42] = 
{
    5, 11, 23, 41, 67, 113, 199, 317, 521, 839, 1361, 2207, 3571, 5779, 9349, 15121,
    24473, 39607, 64081, 103681, 167759, 271429, 439199, 710641, 1149857, 1860503, 3010349,
    4870843, 7881193, 12752029, 20633237, 33385273, 54018521, 87403763, 141422317, 228826121,
    370248451, 599074561, 969323023, 1568397599, 2537720629, 4106118251
};

uint32_t __LFDictionaryMaxCapacities[42] = 
{
    4, 8, 17, 29, 47, 76, 123, 199, 322, 521, 843, 1364, 2207, 3571, 5778, 9349,
    15127, 24476, 39603, 64079, 103682, 167761, 271443, 439204, 710647, 1149851, 1860498,
    3010349, 4870847, 7881196, 12752043, 20633239, 33385282, 54018521, 87403803, 141422324,
    228826127, 370248451, 599074578, 969323029, 1568397607, 2537720636
}; 

//------------------------------
// Runtime functions
//------------------------------

void		__LFDictionaryInit(LFTypeRef ref);
LFTypeRef	__LFDictionaryCopy(LFTypeRef ref);
void		__LFDictionaryDealloc(LFTypeRef ref);
LFHashCode	__LFDictionaryHash(LFTypeRef ref);
uint8_t		__LFDictionaryEqual(LFTypeRef refa, LFTypeRef refb);

void __LFDictionaryEnumerate(LFDictionaryRef dict, LFEnumeratorRef enumerator, uint32_t maxCount);

static LFRuntimeClass __LFDictionaryClass;
static LFTypeID __LFDictionaryTypeID = kLFInvalidTypeID;


void __LFRuntimeInitDictionary()
{
	__LFDictionaryClass.version = 1;
	__LFDictionaryClass.name = "LFDictionary";
	
	__LFDictionaryClass.init = __LFDictionaryInit;
	__LFDictionaryClass.copy = __LFDictionaryCopy;
	__LFDictionaryClass.dealloc = __LFDictionaryDealloc;
	__LFDictionaryClass.equal = __LFDictionaryEqual;
	__LFDictionaryClass.hash  = __LFDictionaryHash;
	
	__LFDictionaryClass.superClass = kLFInvalidTypeID;
	__LFDictionaryClass.protocolBag = LFRuntimeCreateProtocolBag();
	
	
	LFProtocolBody *enumeratorBody = LFRuntimeCreateProtocolBody(LFEnumeratorGetProtocolID());
	LFRuntimeProtocolBodySetIMP(enumeratorBody, LFEnumeratorGetSelector(kLFEnumeratorProtocolEnumerateWithObjects), LFProtocolMethod20(__LFDictionaryEnumerate));
	
	LFRuntimeAppendProtocol(__LFDictionaryClass.protocolBag, enumeratorBody);
	
	__LFDictionaryTypeID = LFRuntimeRegisterClass((const LFRuntimeClass *)&__LFDictionaryClass);
}


void __LFDictionaryInit(LFTypeRef ref)
{
	LFDictionaryRef dict = (LFDictionaryRef)ref;
	
	dict->callbacks.LFDictionaryKeyEqualCallback	= LFEqual;
	dict->callbacks.LFDictionaryKeyHashCallback	= LFHash;
	dict->callbacks.LFDictionaryKeyRetainCallback	= LFRetain;
	dict->callbacks.LFDictionaryKeyReleaseCallback = LFRelease;
	
	dict->callbacks.LFDictionaryValueRetainCallback	= LFRetain;
	dict->callbacks.LFDictionaryValueReleaseCallback = LFRelease;
	
	dict->count = 0;
	dict->bucketCount = __LFDictionaryBucketCount[0];
	dict->buckets = (struct __LFDictionaryBucket **)malloc(dict->bucketCount * sizeof(struct __LFDictionaryBucket *));
	
	if(dict->buckets)
	{
		memset(dict->buckets, 0, dict->bucketCount * sizeof(struct __LFDictionaryBucket *));
	}
}

LFTypeRef __LFDictionaryCopy(LFTypeRef ref)
{
	LFDictionaryRef source = (LFDictionaryRef)ref;
	LFDictionaryRef copy = LFDictionaryCreate(&source->callbacks);
	
    if(!copy)
        return NULL;
    
	copy->bucketCount = source->bucketCount;
	copy->count = source->count;
	
    free(copy->buckets);
	copy->buckets = (struct __LFDictionaryBucket **)malloc(copy->bucketCount * sizeof(struct __LFDictionaryBucket *));
	
#ifdef LF_TARGET_LITEC
	void *__LFDictionaryValueRetainCallback(void *object);	
	void *__LFDictionaryKeyRetainCallback(void *object);
	
	__LFDictionaryValueRetainCallback = copy->callbacks.LFDictionaryValueRetainCallback;
	__LFDictionaryKeyRetainCallback = copy->callbacks.LFDictionaryKeyRetainCallback;
#endif
	
	if(copy->buckets)
	{
		memset(copy->buckets, 0, copy->bucketCount * sizeof(struct __LFDictionaryBucket *));
		LFIndex index;
		
		for(index=0; index<copy->bucketCount; index++)
		{
			struct __LFDictionaryBucket *cbucket = (source->buckets)[index];
			if(cbucket)
			{
				struct __LFDictionaryBucket *sbucket = (struct __LFDictionaryBucket *)malloc(sizeof(struct __LFDictionaryBucket));
				if(!sbucket)
				{
					LFRelease(copy);
					return NULL;
				}
				
				memcpy(sbucket, cbucket, sizeof(struct __LFDictionaryBucket));
				
#ifdef LF_TARGET_LITEC
				if(sbucket->key)
				{
					sbucket->key = __LFDictionaryKeyRetainCallback(cbucket->key);
					sbucket->value = __LFDictionaryValueRetainCallback(cbucket->value);
				}
#else
				if(sbucket->key)
				{
					sbucket->key = copy->callbacks.LFDictionaryKeyRetainCallback(cbucket->key);
					sbucket->value = copy->callbacks.LFDictionaryValueRetainCallback(cbucket->value);
				}
#endif
				
				// Copy the overflow buckets...
				while(cbucket->next)
				{
					struct __LFDictionaryBucket *nbucket = (struct __LFDictionaryBucket *)malloc(sizeof(struct __LFDictionaryBucket));
					if(!nbucket)
					{
						LFRelease(copy);
						return NULL;
					}
					
					memcpy(nbucket, cbucket->next, sizeof(struct __LFDictionaryBucket));
					
#ifdef LF_TARGET_LITEC
					if(nbucket->key)
					{
						nbucket->key = __LFDictionaryKeyRetainCallback(cbucket->next->key);
						nbucket->value = __LFDictionaryValueRetainCallback(cbucket->next->value);
					}
#else
					if(nbucket->key)
					{
						nbucket->key = copy->callbacks.LFDictionaryKeyRetainCallback(cbucket->next->key);
						nbucket->value = copy->callbacks.LFDictionaryValueRetainCallback(cbucket->next->value);
					}
#endif
					
					sbucket->next = nbucket;
					
					sbucket = nbucket;
					cbucket = cbucket->next;
				}
				
				
				(copy->buckets)[index] = sbucket;
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

void __LFDictionaryDealloc(LFTypeRef ref)
{
	LFDictionaryRef dict = (LFDictionaryRef)ref;
	
#ifdef LF_TARGET_LITEC
	void __LFDictionaryValueReleaseCallback(void *object);
	void __LFDictionaryKeyReleaseCallback(void *object);
	
	__LFDictionaryValueReleaseCallback = dict->callbacks.LFDictionaryValueReleaseCallback;
	__LFDictionaryKeyReleaseCallback = dict->callbacks.LFDictionaryKeyReleaseCallback;
#endif
	
	LFIndex index;
	for(index=0; index<dict->bucketCount; index++)
	{
		struct __LFDictionaryBucket *bucket = (dict->buckets)[index];
		struct __LFDictionaryBucket *tbucket;
		
		if(bucket)
		{
			while(bucket)
			{
				tbucket = bucket;		
				
#ifdef LF_TARGET_LITEC
				__LFDictionaryValueReleaseCallback(bucket->value);
				__LFDictionaryKeyReleaseCallback(bucket->key);
#else
				dict->callbacks.LFDictionaryValueReleaseCallback(bucket->value);
				dict->callbacks.LFDictionaryKeyReleaseCallback(bucket->key);
#endif
				bucket = bucket->next;
				free(tbucket);
			}
		}
	}
	
	free(dict->buckets);
}

LFHashCode __LFDictionaryHash(LFTypeRef ref)
{
	LFDictionaryRef dict = (LFDictionaryRef)ref;
	return (LFHashCode)dict->count;
}

uint8_t __LFDictionaryEqual(LFTypeRef refa, LFTypeRef refb)
{
	LFDictionaryRef dicta = (LFDictionaryRef)refa;
	LFDictionaryRef dictb = (LFDictionaryRef)refb;
	
	if(dicta->count != dictb->count)
		return 0;
	
	LFEnumeratorRef enumerator = LFDictionaryKeyEnumerator(dicta);
	LFTypeRef object;
	
	while((object = LFEnumeratorNextObject(enumerator)))
	{
		if(!LFDictionaryValueForKey(dictb, object))
			return 0;
	}
	
	return 1;
}



LFTypeID LFDictionaryGetTypeID()
{
	return __LFDictionaryTypeID;
}

LFDictionaryRef LFDictionaryCreate(LFDictionaryCallbacks *callbacks)
{
	LFDictionaryRef dict = (LFDictionaryRef)LFRuntimeCreateInstance(__LFDictionaryTypeID, sizeof(struct __LFDictionary));
	
	if(callbacks)
	{
		dict->callbacks.LFDictionaryKeyEqualCallback	= callbacks->LFDictionaryKeyEqualCallback;
		dict->callbacks.LFDictionaryKeyHashCallback		= callbacks->LFDictionaryKeyHashCallback;
		dict->callbacks.LFDictionaryKeyRetainCallback	= callbacks->LFDictionaryKeyRetainCallback;
		dict->callbacks.LFDictionaryKeyReleaseCallback	= callbacks->LFDictionaryKeyReleaseCallback;
		
		dict->callbacks.LFDictionaryValueRetainCallback	 = callbacks->LFDictionaryValueRetainCallback;
		dict->callbacks.LFDictionaryValueReleaseCallback = callbacks->LFDictionaryValueReleaseCallback;
	}
	
	return dict;
}




struct __LFDictionaryBucket *__LFDictionaryFindBucket1(LFDictionaryRef dict, void *key)
{
#ifdef LF_TARGET_LITEC
	uint8_t 	__LFDictionaryKeyEqualCallback(void *objecta, void *objectb);
	LFHashCode 	__LFDictionaryKeyHashCallback(void *object);
	
	__LFDictionaryKeyHashCallback  = dict->callbacks.LFDictionaryKeyHashCallback;
	__LFDictionaryKeyEqualCallback = dict->callbacks.LFDictionaryKeyEqualCallback;
	
	LFHashCode hash = __LFDictionaryKeyHashCallback(key);
	LFIndex index	= hash % dict->bucketCount;
#else
	LFHashCode hash = dict->callbacks.LFDictionaryKeyHashCallback(key);
	LFIndex index	= hash % dict->bucketCount;
#endif
	
	
	struct __LFDictionaryBucket *bucket = (dict->buckets)[index];
	if(bucket)
	{
		if(bucket->key)
		{
#ifdef LF_TARGET_LITEC
			if(__LFDictionaryKeyEqualCallback(bucket->key, key))
				return bucket;
#else
			if(dict->callbacks.LFDictionaryKeyEqualCallback(bucket->key, key))
				return bucket;
#endif
		}
		
		
		if(bucket->next)
		{
			bucket = bucket->next;
			
			while(bucket)
			{
				if(bucket->key)
				{
#ifdef LF_TARGET_LITEC
					if(__LFDictionaryKeyEqualCallback(bucket->key, key))
						return bucket;
#else
					if(dict->callbacks.LFDictionaryKeyEqualCallback(bucket->key, key))
						return bucket;
#endif
				}
				
				if(!bucket->next)
					break;
				
				
				bucket = bucket->next;
			}
		}
	}
	
	return NULL;
}

struct __LFDictionaryBucket *__LFDictionaryFindBucket2(LFDictionaryRef dict, void *key)
{
#ifdef LF_TARGET_LITEC
	uint8_t 	__LFDictionaryKeyEqualCallback(void *objecta, void *objectb);
	LFHashCode 	__LFDictionaryKeyHashCallback(void *object);
	
	__LFDictionaryKeyHashCallback  = dict->callbacks.LFDictionaryKeyHashCallback;
	__LFDictionaryKeyEqualCallback = dict->callbacks.LFDictionaryKeyEqualCallback;
	
	LFHashCode hash = __LFDictionaryKeyHashCallback(key);
	LFIndex index	= hash % dict->bucketCount;
#else
	LFHashCode hash = dict->callbacks.LFDictionaryKeyHashCallback(key);
	LFIndex index	= hash % dict->bucketCount;
#endif
	
	struct __LFDictionaryBucket *bucket = (dict->buckets)[index];
	struct __LFDictionaryBucket *freeBucket = NULL;
	
	if(bucket)
	{
		if(bucket->key)
		{
#ifdef LF_TARGET_LITEC
			if(__LFDictionaryKeyEqualCallback(bucket->key, key))
				return bucket;
#else
			if(dict->callbacks.LFDictionaryKeyEqualCallback(bucket->key, key))
				return bucket;
#endif
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
#ifdef LF_TARGET_LITEC
					if(__LFDictionaryKeyEqualCallback(key, bucket->key))
						return bucket;
#else
					if(dict->callbacks.LFDictionaryKeyEqualCallback(key, bucket->key))
						return bucket;
#endif
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
		
		
		bucket->next = (struct __LFDictionaryBucket *)malloc(sizeof(struct __LFDictionaryBucket));
		
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
		bucket = (struct __LFDictionaryBucket *)malloc(sizeof(struct __LFDictionaryBucket));
		
		if(bucket)
		{		
			bucket->key = NULL;
			bucket->next = NULL;
		}
		
		(dict->buckets)[index] = bucket;
		return bucket;
	}
	
	return NULL;
}




void __LFDictionaryRehashBuckets(LFDictionaryRef dict, struct __LFDictionaryBucket **buckets, uint32_t count)
{
	LFIndex index;
	for(index=0; index<count; index++)
	{
		struct __LFDictionaryBucket *bucket = buckets[index];
		struct __LFDictionaryBucket *tbucket;
		
		if(bucket)
		{
			while(bucket)
			{
				tbucket = bucket;			
				
				if(bucket->key)
				{
					struct __LFDictionaryBucket *nbucket = __LFDictionaryFindBucket2(dict, bucket->key);
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

void __LFDictionaryExpandIfNeeded(LFDictionaryRef dict)
{		
	uint32_t bucketCount = 0;
	LFIndex index;
	
	for(index=0; index<42; index++)
	{
		if(dict->bucketCount == __LFDictionaryBucketCount[index])
		{
			if(dict->count > __LFDictionaryMaxCapacities[index])
			{
				bucketCount = __LFDictionaryBucketCount[index+1];
			}
			
			break;
		}
	}
	
	if(bucketCount > 0 && bucketCount != dict->bucketCount)
	{
		LFIndex oldCount = dict->bucketCount;
		
		struct __LFDictionaryBucket **buckets = dict->buckets;
		struct __LFDictionaryBucket **tbuckets = (struct __LFDictionaryBucket **)malloc(bucketCount * sizeof(struct __LFDictionaryBucket *));
		
		if(tbuckets)
		{
			dict->bucketCount = bucketCount;
			dict->buckets = tbuckets;
			
			memset(dict->buckets, 0, dict->bucketCount * sizeof(struct __LFDictionaryBucket *));
			__LFDictionaryRehashBuckets(dict, buckets, oldCount);
		}
	}
}

void __LFDictionaryCollapseIfNeeded(LFDictionaryRef dict)
{		
	uint32_t bucketCount = 0;
	LFIndex index;
	
	for(index=1; index<42; index++)
	{
		if(dict->count < __LFDictionaryMaxCapacities[index])
		{
			bucketCount = __LFDictionaryBucketCount[index-1];
			break;
		}
	}
	
	if(bucketCount > 0 && bucketCount != dict->bucketCount)
	{
		LFIndex oldCount = dict->bucketCount;
		
		struct __LFDictionaryBucket **buckets = dict->buckets;
		struct __LFDictionaryBucket **tbuckets = (struct __LFDictionaryBucket **)malloc(bucketCount * sizeof(struct __LFDictionaryBucket *));
		
		if(tbuckets)
		{
			dict->bucketCount = bucketCount;
			dict->buckets = tbuckets;
			
			memset(dict->buckets, 0, dict->bucketCount * sizeof(struct __LFDictionaryBucket *));
			__LFDictionaryRehashBuckets(dict, buckets, oldCount);
		}
	}
}


void *__LFDictionaryGetNextEntry(LFDictionaryRef dict, uint32_t *__bucket, uint32_t *__overflow, char key)
{	
	LFIndex index1 = (LFIndex)*__bucket;
	LFIndex index2;
	
    if(index1 >= dict->bucketCount)
        return NULL;
    
	struct __LFDictionaryBucket *bucket = (dict->buckets)[index1];
	
	while(!bucket)
	{
		index1 ++;		
		if(index1 >= dict->bucketCount)
			return NULL;
		
		bucket = (dict->buckets)[index1];
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
			return __LFDictionaryGetNextEntry(dict, __bucket, __overflow, key);
		
		if(key)
			return bucket->key;
		else
			return bucket->value;
	}
	else
	{
		*__bucket = *__bucket + 1;
		return __LFDictionaryGetNextEntry(dict, __bucket, __overflow, key);
	}
	
	
	return NULL;
}

void __LFDictionaryEnumerate(LFDictionaryRef dict, LFEnumeratorRef enumerator, uint32_t maxCount)
{
	LFRuntimeLock(dict);
	
	if(enumerator->state == 0)
	{
		enumerator->extra[0] = dict->mutations;
		enumerator->extra[1] = 0; // Bucket index
		enumerator->extra[2] = 0; // Overflow bucket
		// Enumerator 4 is the type flag, 1 = key enumerator, 0 = value enmurator
		enumerator->itemCount = 0;
	}
	else
	{
		LFAssert(enumerator->extra[0] == dict->mutations, "Enumerating a mutated LFDictionary is not supported!");
		enumerator->itemCount = 0;
	}
	
	
	LFIndex index;
	for(index=0; index<maxCount; index++)
	{
		void *object = __LFDictionaryGetNextEntry(dict, &enumerator->extra[1], &enumerator->extra[2], (char)enumerator->extra[4]);
		if(!object)
		{
			enumerator->itemCount = index;
			
			LFRuntimeUnlock(dict);
			return;
		}
		
		enumerator->itemsPtr[index] = object;
	}
	
	enumerator->itemCount = maxCount;
	LFRuntimeUnlock(dict);
}



void LFDictionarySetValueForKey(LFDictionaryRef dict, void *value, void *key)
{
	LFRuntimeLock(dict);
	
	if(LFZombieScribble(dict, "LFDictionarySetValueForKey") || !LFRuntimeValidate(dict, __LFDictionaryTypeID))
	{
		LFRuntimeUnlock(dict);
		return;
	}
    
	
	if(!value)
	{
		LFDictionaryRemoveValueForKey(dict, key);
		LFRuntimeUnlock(dict);
		
		return;
	}
	
	struct __LFDictionaryBucket *bucket = __LFDictionaryFindBucket2(dict, value);
	
#ifdef LF_TARGET_LITEC
	void *__LFDictionaryValueRetainCallback(void *object);
	void *__LFDictionaryKeyRetainCallback(void *object);
	
	__LFDictionaryValueRetainCallback = dict->callbacks.LFDictionaryValueRetainCallback;
	__LFDictionaryKeyRetainCallback = dict->callbacks.LFDictionaryKeyRetainCallback;
#endif
	
	if(!bucket)
		return;
	
	if(bucket->key == NULL)
	{
#ifdef LF_TARGET_LITEC
		bucket->value = __LFDictionaryValueRetainCallback(value);
		bucket->key = __LFDictionaryKeyRetainCallback(key);
#else
		bucket->value = dict->callbacks.LFDictionaryValueRetainCallback(value);
		bucket->key = dict->callbacks.LFDictionaryKeyRetainCallback(key);
#endif
		dict->count ++;
		dict->mutations ++;
		
		__LFDictionaryExpandIfNeeded(dict);
	}
	
	LFRuntimeUnlock(dict);
}

void LFDictionaryRemoveValueForKey(LFDictionaryRef dict, void *key)
{
	LFRuntimeLock(dict);
	
	if(LFZombieScribble(dict, "LFDictionaryRemoveValueForKey") || !LFRuntimeValidate(dict, LFDictionaryGetTypeID()))
	{
		LFRuntimeUnlock(dict);
		return;
	}
	
	struct __LFDictionaryBucket *bucket = __LFDictionaryFindBucket1(dict, key);
	
#ifdef LF_TARGET_LITEC
	void __LFDictionaryValueReleaseCallback(void *object);
	void __LFDictionaryKeyReleaseCallback(void *object);
	
	__LFDictionaryKeyReleaseCallback = dict->callbacks.LFDictionaryKeyReleaseCallback;
	__LFDictionaryValueReleaseCallback = dict->callbacks.LFDictionaryValueReleaseCallback;
#endif
	
	if(!bucket)
		return;
	
	if(bucket->key)
	{
#ifdef LF_TARGET_LITEC
		__LFDictionaryValueReleaseCallback(bucket->value);
		__LFDictionaryKeyReleaseCallback(bucket->key);
#else
		dict->callbacks.LFDictionaryValueReleaseCallback(bucket->value);
		dict->callbacks.LFDictionaryKeyReleaseCallback(bucket->key);
#endif
		bucket->key = NULL;
		bucket->value = NULL;
		
		dict->count --;
		dict->mutations ++;
		
		__LFDictionaryCollapseIfNeeded(dict);
	}
	
	LFRuntimeUnlock(dict);
}

void LFDictionaryRemoveAllValues(LFDictionaryRef dict)
{
	LFRuntimeLock(dict);
	
	if(LFZombieScribble(dict, "LFDictionaryRemoveAllValues") || !LFRuntimeValidate(dict, LFDictionaryGetTypeID()))
	{
		LFRuntimeUnlock(dict);
		return;
	}
	
#ifdef LF_TARGET_LITEC
	void __LFDictionaryValueReleaseCallback(void *object);
	void __LFDictionaryKeyReleaseCallback(void *object);
	
	__LFDictionaryKeyReleaseCallback = dict->callbacks.LFDictionaryKeyReleaseCallback;
	__LFDictionaryValueReleaseCallback = dict->callbacks.LFDictionaryValueReleaseCallback;
#endif
	
	LFIndex index;
	for(index=0; index<dict->bucketCount; index++)
	{
		struct __LFDictionaryBucket *bucket = (dict->buckets)[index];
		struct __LFDictionaryBucket *tbucket;
		
		if(bucket)
		{
			while(bucket)
			{
				tbucket = bucket;		
				
#ifdef LF_TARGET_LITEC
				__LFDictionaryValueReleaseCallback(bucket->value);
				__LFDictionaryKeyReleaseCallback(bucket->key);
#else
				dict->callbacks.LFDictionaryValueReleaseCallback(bucket->value);
				dict->callbacks.LFDictionaryKeyReleaseCallback(bucket->key);
#endif
				bucket = bucket->next;
				
				free(tbucket);
			}
		}
	}
	
	free(dict->buckets);
	
	dict->count = 0;
	dict->bucketCount = __LFDictionaryBucketCount[0];
	dict->buckets = (struct __LFDictionaryBucket **)malloc(dict->bucketCount * sizeof(struct __LFDictionaryBucket *));
	dict->mutations ++;
	
	if(dict->buckets)
		memset(dict->buckets, 0, dict->bucketCount * sizeof(struct __LFDictionaryBucket *));
	
	LFRuntimeUnlock(dict);
}

void *LFDictionaryValueForKey(LFDictionaryRef dict, void *key)
{
	LFRuntimeLock(dict);
	
	if(LFZombieScribble(dict, "LFDictionaryValueForKey") || !LFRuntimeValidate(dict, LFDictionaryGetTypeID()))
	{
		LFRuntimeUnlock(dict);
		return NULL;
	}
	
	struct __LFDictionaryBucket *bucket = __LFDictionaryFindBucket1(dict, key);
	
	if(!bucket)
		return NULL;
	
	void *value = bucket->value;
	
	LFRuntimeUnlock(dict);
	return value;
}

uint32_t LFDictionaryCount(LFDictionaryRef dict)
{
	LFRuntimeLock(dict);
	if(LFZombieScribble(dict, "LFDictionaryCount") || !LFRuntimeValidate(dict, LFDictionaryGetTypeID()))
	{
		LFRuntimeUnlock(dict);
		return 0;
	}
	
	uint32_t count = dict->count;
	LFRuntimeUnlock(dict);
	return count;
}


LFEnumeratorRef LFDictionaryKeyEnumerator(LFDictionaryRef dict)
{
	LFRuntimeLock(dict);
	
	if(LFZombieScribble(dict, "LFDictionaryKeyEnumerator") || !LFRuntimeValidate(dict, LFDictionaryGetTypeID()))
	{
		LFRuntimeUnlock(dict);
		return NULL;
	}
	
	LFEnumeratorRef enumerator = LFEnumeratorCreate(dict);
	enumerator->extra[4] = 1;
	
	LFRuntimeUnlock(dict);
	return LFAutorelease(enumerator);
}

LFEnumeratorRef LFDictionaryValueEnumerator(LFDictionaryRef dict)
{
	LFRuntimeLock(dict);
	
	if(LFZombieScribble(dict, "LFDictionaryValueEnumerator") || !LFRuntimeValidate(dict, LFDictionaryGetTypeID()))
	{
		LFRuntimeUnlock(dict);
		return NULL;
	}
	
	LFEnumeratorRef enumerator = LFEnumeratorCreate(dict);
	enumerator->extra[4] = 0;
	
	LFRuntimeUnlock(dict);
	return LFAutorelease(enumerator);
}

