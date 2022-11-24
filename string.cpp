#include	"SafeProgramming.h"
#include	"Object.h"
#include	"String.h"
#include	<windows.h>
#include	<string.h>

// Macros
#ifdef _UNICODE
	#define	FINDINDEX(_a, _b)		((_a != NULL)?(wcschr(_a, _b)):NULL)
#else
	#define	FINDINDEX(_a, _b)		((_a != NULL)?(strchr(_a, _b)):NULL)
#endif

// ----
CString::CString()
{
#ifdef DYNAMIC_CSTRING
	Buffer = NULL;
#else
	*Buffer = 0;
#endif
}

CString::CString(char* _String)
{
#ifdef DYNAMIC_CSTRING
	Buffer = new TCHAR[strlen(_String) + 10];
#endif

#ifdef _UNICODE
	swprintf(Buffer, TEXT("%hs"), _String);
#else
	strcpy(Buffer, _String);
#endif
}

CString::CString(CString* _String)
{
	if (	(_String) 
		&&	(_String->GetString())
		)
	{
#ifdef DYNAMIC_CSTRING
		Buffer = new TCHAR[_String->GetStringLength() + 10];
#endif

#ifdef _UNICODE
		wcscpy(Buffer, _String->GetString());
//		wsprintf(Buffer, TEXT("%s"), _String->GetString());
#else
		strcpy(Buffer, _String->GetString());
		//sprintf(Buffer, TEXT("%s"), _String->GetString());
#endif
		//memcpy((void*)Buffer, (void*)_String->GetString(), _String->GetStringSize());
	}
}

// ----
CString::~CString()
{
#ifdef DYNAMIC_CSTRING
	if ((((int)Buffer) % 2) == 1)
	{
		// Time to start swearing.
		OutputDebugString(TEXT("CString::~CString. Major problem!\n"));
	}
	else
	{
		SAFELY_DELETE_ARRAY(Buffer);
	}
#endif
}


// ----
// Release memory.
void	CString::Clear()
{
#ifdef DYNAMIC_CSTRING
	SAFELY_DELETE_ARRAY(Buffer);
#else
	*Buffer = 0;
#endif
}

// ----
// Reads in the content of a file. (Not UNICODE!!!)
// Return value indicates number of bytes read. (I don't know how the file was opened [text/binary])
int		CString::LoadString(FILE* _FH, int _ReadLength)
{
#ifdef DYNAMIC_CSTRING
	// Delete any existing string data
	SAFELY_DELETE_ARRAY(Buffer);
#else
	*Buffer = 0;
#endif

	long	ReadLength = _ReadLength;

	// Calculate length for reading.
	if (ReadLength == 0)
	{
		long oldpos;
		oldpos = ftell(_FH);					// Record start pos
		fseek(_FH, 0, SEEK_END);				// Seek end
		ReadLength = ftell(_FH) - oldpos;		// Calc difference
		fseek(_FH, oldpos, SEEK_SET);			// Go back to original point
	}

	if (ReadLength > 0)
	{
		char*	tempString = new char[ReadLength + 10];		// Terminating null.
		memset((void*)tempString, 0, ReadLength + 10);		// Clear all to null.
		ReadLength = fread(tempString, ReadLength, 1, _FH);

		*this = tempString;
		delete[] tempString;
	}
	else
	{
		ReadLength = 0;
	}

	return ReadLength;
}

// ----
// Reads in the content of a file. (Not UNICODE!!!)
// Return value indicates number of bytes read. (I don't know how the file was opened [text/binary])
int		CString::LoadEncodedString(FILE* _FH, int _ReadLength)
{
#ifdef DYNAMIC_CSTRING
	// Delete any existing string data
	SAFELY_DELETE_ARRAY(Buffer);
#else
	*Buffer = 0;
#endif

	long	ReadLength = _ReadLength;

	// Calculate length for reading.
	if (ReadLength == 0)
	{
		long oldpos;
		oldpos = ftell(_FH);					// Record start pos
		fseek(_FH, 0, SEEK_END);				// Seek end
		ReadLength = ftell(_FH) - oldpos;		// Calc difference
		fseek(_FH, oldpos, SEEK_SET);			// Go back to original point
	}

	if (ReadLength > 0)
	{
		char*	tempString = new char[ReadLength + 10];		// Terminating null.
		memset((void*)tempString, 0, ReadLength + 10);		// Clear all to null.
		fread(tempString, ReadLength, 1, _FH);

		// Loop through the data unencrypting it (if you can really call it encryption).
		for (int i = ReadLength; i--;)
		{
			*(tempString + i) = (*(tempString + i)) ^ 0xFF; // Really complicated encryption.
		}

		*this = tempString;
		delete[] tempString;
	}
	else
	{
		ReadLength = 0;
	}

	return ReadLength;
}

