#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#include	"SafeProgramming.h"
#include	"Object.h"
#include	"GameDefines.h"
#include	<windows.h>
#include	<gx.h>


#define	CONTROL_NO_ERROR	OBJECT_NO_ERROR


// The in game buttons.
enum EButton
{
	Up				= 0,
	Down,
	Left,
	Right,
	Select, 
	MouseSelect,			// Stylus
	Backup,
	Pause,
	
	// The following button definitions are for debug / development use only.
	TakePicture,			
	DebugQuickExit
};

#define	CONTROL_ERROR_NO_ERROR						OBJECT_NO_ERROR
#define CONTROL_ERROR_DONT_CARE_ABOUT_THAT_BUTTON	1

// ----
// For storing keyboard info.
struct	CharQueueStruct
{
	TCHAR	Char;
	DWORD	KeyData;
};
#define	MAX_CHAR_QUEUE_INDEX	32

// ----
// Control input / processing, etc.
class CControls : public CObject
{
public:
	CControls();
	~CControls();

	// ----
	// Init the object.
	bool			Init();

	// ----
	// Returns a pointer to the information collected from GAPI.
	GXKeyList*		AccessKeyList()	{return &GAPIKeyList;}

	// ----
	// Process / update key state.
	bool			Process();
	
	// This method is no longer required.
#ifdef USE_DEPRICATED
	// ----
	// Call this method before making calls to SetButton. 
	// State information is otherwise lost.
	bool			PrepareForExternalButtonSet();
#endif

	// ----
	// Allows mouse info to be passed this class.
	void			SetMouse(POINT*	_In);

	// ----
	POINT*			GetMousePosition();

	// ----
	// Set current button state.
	bool			SetButton(EButton _Button, bool	_State = true);

	// ----
	// Get current button state and a debounced version.
	bool			IsButtonPressed(EButton _Button);
	bool			IsButtonPressedDB(EButton _Button);

	// ----
	// Returns true if an button is pressed.
	bool			AreAnyKeysPressed();

	// ---- 
	// Allows the system to do on-the-fly conversion of 
	// coordinates into the new screen orientation.
	void			SetOrientation(EOrientation	_Mode);

	// ----
	EOrientation	GetOrientation();

#if (defined(PROJECT) && (PROJECT == SPEEDBALL2))
	// ----
	// Returns the direction, as defined in game defines, 
	// for the direction the user is pressing on the pad.
	int				GetDirection();
#endif

	// ----
	// Store keyboard info.
	bool	QueueChar(TCHAR	_Char, DWORD	_KeyData);

	// ----
	// Access stored keyboard info.
	bool	GetChar(TCHAR	*_Char, DWORD	*_KeyData);

protected:
	DWORD			PreviousButtonState;
	DWORD			CurrentButtonState;

	GXKeyList		GAPIKeyList;
	EOrientation	Orientation;
	POINT			Mouse;
	CharQueueStruct	CharQueue[MAX_CHAR_QUEUE_INDEX];
	int				CharQueueIndex;
};

#endif
