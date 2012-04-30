//
//  LFRuntime.h
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

#ifndef _LFRUNTIME_H_
#define _LFRUNTIME_H_

/**
 * @defgroup LFRuntime The runtime
 * @{
 * @brief The Lite Foundation runtime builds the core functionality used by Lite Foundation
 *
 * The runtime provides low-level API for creating and using classes. It also provides the memory management for instances.
 * <br />
 * <br />
 * Normally there is little need to use the Lite Foundation runtime directly, however, there are a few functions you should know that are implemented directly in the runtime,
 * these functions are LFRelease(), LFRetain(), LFCopy(), LFHash() and LFEqual().
 * However, if you want to extend Lite Foundation by building you own classes or protocols, you should be familiar with the runtimes functions and how it works internally.
 * How the runtime works and how to extend it is covered in the runtime guide that can be found <a href="http://widerwille.com/litefoundation/runtime.pdf">here</a>
 * <br />
 * <br />
 * The runtime is implemented in the following files: LFRuntime.h, LFRuntime.c, LFBase.h, LFBase.c, LFProtocol.h, LFProtocol.c, LFInternal.h, LFInternal.c, LFDebug.h, LFDebug.c, LFDebugInternal.h
 * @}
 **/

/**
 * @addtogroup LFRuntime
 * @{
 **/

#include "LFBase.h"
#include "LFProtocol.h"
#include "LFDebug.h"

/**
 * A protocol bag is a container for protocol bodies that can be attached to runtime classes
 * @remark The protocol bag functions don't copy the protocol bodies!
 * @sa LFRuntimeCreateProtocolBag()
 * @sa LFRuntimeDestroyProtocolBag()
 * @sa LFRuntimeAppendProtocol()
 **/
typedef struct
{
	/**
	 * Array of bodys, the size of the array is always bodyCount * sizeof(LFProtocolBody *)
	 **/
	LFProtocolBody **bodys;
	/**
	 * The length of the body array
	 **/
	uint32_t bodyCount;
} LFProtocolBag;

/**
 * Creates a new protocol bag and returns it
 **/
LF_EXPORT LFProtocolBag *LFRuntimeCreateProtocolBag();
/**
 * Destroys a protocol bag
 * @remark Make sure that the bag isn't used, eg. that the class that holds it is unregistered.
 * @sa LFRuntimeUnregisterClass()
 **/
LF_EXPORT void LFRuntimeDestroyProtocolBag(LFProtocolBag *bag);
/**
 * Adds the protocol body into the bag
 * @remark A protocol body can be added only once and can't be removed anymore.
 * @remark The function won't copy the body, so make sure that the memory area stays allocated during the bags lifetime
 * @remark Once added to a bag, a body shouldn't be altered anymore
 **/
LF_EXPORT void LFRuntimeAppendProtocol(LFProtocolBag *bag, LFProtocolBody *body);

/**
 * A constant value to mar an invalid type ID. The runtime won't hand out this ID for any class, so its safe to use it to initalize type IDs.
 **/
#define kLFInvalidTypeID 0

/**
 * A class description
 * @remark Either set the members to their appropriate values, or assign NULL to them!
 **/
typedef struct
{
	/**
	 * The version of the class, this is currently 1
	 * @remark Classes that define a version of 0 will automatically get their superClass member overwritten to kLFInvalidTypeID for backward compatibility.
	 **/
	LFIndex version;
	/**
	 * The name of the class.
	 * @remark This field is only for the name, no extra strings or infos!
	 **/
	const char *name;
#ifndef LF_TARGET_LITEC
	/**
	 * Invoked when the runtime creates a new instance of the class. The callback should initialize all needed values.
	 * @remark If the class defines a superclass, the runtime will first call all superclasses init callbacks before calling this callback!
	 **/
	void (*init)(LFTypeRef ref);
	/**
	 * Invoked when the runtime is asked to create a copy of an object. The returned value must be a valid copy or NULL.
	 **/
    LFTypeRef (*copy)(LFTypeRef ref);
	/**
	 * Invoked when the runtime is about to purge the object from memory. 
	 * The callback should free all allocated memory.
	 * @remark The function will call all superclasses dealloc callbacks after it called this callback!
	 **/
    void (*dealloc)(LFTypeRef ref);

	/**
	 * Called when the runtime is asked if two objects are equal
	 * The callback must return either true or false.
	 **/
    uint8_t (*equal)(LFTypeRef refa, LFTypeRef refb);
	/**
	 * Invoked when the runtime is asked for the hash of an object. 
	 * A hash function should be rather fast, there can be collisions in you hash even if the object has less than 4294967295 states. 
	 * However, the result needs to be the very same at any time, given that the object hasnʼt changed its content. Two equal objects are supposed to generate the very same hash!
	 **/
    LFHashCode  (*hash)(LFTypeRef ref);
#else
	void init(LFTypeRef ref);
	LFTypeRef copy(LFTypeRef ref);
    void dealloc(LFTypeRef ref);
	
    uint8_t equal(LFTypeRef refa, LFTypeRef refb);
    LFHashCode  hash(LFTypeRef ref);
#endif
	
	/**
	 * The classes super class ID.
	 **/
	LFTypeID superClass;
	
	/**
	 * The protocol bag attached to the class
	 **/
	LFProtocolBag *protocolBag;
} LFRuntimeClass;


/**
 * Registers a class within the runtime
 * @return The type ID for the class which can be used to create instances of the class. This is not a constant value! If the function fails, it will return kLFInvalidTypeID
 **/
LF_EXPORT LFTypeID LFRuntimeRegisterClass(const LFRuntimeClass *cls);
/**
 * Returns the class description associated with the given type ID, or NULL.
 **/