// ----
// Copies the string across exactly as is, without any
// fancy character conversion.
void	CString::AssignAsData(char*	_String)
{
	int length;
	TCHAR*	Index;
	
	// Free old string always.
	SAFELY_DELETE_ARRAY(Buffer);

	// Validate address.
	if (!_String)
		return;
	length = strlen(_String);
	
	// No length.
	if (!length)
		return;

	// Create new buffer
	Buffer = new TCHAR[length + 1];
	Index = Buffer;

	// Loop through copying data from one string directly to the other.
	for (length; length--;)
	{
		*Index = (unsigned char)*_String;
		Index++;
		_String++;
	}
	*Index = 0;
}


// ----
// Overload inherited method
TCHAR*	CString::ToString()
{
	return Buffer;
}


// ----
TCHAR*	CString::GetString()
{
	return Buffer;
}

// ----
UINT	CString::GetStringLength()
{
#ifdef DYNAMIC_CSTRING
	if (Buffer)
	{
#endif

#ifdef _UNICODE
		return wcslen(Buffer);
#else
		return strlen(Buffer);
#endif

#ifdef DYNAMIC_CSTRING
	}
	return 0;
#endif
}

// ----
// Returns the size of the string in bytes.
UINT	CString::GetStringSize()
{
	return ((GetStringLength() + 1) * sizeof(TCHAR));
}

#ifdef INCLUDE_UNFINISHED_CODE
// ----
// Returns true if the string contains one, or more, 
// new line characters.
bool	CString::DoesStringContainNewLine()
{
	if (Buffer)
		return (*Buffer == '\n');  //return (FINDINDEX(Buffer, '\n'));
	else
		return false;
}
#endif

// ----
// Simply remove the last character from the string. (Bit of a quick fix)
void	CString::DeleteLastChar()
{
	int Index;

#ifdef DYNAMIC_CSTRING
	if (Buffer)
	{
#endif
#ifdef _UNICODE
		Index = wcslen(Buffer);
#else
		Index = strlen(Buffer);
#endif
		if (Index >= 1)
			*(Buffer + Index - 1) = 0;
#ifdef DYNAMIC_CSTRING
	}
#endif
}

#ifdef INCLUDE_UNFINISHED_CODE
// ----
// Takes the first word from the object string and 
// moves it into the _IntoHere object.
void	CString::MoveFirstWord(CString*	_IntoHere, char	_Delimiter)
{
	// The two other methods can do this combined.
	CopyFirstWord(_IntoHere, _Delimiter);
	DeleteFirstWord(_Delimiter);
}

// ----
// Copies the first word of the object string into the
// referenced object.
void	CString::CopyFirstWord(CString*	_IntoHere, char	_Delimiter)
{
	if (!_IntoHere)
		return;

	if (!Buffer)
	{
		SAFELY_DELETE_ARRAY(_IntoHere->Buffer);	// Free the other buffer's memory.
		return;
	}

	// Copy entire string.
	*_IntoHere = Buffer;

	// Then remove everything after the first delimiter.
	TCHAR*	Index1 = FINDINDEX(_IntoHere->Buffer, '\n');
	TCHAR*	Index2 = FINDINDEX(_IntoHere->Buffer, ' ');

	if ((Index1) && (Index1 < Index2))
		*Index1 = 0;
	if ((Index2) && (Index2 < Index1))
		*Index2 = 0;
}
	
// ----
// Removes the first word of the buffer, including 
// trailing _Delimiter.
void	CString::DeleteFirstWord(char	_Delimiter)
{
	CString TempBuffer;

	if (!Buffer)
		return;

	TCHAR*	Index =NULL;
	TCHAR*	Index1 = FINDINDEX(Buffer, '\n');
	TCHAR*	Index2 = FINDINDEX(Buffer, ' ');

	// Find which is the first delimiter
	if ((Index1) && (Index1 < Index2))
		Index = Index1;
	if ((Index2) && (Index2 < Index1))
		Index = Index1;

	if (Index)
	{
		*Index = 0;
		Index++;
		if (*Index)
		{
			// Can't directly copy the buffer as the equals
			// operator will free the memory before trying 
			// to access it in the assignment - thus need 
			// to make a copy first.

			// Copy the remaining buffer.
			TempBuffer = Index;

			// Copy it back again
			*this = TempBuffer;
		}
		else
		{
			SAFELY_DELETE_ARRAY(Buffer);
		}
	}
	else
	{
		SAFELY_DELETE_ARRAY(Buffer);
	}
}
#endif

