//
//  LFProtocol.h
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

#ifndef _LFPROTOCOL_H_
#define _LFPROTOCOL_H_

/**
 * @addtogroup LFRuntime
 * @{
 **/

#include "LFBase.h"

/**
 * Type used to represent a protocol method name
 * @sa LFProtocolImp
 * @sa LFProtocolHeader
 * @sa LFRuntimeProtocolBodySetIMP()
 **/
typedef const char* LFProtocolSel;
/**
 * Type used to represent a registered protocol.
 **/
typedef uint32_t LFProtocolID;

/**
 * Constant for an invalid protocol
 **/
#define kLFProtocolInvalidID 0

/**
 * Struct that defines a protocol implementation
 * @sa LFProtocolBody
 **/
typedef struct
{
	/**
	 * The selector implemented by the implementation
	 **/
	LFProtocolSel selector;
	/**
	 * The callback method that should be invoked when the selector is called
	 **/
	void *method;
} LFProtocolImp;


/**
 * Struct that defines a protocol description
 **/
typedef struct
{
	/**
	 * Version of the protocol
	 * @remark Should be set to 0
	 **/
	LFIndex version;
	/**
	 * Name of the protocol without any extra stuff
	 **/
	const char *name;
	
	/**
	 * Array with selectors defined by the protocol
	 **/
	LFProtocolSel *selectors;
	/**
	 * The number of selectors defined by the protocol
	 **/
	uint32_t selectorCount;
} LFProtocolHeader;

/**
 * Struct that defines a protocol implementation
 **/
typedef struct
{
	/**
	 * The protocol that is implemented
	 **/
	LFProtocolHeader *protocol;
	/**
	 * Pointer to the actual implementations, this is a array with n members, where n is the number of selectors the protocol defines
	 **/
	LFProtocolImp	 *imps;
} LFProtocolBody;

/**
 * Registers the protocol within the runtime
 * @remark Just like LFRuntimeRegisterClass(), this function won't copy the header data thus you must ensure that the memory of the struct is not freed as long as the protocol is registered
 **/
LF_EXPORT LFProtocolID LFRuntimeRegisterProtocol(const LFProtocolHeader *header);
/**
 * Unregisters a protocol. Please see LFRuntimeUnregisterClass() for general considerations about unregistering
 **/
LF_EXPORT void LFRuntimeUnregisterProtocol(LFProtocolID pid);

/**
 * Creates a new protocol body for the given protocol id
 **/
LF_EXPORT LFProtocolBody *LFRuntimeCreateProtocolBody(LFProtocolID pid);
/**
 * Destroys a protocol body.
 * @remark When assigning a protocol body to a bag, it won't copy the body itself so you must not destroy the body as long as it is registered with any bag!
 **/
LF_EXPORT void LFRuntimeDestroyProtocolBody(LFProtocolBody *body);
/**
 * Sets the implementation for the given selector
 * @param method The callback function that should be called when the selector is invoked
 * @sa LFProtocolMethod10()
 * @sa LFProtocolMethod20()
 * @sa LFProtocolMethod30()
 * @sa LFProtocolMethod11()
 * @sa LFProtocolMethod21()
 * @sa LFProtocolMethod31()
 **/
LF_EXPORT void LFRuntimeProtocolBodySetIMP(LFProtocolBody *body, LFProtocolSel selector, void *method);

/**
 * Returns true if the object implements the given protocol, otherwise false.
 **/
LF_EXPORT uint8_t LFImplementsProtocol(LFTypeRef ref, LFProtocolID pid);

/**
 * Returns true if the object implements the given selector.
 **/
LF_EXPORT uint8_t LFRespondsToSelector(LFTypeRef ref, LFProtocolSel selector);


#ifndef LF_TARGET_LITEC
/**
 * @cond
 **/
typedef void (*__LFProtocolSignature1_0)(LFTypeRef arg1);
typedef void (*__LFProtocolSignature2_0)(LFTypeRef arg1, LFTypeRef arg2);
typedef void (*__LFProtocolSignature3_0)(LFTypeRef arg1, LFTypeRef arg2, LFTypeRef arg3);

typedef LFTypeRef (*__LFProtocolSignature1_1)(LFTypeRef arg1);
typedef LFTypeRef (*__LFProtocolSignature2_1)(LFTypeRef arg1, LFTypeRef arg2);
typedef LFTypeRef (*__LFProtocolSignature3_1)(LFTypeRef arg1, LFTypeRef arg2, LFTypeRef arg3);
/**
 * @endcond
 **/

/**
 * Macro to cast a method with 1 argument and no return value to a void pointer
 * @sa LFRuntimeProtocolBodySetIMP()
 * @sa LFProtocolMethod11()
 **/
#define LFProtocolMethod10(method) (void *)((__LFProtocolSignature1_0)method)
/**
 * Macro to cast a method with 2 argument and no return value to a void pointer
 * @sa LFRuntimeProtocolBodySetIMP()
 * @sa LFProtocolMethod21()
 **/
#define LFProtocolMethod20(method) (void *)((__LFProtocolSignature1_0)method)
/**
 * Macro to cast a method with 3 argument and no return value to a void pointer
 * @sa LFRuntimeProtocolBodySetIMP()
 * @sa LFProtocolMethod31()
 **/
