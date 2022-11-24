#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#include	"Object.h"
#include	"SafeProgramming.h"
#include	"Platform.h"
#include	"String.h"
#include	<windows.h>

// ----
// Store sound sample info.
class CSample : public CObject
{
public:
	CSample();
	~CSample();

	// ----
	bool	Init(	CString*	_FileName, 
					CPlatform*	_PlatformRef);

	// ----
	// A rather last minute method to load 
	// in and process mod sample data.
	bool	ModInit(FILE*			_FH,
					unsigned int	ReadBytes,
					int				LoopStart,
					int				LoopEnd);

	// ----
	short*	GetData() {return Data;}
	DWORD	GetDataLength()	{return DataLength;}

protected:
	CPlatform	*PlatformRef;
	
	UNALIGNED	short		*Data;
	DWORD		DataLength;
public:
	short		LoopStart;
	short		LoopEnd;
};

#endif