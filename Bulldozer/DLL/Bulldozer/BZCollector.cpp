//
//  BZCollector.c
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
#include "BZCollector.h"
#include "BZHashTable.h"

static uintptr_t _BZCollectorBegin = 0x0;

BZHashTable *__BZCollectableHashTable();
char __BZCollectableIsStatic(void *pointer);
void BZHashTableCollapseIfNeeded(BZHashTable *table);


void BZCollectorInit(void *address)
{
    _BZCollectorBegin = (uintptr_t)address;
}



void BZCollectorTraceCollectable(BZCollectable *tcollectable, BZHashTable *collectableTable, BZHashTable *preserveTable)
{
    void **collection;
    if(tcollectable->count == 0)
    {
        collection = (void **)tcollectable->pointer;

        if(*collection == NULL)
            return;
    }
    

    uintptr_t heapCollection = (uintptr_t)tcollectable->pointer;
    uintptr_t heapCollectionEnd = (heapCollection + tcollectable->size) - sizeof(void *);

    while(heapCollection <= heapCollectionEnd)
    {
        collection = (void **)heapCollection;

        if(BZHashTableContains(preserveTable, *collection))
        {
            heapCollection += sizeof(void *);
            continue;
        }


        if(BZHashTableContains(collectableTable, *collection))
        {
            // If the pointer is visible, add it to the preserve table, so we don't delete it by accident
            BZCollectable *collectable = (BZCollectable *)BZHashTableDataForKey(collectableTable, *collection);

            BZHashTableInsert(preserveTable, collectable->pointer, collectable);
            BZCollectorTraceCollectable(collectable, collectableTable, preserveTable);
            
            heapCollection += sizeof(void *);
            continue;
        }

        heapCollection ++;
    }
}

void BZCollectorCollect(int maxCollections)
{
    if(_BZCollectorBegin == 0x0)
    {
        printf("Called BZCollectorCollect() without initializing the collector first!");
        return;
    }



    BZHashTable *preserveTable = BZHashTableCreate(); // Stores the objects that are currently visible and need to be preserved
    BZHashTable *collectableTable = __BZCollectableHashTable();

    if(!collectableTable)
    {
        BZHashTableDestroy(preserveTable);
        return; // This is the fast path just in case no collectable is available at the moment.
    }


    int collected = 0;
    collectableTable->noCollapse = 1;

    uintptr_t collectionPoint = (uintptr_t)&collected;
    while(collectionPoint < _BZCollectorBegin)
    {
        // Travel through the stack, keep in mind that the stack grows from top to bottom, so we increment the collectionPoint!
        void **collection = (void **)collectionPoint;
        if(BZHashTableContains(collectableTable, *collection))
        {
            // If the pointer is visible, add it to the preserve table, so we don't delete it by accident
            BZCollectable *collectable = (BZCollectable *)BZHashTableDataForKey(collectableTable, *collection);
            BZHashTableInsert(preserveTable, collectable->pointer, collectable);

            BZCollectorTraceCollectable(collectable, collectableTable, preserveTable);
        }

        collectionPoint ++;
    }


    // Now that everything is set up, lets travel through the collectables and remove the dangling ones.
    int i;
    for(i=0; i<collectableTable->capacity; i++)
    {
        BZBucket *bucket = collectableTable->buckets[i];
        while(bucket)
        {
            if(bucket->key == NULL)
            {
                bucket = bucket->next;
                continue;
            }
            
            
            
            // Check if the pointer is dangling, that is, if the pointer is neither in the preserve table nor referenced by a static pointer.
            if(!BZHashTableContains(preserveTable, bucket->key) && !__BZCollectableIsStatic(bucket->key))
            {
                // Dangling pointer... cya
                BZCollectableRelease((BZCollectable *)bucket->data);
                collected ++;
            }
            
            bucket = bucket->next;
        }

        // If there was an collection limit set and we are above it, lets end it here!
        if(maxCollections > 0 && collected >= maxCollections)
            break;
    }

    collectableTable->noCollapse = 0;
    
    BZHashTableCollapseIfNeeded(collectableTable);
    BZHashTableDestroy(preserveTable);
}
