//
//  LFKVCoding.c
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

#include "LFKVCoding.h"
#include "LFAutoreleasePool.h"

// KVO Hash table

uint32_t __LFKVOBucketCount[42] = 
{
    5, 11, 23, 41, 67, 113, 199, 317, 521, 839, 1361, 2207, 3571, 5779, 9349, 15121,
    24473, 39607, 64081, 103681, 167759, 271429, 439199, 710641, 1149857, 1860503, 3010349,
    4870843, 7881193, 12752029, 20633237, 33385273, 54018521, 87403763, 141422317, 228826121,
    370248451, 599074561, 969323023, 1568397599, 2537720629, 4106118251
};

uint32_t __LFKVOBucketCapacities[42] = 
{
    4, 8, 17, 29, 47, 76, 123, 199, 322, 521, 843, 1364, 2207, 3571, 5778, 9349,
    15127, 24476, 39603, 64079, 103682, 167761, 271443, 439204, 710647, 1149851, 1860498,
    3010349, 4870847, 7881196, 12752043, 20633239, 33385282, 54018521, 87403803, 141422324,
    228826127, 370248451, 599074578, 969323029, 1568397607, 2537720636
}; 

LFHashCode __LFKVOGetHash(char *key)
{
	LFIndex length = (LFIndex)strlen(key);
	LFIndex	iterator;
	
	LFHashCode res = length;
	
	if(length <= 16) 
	{
		for(iterator=0; iterator<length; iterator++) 
			res = res * 257 + key[iterator];
	} 
	else 
	{
		// Hash the first and last 8 bytes
		for(iterator = 0; iterator<8; iterator++) 
			res = res * 257 + key[iterator];
		
		for(iterator = length - 8; iterator<length; iterator++) 
			res = res * 257 + key[iterator];
	}
	
	return (res << (length & 31));
}

struct __LFRuntimeTableBucket *__LFKVOBucketLookup1(LFRuntimeBase *base, char *key)
{
    __LFRuntimeTable *table = base->__kvoTable;
    LFHashCode hash = __LFKVOGetHash(key);
    LFIndex index = hash % table->bucketCount;
    
    struct __LFRuntimeTableBucket *bucket = table->buckets[index];
    while(bucket)
    {
        if(strcmp(key, bucket->key) == 0)
            return bucket;
        
        bucket = bucket->next;
    }
    
    return NULL;
}

struct __LFRuntimeTableBucket *__LFKVOBucketLookup2(LFRuntimeBase *base, char *key)
{
    __LFRuntimeTable *table = base->__kvoTable;
    LFHashCode hash = __LFKVOGetHash(key);
    LFIndex index = hash % table->bucketCount;
    
    struct __LFRuntimeTableBucket *bucket = table->buckets[index];
    struct __LFRuntimeTableBucket *lastBucket = NULL;
    while(bucket)
    {
        if(strcmp(key, bucket->key) == 0)
            return bucket;
        
        lastBucket = bucket;
        bucket = bucket->next;
    }
    
    
    bucket = (struct __LFRuntimeTableBucket *)malloc(sizeof(struct __LFRuntimeTableBucket));
    bucket->key = (char *)malloc((strlen(key) + 1) * sizeof(char));
    bucket->data = NULL;
    bucket->next = NULL;
    
    strcpy(bucket->key, key);
    
    if(!lastBucket)
    {
        table->buckets[index] = bucket;
    }
    else
    {
        lastBucket->next = bucket;
    }
    
    table->count ++;
    return bucket;
}

void __LFKVORehash(LFRuntimeBase *base, struct __LFRuntimeTableBucket **buckets, uint32_t count)
{
	LFIndex index;
	for(index=0; index<count; index++)
	{
		struct __LFRuntimeTableBucket *bucket = buckets[index];
		struct __LFRuntimeTableBucket *tbucket;
		
        while(bucket)
        {
            tbucket = bucket;			
            
            if(bucket->data)
            {
                struct __LFRuntimeTableBucket *nbucket = __LFKVOBucketLookup2(base, bucket->key);
                nbucket->data = bucket->data;
            }
            
            bucket = bucket->next;
            
            free(tbucket->key);
            free(tbucket);
        }
	}
	
	free(buckets);
} 

void __LFKVORehashIfNeeded(LFRuntimeBase *base)
{
    uint32_t bucketCount = 0;
	LFIndex index;
	
	for(index=0; index<42; index++)
	{
		if(base->__kvoTable->bucketCount == __LFKVOBucketCount[index])
		{
			if(base->__kvoTable->count > __LFKVOBucketCapacities[index])
			{
				bucketCount = __LFKVOBucketCount[index+1];
			}
			
			break;
		}
	}
    
	
	if(bucketCount > 0 && bucketCount != base->__kvoTable->bucketCount)
	{
        LFIndex oldCount = base->__kvoTable->bucketCount;
        struct __LFRuntimeTableBucket **buckets = base->__kvoTable->buckets;
        struct __LFRuntimeTableBucket **nbuckets = (struct __LFRuntimeTableBucket **)malloc(bucketCount * sizeof(struct __LFRuntimeTableBucket *));
        
        if(nbuckets)
        {
            base->__kvoTable->bucketCount = bucketCount;
            base->__kvoTable->buckets = nbuckets;
            
            memset(base->__kvoTable->buckets, 0, base->__kvoTable->bucketCount * sizeof(struct __LFRuntimeTableBucket *));
            __LFKVORehash(base, buckets, oldCount);
        }
	}
}