#ifdef _UNICODE			// Unicode only method
// ----
void CString::operator= (WCHAR* _String)
{
#ifdef DYNAMIC_CSTRING
	SAFELY_DELETE_ARRAY(Buffer);
#else
	*Buffer = 0;
#endif

	if (	(_String) 
		&&	(wcslen(_String) > 0)
		)
	{
#ifdef DYNAMIC_CSTRING
		Buffer = new TCHAR[wcslen(_String) + 10];
#endif
		wcscpy(Buffer, _String);
	}
}
#endif

// ----
void CString::operator= (char* _String)
{
#ifdef DYNAMIC_CSTRING
	SAFELY_DELETE_ARRAY(Buffer);
#endif

	if (	(_String) 
		&&	(strlen(_String) > 0)
		)
	{
#ifdef DYNAMIC_CSTRING
		Buffer = new TCHAR[strlen(_String) + 10];
#endif

#ifdef _UNICODE
		swprintf(Buffer, TEXT("%hs"), _String);
#else
		strcpy(Buffer, _String);
#endif
	}
}

// ----
void CString::operator= (CString* _String)
{
#ifdef DYNAMIC_CSTRING
	SAFELY_DELETE_ARRAY(Buffer);
#endif

	if (	(_String) 
		&&	(_String->GetString())
		)
	{
#ifdef DYNAMIC_CSTRING
		Buffer = new TCHAR[_String->GetStringLength() + 10];
#endif
		
#ifdef _UNICODE
		wcscpy(Buffer, _String->GetString());
		//wsprintf(Buffer, TEXT("%s"), _String->GetString());
#else
		strcpy(Buffer, _String->GetString());
		//sprintf(Buffer, TEXT("%s"), _String->GetString());
#endif
	// Older method.
	//	memcpy((void*)Buffer, (void*)_String->GetString(), _String->GetStringSize());
	}
}
//TCHAR* operator= (WCHAR* _String);

// ----
void	CString::operator= (CString& _String)
{
#ifdef DYNAMIC_CSTRING
	SAFELY_DELETE_ARRAY(Buffer);
#endif

	if	(_String.GetStringLength() > 0)
	{
#ifdef DYNAMIC_CSTRING
		Buffer = new TCHAR[_String.GetStringLength() + 10];
#endif

#ifdef _UNICODE
		wcscpy(Buffer, _String.GetString());
		//wsprintf(Buffer, TEXT("%s"), _String.GetString());
#else
		strcpy(Buffer, _String.GetString());
		//sprintf(Buffer, TEXT("%s"), _String.GetString());
#endif

		//memcpy((void*)Buffer, (void*)_String.GetString(), _String.GetStringSize());
	}
}


// ----
void CString::operator+= (char* _String)
{
	if (	(_String) 
		&&	(strlen(_String) > 0)
		)
	{
#ifdef DYNAMIC_CSTRING
		TCHAR* Temp = new TCHAR[GetStringLength() + strlen(_String) + 10];
	#ifdef _UNICODE
		swprintf(Temp, TEXT("%s%hs"), Buffer, _String);
	#else
		sprintf(Temp, TEXT("%s%hs"), Buffer, _String);
	#endif
		SAFELY_DELETE_ARRAY(Buffer);
		Buffer = Temp;

#else	// Static
	#ifdef _UNICODE
		swprintf(Buffer, TEXT("%s%hs"), Buffer, _String);
	#else
		sprintf(Buffer, TEXT("%s%hs"), Buffer, _String);
	#endif
#endif
	}
}

// ----
void CString::operator+= (CString* _String)
{
	if (	(_String)
		&&	(_String->GetStringLength() > 0)
		)
	{
#ifdef DYNAMIC_CSTRING
		TCHAR* Temp = new TCHAR[GetStringLength() + _String->GetStringLength() + 10];
		if (Temp)
		{
			*Temp = 0;

			if (Buffer)
			{
#ifdef _UNICODE
				wcscpy(Temp, Buffer);
#else
				strcpy(Temp, Buffer);
#endif
				SAFELY_DELETE_ARRAY(Buffer);
			}
			Buffer = Temp;
			

#ifdef _UNICODE
			wcscat(Buffer, _String->GetString());
			//wsprintf(Temp, TEXT("%s%s"), Buffer, _String->GetString());
			//SAFELY_DELETE_ARRAY(Buffer);
			//Buffer = Temp;		
#else
			strcat(Buffer, _String->GetString());
			//sprintf(Temp, TEXT("%s%s"), Buffer, _String->GetString());
			//SAFELY_DELETE_ARRAY(Buffer);
			//Buffer = Temp;
#endif
		}
#else	// Static
		if (*Buffer)
		{
#ifdef _UNICODE
			wcscat(Buffer, _String->GetString());
			//wsprintf(Buffer, TEXT("%s%s"), Buffer, _String->GetString());

#else
			strcat(Buffer, _String->GetString());
			//sprintf(Buffer, TEXT("%s%s"), Buffer, _String->GetString());
#endif
		}
		else
		{
#ifdef _UNICODE
			wcscpy(Buffer, _String->GetString());
			//wsprintf(Buffer, TEXT("%s"), _String->GetString());
#else
			strcpy(Buffer, _String->GetString());
			//sprintf(Buffer, TEXT("%s"), _String->GetString());
#endif
		}
#endif
	}
}

