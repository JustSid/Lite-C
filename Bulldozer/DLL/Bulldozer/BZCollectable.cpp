//
//  BZCollectable.c
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
#include "BZCollectable.h"
#include "BZHashTable.h"

static BZHashTable *BZCollectableHashTable = NULL;
static BZCollectable *BZStaticCollectables[BZCollectableMaxStaticCollectables];
static int BZStaticCollectableCount = 0;


BZHashTable *__BZCollectableHashTable()
{
    return BZCollectableHashTable;
}

// Checks if one of the static objects points to the pointer
char __BZCollectableIsStatic(void *pointer)
{
    int i;
    for(i=0; i<BZStaticCollectableCount; i++)
    {
        BZCollectable *collectable = BZStaticCollectables[i];
        void **staticPointer = (void **)collectable->pointer; // The collectable pointer is actually a pointer to a pointer to allow this to work...

        if(*staticPointer == pointer)
            return 1;
    }

    return 0;
}




BZCollectable *BZCollectableInsert(void *pointer, long size, BZCollectableFreeCallback freeCallback)
{
    if(BZCollectableHashTable)
    {
        if(BZHashTableContains(BZCollectableHashTable, pointer))
        {
            BZCollectable *collectable = (BZCollectable *)BZHashTableDataForKey(BZCollectableHashTable, pointer);
            collectable->count ++;

            return collectable;
        }
    }


    BZCollectable *collectable = (BZCollectable *)malloc(sizeof(BZCollectable));
    if(collectable)
    {
        collectable->pointer  = pointer;
        collectable->count    = 1;
        collectable->size     = size;
        collectable->callback = freeCallback;

        if(!BZCollectableHashTable)
            BZCollectableHashTable = BZHashTableCreate();

        BZHashTableInsert(BZCollectableHashTable, pointer, collectable);
    }

    return collectable;
}

BZCollectable *BZCollectableStatic(void **pointer, long size)
{
    // Create a new collectable!
    BZCollectable *collectable = (BZCollectable *)malloc(sizeof(BZCollectable));
    if(collectable)
    {
        collectable->pointer  = (void *)pointer;
        collectable->count    = 0; // Mark as static
        collectable->size     = size;
        collectable->callback = NULL;

        // Insert the collectable!
        BZStaticCollectables[BZStaticCollectableCount] = collectable;
        BZStaticCollectableCount ++;
    }

    return collectable;
}

BZCollectable *BZCollectableForPointer(void *pointer)
{
    if(BZCollectableHashTable)
    {
        if(BZHashTableContains(BZCollectableHashTable, pointer))
        {
            BZCollectable *collectable = (BZCollectable *)BZHashTableDataForKey(BZCollectableHashTable, pointer);
            return collectable;
        }
    }

    return NULL;
}



void BZCollectableReplace(void *pointer, void *newPointer, long newSize)
{
    if(BZCollectableHashTable)
    {
        BZCollectable *collectable = (BZCollectable *)BZHashTableDataForKey(BZCollectableHashTable, pointer);
        if(collectable)
        {
            collectable->pointer = newPointer; // Update the pointer
            collectable->size    = newSize;

            // And just re-insert the collectable...
            BZHashTableRemove(BZCollectableHashTable, pointer);
            BZHashTableInsert(BZCollectableHashTable, newPointer, collectable);
        }
    }
}

void BZCollectableRelease(BZCollectable *collectable)
{
    if(collectable)
    {
        if(collectable->count == 0)
            return; // The collectable is marked as static collectable, so we ignore it


        collectable->count --;
        if(collectable->count == 0)
        {
            // The use count dropped to zero, time to get rid of that object!
            BZHashTableRemove(BZCollectableHashTable, collectable->pointer);
            collectable->callback(collectable->pointer);

            free(collectable);
        }
    }
}