__LFRuntimeTable *__LFKVOCreateTable()
{
    __LFRuntimeTable *table = (__LFRuntimeTable *)malloc(sizeof(__LFRuntimeTable));
    table->count = 0;
    table->bucketCount = __LFKVOBucketCount[0];
    table->buckets = (struct __LFRuntimeTableBucket **)malloc(table->bucketCount * sizeof(struct __LFRuntimeTableBucket *));
    
    memset(table->buckets, 0, table->bucketCount * sizeof(struct __LFRuntimeTableBucket *));
    
    return table;
}

void __LFKVODeleteTable(__LFRuntimeTable *table)
{
    LFIndex index;
    for(index=0; index<table->bucketCount; index++)
    {
        struct __LFRuntimeTableBucket *bucket = table->buckets[index];
        while(bucket)
        {
            struct __LFRuntimeTableBucket *tbucket = bucket;
            free(bucket->key);            
            free(bucket);
            
            bucket = tbucket->next;
        }
    }
    
    free(table->buckets);
    free(table);
}


// Observer handling
void LFAddObserverForKey(LFTypeRef ref, char *key, LFObserverGenericCallback callback, LFRuntimeObserverType type)
{
    __LFRuntimeKVObserver *observer = (__LFRuntimeKVObserver *)malloc(sizeof(__LFRuntimeKVObserver));
    observer->type = type;
    observer->callback = callback;
    observer->key = (char *)malloc((strlen(key) + 1) * sizeof(char));
    
    strcpy(observer->key, key);
    
    
    // Insert the new observer
    LFRuntimeBase *base = (LFRuntimeBase *)ref;
    __LFRuntimeKVObserver **list = base->__kvoList;
    
    if(list)
    {
        LFIndex index, found = 0;
        for(index=0; index<base->__kvoCount; index++)
        {
            if(base->__kvoList[index] == NULL)
            {
                list[index] = observer;
                found = 1;
                break;
            }
        }
        
        if(!found)
        {
            list = (__LFRuntimeKVObserver **)realloc(list, (base->__kvoCount + 1) * sizeof(__LFRuntimeKVObserver *));
            list[base->__kvoCount] = observer;
        
            base->__kvoCount ++;
        }
    }
    else
    {
        list = (__LFRuntimeKVObserver **)malloc(sizeof(__LFRuntimeKVObserver *));
        list[0] = observer;
        
        base->__kvoCount = 1;
        base->__kvoList = list;
    }
}

void LFRemoveObserverForKey(LFTypeRef ref, char *key, LFObserverGenericCallback callback, LFRuntimeObserverType type)
{
    LFRuntimeBase *base = (LFRuntimeBase *)ref;
    LFIndex index;
    __LFRuntimeKVObserver **list = base->__kvoList;
    
    if(list)
    {
        for(index=0; index<base->__kvoCount; index++)
        {
            __LFRuntimeKVObserver *observer = list[index];
            if((observer->type & type) && observer->callback == callback && strcmp(key, observer->key) == 0)
            {
                observer->type &= ~type;
                
                if(observer->type == 0)
                {
                    free(observer->key);
                    free(observer);
                
                    list[index] = NULL;
                }
            }
        }
    }
}

// Observer notifying

void __LFKVOAccessOrChangeWithKey(LFTypeRef ref, char *key, LFRuntimeObserverType type)
{
    LFRuntimeBase *base = (LFRuntimeBase *)ref;
    LFIndex index;
    
    if(!base->__kvoList)
        return;
    
    for(index=0; index<base->__kvoCount; index++)
    {
        __LFRuntimeKVObserver *observer = base->__kvoList[index];
        
        if(!observer)
            continue;
        
        if(observer->type & type)
        {
            if(strcmp(observer->key, key) == 0)
            {
                struct __LFRuntimeTableBucket *bucket = __LFKVOBucketLookup1((LFRuntimeBase *)ref, key);
                if(!bucket)
                    continue;
                
                observer->callback(ref, key, bucket->data, type);
            }
        }
    }
}



void LFWillAccessValueForKey(LFTypeRef ref, char *key)
{
    __LFKVOAccessOrChangeWithKey(ref, key, kLFObserverTypeWillAccess);
}

void LFDidAccessValueForKey(LFTypeRef ref, char *key)
{
    __LFKVOAccessOrChangeWithKey(ref, key, kLFObserverTypeDidAccess);
}

void LFWillChangeValueForKey(LFTypeRef ref, char *key)
{
    __LFKVOAccessOrChangeWithKey(ref, key, kLFObserverTypeWillChange);
}

void LFDidChangeValueForKey(LFTypeRef ref, char *key)
{
    __LFKVOAccessOrChangeWithKey(ref, key, kLFObserverTypeDidChange);
}



// Accessors
LFTypeRef LFGetValueForKey(LFTypeRef ref, char *key)
{
    struct __LFRuntimeTableBucket *bucket = __LFKVOBucketLookup1((LFRuntimeBase *)ref, key);
    if(bucket)
    {
        LFWillAccessValueForKey(ref, key);
        LFTypeRef data = (LFTypeRef)bucket->data;
        LFDidAccessValueForKey(ref, key);
        
        return data;
    }
    
    return NULL;
}

void LFSetValueForKey(LFTypeRef ref, char *key, LFTypeRef value)
{
    struct __LFRuntimeTableBucket *bucket = __LFKVOBucketLookup2((LFRuntimeBase *)ref, key);
    if(bucket)
    {
        LFWillChangeValueForKey(ref, key);
        
        if(bucket->data)
            LFAutorelease(bucket->data);
        
        bucket->data = LFRetain(value);
        LFDidChangeValueForKey(ref, key);
    }
}

