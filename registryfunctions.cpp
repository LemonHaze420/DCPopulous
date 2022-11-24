#include "RegistryFunctions.h"
#include <stdio.h>
#ifdef REMOTE_REGISTRY_BUILD
	#include <Rapi.h>
#endif


// ----
// Validates that a key exists.
bool ValidateRegistry(	HKEY		_ExistingKey,	// Existing key or default root.
						CString*	_SubKey,		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
						CString*	_ValueName,		// Name of value to retrieve.
						DWORD		_ValueType)
{
	LONG			ret;				// Values returned from function calls.
	HKEY			KeyResult;			// Our key reference.
	DWORD			BufferLength;		// Size of the buffer.
	DWORD			ValueType;
	
	// Open the spefified key.
#ifdef _WIN32_WCE
	ret = RegOpenKeyEx(	_ExistingKey,			// Handle to open key.
						_SubKey->GetString(),	// Address of name of subkey to open.
						0,						// Reserved.
						0,						// Security access mask.	(Currently not supported)
						&KeyResult);			// Address of handle to open key.
#else
	#ifndef REMOTE_REGISTRY_BUILD
	ret = RegOpenKeyEx(	_ExistingKey,			// Handle to open key.
						_SubKey->GetString(),	// Address of name of subkey to open.
						0,						// Reserved.
						KEY_ALL_ACCESS,			// Security access mask.
						&KeyResult);			// Address of handle to open key.
	#else
	ret = CeRegOpenKeyEx(	_ExistingKey,			// Handle to open key.
							_SubKey->GetString(),	// Address of name of subkey to open.
							0,						// Reserved.
							0,						// Security access mask.
							&KeyResult);			// Address of handle to open key.
	#endif
#endif

 	if (SUCCEEDED(ret))
	{
		// Accertain the amount of space required...
		BufferLength = 0;
#ifndef REMOTE_REGISTRY_BUILD
		ret = RegQueryValueEx(	KeyResult,						// handle to key to query
								_ValueName->GetString(),		// address of name of value to query
								0,								// reserved
								&ValueType,						// address of buffer for value type
								NULL,							// address of data buffer
								(unsigned long*)&BufferLength);	// address of data buffer size
#else
		ret = CeRegQueryValueEx( KeyResult,						// handle to key to query
								_ValueName->GetString(),		// address of name of value to query
								0,								// reserved
								&ValueType,						// address of buffer for value type
								NULL,							// address of data buffer
								(unsigned long*)&BufferLength);	// address of data buffer size
#endif
		
		if (	(SUCCEEDED(ret))
			&&	(ValueType == _ValueType)
			)
		{
			return true;
		}
	}
	return false;
}


