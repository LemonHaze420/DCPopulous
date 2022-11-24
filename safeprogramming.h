#ifndef _SAFE_PROGRAMMING_H_
#define _SAFE_PROGRAMMING_H_

#include	"CompilerEnvironment.h"
#include	<windows.h>
#include	<stdlib.h>
#include	<stdio.h>

#ifdef _CPPRTTI	
	#include	<typeinfo.h>
#endif

#ifdef UNICODE

// ----
// Checks whether _ObjectRef is valid before using it!
// Outputs a debug message if the object is invalid.
#define SAFELY_CALL(_ObjectRef, _Method)															\
if (!_ObjectRef)																					\
{																									\
	TCHAR dbmsg[256];																				\
	swprintf(dbmsg, L"WARNING: Attempt to use NULL object at line \"%d\" in \"%hs\"\n", __LINE__,  __FILE__);	\
	OutputDebugString(dbmsg);																		\
}																									\
else																								\
	_ObjectRef->_Method;

// ----
// A nice little macro that checks whether an object 
// _ObjectRef is valid, calling a method if it is. 
// Also preforms a test on the return value.
// Outputs a debug message if the object is invalid.
#define SAFELY_CONTROL(_Control, _ObjectRef, _Method, _Test, _TrueState)							\
if (!_ObjectRef)																					\
{																									\
	TCHAR dbmsg[256];																				\
	swprintf(dbmsg, L"WARNING: Attempt to use NULL object at line \"%d\" in \"%hs\"\n", __LINE__,  __FILE__);	\
	OutputDebugString(dbmsg);																		\
}																									\
else																								\
	_Control (_ObjectRef->_Method _Test _TrueState)

// ----
// Prints out a string representation of the class data
#ifdef _CPPRTTI
#define SAFELY_PRINT_OBJECT(_ObjectRef)											\
if (_ObjectRef)																	\
{																				\
	TCHAR dbmsg[256];															\
	swprintf(dbmsg, L"Output of object %hs:\n", typeid(*_ObjectRef).name());	\
	OutputDebugString(dbmsg);												\
	OutputDebugString(_ObjectRef->ToString());									\
	OutputDebugString(TEXT("\n"));												\
}
#else
#define SAFELY_PRINT_OBJECT(_ObjectRef)											\
if (	(_ObjectRef)															\
	&&	(_ObjectRef->ToString())												\
	)																			\
{																				\
	OutputDebugString(TEXT("String representation of object:\n"));				\
	OutputDebugString(_ObjectRef->ToString());									\
	OutputDebugString(TEXT("\nEnd.\n"));										\
}
#endif

#else		// Non-UNICODE

// ----
// Checks whether _ObjectRef is valid before using it!
// Outputs a debug message if the object is invalid.
#define SAFELY_CALL(_ObjectRef, _Method)															\
if (!_ObjectRef)																					\
{																									\
	TCHAR dbmsg[256];																				\
	sprintf(dbmsg, "WARNING: Null object used at line \"%d\" in \"%hs\"\n", __LINE__,  __FILE__);	\
	OutputDebugString(dbmsg);																		\
}																									\
else																								\
	_ObjectRef->_Method;

// ----
// A nice little macro that checks whether an object 
// _ObjectRef is valid, calling a method if it is. 
// Also preforms a test on the return value.
// Outputs a debug message if the object is invalid.
#define SAFELY_CONTROL(_Control, _ObjectRef, _Method, _Test, _TrueState)							\
if (!_ObjectRef)																					\
{																									\
	TCHAR dbmsg[256];																				\
	sprintf(dbmsg, "WARNING: Attempt to use NULL object at line \"%d\" in \"%hs\"\n", __LINE__,  __FILE__);	\
	OutputDebugString(dbmsg);																		\
}																									\
else																								\
	_Control (_ObjectRef->_Method _Test _TrueState)

