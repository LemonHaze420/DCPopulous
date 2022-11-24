#ifndef	_REGISTRYFUNCTIONS_H_
#define	_REGISTRYFUNCTIONS_H_

#include	<windows.h>
#include	"String.h"

// Validates that a key exists.
bool PCValidateRegistry(HKEY		_ExistingKey,	// Existing key or default root.
						CString*	_SubKey,		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
						CString*	_ValueName,		// Name of value to retrieve.
						DWORD		_ValueType);


// Registry load / save routines.
// Both functions return the values returned by the calls to the registry. 
// Zero indicates success, and a non-zero value indicates an error.
LONG LoadFromRegistry(	HKEY		_ExistingKey,	// Existing key or default root.
						CString*	_SubKey,		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
						CString*	_ValueName,		// Name of value to retrieve.
						DWORD*		_ValueType,		// A pointer to a DWORD filled-in by the function.
						char**		_Value,			// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
						DWORD*		_DataLength);	// A pointer to a DWORD filled-in by the function that indicates the length of the data (in bytes) returned for _Value.

LONG SaveToRegistry(	HKEY		_ExistingKey,	// Existing key or default root.
						CString*	_SubKey,		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
						CString*	_ValueName,		// Name of the value to store.
						DWORD		_ValueType,		// Specifies the format for storing the data.
						char*		_Value,			// A pointer to the data to store.
						DWORD		_DataLength);	// Length of the _Value data, in bytes.

LONG RemoveRegistryValue(	HKEY		_ExistingKey,	// Existing key or default root.
							CString*	_SubKey,		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							CString*	_ValueName);	// Name of the value to remove

#endif