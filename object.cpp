#include	"Object.h"
#include	"SafeProgramming.h"
#include	<windows.h>


// Class variable
#ifndef ULTRA_FAST_OBJECT
DWORD		CObject::ObjectCount	= 0;
DWORD		CObject::MaxObjectCount	= 0;
CObject*	CObject::FirstObject	= NULL;
#endif

// ----
CObject::CObject()
{
	ErrorCode = 0;
#ifndef ULTRA_FAST_OBJECT
	StringRepresentation	= NULL;
	CreationInfo			= NULL;
#endif

#if	(!defined(FAST_OBJECT) && !defined(ULTRA_FAST_OBJECT))
	NextObject = NULL;

	if (ObjectCount == 0)
	{
		FirstObject	= this;
	}
	else
	{
		// Add to end of object chain. 
		CObject* i = FirstObject;
		while (i->NextObject)
		{
			// Check that we're not already in the list.
			if (this == i)
			{
				// Panic!!!
				OutputDebugString(TEXT("PANIC: Loop in object chain possible!\n"));
			}
			else
			{
				i = i->NextObject;
			}
		}
		i->NextObject = this;
	}
#endif

#ifndef ULTRA_FAST_OBJECT
	// Count object in...
	if (MaxObjectCount < (2 << 30))
		ObjectID = MaxObjectCount++;
	else
		ObjectID = 0;

	if (ObjectCount < (2 << 30))
		ObjectCount++;
#endif

}

// ----
CObject::~CObject()
{
#ifndef ULTRA_FAST_OBJECT
	// Release any memory used for string representation.

	SAFELY_DELETE_ARRAY(StringRepresentation);
	SAFELY_DELETE_ARRAY(CreationInfo);
#endif

#if (!defined(FAST_OBJECT) && !defined(ULTRA_FAST_OBJECT))
	if (FirstObject)
	{
		if (FirstObject == this)
		{
			FirstObject = FirstObject->NextObject;
		}
		else
		{
			CObject* i = FirstObject;
			while ((i->NextObject) && (i->NextObject != this))
			{
				i = i->NextObject;
			}

			if (i->NextObject)
				i->NextObject = NextObject;
			else
			{
				// Panic!!!
				OutputDebugString(TEXT("PANIC: Object not found in object chain!\n"));
			}
		}
	}
	else
	{
		// Somethings gone wrong here!
		OutputDebugString(TEXT("ERROR: No objects in chain, but object still exist!\n"));
	}
#endif

#ifndef ULTRA_FAST_OBJECT
	// Count the object out...
	ObjectCount--;
#endif
}

// ----
// Return the object count.
int CObject::GetObjectCount()
{
#ifndef ULTRA_FAST_OBJECT
	return ObjectCount;
#else
	return 0;
#endif
}

// ----
TCHAR*	CObject::GetCreationInfo()
{
#ifndef ULTRA_FAST_OBJECT
	return CreationInfo;
#else
	return NULL;
#endif
}


// ----
void	CObject::SetCreation(char* _VarName, char* _Constructor, char* _File, int _Line)
{
#ifndef ULTRA_FAST_OBJECT
	SAFELY_DELETE_ARRAY(CreationInfo);
	
#define OBJECT_STR	"Object #"
#define	CREATED_STR	" created at line "
#define INFILE_STR	" in file "

	// Build string.
	CreationInfo = new TCHAR[		strlen(OBJECT_STR) 
								+	10
								+	1
								+	strlen(_VarName)
								+	1
								+	strlen(_Constructor)
								+	1
								+	strlen(CREATED_STR) 
								+	10
								+	strlen(INFILE_STR) 
								+	strlen(_File)
								+	3 // . + \n + \0
								];	
	
	if (CreationInfo)
	{
#ifdef _UNICODE
		swprintf(	CreationInfo, 
					TEXT("%hs%d %hs %hs %hs%d%hs%hs.\n"), 
					OBJECT_STR, 
					ObjectID, 
					_VarName,
					_Constructor,
					CREATED_STR, 
					_Line, 
					INFILE_STR, 
					_File);

#else
		sprintf(CreationInfo, 
				"%s%d %s %s %s%d%s%s.\n", 
				OBJECT_STR, 
				ObjectID, 
				_VarName,
				_Constructor,
				CREATED_STR, 
				_Line, 
				INFILE_STR, 
				_File);
#endif
	}
#endif
}


// ----
void	CObject::OutputObjectList()
{
#ifndef ULTRA_FAST_OBJECT
	OutputDebugString(TEXT("\nStarting object list...\n"));
	CObject* i = FirstObject;
	while (i)
	{
		if (i->CreationInfo)
			OutputDebugString(i->CreationInfo);
		else
		{
			TCHAR buffer[256];
#ifdef _UNICODE
			swprintf(buffer, TEXT("Object #%d is of unknown origin (check how the object was created).\n"), i->ObjectID);
#else
			sprintf(buffer, TEXT("Object #%d is of unknown origin (check how the object was created).\n"), i->ObjectID);
#endif
			OutputDebugString(buffer);
		}
		i = i->NextObject;
	}
	OutputDebugString(TEXT("End of list.\n"));
#else
	OutputDebugString(TEXT("This build does not support object information.\nUndefine ULTRA_FAST_OBJECT, and optionally FAST_OBJECT.\n"));
#endif
}

// ----
// Returns an internally stored error code.
// The error code is particular to a given class. 
DWORD	CObject::GetErrorCode()
{
	DWORD ret = ErrorCode;
	ErrorCode = 0;

	return ret;
}


// ----
// Returns a string representation for the object's data.
// WARNING: This is only intended for debug use as it is gonna be slow!!!
TCHAR*	CObject::ToString()
{
#ifndef ULTRA_FAST_OBJECT
	SAFELY_DELETE_ARRAY(StringRepresentation);
	
	// Build string.
	char*	DataRep = new char[sizeof(*this) + 2];	
	
	if (DataRep)
	{
		memcpy((void*)DataRep, (void*)this, sizeof(*this));
		DataRep[sizeof(*this)] = 0;

#ifdef _UNICODE
		TCHAR*	LDataRep = new TCHAR[sizeof(*this) + 2];
		swprintf(LDataRep, L"%hs\n", DataRep);
		delete[] DataRep;
		StringRepresentation = LDataRep;
#else
		StringRepresentation = DataRep;
#endif
	}

	return StringRepresentation;
#else
	return NULL;
#endif
}