// ----
// Prints out a string representation of the class data
#ifdef _CPPRTTI
#define SAFELY_PRINT_OBJECT(_ObjectRef)										\
if (_ObjectRef)																\
{																			\
	TCHAR dbmsg[256];														\
	sprintf(dbmsg, "Output of object %hs:\n", typeid(*_ObjectRef).name());	\
	OutputDebugString(dbmsg);												\
	OutputDebugString(_ObjectRef->ToString());								\
	OutputDebugString("\n");												\
}

#else

#define SAFELY_PRINT_OBJECT(_ObjectRef)										\
if (_ObjectRef)																\
{																			\
	OutputDebugString("Object output:\n");									\
	OutputDebugString(_ObjectRef->ToString());								\
	OutputDebugString("\nEnd.\n");											\
}
#endif

#endif

// ----
// Returns the result of the method call, or NULL if the object is NULL.
#define SAFELY_RETURN(_ObjectRef, _Method)		\
(_ObjectRef?_ObjectRef->_Method:NULL)

#ifdef NOT_FINAL_BUILD
// ----
// Free the object, but only if it exists!
#define SAFELY_DELETE(_ObjectRef)								\
if (_ObjectRef)													\
{																\
/*	OutputDebugString(TEXT("DELETING "));		The compiler is crap and can't get this to work!				\
	TCHAR*	CI = _ObjectRef->GetCreationInfo();					\
	OutputDebugString(CI?CI:TEXT("Don't know\n"));				\
	OutputDebugString(TEXT("at line "));						\
	TCHAR dbmsg[512];											\
	swprintf(dbmsg, TEXT("%d in %hs\n"), __LINE__,  __FILE__);	\
	OutputDebugString(dbmsg);								*/	\
	delete _ObjectRef;											\
	_ObjectRef = NULL;											\
}
#else
// ----
// Free the object, but only if it exists!
#define SAFELY_DELETE(_ObjectRef)								\
if (_ObjectRef)													\
{																\
	delete _ObjectRef;											\
	_ObjectRef = NULL;											\
}
#endif

#ifdef NOT_FINAL_BUILD
#define SAFELY_DELETE_C2248(_ObjectRef)							\
if (_ObjectRef)													\
{																\
	OutputDebugString(TEXT("Deleting an object that caused C2248 compiler error (bug)."));\
	delete _ObjectRef;											\
	_ObjectRef = NULL;											\
}
#else
#define SAFELY_DELETE_C2248(_ObjectRef)							\
if (_ObjectRef)													\
{																\
	delete _ObjectRef;											\
	_ObjectRef = NULL;											\
}
#endif

// ----
// Free an array, but only if it exists!
#define SAFELY_DELETE_ARRAY(_ObjectRef)			\
if (_ObjectRef)									\
{												\
	delete[] _ObjectRef;						\
	_ObjectRef = NULL;							\
}


#ifdef NOT_FINAL_BUILD
// ----
// Create an object, and tell it where it was created.
#define NEW(_ObjectRef, _Constructor)																\
_ObjectRef = new _Constructor;																		\
if (_ObjectRef)																						\
{																									\
	_ObjectRef->SetCreation(#_ObjectRef, #_Constructor, __FILE__, __LINE__);													\
	TCHAR*	CI = _ObjectRef->GetCreationInfo();														\
	OutputDebugString(CI?CI:TEXT("I could tell you where I'm from, but I'd have to kill ya!\n"));	\
}																									\
else																								\
{																									\
	OutputDebugString(TEXT("Object creation FAILED!!!\n"));											\
}
#else
#define NEW(_ObjectRef, _Constructor)	_ObjectRef = new _Constructor;
#endif

#define NEW_C2248(_ObjectRef, _Constructor)	_ObjectRef = new _Constructor;


// ----
// Lifted from Microsoft example.
// Use NODEFAULT when the default case can not be reached. 
// This could speed up a switch statement.
//#ifdef _DEBUG
#define NODEFAULT	ASSERT(0)		
//#else
//#define NODEFAULT	__assume(0)
//#endif


#endif