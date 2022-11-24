#ifndef _CSTRING_H_
#define _CSTRING_H_

#include	"Object.h"
#include	<windows.h>
#include	<string.h>
#include	<stdio.h>

// ----
// Stores a string.
class CString : CObject
{
public:
	
	CString();
	CString(char* _String);
	CString(CString* _String);

	~CString();

	// ----
	// Release memory.
	void	Clear();

	// ----
	// Reads in the content of a file.
	// Return value indicates number of bytes read. (I don't know how the file was opened [text/binary])
	int		LoadString(FILE* _FH, int _ReadLength = 0);
	int		LoadEncodedString(FILE* _FH, int _ReadLength = 0);

	// ----
	// Copies the string across exactly as is, without any
	// fancy character conversion.
	void	AssignAsData(char*	_String);

	// ----
	// Overload inherited method
	TCHAR*	ToString();

	// ----
	TCHAR*	GetString();

	// ----
	UINT	GetStringLength();

	// ----
	// Returns the size of the string in bytes.
	UINT	GetStringSize();

#ifdef INCLUDE_UNFINISHED_CODE
	// ----
	// Returns true if the string contains one, or more, 
	// new line characters.
	bool	DoesStringContainNewLine();
#endif

	// ----
	// Simply remove the last character from the string. (Bit of a quick fix)
	void	DeleteLastChar();

#ifdef INCLUDE_UNFINISHED_CODE
	// ----
	// Takes the first word from the object string and 
	// moves it into the _IntoHere object.
	void	MoveFirstWord(CString*	_IntoHere, char _Delimiter = ' ');

	// ----
	// Copies the first word of the object string into the
	// referenced object.
	void	CopyFirstWord(CString*	_IntoHere, char _Delimiter = ' ');
	
	// ----
	// Removes the first word of the buffer, including 
	// trailing space.
	void	DeleteFirstWord(char _Delimiter = ' ');
#endif

#ifdef _UNICODE			// Unicode only method
	void	operator= (WCHAR* _String);
#endif

	void	operator= (char* _String);
	void	operator= (CString* _String);
	void	operator= (CString& _String);

	void	operator+= (char* _String);
	void	operator+= (CString* _String);
	void	operator+= (CString& _String);

	bool	operator== (char* _StringRight);
	bool	operator== (CString* _StringRight);

protected:
#ifdef DYNAMIC_CSTRING
#ifdef _UNICODE
	UNALIGNED TCHAR	*Buffer;
#else
	TCHAR	*Buffer;
#endif
#else
	TCHAR	Buffer[2048];
#endif
};

#endif