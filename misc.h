#ifndef		_MISC_H_
#define 	_MISC_H_

#include	<windows.h>
#include	"SafeProgramming.h"
#include	"String.h"
#include	"GameDefines.h"

// ----
// Useful macros that make life that little bit easier.

// Unicode macros
#ifdef _UNICODE
	#define	SPF(_a, _b)				wsprintf(_a, TEXT(_b)
	#define	STRLEN(_a)				((_a != NULL)?(wcslen(_a)):0)
	#define FOPEN(_a, _b)			_wfopen(_a, TEXT(_b))
#else
// Non-unicode macros
	#define	SPF(_a, _b)				sprintf(_a, TEXT(_b)
	#define	STRLEN(_a)				((_a != NULL)?strlen(_a):0)
	#define FOPEN(_a, _b)			fopen(_a, TEXT(_b))
#endif	

#define	ODLOCALE(_a)	{OutputDebugString(TEXT(__FILE__)); OutputDebugString(TEXT(_a));}

#if 0
//#ifdef NOT_FINAL_BUILD	
// Output Debug String
#define	ODS(_a)			{OutputDebugString(TEXT(_a));}
#define	ODSN(_a)		{ODS(_a); OutputDebugString(TEXT("\n"));}

#define	ODSA(_a)		if (_a) {ODCS((&CString(_a)));}
#define	ODSAN(_a)		{ODSA(_a); OutputDebugString(TEXT("\n"));}

#define	ODCS(_a)		if ((_a) && (_a->GetString())) OutputDebugString(_a->GetString());
#define	ODCSN(_a)		{ODCS(_a); OutputDebugString(TEXT("\n"));}


// Output Debug Integer
#define	ODI(_a)				\
{TCHAR	PrintArray[40];		\
SPF(PrintArray, "%d"), _a);	\
OutputDebugString(PrintArray);}
#define	ODIN(_a)		{ODI(_a); OutputDebugString(TEXT("\n"));}
// Output Debug Float
#define	ODF(_a)				\
{TCHAR	PrintArray[40];		\
SPF(PrintArray, "%f"), _a);	\
OutputDebugString(PrintArray);}
#define	ODFN(_a)		{ODF(_a); OutputDebugString(TEXT("\n"));}

#else

// Output Debug String
#define	ODS(_a)			{}
#define	ODSN(_a)		{}

#define	ODSA(_a)		{}
#define	ODSAN(_a)		{}

#define	ODCS(_a)		{}
#define	ODCSN(_a)		{}

// Output Debug Integer
#define	ODI(_a)			{}
#define	ODIN(_a)		{}
// Output Debug Float
#define	ODF(_a)			{}
#define	ODFN(_a)		{}

#endif*/
// ----
float GetTimeDifference(LPSYSTEMTIME _Now, LPSYSTEMTIME _Then);


// ----
// Test for button press 
#define	IsBP(_Button)												\
if ((ControllerRef) && (ControllerRef->IsButtonPressed(_Button)))

// ----
// Test for button press 
#define	IsBPDB(_Button)												\
if ((ControllerRef) && (ControllerRef->IsButtonPressedDB(_Button)))

// ----
// Quick tests to see if a particular button is pressed.
#define	IfSelect	IsBP(Select)
#define	IfBackup	IsBP(Backup)
#define	IfUp		IsBP(Up)
#define	IfDown		IsBP(Down)
#define	IfLeft		IsBP(Left)
#define	IfRight		IsBP(Right)
#define	IfPause		IsBP(Pause)

// ----
// Quick tests to see if a particular button is pressed - debounced.
#define	IfSelectDB	IsBPDB(Select)
#define	IfBackupDB	IsBPDB(Backup)
#define	IfUpDB		IsBPDB(Up)
#define	IfDownDB	IsBPDB(Down)
#define	IfLeftDB	IsBPDB(Left)
#define	IfRightDB	IsBPDB(Right)
#define	IfPauseDB	IsBPDB(Pause)

// ----
// Bit more complicated versions of the above. 
// Best to see where it is used and figure out its purpose.
#define	IfSelectOrMouseSelectDB(_MouseChangedPos)				\
if (	(ControllerRef)											\
	&&	(	ControllerRef->IsButtonPressedDB(Select)			\
		||	(	ControllerRef->IsButtonPressedDB(MouseSelect)	\
			&&	(_MouseChangedPos)								\
	)	)	)

// ----
#define SOUND_SELECT								\
SAFELY_CALL(SoundSystemRef, PlaySample(SelectSound, 1, 0.0f, true));


// ----
// Tells you the length of a file - "_filelength" is not provided for CE.
long GetFileSize(CString* _FileName);


// ----
// Returns true if the point is within the bounds of the 
// rect - the test is inclusive on the top left edge, and 
// exclusive on the bottom right.
bool	IsPointInRect(POINT* _Pos, RECT* _Rect);


// ----
// Rounds _Value to the nearest _Span.
// Modes 
//	-1	=> round down
//	0	=> round at mid point
//	1	=> round up
int		Round(int _Value, int _Snap = 10, int _Mode = -1);


// ----
// Returns whether the file could be opened.
bool	TestReadFile(CString*	_FileName);

// ----
void	ChangeEncryptionState(char*	_Buffer, int _Count);

// ----
// Fills the passed CString object with a ansii string form of the UNICODE 
// username value, obtained from the registry.
// A return of true indicated success, and false means no name was generated
bool	GetRegistrationString(CString*	RegString);

// ----
// Validate that a user's name and the key they provide match!
bool	ValidateRegistrationDetails(CString			*UserName, 
									unsigned short	UserKey,
									char			*RPN);

// ----
// Encrypt and decrypt the activation code;
unsigned long	EncryptCode(unsigned long aaaa);
unsigned long	DecryptCode(unsigned long aaaa);

// ----
#define	EndianInt16(a)		\
(							\
((a >> 8) & 0x00ff)	|		\
((a << 8) & 0xff00)			\
)

// ----
#define	EndianInt32(a)		\
(							\
((a >>24) & 0x000000ff)	|	\
((a >> 8) & 0x0000ff00)	|	\
((a << 8) & 0x00ff0000)	|	\
((a <<24) & 0xff000000)		\
)


// Convert from portrait mode co-ordiantes to the 
// co-ordinates for the specified orientation.
void	ConvertCoordSpace(POINT _A, POINT	*_B, EOrientation _C);
#endif