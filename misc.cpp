#include	"Misc.h"
#include	"RegistryFunctions.h"

// ----
float GetTimeDifference(LPSYSTEMTIME _Now, LPSYSTEMTIME _Then)
{
	float	ret = 0.0f;

	if ((_Now) && (_Then))
	{
		ret += (float)((_Now->wYear			- _Then->wYear)				* 60 * 60 * 24 * 32 * 14);
		ret += (float)((_Now->wMonth		- _Then->wMonth)			* 60 * 60 * 24 * 32);
		ret += (float)((_Now->wDay			- _Then->wDay)				* 60 * 60 * 24);
		ret += (float)((_Now->wHour			- _Then->wHour)				* 60 * 60);
		ret += (float)((_Now->wMinute		- _Then->wMinute)			* 60);
		ret += (float)((_Now->wSecond		- _Then->wSecond));
		ret += (float)((_Now->wMilliseconds	- _Then->wMilliseconds))	/ 1000.0f;
	}
	return ret;
}


// ----
// Tells you the length of a file - "_filelength" is not provided for CE.
long GetFileSize(CString* _FileName)
{
	FILE	*FH;
	long	ret	= -1;
	
	if (_FileName && _FileName->GetString())
	{
		FH = FOPEN(_FileName->GetString(), "rb");
		if (FH)
		{
			fseek(FH, 0, SEEK_END);		// Seek end	
			ret = ftell(FH);
			fclose(FH);
		}
	}
	return ret;
}

// ----
// Returns true if the point is within the bounds of the 
// rect - the test is inclusive on the top left edge, and 
// exclusive on the bottom right.
bool	IsPointInRect(POINT* _Pos, RECT* _Rect)
{
	return 	(	(_Pos) 
			&&	(_Rect)
			&&	(_Pos->y >=	_Rect->top)
			&& 	(_Pos->y <	_Rect->bottom)
			&& 	(_Pos->x >= _Rect->left)
			&& 	(_Pos->x <	_Rect->right)
			);
}


// ----
// Rounds _Value to the nearest _Span.
// Modes 
//	-1	=> round down
//	0	=> round at mid point
//	1	=> round up
int		Round(int _Value, int _Snap, int _Mode) 
{
	switch (_Mode)
	{
	case -1:
		return ((_Value / _Snap) * _Snap);
	case 0:
		return (((_Value + (_Snap >> 1)) / _Snap) * _Snap);
	case 1:
		return (((_Value + (_Snap >> 1)) / _Snap) * _Snap);
	default:
		NODEFAULT;
		return 0;
	}
}

// ----
// Returns whether the file could be opened.
bool	TestReadFile(CString*	_FileName)
{
	FILE	*FH;
	
	if (_FileName && _FileName->GetString())
	{
		FH = FOPEN(_FileName->GetString(), "rb");
		if (FH)
		{
			fclose(FH);
			return true;
		}
	}
	return false;
}

// ----
// Really complicated encryption algorithm
void	ChangeEncryptionState(char*	_Buffer, int _Count)
{
	if ((_Count < 1) || (!_Buffer))
		return;

	for (int i = _Count; i--;)
	{
		*(_Buffer + i) = (*(_Buffer + i)) ^ 0xFF; 
	}
}

// Convert from portrait mode co-ordiantes to the 
// co-ordinates for the specified orientation.
void	ConvertCoordSpace(POINT _A, POINT	*_B, EOrientation _C)
{
	switch (_C)
	{
	case EO_Portrait_Normal:
		_B->x = _A.x;
		_B->y = _A.y;
		break;
	case EO_Landscape_Normal:
		_B->x = _A.y;
		_B->y = 239 - _A.x;
		break;
	case EO_Landscape_Flipped:
		_B->x = 319 - _A.y;
		_B->y = _A.x;
		break;
	}
}


// ----
bool	GetRegistrationString(CString*	RegString)
{
	DWORD	DataType	= 0;
	char*	Value		= NULL;
	char	Buffer[512], *BufferPtr;
	unsigned short	BuffItem;
	DWORD	DataLength	= 0;
	LONG	ret;
	bool	Ret;

	// ----
	// Load Volume from registry if we can.
	ret = LoadFromRegistry(	HKEY_CURRENT_USER,					// Existing key or default root.
							&CString("ControlPanel\\Owner"),	// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString("Owner"),					// Name of value to retrieve.
							&DataType,							// A pointer to a DWORD filled-in by the function.
							&Value,								// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		WCHAR	*ValuePtr;

		ValuePtr = (unsigned short*)Value;
		BufferPtr = Buffer;
		while (		(*ValuePtr) 
				&&	(DataLength > 0)
				)
		{
			// Need to read it as a unicode variable
			BuffItem = *(unsigned short*)ValuePtr;
			if (	(	(BuffItem >= 32)
					&& 	(BuffItem < 127))
				||	(	(BuffItem >= 160)
					&& 	(BuffItem <= 255))
				)
			{
				*BufferPtr = BuffItem & 0xFF;
				BufferPtr++;
			}
			
			ValuePtr++;					// Move to next widechar
			DataLength -= 2;
		}
		*BufferPtr = NULL;	// Null terminate

		// Now check that the name is okay - need to remove spaces, etc
		Ret = true;	// start by assuming it is okay

		(*RegString) = Buffer;
		if (!RegString->GetStringLength())
			Ret = false;	// Null string
	}
	else
	{
		Ret = false;
	}

	if (Value)
		delete []Value;

	if (!Ret)
		(*RegString) = "Unable to obtain username";

	return Ret;
}