// ----
// Registry load / save routines.
LONG LoadFromRegistry(	HKEY		_ExistingKey,	// Existing key or default root.
						CString*	_SubKey,		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
						CString*	_ValueName,		// Name of value to retrieve.
						DWORD*		_ValueType,		// A pointer to a DWORD filled-in by the function.
						char**		_Value,			// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
						DWORD*		_DataLength)	// A pointer to a DWORD filled-in by the function that indicates the length of the data (in bytes) returned for _Value.
{
	LONG			ret;				// Values returned from function calls.
	HKEY			KeyResult;			// Our key reference.
	unsigned char*	Buffer;				// Buffer for data (the address of this buffer will be returned to the user).
	DWORD			BufferLength;		// Size of the buffer.
	
	// Open root key.
#ifdef _WIN32_WCE
	ret = RegOpenKeyEx(	_ExistingKey,			// Handle to open key.
						_SubKey->GetString(),	// Address of name of subkey to open.
						0,						// Reserved.
						0,						// Security access mask.	(Currently not supported)
						&KeyResult);			// Address of handle to open key.
#else
	#ifndef REMOTE_REGISTRY_BUILD
	ret = RegOpenKeyEx(	_ExistingKey,			// Handle to open key.
						_SubKey->GetString(),	// Address of name of subkey to open.
						0,						// Reserved.
						KEY_ALL_ACCESS,			// Security access mask.
						&KeyResult);			// Address of handle to open key.
	#else
	ret = CeRegOpenKeyEx(	_ExistingKey,			// Handle to open key.
							_SubKey->GetString(),	// Address of name of subkey to open.
							0,						// Reserved.
							0,						// Security access mask.
							&KeyResult);			// Address of handle to open key.
	#endif	
#endif

 	if (SUCCEEDED(ret))
	{
		// Accertain the amount of space required...
		BufferLength = 0;
#ifndef REMOTE_REGISTRY_BUILD
		ret = RegQueryValueEx(	KeyResult,						// handle to key to query
								_ValueName->GetString(),		// address of name of value to query
								0,								// reserved
								_ValueType,						// address of buffer for value type
								NULL,							// address of data buffer
								(unsigned long*)&BufferLength);	// address of data buffer size
#else
		ret = CeRegQueryValueEx( KeyResult,						// handle to key to query
								_ValueName->GetString(),		// address of name of value to query
								0,								// reserved
								_ValueType,						// address of buffer for value type
								NULL,							// address of data buffer
								(unsigned long*)&BufferLength);	// address of data buffer size
#endif

		if (SUCCEEDED(ret) && (BufferLength > 0))
		{
			// Allocate sufficient memory.
			Buffer = new unsigned char[BufferLength];
			if (Buffer)
			{
#ifndef REMOTE_REGISTRY_BUILD
				ret = RegQueryValueEx(	KeyResult,						// handle to key to query
										_ValueName->GetString(),		// address of name of value to query
										0,								// reserved
										_ValueType,						// address of buffer for value type
										Buffer,							// address of data buffer
										(unsigned long*)&BufferLength);	// address of data buffer size
#else
				ret = CeRegQueryValueEx( KeyResult,						// handle to key to query
										_ValueName->GetString(),		// address of name of value to query
										0,								// reserved
										_ValueType,						// address of buffer for value type
										Buffer,							// address of data buffer
										(unsigned long*)&BufferLength);	// address of data buffer size
#endif

				// Close key.
#ifndef REMOTE_REGISTRY_BUILD
				RegCloseKey(KeyResult);	
#else
				CeRegCloseKey(KeyResult);	
#endif

				if (SUCCEEDED(ret))
				{
					// ValueType is already assigned.
					*_Value			= (char*)Buffer;
					*_DataLength	= BufferLength;	
					return ret;
				}
				else
				{
					**_Value			= NULL;
					*_DataLength	= 0;
					delete[]		Buffer;
					Buffer			= NULL;
					return ret;
				}

				// Will have always returned by now.
			}
		}
		else
		{
#ifndef REMOTE_REGISTRY_BUILD
			RegCloseKey(KeyResult);	
#else
			CeRegCloseKey(KeyResult);	
#endif
			return ret;
		}
	}
	
	return ret;
}


