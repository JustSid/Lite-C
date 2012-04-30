//
//  BZHashTable.h
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

#ifndef BZHASHTABLE_H_
#define BZHASHTABLE_H_

typedef struct BZBucket
{
    void *key;
    void *data;

    struct BZBucket *next;
} BZBucket;

typedef struct
{
    unsigned int count;
    unsigned int capacity;

    bool noCollapse;
    BZBucket **buckets;
} BZHashTable;


BZExtern BZHashTable *BZHashTableCreate();
BZExtern void BZHashTableDestroy(BZHashTable *table);

BZExtern void BZHashTableInsert(BZHashTable *table, void *key, void *data);
BZExtern void BZHashTableRemove(BZHashTable *table, void *key);
BZExtern char BZHashTableContains(BZHashTable *table, void *key);

BZExtern void *BZHashTableDataForKey(BZHashTable *table, void *key);

#endif