// ----
void	RPNStackPush(	unsigned long	*Stack,
						unsigned short	*StackPtr,
						unsigned long	Value)
{
	Stack[*StackPtr] = Value;
	*StackPtr = *StackPtr + 1;
}

// ----
unsigned long	RPNStackPop(unsigned long	*Stack,
							unsigned short	*StackPtr)
{
	*StackPtr = *StackPtr - 1;
	return Stack[*StackPtr];
}

#define	NumRPNOps 18
char	RPNList[NumRPNOps][4] = 
{
	"i",
	"c",
	"key",
	"+",
	"-",

	"*",
	"/",
	"%",
	"==",
	">=",

	"&&",
	"||",
	"!",
	"<<",
	">>",

	"~",
	"&",
	"|"
};

// A logical match up for the array above.
#define	RPN_I			0
#define	RPN_C			1
#define	RPN_KEY			2
#define	RPN_ADD			3
#define	RPN_SUB			4
#define	RPN_MULU		5
#define	RPN_DIVU		6
#define	RPN_MOD			7
#define	RPN_EQU			8
#define	RPN_GTE			9
#define	RPN_LOGAND		10
#define	RPN_LOGOR		11
#define	RPN_LOGNOT		12
#define	RPN_LSL			13
#define	RPN_LSR			14
#define	RPN_INVERT		15	// Isn't this just RPN_LOGNOT by a different name?
#define	RPN_BITAND		16
#define	RPN_BITOR		17

// ----
// Returns the OP code index - returns -1 if it is an 
// unknown value (would probably indicate a ture value).
signed long		IsRPNOp(char	*Str)
{
	for (int i = NumRPNOps; i--;)
	{
		if (!strcmp(RPNList[i], Str))
		{
			return i;
		}
	}

//	OutputDebugStringA(Str);
	return -1; // Not found in list, assuming that it is a true number
}