LF_EXPORT const LFRuntimeClass *LFRuntimeGetClassWithID(LFTypeID tid);
/**
 * Unregisters the class from the runtime
 * @remark If its a good idea to unregister a class or not is not the runtimes decision, however, please make sure that there is no living instance of the class!
 **/
LF_EXPORT void LFRuntimeUnregisterClass(LFTypeID tid);

/**
 * @cond
 **/
#define kLFRuntimeTrackedBit		4
#define kLFRuntimeMissedDeallocBit	3
#define kLFRuntimeZombieBit			2
#define kLFRuntimeNoRetainBit		1

struct __LFRuntimeTableBucket
{
    char *key;
    void *data;
    
    struct __LFRuntimeTableBucket *next;
};

typedef struct
{
    LFIndex bucketCount;
    LFIndex count;
    struct __LFRuntimeTableBucket **buckets;
} __LFRuntimeTable;

typedef struct
{
    LFObserverGenericCallback callback;
    LFRuntimeObserverType type;
    
    char *key;
} __LFRuntimeKVObserver;
/**
 * @endcond
 **/

/**
 * The base struct for every Lite Foundation class. A class struct must add this at the very first place, so that the runtime can work with the object
 * @remark The structs content remains undocumented as it might change from release to release. You must not edit any value!
 * @remark Binary compatibility for this class isn't guaranteed.
 **/
typedef struct
{
	/**
	 * @cond
	 **/
	uint32_t __info;
    uint32_t __size;
	uint16_t __rc;
    uint8_t __locks;
	
    __LFRuntimeTable *__kvoTable;
    __LFRuntimeKVObserver **__kvoList;
    LFIndex __kvoCount;
    
#ifdef LF_TARGET_OSX
	pthread_t __owner; // Only valid when locked and in multithreaded environment!
#endif
#ifdef LF_TARGET_WIN32
	DWORD __owner;
#endif
	/**
	 * @endcond
	 **/
} LFRuntimeBase;

/**
 * Creates a new instance of the class with the given ID.
 * @param tid A valid type ID of a class registered within the runtime
 * @param bytes The number of bytes the runtime should allocate for the instance, this is usally sizeof(class struct)
 **/
LF_EXPORT LFTypeRef LFRuntimeCreateInstance(LFTypeID tid, size_t bytes);
/**
 * Changes the given objects type ID
 * @param ref Class instance created by the runtime
 * @param tid The new type ID of the object, if the id is unknown to the runtime, the function won't do anything
 * @remark Changing an objects ID doesn't alter the object itself but the information stored about it within the runtime. This is useful for eg. creating a class that can turn into a fault.
 **/
LF_EXPORT void LFRuntimeSetInstanceID(LFTypeRef ref, LFTypeID tid);

LF_EXPORT char *LFRuntimeGetClassName(LFTypeRef ref);
LF_EXPORT const LFRuntimeClass *LFRuntimeGetClassWithName(char *name);

/**
 * Returns the objects type ID, or, if the object is invalid kLFInvalidTypeID
 **/
LF_EXPORT LFTypeID LFGetTypeID(LFTypeRef ref);
/**
 * Retains the object, this means, incrementing its reference count by one.
 * @remark Read the getting started guide for more informations <a href="http://widerwille.com/litefoundation/started.pdf">here</a>
 * @remark The function returns the object to allow its use in nested function calls
 **/
LF_EXPORT LFTypeRef LFRetain(LFTypeRef ref);
/**
 * Releases the object, this means, decrementing its reference count by one.
 * If the reference count hits zero, the runtime will invoke the dealloc callback of the objects class and then free the objects memory
 * @remark Read the getting started guide for more informations <a href="http://widerwille.com/litefoundation/started.pdf">here</a>
 **/
LF_EXPORT void LFRelease(LFTypeRef ref);
/**
 * Returns the retain count aka reference count of the object, or 0 if the object is invalid
 * @remark Do not depend on the returned value as the runtime or other parts of Lite Foundation might retain objects
 **/
LF_EXPORT LFIndex LFGetRetainCount(LFTypeRef ref);

/**
 * Returns true if both objects are equal, otherwise false
 * @remark The function invokes the equal callback of the objects class if both objects derived from the same class.
 **/
LF_EXPORT uint8_t LFEqual(LFTypeRef refa, LFTypeRef refb);
/**
 * Returns the hash of the object. The hash is generated either by calling the objects class hash function, if any provided, or by transforming the objects pointer value into a hash
 * @remark There are certainly collisions given the range of possible hashes, however, a object is supposed to generate the same hash as long as its content isn't mutated.
 **/
LF_EXPORT LFHashCode LFHash(LFTypeRef ref);
/**
 * Creates a copy of the object
 * @remark This function invokes the objects class copy callback and asks it for a copy, if no callback is provided, the function will return NULL
 * @remark A copy can be deep or shallow, depending on the implementation, Lite Foundations classes always create a deep copy, but other classes might create only a shallow copy.
 **/
LF_EXPORT LFTypeRef LFCopy(LFTypeRef ref);
/**
 * Returns true if the object is a instance of the given type, otherwise false
 * @remark The function will also return false if the object is invalid
 **/
LF_EXPORT uint8_t LFIsKindOf(LFTypeRef ref, LFTypeID tid);

/**
 * Locks the given object from access from other threads
 * @remark The lock is implemented as recursive lock
 **/
LF_EXPORT void LFRuntimeLock(LFTypeRef ref);
/**
 * Unlocks the given object
 * @remark The lock is implemented as recursive lock
 **/
LF_EXPORT void LFRuntimeUnlock(LFTypeRef ref);

/**
 * @}
 **/

#endif
