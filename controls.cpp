#include	"Controls.h"
#include	"Object.h"
#include	<gx.h>

CControls::CControls()
{
	CurrentButtonState	= 0;
	PreviousButtonState	= 0;
	Orientation			= EO_Portrait_Normal;
	CharQueueIndex		= 0;
}

CControls::~CControls()
{
	GXCloseInput();
}

// ----
// Init the object.
bool	CControls::Init()
{
	CurrentButtonState	= 0;
	PreviousButtonState	= 0;
	CharQueueIndex		= 0;

	GXCloseInput();

	bool ret = (GXOpenInput() == 1);
	if (ret)
		GAPIKeyList = GXGetDefaultKeys(GX_NORMALKEYS);
	else
		ErrorCode = GetLastError();

	return ret;
}

// ----
// Process / update key state.
bool	CControls::Process()
{
	// Store previous key states.
	PreviousButtonState = CurrentButtonState;

	// Start by assuming there won't be an error.
	ErrorCode	= CONTROL_NO_ERROR;

	// Set buttons according to the values returned by GetAsyncKeyState.
	SetButton(Up,				(GetAsyncKeyState(GAPIKeyList.vkUp)		& 0x8000) != 0);
	SetButton(Down,				(GetAsyncKeyState(GAPIKeyList.vkDown)	& 0x8000) != 0);
	SetButton(Left,				(GetAsyncKeyState(GAPIKeyList.vkLeft)	& 0x8000) != 0);
	SetButton(Right,			(GetAsyncKeyState(GAPIKeyList.vkRight)	& 0x8000) != 0);
	SetButton(Select,			(GetAsyncKeyState(GAPIKeyList.vkA)		& 0x8000) != 0);
	SetButton(MouseSelect,		(GetAsyncKeyState(VK_LBUTTON)			& 0x8000) != 0);
	SetButton(Backup,			(GetAsyncKeyState(GAPIKeyList.vkB)		& 0x8000) != 0);;
	SetButton(Pause,			(GetAsyncKeyState(GAPIKeyList.vkStart)	& 0x8000) != 0);
	SetButton(DebugQuickExit,	(GetAsyncKeyState(GAPIKeyList.vkC)		& 0x8000) != 0);
#ifdef NOT_FINAL_BUILD
	//SetButton(TakePicture,		(	(GetAsyncKeyState(VK_LBUTTON)			& 0x8000)
	//							&&	(GetAsyncKeyState(GAPIKeyList.vkB)		& 0x8000)));

	//SetButton(TakePicture,		(GetAsyncKeyState(GAPIKeyList.vkB)		& 0x8000) != 0);
	SetButton(DebugQuickExit,	(GetAsyncKeyState(GAPIKeyList.vkC)		& 0x8000) != 0);
#endif

	return true;
}

#ifdef USE_DEPRICATED
// ----
// Call this method before making calls to SetButton. 
// State information is otherwise lost.
bool	CControls::PrepareForExternalButtonSet()
{
	PreviousButtonState = CurrentButtonState;
	return true;
}
#endif

// ----
// Allows mouse info to be passed this class.
void	CControls::SetMouse(POINT*	_In)
{
	if (_In)
	{
		// Process orientation.
		switch (Orientation)
		{
		case EO_Portrait_Normal:
			Mouse.x = _In->x;
			Mouse.y = _In->y;
			break;

		case EO_Landscape_Normal:
			Mouse.x = _In->y;
			Mouse.y = (240 - 1) - _In->x;
			break;

		case EO_Landscape_Flipped:
			Mouse.x = (320 - 1) - _In->y;
			Mouse.y = _In->x;
			break;
		default:
			break;
		}
	}
}

// ----
POINT*	CControls::GetMousePosition()
{
	return &Mouse;
}

// ----
// Set current button state.
bool	CControls::SetButton(EButton _Button, bool	_State)
{
	int index = (int)_Button;
	
	if (index < 31)
	{
		if (_State)
		{
			CurrentButtonState |= (1 << index);
		}
		else
		{
			DWORD mask = 0xFFFFFFFF;
			mask ^= (1 << index);
			CurrentButtonState &= mask;				
		}

		return true;
	}
	else
	{
		return false;
	}
}

// ----
// Get current button state and a debounced version.
bool	CControls::IsButtonPressed(EButton _Button)
{
	int index = (int)_Button;
	
	if (index < 31)
	{
		return ((CurrentButtonState & (1 << index)) > 0);
	}
	else
	{
		return false;
	}
}

bool	CControls::IsButtonPressedDB(EButton _Button)
{
	int index = (int)_Button;
	
	if (index < 31)
	{
		return	(	(CurrentButtonState & (1 << index))
				&&	(!(PreviousButtonState & (1 << index)))
					);
	}
	else
	{
		return false;
	}
}


// ----
// Returns true if an button is pressed.
bool	CControls::AreAnyKeysPressed()
{
	return (CurrentButtonState != 0);
}

// ---- 
// Allows the system to do on-the-fly conversion of 
// coordinates into the new screen orientation.
void	CControls::SetOrientation(EOrientation	_Mode)
{
	Orientation = _Mode;
}

// ----
EOrientation	CControls::GetOrientation()
{
	return Orientation;
}

#if (defined(PROJECT) && (PROJECT == SPEEDBALL2))
// ----
// Returns the direction, as defined in game defines, 
// for the direction the user is pressing on the pad.
int		CControls::GetDirection()
{
	if (IsButtonPressed(Up))
	{
		if (IsButtonPressed(Left))
			return NW;

		if (IsButtonPressed(Right))
			return NE;

		return N;
	}
	
	if (IsButtonPressed(Down))
	{
		if (IsButtonPressed(Left))
			return SW;

		if (IsButtonPressed(Right))
			return SE;

		return S;
	}

	if (IsButtonPressed(Left))
			return W;

	if (IsButtonPressed(Right))
			return E;

	return STILL;
}
#endif

// ----
bool	CControls::QueueChar(TCHAR	_Char, DWORD	_KeyData)
{
	if (CharQueueIndex < MAX_CHAR_QUEUE_INDEX)
	{
		CharQueue[CharQueueIndex].Char		= _Char;
		CharQueue[CharQueueIndex].KeyData	= _KeyData;
		CharQueueIndex++;
		return true;
	}

	return false;
}

// ----
bool	CControls::GetChar(TCHAR	*_Char, DWORD	*_KeyData)
{
	if (CharQueueIndex > 0)
	{
		CharQueueIndex--;
		if (_Char)		{*_Char		= CharQueue[CharQueueIndex].Char;	}
		if (_KeyData)	{*_KeyData	= CharQueue[CharQueueIndex].KeyData;}
		return true;
	}
	return false;
}