// ----
// Validate that a user's name and the key they provide match!
bool	ValidateRegistrationDetails(CString			*UserName, 
									unsigned short	UserKey,
									char			*RPN)
{
	unsigned long	RPNStack[32];		// Stack
	unsigned short	RPNStackIndex;		// Stack index
	WCHAR			*UserNamePtr;
	char			RPNString[512];		// A working copy of our RPN string
	char			*RPNStringPtr;		// Pointer to a RPN token

	unsigned short	i;					// PUN index
	unsigned short	c;					// PUN char	- could be a char, but will use a u16 for ease of understanding
	unsigned short	Key;				// Resultant key
	unsigned short	IntermediateA;		// Used during a calculation.
	unsigned short	IntermediateB;		// Used during a calculation.

	RPNStackIndex	= 0;
	Key				= 0;

	if (	(!UserName)
		||	(!UserName->GetString())
		)
		return false;

	// Need to convert to Handango string - if it is longer
	// then 10 chars we take the first and last 5 chars
	WCHAR	UserNameBuffer[512];
	wcscpy(UserNameBuffer, UserName->GetString());
	if (wcslen(UserNameBuffer) > 10)
	{
		wcscpy(UserNameBuffer + 5, UserNameBuffer + wcslen(UserNameBuffer) - 5);
	}
	UserNamePtr		= UserNameBuffer;

	// Loop through each of the letters in the PUN in-turn
	for (i = 0; i < wcslen(UserNameBuffer); i++)
	{
		c = (*UserNamePtr) & 0xFF;	// Convert the WCHAR back to a char.

		// Okay, we have our values for Key, i and c. 
		// Now put them through the RPN process.

		strcpy(RPNString, RPN);	// Make a working copy of our RPN string
		RPNStringPtr = strtok(RPNString, ", ");
		while (RPNStringPtr)
		{
			switch (IsRPNOp(RPNStringPtr))
			{
			case RPN_I:			// Push I onto the stack
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)i);
				break;

			case RPN_C:			// Push C onto the stack
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)c);
				break;

			case RPN_KEY:			// Push KEY onto the stack
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)Key);
				break;

			case RPN_ADD:	
				// Don't forget to pull the values in reverse order - intermediate B then A.
				IntermediateB = RPNStackPop(RPNStack,	&RPNStackIndex);
				IntermediateA = RPNStackPop(RPNStack,	&RPNStackIndex);
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)(IntermediateA + IntermediateB));
				break;

			case RPN_SUB:
				// Don't forget to pull the values in reverse order - intermediate B then A.
				IntermediateB = RPNStackPop(RPNStack,	&RPNStackIndex);
				IntermediateA = RPNStackPop(RPNStack,	&RPNStackIndex);
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)(IntermediateA - IntermediateB));
				break;

			case RPN_MULU:	
				// Don't forget to pull the values in reverse order - intermediate B then A.
				IntermediateB = RPNStackPop(RPNStack,	&RPNStackIndex);
				IntermediateA = RPNStackPop(RPNStack,	&RPNStackIndex);
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)(IntermediateA * IntermediateB));
				break;

			case RPN_DIVU:		
				// Don't forget to pull the values in reverse order - intermediate B then A.
				IntermediateB = RPNStackPop(RPNStack,	&RPNStackIndex);
				IntermediateA = RPNStackPop(RPNStack,	&RPNStackIndex);
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)(IntermediateA / IntermediateB));
				break;

			case RPN_MOD:			
				// Don't forget to pull the values in reverse order - intermediate B then A.
				IntermediateB = RPNStackPop(RPNStack,	&RPNStackIndex);
				IntermediateA = RPNStackPop(RPNStack,	&RPNStackIndex);
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)(IntermediateA % IntermediateB));
				break;

			case RPN_EQU:	
				// Don't forget to pull the values in reverse order - intermediate B then A.
				IntermediateB = RPNStackPop(RPNStack,	&RPNStackIndex);
				IntermediateA = RPNStackPop(RPNStack,	&RPNStackIndex);
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(IntermediateA == IntermediateB)?(1):(0));
				break;

			case RPN_GTE:
				// Don't forget to pull the values in reverse order - intermediate B then A.
				IntermediateB = RPNStackPop(RPNStack,	&RPNStackIndex);
				IntermediateA = RPNStackPop(RPNStack,	&RPNStackIndex);
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)(IntermediateA >= IntermediateB)?(1):(0));
				break;

			case RPN_LOGAND:
				// Don't forget to pull the values in reverse order - intermediate B then A.
				IntermediateB = RPNStackPop(RPNStack,	&RPNStackIndex);
				IntermediateA = RPNStackPop(RPNStack,	&RPNStackIndex);
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)(IntermediateA && IntermediateB));
				break;

			case RPN_LOGOR:	
				IntermediateB = RPNStackPop(RPNStack,	&RPNStackIndex);
				IntermediateA = RPNStackPop(RPNStack,	&RPNStackIndex);
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)(IntermediateA || IntermediateB));
				break;

			case RPN_LOGNOT:
				IntermediateA = RPNStackPop(RPNStack,	&RPNStackIndex);
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)(!IntermediateA));
				break;

			case RPN_LSL:					
				IntermediateB = RPNStackPop(RPNStack,	&RPNStackIndex);
				IntermediateA = RPNStackPop(RPNStack,	&RPNStackIndex);
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)(IntermediateA << IntermediateB));
				break;

			case RPN_LSR:					
				IntermediateB = RPNStackPop(RPNStack,	&RPNStackIndex);
				IntermediateA = RPNStackPop(RPNStack,	&RPNStackIndex);
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)(IntermediateA >> IntermediateB));
				break;

			case RPN_INVERT:
				IntermediateA = RPNStackPop(RPNStack,	&RPNStackIndex);
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)(!IntermediateA));
				break;

			case RPN_BITAND:					
				IntermediateB = RPNStackPop(RPNStack,	&RPNStackIndex);
				IntermediateA = RPNStackPop(RPNStack,	&RPNStackIndex);
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)(IntermediateA & IntermediateB));
				break;

			case RPN_BITOR:		
				IntermediateB = RPNStackPop(RPNStack,	&RPNStackIndex);
				IntermediateA = RPNStackPop(RPNStack,	&RPNStackIndex);
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								(unsigned short)(IntermediateA | IntermediateB));
				break;

			default:	// Should indicate a number.
				RPNStackPush(	RPNStack,
								&RPNStackIndex,
								atoi(RPNStringPtr));
				break;
			}

			// Prepare for next loop.
			RPNStringPtr = strtok(NULL, ", ");	// Point to next RPN token
		}

		Key = RPNStackPop(RPNStack,	&RPNStackIndex);
		UserNamePtr++;
	}

	// All done - Key should now be the correct Key
	return Key == UserKey;
}

#define	ACTIVATIONMASK		0x370ADB39
// ----
// Encrypt and decrypt the activation code;
unsigned long	EncryptCode(unsigned long aaaa)
{
	return aaaa ^ ACTIVATIONMASK;
}

unsigned long	DecryptCode(unsigned long aaaa)
{
	return aaaa ^ ACTIVATIONMASK;
}