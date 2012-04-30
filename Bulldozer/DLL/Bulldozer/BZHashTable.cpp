//
//  BZHashTable.c
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

#include "BZBase.h"
#include "BZHashTable.h"

unsigned int __BZCapacity[42] =
{
    5, 11, 23, 41, 67, 113, 199, 317, 521, 839, 1361, 2207, 3571, 5779, 9349, 15121,
    24473, 39607, 64081, 103681, 167759, 271429, 439199, 710641, 1149857, 1860503, 3010349,
    4870843, 7881193, 12752029, 20633237, 33385273, 54018521, 87403763, 141422317, 228826121,
    370248451, 599074561, 969323023, 1568397599, 2537720629, 4106118251
};

unsigned int __BZMaxCapacities[42] =
{
    4, 8, 17, 29, 47, 76, 123, 199, 322, 521, 843, 1364, 2207, 3571, 5778, 9349,
    15127, 24476, 39603, 64079, 103682, 167761, 271443, 439204, 710647, 1149851, 1860498,
    3010349, 4870847, 7881196, 12752043, 20633239, 33385282, 54018521, 87403803, 141422324,
    228826127, 370248451, 599074578, 969323029, 1568397607, 2537720636
};




BZHashTable *BZHashTableCreate()
{
    BZHashTable *table = (BZHashTable *)malloc(sizeof(BZHashTable));
    if(table)
    {
        table->count = 0;
        table->capacity = __BZCapacity[0];
        table->buckets = (BZBucket **)malloc(table->capacity * sizeof(BZBucket *));
        table->noCollapse = 0;

        if(table->buckets)
            memset(table->buckets, 0, table->capacity * sizeof(BZBucket *));
    }

    return table;
}

void BZHashTableDestroy(BZHashTable *table)
{
    int i;
    for(i=0; i<table->capacity; i++)
    {
        if(table->buckets[i])
        {
            BZBucket *bucket = table->buckets[i];
            
            do { 
                BZBucket *next = bucket->next;
                free(bucket);
                
                bucket = next;
            } while(bucket);
            
        }
    }
    
    free(table->buckets);
    free(table);
}



BZBucket *BZHashTableFindBucket1(BZHashTable *table, void *key)
{
	intptr_t index = ((uintptr_t)key) % table->capacity;
	BZBucket *bucket = table->buckets[index];
    
	if(bucket)
	{
		if(bucket->key == key)
            return bucket;
        
        while(bucket->next)
        {
            bucket = bucket->next;
            
            if(bucket->key == key)
                return bucket;
        }
	}

	return NULL;
}

BZBucket *BZHashTableFindBucket2(BZHashTable *table, void *key)
{
	intptr_t index = ((uintptr_t)key) % table->capacity;
	BZBucket *bucket = table->buckets[index];

	if(bucket)
	{
        if(bucket->key == key)
            return bucket;
        
        while(bucket->next)
        {
            bucket = bucket->next;
            
            if(bucket->key == key)
                return bucket;
        }

        

		bucket->next = (BZBucket *)malloc(sizeof(BZBucket));
		if(bucket->next)
		{
			bucket = bucket->next;

			bucket->key  = key;
            bucket->data = NULL;
			bucket->next = NULL;

			return bucket;
		}
	}
	else
	{
		bucket = (BZBucket *)malloc(sizeof(BZBucket));
		if(bucket)
		{
			bucket->key  = key;
            bucket->data = NULL;
			bucket->next = NULL;
		}


		table->buckets[index] = bucket;
		return bucket;
	}

	return NULL;
}



void BZHashTableRehash(BZHashTable *table, BZBucket **buckets, uint32_t count)
{
	int32_t index;
	for(index=0; index<count; index++)
	{
		BZBucket *bucket = buckets[index];
		BZBucket *tbucket;

		if(bucket)
		{
			while(bucket)
			{
				tbucket = bucket;

				if(bucket->key)
				{
					BZBucket *nbucket = BZHashTableFindBucket2(table, bucket->key);

					nbucket->key  = bucket->key;
                    nbucket->data = bucket->data;
				}

				bucket = bucket->next;
				free(tbucket);
			}

			free(bucket);
		}
	}

	free(buckets);
}

void BZHashTableExpandIfNeeded(BZHashTable *table)
{
	uint32_t capacity = 0;
	int32_t index;

	for(index=0; index<42; index++)
	{
		if(table->capacity == __BZCapacity[index])
		{
			if(table->count > __BZMaxCapacities[index])
				capacity = __BZCapacity[index+1];

			break;
		}
	}

	if(capacity > 0 && capacity != table->capacity)
	{
		int32_t oldCount = table->capacity;

		BZBucket **buckets = table->buckets;
		BZBucket **tbuckets = (BZBucket **)malloc(capacity * sizeof(BZBucket *));

		if(tbuckets)
		{
			table->capacity = capacity;
			table->buckets = tbuckets;

			memset(table->buckets, 0, table->capacity * sizeof(BZBucket *));
			BZHashTableRehash(table, buckets, oldCount);
		}
	}
}

void BZHashTableCollapseIfNeeded(BZHashTable *table)
{
    if(table->noCollapse)
        return;
    
	uint32_t capacity = 0;
	int32_t index;

	for(index=1; index<42; index++)
	{
		if(table->count < __BZMaxCapacities[index])
		{
			capacity = __BZCapacity[index-1];
			break;
		}
	}

	if(capacity > 0 && capacity != table->capacity)
	{
		int32_t oldCount = table->capacity;

		BZBucket **buckets = table->buckets;
		BZBucket **tbuckets = (BZBucket **)malloc(capacity * sizeof(BZBucket *));

		if(tbuckets)
		{
			table->capacity = capacity;
			table->buckets = tbuckets;

			memset(table->buckets, 0, table->capacity * sizeof(BZBucket *));
			BZHashTableRehash(table, buckets, oldCount);
		}
	}
}




void BZHashTableInsert(BZHashTable *table, void *key, void *data)
{
    BZBucket *bucket = BZHashTableFindBucket2(table, key);
    if(bucket)
    {
        bucket->data = data;
        
        table->count ++;
        BZHashTableExpandIfNeeded(table);
    }
}

void BZHashTableRemove(BZHashTable *table, void *key)
{
    BZBucket *bucket = BZHashTableFindBucket1(table, key);
    if(bucket)
    {
        table->count --;

        bucket->key  = NULL;
        bucket->data = NULL;

        BZHashTableCollapseIfNeeded(table);
    }
}

char BZHashTableContains(BZHashTable *table, void *key)
{
    if(!key)
        return 0;

    BZBucket *bucket = BZHashTableFindBucket1(table, key);
    return (bucket != NULL);
}

void *BZHashTableDataForKey(BZHashTable *table, void *key)
{
    BZBucket *bucket = BZHashTableFindBucket1(table, key);
    if(bucket)
        return bucket->data;

    return NULL;
}