// ----
void CString::operator+= (CString& _String)
{
	if (_String.GetStringLength() > 0)
	{
#ifdef DYNAMIC_CSTRING
		TCHAR* Temp = new TCHAR[GetStringLength() + _String.GetStringLength() + 1];
		if (Temp)
		{
			*Temp = 0;

			if (Buffer)
			{
#ifdef _UNICODE
				wcscpy(Temp, Buffer);
#else
				strcpy(Temp, Buffer);
#endif
				SAFELY_DELETE_ARRAY(Buffer);
			}
			Buffer = Temp;

#ifdef _UNICODE
			wcscat(Buffer, _String.GetString());
			//wsprintf(Temp, TEXT("%s%s"), Buffer, _String.GetString());
			//SAFELY_DELETE_ARRAY(Buffer);
			//Buffer = Temp;
#else
			strcat(Buffer, _String.GetString());
			//sprintf(Temp, "%s%s", Buffer, _String.GetString());
			//SAFELY_DELETE_ARRAY(Buffer);
			//Buffer = Temp;
#endif
		}
#else	// Static
		
		if (*Buffer)
		{
#ifdef _UNICODE
			wcscat(Buffer, _String.GetString());
			//wsprintf(Buffer, TEXT("%s%s"), Buffer, _String.GetString());
#else
			strcat(Buffer, _String.GetString());
			//sprintf(Buffer, TEXT("%s%s"), Buffer, _String.GetString());
#endif
		}
		else
		{
#ifdef _UNICODE
			wcscpy(Buffer, _String.GetString());
			//wsprintf(Buffer, TEXT("%s"), _String.GetString());
#else
			strcpy(Buffer, _String.GetString());
			//sprintf(Buffer, TEXT("%s"), _String.GetString());
#endif
		}
#endif
	}
}

// ----
bool	CString::operator== (char* _StringRight)
{
#ifdef DYNAMIC_CSTRING
	// Two zero length strings are the same, okay!!!
	if (	(!Buffer) 
		&&	(!_StringRight))
	{
		return true;
	}

	if (!Buffer)
		return false;

	if (!_StringRight)
		return false;

	// Both buffers should at least contain some data at this point.

	// Return false if not the same lenght.
	if (strlen(_StringRight) != GetStringLength())
		return false;

	// Now need a more complicated test on the actual data.
#ifdef _UNICODE
	bool ret = false;
	CString* TempCString = new CString(_StringRight);
	if (TempCString)
	{
		ret = (*this == TempCString);
		delete TempCString; 
	}
	return ret;
#else
	return (strcmp(Buffer, _StringRight) == 0);
#endif
#else	// Static
	
	if (	(!_StringRight)
		&&	(*Buffer == 0))
		return true;

	if (!_StringRight)
		return false;

	if (*Buffer == 0)
		return false;

#ifdef _UNICODE
	bool ret = false;
	CString* TempCString = new CString(_StringRight);
	if (TempCString)
	{
		ret = (*this == TempCString);
		delete TempCString; 
	}
	return ret;
#else
	return (strcmp(Buffer, _StringRight) == 0);
#endif
#endif
}

// ----
bool	CString::operator== (CString* _StringRight)
{
#ifdef DYNAMIC_CSTRING
	if (!_StringRight)
		return false;

	if (	(!Buffer) 
		&&	(!_StringRight->GetString()))
	{
		return true;
	}

	if (!Buffer)
		return false;

#ifdef _UNICODE
	return (!wcscmp(Buffer, _StringRight->GetString()));
#else
	return (!strcmp(Buffer, _StringRight->GetString()));
#endif
#else	// Static

	if (!_StringRight)
		return false;

	if (	(*Buffer == 0) 
		&&	(!_StringRight->GetString()))
		return true;

	if (*Buffer == 0)
		return false;

#ifdef _UNICODE
	return (!wcscmp(Buffer, _StringRight->GetString()));
#else
	return (!strcmp(Buffer, _StringRight->GetString()));
#endif	
#endif
}