// ----
LONG SaveToRegistry(HKEY		_ExistingKey,	// Existing key or default root.
					CString*	_SubKey,		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
					CString*	_ValueName,		// Name of the value to store.
					DWORD		_ValueType,		// Specifies the format for storing the data.
					char*		_Value,			// A pointer to the data to store.
					DWORD		_DataLength)	// Length of the _Value data, in bytes.
{
	LONG			ret;				// Values returned from function calls.
	HKEY			KeyResult;			// Our key reference.
	DWORD			KeyState;			// State of that key.
	
	// Create / Open key.
#ifdef _WIN32_WCE
	ret = RegCreateKeyEx(	_ExistingKey,			// handle to an open key
							_SubKey->GetString(),	// address of subkey name
							0,						// reserved
							TEXT("REG_SZ"),			// address of class string
							0,						// special options flag (no currently supported)
							0,						// desired security access mask (no supported)
							NULL,					// address of key security structure
							&KeyResult,				// address of buffer for opened handle
							&KeyState);				// address of disposition value buffer
#else
	#ifndef REMOTE_REGISTRY_BUILD
	ret = RegCreateKeyEx(	_ExistingKey,			// handle to an open key
							_SubKey->GetString(),	// address of subkey name
							0,						// reserved
							TEXT("REG_SZ"),			// address of class string
							REG_OPTION_NON_VOLATILE,// special options flag
							KEY_ALL_ACCESS,			// desired security access
							NULL,					// address of key security structure
							&KeyResult,				// address of buffer for opened handle
							&KeyState);				// address of disposition value buffer
	#else
	// Okay this build allows me to save information 
	// to a CE device attacted to the machine I'm using.
	ret = CeRegCreateKeyEx( _ExistingKey,			// handle to an open key
							_SubKey->GetString(),	// address of subkey name
							0,						// reserved
							TEXT("REG_SZ"),			// address of class string
							0,						// special options flag
							0,						// desired security access
							NULL,					// address of key security structure
							&KeyResult,				// address of buffer for opened handle
							&KeyState);
	#endif
#endif
	
	if (SUCCEEDED(ret))
	{
#ifndef REMOTE_REGISTRY_BUILD
		ret = RegSetValueEx(KeyResult,						// handle to key to set value for
							_ValueName->GetString(),		// name of the value to set
							0,								// reserved
							_ValueType,						// flag for value type
							(const unsigned char*)_Value,	// address of value data
							_DataLength);					// size of value data
#else
		ret = CeRegSetValueEx(KeyResult,						// handle to key to set value for
								_ValueName->GetString(),		// name of the value to set
								0,								// reserved
								_ValueType,						// flag for value type
								(unsigned char*)_Value,	// address of value data
								_DataLength);					// size of value data

#endif
		// Free handle.
#ifndef REMOTE_REGISTRY_BUILD
		RegCloseKey(KeyResult);	
#else
		CeRegCloseKey(KeyResult);	
#endif
		return ret;
	}
	return ret;
}


// ----
// Saves data to the registry.
LONG RemoveRegistryValue(	HKEY		_ExistingKey,	// Existing key or default root.
							CString*	_SubKey,		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							CString*	_ValueName)		// Name of the value to remove
{
	LONG			ret;				// Values returned from function calls.
	HKEY			KeyResult;			// Our key reference.
		
	// Open root
#ifdef _WIN32_WCE
	ret = RegOpenKeyEx(	_ExistingKey,			// Handle to open key.
						_SubKey->GetString(),	// Address of name of subkey to open.
						0,						// Reserved.
						0,						// Security access mask.	(Currently not supported)
						&KeyResult);			// Address of handle to open key.
#else
	#ifndef REMOTE_REGISTRY_BUILD
	ret = RegOpenKeyEx(	_ExistingKey,			// Handle to open key.
						_SubKey->GetString(),	// Address of name of subkey to open.
						0,						// Reserved.
						KEY_ALL_ACCESS,			// Security access mask.
						&KeyResult);			// Address of handle to open key.
	#else
	ret = CeRegOpenKeyEx(	_ExistingKey,			// Handle to open key.
							_SubKey->GetString(),	// Address of name of subkey to open.
							0,						// Reserved.
							0,						// Security access mask.
							&KeyResult);			// Address of handle to open key.
	#endif
#endif

 	if (SUCCEEDED(ret))
	{
#ifndef REMOTE_REGISTRY_BUILD
		ret = RegDeleteValue(	KeyResult, 
								_ValueName->GetString());
#else
		ret = CeRegDeleteValue(	KeyResult, 
								_ValueName->GetString());
#endif
	}

	// Close key.
#ifndef REMOTE_REGISTRY_BUILD
		RegCloseKey(KeyResult);	
#else
		CeRegCloseKey(KeyResult);	
#endif
	return ret;
}