#define LFProtocolMethod30(method) (void *)((__LFProtocolSignature1_0)method)
/**
 * Macro to cast a method with 1 argument and a return value to a void pointer
 * @sa LFRuntimeProtocolBodySetIMP()
 * @sa LFProtocolMethod10()
 **/
#define LFProtocolMethod11(method) (void *)((__LFProtocolSignature1_0)method)
/**
 * Macro to cast a method with 2 argument and a return value to a void pointer
 * @sa LFRuntimeProtocolBodySetIMP()
 * @sa LFProtocolMethod20()
 **/
#define LFProtocolMethod21(method) (void *)((__LFProtocolSignature1_0)method)
/**
 * Macro to cast a method with 3 argument and a return value to a void pointer
 * @sa LFRuntimeProtocolBodySetIMP()
 * @sa LFProtocolMethod30()
 **/
#define LFProtocolMethod31(method) (void *)((__LFProtocolSignature1_0)method)
#else
#define LFProtocolMethod10(method) (void *)method
#define LFProtocolMethod20(method) (void *)method
#define LFProtocolMethod30(method) (void *)method
#define LFProtocolMethod11(method) (void *)method
#define LFProtocolMethod21(method) (void *)method
#define LFProtocolMethod31(method) (void *)method
#endif


/**
 * Invokes the given protocol selector on the object
 * @remark If the object doesn't implement the protocol that defines the selector, the method will try to invoke the superclass implementation.
 **/
LF_EXPORT void LFRuntimeInvoke0(LFTypeRef ref, LFProtocolSel selector);
/**
 * Invokes the given protocol selector on the object and passes the given argument
 * @remark If the object doesn't implement the protocol that defines the selector, the method will try to invoke the superclass implementation.
 **/
LF_EXPORT void LFRuntimeInvoke1(LFTypeRef ref, LFProtocolSel selector, LFTypeRef arg1);
/**
 * Invokes the given protocol selector on the object and passes all given argument
 * @remark If the object doesn't implement the protocol that defines the selector, the method will try to invoke the superclass implementation.
 **/
LF_EXPORT void LFRuntimeInvoke2(LFTypeRef ref, LFProtocolSel selector, LFTypeRef arg1, LFTypeRef arg2);
/**
 * Invokes the given protocol selector on the object by using its superclass or one of its superclasses implementation
 * @sa LFRuntimeInvoke0()
 **/
LF_EXPORT void LFRuntimeSuperInvoke0(LFTypeRef ref, LFProtocolSel selector);
/**
 * Invokes the given protocol selector on the object by using its superclass or one of its superclasses implementation
 * @sa LFRuntimeInvoke1()
 **/
LF_EXPORT void LFRuntimeSuperInvoke1(LFTypeRef ref, LFProtocolSel selector, LFTypeRef arg1);
/**
 * Invokes the given protocol selector on the object by using its superclass or one of its superclasses implementation
 * @sa LFRuntimeInvoke2()
 **/
LF_EXPORT void LFRuntimeSuperInvoke2(LFTypeRef ref, LFProtocolSel selector, LFTypeRef arg1, LFTypeRef arg2);

/**
 * Invokes the given protocol selector on the object
 * @remark If the object doesn't implement the protocol that defines the selector, the method will try to invoke the superclass implementation.
 **/
LF_EXPORT LFTypeRef LFRuntimeRInvoke0(LFTypeRef ref, LFProtocolSel selector);
/**
 * Invokes the given protocol selector on the object and passes the given argument
 * @remark If the object doesn't implement the protocol that defines the selector, the method will try to invoke the superclass implementation.
 **/
LF_EXPORT LFTypeRef LFRuntimeRInvoke1(LFTypeRef ref, LFProtocolSel selector, LFTypeRef arg1);
/**
 * Invokes the given protocol selector on the object and passes all given argument
 * @remark If the object doesn't implement the protocol that defines the selector, the method will try to invoke the superclass implementation.
 **/
LF_EXPORT LFTypeRef LFRuntimeRInvoke2(LFTypeRef ref, LFProtocolSel selector, LFTypeRef arg1, LFTypeRef arg2);

/**
 * Invokes the given protocol selector on the object by using its superclass or one of its superclasses implementation
 * @sa LFRuntimeRInvoke0()
 **/
LF_EXPORT LFTypeRef LFRuntimeSuperRInvoke0(LFTypeRef ref, LFProtocolSel selector);
/**
 * Invokes the given protocol selector on the object by using its superclass or one of its superclasses implementation
 * @sa LFRuntimeRInvoke1()
 **/
LF_EXPORT LFTypeRef LFRuntimeSuperRInvoke1(LFTypeRef ref, LFProtocolSel selector, LFTypeRef arg1);
/**
 * Invokes the given protocol selector on the object by using its superclass or one of its superclasses implementation
 * @sa LFRuntimeRInvoke2()
 **/
LF_EXPORT LFTypeRef LFRuntimeSuperRInvoke2(LFTypeRef ref, LFProtocolSel selector, LFTypeRef arg1, LFTypeRef arg2);

/**
 * @}
 **/

#endif
