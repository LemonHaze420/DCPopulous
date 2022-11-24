#include	"SafeProgramming.h"

#include	<windows.h>
#include	<gx.h>
#include	"GraphicsClasses.h"
#include	"Object.h"
#include	"String.h"
#include	"GameWindow.h"
#include	"Misc.h"

// -----
// This code sets up the rendering for all devices.
#define		RENDERSETUP(_Size, Depth)										\
unsigned char	*WriteAddress;	/* Addess we're writing to.	*/				\
_Size			*ReadAddress;	/* and reading from.		*/				\
_Size			Trans;			/* Transparent 'colour'		*/				\
int				UpperX;			/* Rendering cut offs.		*/				\
int				UpperY;														\
																			\
/* Assignment values common to all orientations. */							\
Trans = _GraphicsData->GetTransKey##Depth();								\
ReadAddress	= (_Size*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);	\
																			\
/* For speed we have a seperate bit of code for the different device. */	\
switch (Orientation)														\
{

// ----
// This macro allows you to build a rendering loop for 
// different screen depths and orientations, depending on 
// the parameters provided.
#define		ORIENTATION_TOP(_Size, _X, _Y)										\
/* Off screen check	*/															\
if (!	(	((_XPos + _Region.right - _Region.left) > 0)						\
		&&	((_YPos + _Region.bottom - _Region.top) > 0)						\
		&&	(_XPos < (int)DispProperties._X)									\
		&&	(_YPos < (int)DispProperties._Y)))									\
	return false;																\
																				\
UpperX = min(_XPos + _Region.right - _Region.left, (int)DispProperties._X);		\
UpperY = min(_YPos + _Region.bottom - _Region.top, (int)DispProperties._Y);		\
																				\
if (_YPos < 0)																	\
{																				\
	ReadOffsetY += ReadDirectionY * (-_YPos);									\
	ReadAddress	= (_Size*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);	\
}																				\
																				\
/* This is the core blitting code. Keep it small, and keep it fast ;o)	*/		\
for (y = (int)max((int)0, (int)_YPos); y < UpperY; y++)							\
{

#define	OC_Portrait_Normal		\
WriteAddress	= ((unsigned char*)WriteBuffer + (y * DispProperties.cbyPitch) + (_XPos * DispProperties.cbxPitch));

#define	OC_Landscape_Normal		\
WriteAddress	= ((unsigned char*)WriteBuffer + (((DispProperties.cxWidth - 1) - y) * DispProperties.cbxPitch) + (_XPos * DispProperties.cbyPitch));

#define	OC_Landscape_Flipped	\
WriteAddress	= ((unsigned char*)WriteBuffer + (y * DispProperties.cbxPitch) + (((DispProperties.cyHeight - 1) - _XPos) * DispProperties.cbyPitch));

#define ORIENTATION_CORE(Orientation)	\
	OC_##Orientation;					\
	for (x = _XPos; x < UpperX; x++)	\
	{									\
		if (	(x >= 0)


// ----
// Tests the sprite data and write if not marked as transparent.
#define	RENDER_TEST_TRANSPARENCY_AND_WRITE(_Size)	\
			&&	(*ReadAddress != Trans))			\
			*(_Size*)WriteAddress = *ReadAddress;

// ----
// Writes data always.
#define	RENDER_ALWAYS_WRITE(_Size)					\
			)										\
			*(_Size*)WriteAddress = *ReadAddress;

// ----
// Writes data always.
#define	RENDER_TOGGLE_DATA(_Size)													\
			&&	(*ReadAddress == Trans))											\
			{																		\
				switch (sizeof(_Size))												\
				{																	\
				case 1:																\
					*(_Size*)WriteAddress = (*(_Size*)WriteAddress) ^ 0xFF;			\
					break;															\
				case 2:																\
					*(_Size*)WriteAddress = (*(_Size*)WriteAddress) ^ 0xFFFF;		\
					break;															\
				case 4:																\
					*(_Size*)WriteAddress = (*(_Size*)WriteAddress) ^ 0xFFFFFFFF;	\
					break;															\
				}																	\
			}




// ----
// This macro must follow your pixel rendering code.
#define	ORIENTATION_TAIL(_Size, _Sign, _XPitch)									\
		WriteAddress _Sign DispProperties._XPitch;									\
		ReadAddress += ReadDirectionX;											\
	}																			\
																				\
	ReadOffsetY += ReadDirectionY;											\
	ReadAddress	= (_Size*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);	\
}



CGraphics::CGraphics()
{
	WindowHandle			= NULL;
	PlatformRef				= NULL;
	BackBuffer				= NULL;
	PrimaryBuffer			= NULL;
	ScreenOpen				= false;
	ForceFullRedraw			= true;
	NumberColoursMatched	= 0;
	GraphicsMemAddress		= NULL;
	SoundSystemRef			= NULL;
	Orientation				= EO_Portrait_Normal;
	ActivePalette			= 0;
}

CGraphics::~CGraphics()
{
	SAFELY_DELETE_ARRAY(BackBuffer);
	SAFELY_DELETE_ARRAY(PrimaryBuffer);
		
	GXCloseDisplay();
}

// ----
// Converts a populous palette index to a colour.
DWORD			CGraphics::PopulousToPal(int _Index)
{
	DWORD			PopulousPalette[]=	{
										0x000000,0x404040,0x606060,0x808080,0xa0a0a0,0xc0c0c0,0x602000,0x804000,
										0xa00000,0xa06000,0xa0a000,0x40a000,0x208000,0x206020,0x204080,0x2060c0,
										0x000000,0x404040,0x606060,0x808080,0xa0a0a0,0xc0c0c0,0x602000,0x804000,
										0xa00000,0xa06000,0xa0a000,0x40a000,0x208000,0x206020,0x204080,0x2060c0
										};

	DWORD			PopulousLOLPalette[]=	{
										0x000000,0x000020,0x002030,0x101020,0x001010,0x003020,0x004040,0x505050,
										0x207070,0x707070,0x909090,0xa0a0a0,0xb0b090,0xc0d0a0,0x909090,0xc07070,
										0x000000,0x000020,0x002030,0x101020,0x001010,0x003020,0x004040,0x505050,
										0x207070,0x707070,0x909090,0xa0a0a0,0xb0b090,0xc0d0a0,0x909090,0xc07070
										};

	switch (ActivePalette)
	{
	case 0:
		return (PopulousPalette[_Index % 32]);// + ((_Index / 16) * 0x0F0F0F));
	case 1:
		return PopulousLOLPalette[_Index % 32];
	}

	return 0;
}

// ----
void		CGraphics::SetActivePalette(int _a)
{
	ActivePalette = _a;
}

// ----
void		CGraphics::SetActiveScreen(int _a)
{
	ASSERT((_a >= 0) && (_a <= 3));
	ActiveScreen = _a;
}

// ----
void		CGraphics::CopyScreen(int _Source, int _Destination)
{
	void	*Dest, *Source;

#ifdef DISABLE_GRAPHICS
	return;
#endif

	SoundSystemRef->Process();

	if (_Destination == 0)
		Dest = GraphicsMemAddress; 
	else
		Dest = Screens[_Destination];

	if (_Source == 0)
		Source = GraphicsMemAddress; 
	else
		Source = Screens[_Source];

	memcpy(Dest, Source, ScreenMemSize);
}

// ----
// Init the object.
bool	CGraphics::Init(HWND _WindowHandle, HPALETTE _Palette, CPreferences* _PreferencesRef, CPlatform*	_PlatformRef)
{
#ifdef DISABLE_GRAPHICS
	DispProperties.cxWidth	= 320;
	DispProperties.cyHeight	= 240;			// notice lack of 'th' in the word height.
	DispProperties.cbxPitch	= 1;			// number of bytes to move right one x pixel - can be negative.
	DispProperties.cbyPitch	= 1;			// number of bytes to move down one y pixel - can be negative.
	DispProperties.cBPP		= 16;			// # of bits in each pixel
	DispProperties.ffFormat = kfDirect565;

	return true;
#endif

	// Store.
	WindowHandle			= _WindowHandle;
	PlatformRef				= _PlatformRef;
	PreferencesRef			= _PreferencesRef;
	Palette					= _Palette;
	NumberColoursMatched	= 0;

	// Close display if already open.
	GXCloseDisplay();

	bool ret = (GXOpenDisplay(WindowHandle, GX_FULLSCREEN) == 1);
	if (!ret)
		ErrorCode = GetLastError();
//	else
		DispProperties = GXGetDisplayProperties();

//	if (PlatformRef->GetMachineFamily() == EM_Compaq_IPaq_Generic) // Compaq
//	{
//		DispProperties.ffFormat = kfDirect565 |	kfDirect;
//	}
//	else
	{
		// Set viewport
		if (!GXSetViewport(0, DispProperties.cyHeight, 0, 0))
			return false;
	}

	//if (PreferencesRef->DoubleBufferingEnabled)
	{
		// My favourite device!
		// Needs double buffering, thus create buffer here and now.
		//char[abs((int)DispProperties.cbyPitch * (int)DispProperties.cyHeight)];

		ScreenMemSize = max(abs((int)DispProperties.cbyPitch * (int)DispProperties.cyHeight), 
							abs((int)DispProperties.cbxPitch * (int)DispProperties.cxWidth));

		BackBuffer		= new char[ScreenMemSize];
		PrimaryBuffer	= new char[ScreenMemSize];

		ScreenOne		= new char[ScreenMemSize];
		ScreenTwo		= new char[ScreenMemSize];


		//PrimaryBuffer	= new char[abs((int)DispProperties.cbyPitch * (int)DispProperties.cyHeight)];
		ForceFullRedraw	= true;
	}
	/*else
	{
		BackBuffer		= NULL;
		PrimaryBuffer	= NULL;
		ForceFullRedraw	= false;
	}*/


	ActiveScreen = 0;
	Screens[0] = NULL;	
	Screens[1] = BackBuffer;
	Screens[2] = ScreenOne;
	Screens[3] = ScreenTwo;

	return true;
}

// ---- 
// Allows the system to do on-the-fly conversion of 
// coordinates into the new screen orientation.
void			CGraphics::SetOrientation(EOrientation	_Mode)
{
	Orientation = _Mode;
}

// ----
EOrientation	CGraphics::GetOrientation()
{
	return Orientation;
}



// ----
void	CGraphics::AssignPlatform(CPlatform* _Platform)
{
	PlatformRef = _Platform;
}

// ----
void	CGraphics::AssignSoundSystem(CSoundSystem* _SoundSystemRef)
{
	SoundSystemRef = _SoundSystemRef;
}


// ----
CPlatform*	CGraphics::GetPlatform()
{
	return PlatformRef;
}


// ----
GXDisplayProperties*	CGraphics::GetDisplay()
{
	return &DispProperties;
}


// ----
bool	CGraphics::StartDraw()
{
#ifdef DISABLE_GRAPHICS
	return true;
#endif
	// Process the sound before we do anything time consuming.
	if (SoundSystemRef)
		SoundSystemRef->Process();

	// Check that the screen isn't already open.
	if (ScreenOpen)	
		return true;					// Return true as the display is still active.
	else
		ScreenOpen = true;

	GraphicsMemAddress = GXBeginDraw();
	if (GraphicsMemAddress != NULL)
		return true;
		
	ErrorCode = GetLastError();
	return false;
}

// ----
bool	CGraphics::EndDraw()
{
#ifdef DISABLE_GRAPHICS
	return true;
#endif

	if (SoundSystemRef)
		SoundSystemRef->Process();

	// Stop the display being closed multiple times.
	if (!ScreenOpen)	
		return true;

	bool ret = (GXEndDraw() == 1);
	if (!ret)
		ErrorCode = GetLastError();
	else
		ScreenOpen = false;

	return ret;
}


// ----
// Copies the back buffer to the front buffer.
void	CGraphics::SwapScreens()
{
#ifdef DISABLE_GRAPHICS
	return;
#endif

	if (SoundSystemRef)
		SoundSystemRef->Process();

	void			*TempVoid;
	if (GraphicsMemAddress)
	{
		if (BackBuffer)
		{
			unsigned char	*WriteAddress				= (unsigned char*)GraphicsMemAddress;
			unsigned char	*WriteAddressLineFeed		= WriteAddress;

			// Read address.
			unsigned char	*ReadAddress				= NULL;
			if (DispProperties.cbyPitch < 0)
				ReadAddress = (unsigned char*)BackBuffer + ((DispProperties.cyHeight - 1) * (-DispProperties.cbyPitch));
			else
				ReadAddress = (unsigned char*)BackBuffer;
			unsigned char	*ReadAddressLineFeed		= ReadAddress;

			// Shadow buffer.
			unsigned char	*ShadowReadAddress			= NULL;
			if (DispProperties.cbyPitch < 0)
				ShadowReadAddress = (unsigned char*)PrimaryBuffer + ((DispProperties.cyHeight - 1) * (-DispProperties.cbyPitch));
			else
				ShadowReadAddress = (unsigned char*)PrimaryBuffer;
			unsigned char	*ShadowReadAddressLineFeed	= ShadowReadAddress;

			int				x, y;		

			switch (DispProperties.cBPP)
			{
			case 8:
				if (ForceFullRedraw)
				{
					for (y = DispProperties.cyHeight; y--;)
					{
						for (x = DispProperties.cxWidth; x--;)
						{
							*WriteAddress			= *ReadAddress;
													
							WriteAddress			+= DispProperties.cbxPitch;
							ReadAddress				+= DispProperties.cbxPitch;
						}									
															
						WriteAddressLineFeed		+= DispProperties.cbyPitch;
						WriteAddress				= WriteAddressLineFeed;
						ReadAddressLineFeed			+= DispProperties.cbyPitch;
						ReadAddress					= ReadAddressLineFeed;
					}
				}
				else
				{
					unsigned char	Value;

					if (false)//(PreferencesRef->SlowVRAMWorkAround)
					{

						for (y = DispProperties.cyHeight; y--;)
						{
							for (x = DispProperties.cxWidth; x--;)
							{
								Value					= 	*ReadAddress;

								if (Value != (*ShadowReadAddress))
									*WriteAddress		= Value;

								WriteAddress			+= DispProperties.cbxPitch;
								ReadAddress				+= DispProperties.cbxPitch;
								ShadowReadAddress		+= DispProperties.cbxPitch;
							}

							WriteAddressLineFeed		+= DispProperties.cbyPitch;
							WriteAddress				= WriteAddressLineFeed;
							ReadAddressLineFeed			+= DispProperties.cbyPitch;
							ReadAddress					= ReadAddressLineFeed;
							ShadowReadAddressLineFeed	+= DispProperties.cbyPitch;
							ShadowReadAddress			= ShadowReadAddressLineFeed;
						}
					}
					else
					{
						for (y = DispProperties.cyHeight; y--;)
						{
							for (x = DispProperties.cxWidth; x--;)
							{
								*WriteAddress		= *ReadAddress;
								WriteAddress		+= DispProperties.cbxPitch;
								ReadAddress			+= DispProperties.cbxPitch;
							}

							WriteAddressLineFeed		+= DispProperties.cbyPitch;
							WriteAddress				= WriteAddressLineFeed;
							ReadAddressLineFeed			+= DispProperties.cbyPitch;
							ReadAddress					= ReadAddressLineFeed;
						}
					}
				}
				break;

			case 16:
				
				if (ForceFullRedraw)
				{
					for (y = DispProperties.cyHeight; y--;)
					{
						for (x = DispProperties.cxWidth; x--;)
						{
							*(WORD*)WriteAddress	= *(WORD*)ReadAddress;

							WriteAddress			+= DispProperties.cbxPitch;// <<1;
							ReadAddress				+= DispProperties.cbxPitch;// <<1;

						}							
													
						WriteAddressLineFeed		+= DispProperties.cbyPitch;
						WriteAddress				= WriteAddressLineFeed;
						ReadAddressLineFeed			+= DispProperties.cbyPitch;
						ReadAddress					= ReadAddressLineFeed;
					}
				}
				else
				{
					WORD	Value;
					int	Step = DispProperties.cbxPitch;// * 2;

					if (false)//(PreferencesRef->SlowVRAMWorkAround)
					{
						for (y = DispProperties.cyHeight; y--;)
						{
							for (x = DispProperties.cxWidth; x--;)
							{
								Value					= 	*(WORD*)ReadAddress;
								if (Value != (*(WORD*)ShadowReadAddress))
									*(WORD*)WriteAddress	= Value;

								WriteAddress			+= Step;//DispProperties.cbxPitch;
								ReadAddress				+= Step;//DispProperties.cbxPitch;
								ShadowReadAddress		+= Step;//DispProperties.cbxPitch;

							}

							WriteAddressLineFeed		+= DispProperties.cbyPitch;
							WriteAddress				= WriteAddressLineFeed;
							ReadAddressLineFeed			+= DispProperties.cbyPitch;
							ReadAddress					= ReadAddressLineFeed;
							ShadowReadAddressLineFeed	+= DispProperties.cbyPitch;
							ShadowReadAddress			= ShadowReadAddressLineFeed;
						}
					}
					else
					{
						for (y = DispProperties.cyHeight; y--;)
						{
							for (x = DispProperties.cxWidth; x--;)
							{
								*(WORD*)WriteAddress	= *(WORD*)ReadAddress;;
								WriteAddress			+= Step;//DispProperties.cbxPitch;
								ReadAddress				+= Step;//DispProperties.cbxPitch;
							}

							WriteAddressLineFeed		+= DispProperties.cbyPitch;
							WriteAddress				= WriteAddressLineFeed;
							ReadAddressLineFeed			+= DispProperties.cbyPitch;
							ReadAddress					= ReadAddressLineFeed;
						}
					}
				}
				break;

			case 32:
				DWORD	Value;

				if (DispProperties.cBPP == 3)
				{
					// Will write this one day!
				}
				else if (DispProperties.cBPP == 4)
				{
					if (ForceFullRedraw)
					{
						for (y = DispProperties.cyHeight; y--;)
						{
							for (x = DispProperties.cxWidth; x--;)
							{
								*(DWORD*)WriteAddress	= *(DWORD*)ReadAddress;

								WriteAddress			+= DispProperties.cbxPitch;
								ReadAddress				+= DispProperties.cbxPitch;
							}							
														
							WriteAddressLineFeed		+= DispProperties.cbyPitch;
							WriteAddress				= WriteAddressLineFeed;
							ReadAddressLineFeed			+= DispProperties.cbyPitch;
							ReadAddress					= ReadAddressLineFeed;
						}
					}
					else
					{
						if (false)//(PreferencesRef->SlowVRAMWorkAround)
						{
						for (y = DispProperties.cyHeight; y--;)
						{
							for (x = DispProperties.cxWidth; x--;)
							{
								Value					= 	*(DWORD*)ReadAddress;

								if (Value != (*(DWORD*)ShadowReadAddress))
									*(DWORD*)WriteAddress	= Value;

								WriteAddress			+= DispProperties.cbxPitch;
								ReadAddress				+= DispProperties.cbxPitch;
								ShadowReadAddress		+= DispProperties.cbxPitch;
							}

							WriteAddressLineFeed		+= DispProperties.cbyPitch;
							WriteAddress				= WriteAddressLineFeed;
							ReadAddressLineFeed			+= DispProperties.cbyPitch;
							ReadAddress					= ReadAddressLineFeed;
							ShadowReadAddressLineFeed	+= DispProperties.cbyPitch;
							ShadowReadAddress			= ShadowReadAddressLineFeed;
						}
						}
						else
						{
							for (y = DispProperties.cyHeight; y--;)
							{
								for (x = DispProperties.cxWidth; x--;)
								{
									*(DWORD*)WriteAddress	= *(DWORD*)ReadAddress;

									WriteAddress			+= DispProperties.cbxPitch;
									ReadAddress				+= DispProperties.cbxPitch;
								}

								WriteAddressLineFeed		+= DispProperties.cbyPitch;
								WriteAddress				= WriteAddressLineFeed;
								ReadAddressLineFeed			+= DispProperties.cbyPitch;
								ReadAddress					= ReadAddressLineFeed;
							}
						}
					}
				}
				break;
			}
			
			if (ForceFullRedraw)
			{
				ForceFullRedraw	= false;
				if (false)//(PreferencesRef->SlowVRAMWorkAround)
				{
					memcpy(PrimaryBuffer, BackBuffer, max(	abs((int)DispProperties.cbyPitch * (int)DispProperties.cyHeight),
															abs((int)DispProperties.cbxPitch * (int)DispProperties.cxWidth)));
				}
			}

			if (false)//(PreferencesRef->SlowVRAMWorkAround)
			{
				// Swap buffer handles
				TempVoid		= BackBuffer;
				BackBuffer		= PrimaryBuffer;
				PrimaryBuffer	= TempVoid;
			}
		
		}
		GraphicsMemAddress = NULL;
	
		// Check whether double buffering state has changed.
		//if (PreferencesRef->DoubleBufferingEnabled)
		{
			if (!BackBuffer)
			{
				BackBuffer		= new char[max(	abs((int)DispProperties.cbyPitch * (int)DispProperties.cyHeight), 
												abs((int)DispProperties.cbxPitch * (int)DispProperties.cxWidth))];
				ForceFullRedraw	= true;
			}

			if (!PrimaryBuffer)
			{
				PrimaryBuffer	= new char[max(	abs((int)DispProperties.cbyPitch * (int)DispProperties.cyHeight), 
												abs((int)DispProperties.cbxPitch * (int)DispProperties.cxWidth))];//char[abs((int)DispProperties.cbyPitch * (int)DispProperties.cyHeight)];
				ForceFullRedraw	= true;
			}
		}
		/*else
		{
			SAFELY_DELETE_ARRAY(BackBuffer);
			SAFELY_DELETE_ARRAY(PrimaryBuffer);
		}*/
	}
}

// ----
// Calling this method will force the next EndDraw to 
// force a full update of the display. Useful when the app 
// is reactivated.
void	CGraphics::ForceRedraw()
{
	ForceFullRedraw = true;
}

// ----
bool	CGraphics::ClearScreen(UINT R, UINT G, UINT B)
{
#ifdef DISABLE_GRAPHICS
	return true;
#endif

	RECT all;
	switch (Orientation)
	{
	case EO_Portrait_Normal:
		all.top = 0;
		all.left = 0;
		all.bottom = DispProperties.cyHeight;
		all.right = DispProperties.cxWidth;
		break;

	case EO_Landscape_Normal:		
		all.top = 0;
		all.left = 0;
		all.bottom = DispProperties.cxWidth;
		all.right = DispProperties.cyHeight;
		break;

	case EO_Landscape_Flipped:
		all.top = 0;
		all.left = 0;
		all.bottom = DispProperties.cxWidth;
		all.right = DispProperties.cyHeight;
		break;
	}

	ForceFullRedraw = true;
	return DrawRect(&all, R, G, B);
}

// ----
bool	CGraphics::DrawPixel(int _XPos, int _YPos, UINT R, UINT G, UINT B)
{
#ifdef DISABLE_GRAPHICS
	return true;
#endif

	if (SoundSystemRef)
		SoundSystemRef->Process();	

	IntCouple	CoOrd;

	// Convert coordinates according to orientation.
	switch (Orientation)
	{
	case EO_Portrait_Normal:
		CoOrd.A	= _XPos;
		CoOrd.B	= _YPos;
		break;

	case EO_Landscape_Normal:
		CoOrd.A	= (DispProperties.cxWidth - 1) - _YPos;
		CoOrd.B	= _XPos;
		break;

	case EO_Landscape_Flipped:
		CoOrd.A	= _YPos;
		CoOrd.B	= (DispProperties.cyHeight - 1) - _XPos;
		break;
	default:
		break;
	}

	//if (GraphicsMemAddress)
	{
		void*	WriteBuffer;
		if (ActiveScreen == 0)
		{
			WriteBuffer = GraphicsMemAddress;
		}
		else
		{
			if (DispProperties.cbyPitch < 0)
				WriteBuffer = (char*)Screens[ActiveScreen] + ((DispProperties.cyHeight - 1) * (-DispProperties.cbyPitch));
			else
				WriteBuffer = Screens[ActiveScreen];
		}

		if (!WriteBuffer)
			return false;

		unsigned char*	WriteAddress			= (unsigned char*)WriteBuffer + CoOrd.B * DispProperties.cbyPitch + CoOrd.A * DispProperties.cbxPitch;
		switch (DispProperties.cBPP)
		{
		case 32:
			if (DispProperties.ffFormat & kfDirect888)
			{
				if (DispProperties.cbxPitch == 3)	// 24 bit
				{
					*(WriteAddress)		= R;
					*(WriteAddress + 1)	= G;
					*(WriteAddress + 2)	= B;
				}
				else if (DispProperties.cbxPitch == 4)
				{
					DWORD Value;
					Value = (R << 16) + (G << 8) + B;

					*(DWORD*)WriteAddress = Value;
				}

				return true;
			}
			break;

		case 16:
			if (	(DispProperties.ffFormat & kfDirect555)
				||	(DispProperties.ffFormat & kfDirect565))
			{
				WORD Value;
				if (PlatformRef)
				{	
					if (DispProperties.ffFormat & kfDirect565)
					{
						R = R >> 3;
						G = G >> 2;
						B = B >> 3;

						Value = (R << 11) + 
								(G << 5) + 
								B;
					}
					else
					{
						R = R >> 3;
						G = G >> 3;
						B = B >> 3;

						Value = (R << 10) + 
								(G << 5) + 
								B;
					}
				}
				else
					return false;

				*(WORD*)WriteAddress = Value;
				return true;
			}
			break;

		case 8:
			// This is not going to work unless I set an appropriate
			// palette,	as the display is not direct address able.
			unsigned char Value;
			if (PlatformRef)
			{	
				if (DispProperties.ffFormat & kfDirect)
				{
					// Direct 8 bit mode. 
					R = (R >> 5);
					G = (G >> 5);
					B = (B >> 6);

					Value = (R << 5) + (G << 2) + B;
				}
				else
				{
					COLORREF InColour;
					InColour = (B << 16) + (G << 8) + R;

					// Making the assumption that the palette is a maximum of 256 entries!!!
					UINT mapcol = Match8BitPalette(InColour);
						//GetNearestPaletteIndex(Palette, InColour);
					if (mapcol != CLR_INVALID)
						Value = mapcol;
					else
						Value = 0;
				}

				*(unsigned char*)WriteAddress = Value;
				return true;
			}
			break;

		default:
			break;
		}
	}
	return false;
}

// ----
bool	CGraphics::DrawRect(RECT* _Region, UINT R, UINT G, UINT B)
{
#ifdef DISABLE_GRAPHICS
	return true;
#endif

	if (SoundSystemRef)
		SoundSystemRef->Process();	

	RECT Temp;

	// Convert coordinates according to orientation.
	switch (Orientation)
	{
	case EO_Portrait_Normal:
		Temp.left	= _Region->left;
		Temp.top	= _Region->top;
		Temp.right	= _Region->right;
		Temp.bottom	= _Region->bottom;
		break;

	case EO_Landscape_Normal:
		Temp.left	= DispProperties.cxWidth - _Region->bottom;
		Temp.top	= _Region->left;
		Temp.right	= DispProperties.cxWidth - _Region->top;
		Temp.bottom	= _Region->right;
		break;

	case EO_Landscape_Flipped:
		Temp.left	= _Region->top;
		Temp.top	= DispProperties.cyHeight - _Region->right;
		Temp.right	= _Region->bottom;
		Temp.bottom	= DispProperties.cyHeight - _Region->left;
		break;
	default:
		break;
	}

	if (	(Temp.left		< 0)
		||	(Temp.top		< 0)
		||	(Temp.right		< 0)
		||	(Temp.bottom	< 0))
	{
		//ASSERT(0);
		ODSN("Rect out of bounds");
		return false;
	}

//	if (	(GraphicsMemAddress)				// No pointer to display memory.
//		//&&	(DispProperties.ffFormat & kfDirect)// The display is direct mapped (not a palettised display).
//		)
	{
		void*	WriteBuffer;
		if (ActiveScreen == 0)
		{
			WriteBuffer = GraphicsMemAddress;
		}
		else
		{
			if (DispProperties.cbyPitch < 0)
				WriteBuffer = (char*)Screens[ActiveScreen] + ((DispProperties.cyHeight - 1) * (-DispProperties.cbyPitch));
			else
				WriteBuffer = Screens[ActiveScreen];
		}

		if (!WriteBuffer)
			return false;

		unsigned char*	WriteAddress			= (unsigned char*)WriteBuffer + Temp.top * DispProperties.cbyPitch + Temp.left * DispProperties.cbxPitch;
		unsigned char*	WriteAddressLineFeed	= WriteAddress;

		switch (DispProperties.cBPP)
		{
		case 32:
			if (DispProperties.ffFormat & kfDirect888)
			{
				if (DispProperties.cbxPitch == 3)	// 24 bit
				{
					/* Old method
					for (int y = Temp.top; y < Temp.bottom; y++)
					{
						for (int x = Temp.left; x < Temp.right; x++)
						
						{
						*/
					for (int y = Temp.top; y < (int)min((int)Temp.bottom, (int)DispProperties.cyHeight); y++)
					{
						for (int x = Temp.left; x < (int)min(Temp.right, (int)DispProperties.cxWidth); x++)
						{
							*(WriteAddress)		= R;
							*(WriteAddress + 1)	= G;
							*(WriteAddress + 2)	= B;
							WriteAddress += DispProperties.cbxPitch;
						}

						WriteAddressLineFeed += DispProperties.cbyPitch;
						WriteAddress = WriteAddressLineFeed;
					}

				}
				else if (DispProperties.cbxPitch == 4)
				{
					DWORD Value;
					Value = (R << 16) + (G << 8) + B;

					/*
					for (int y = Temp.top; y < Temp.bottom; y++)
					{
						for (int x = Temp.left; x < Temp.right; x++)
						{
						*/
					for (int y = Temp.top; y < (int)min((int)Temp.bottom, (int)DispProperties.cyHeight); y++)
					{
						for (int x = Temp.left; x < (int)min(Temp.right, (int)DispProperties.cxWidth); x++)
						{
							*(DWORD*)WriteAddress = Value;
							WriteAddress += DispProperties.cbxPitch;
						}

						WriteAddressLineFeed += DispProperties.cbyPitch;
						WriteAddress = WriteAddressLineFeed;
					}
				}

				return true;
			}
			break;

		case 16:
			if (	(DispProperties.ffFormat & kfDirect555)
				||	(DispProperties.ffFormat & kfDirect565))
			{
				WORD Value;
				if (PlatformRef)
				{	
					if (DispProperties.ffFormat & kfDirect565)
					{
						R = R >> 3;
						G = G >> 2;
						B = B >> 3;

						Value = (R << 11) + 
								(G << 5) + 
								B;
					}
					else
					{
						R = R >> 3;
						G = G >> 3;
						B = B >> 3;

						Value = (R << 10) + 
								(G << 5) + 
								B;
					}
				}
				else
					return false;

				for (int y = Temp.top; y < (int)min((int)Temp.bottom, (int)DispProperties.cyHeight); y++)
				{
					for (int x = Temp.left; x < (int)min(Temp.right, (int)DispProperties.cxWidth); x++)
					{
						*(WORD*)WriteAddress = Value;
						WriteAddress += DispProperties.cbxPitch;
					}

					WriteAddressLineFeed += DispProperties.cbyPitch;
					WriteAddress = WriteAddressLineFeed;
				}

				return true;
			}
			break;

		case 8:
			// This is not going to work unless I set an appropriate
			// palette,	as the display is not direct address able.
			unsigned char Value;
			if (PlatformRef)
			{	
				if (DispProperties.ffFormat & kfDirect)
				{
					// Direct 8 bit mode. 
					R = (R >> 5);
					G = (G >> 5);
					B = (B >> 6);

					Value = (R << 5) + (G << 2) + B;
				}
				else
				{
					COLORREF InColour;
					InColour = (B << 16) + (G << 8) + R;

					// Making the assumption that the palette is a maximum of 256 entries!!!
					UINT mapcol = Match8BitPalette(InColour);//GetNearestPaletteIndex(Palette, InColour);
					if (mapcol != CLR_INVALID)
						Value = mapcol;
					else
						Value = 0;
				}

				/*
				for (int y = Temp.top; y < Temp.bottom; y++)
				{
					for (int x = Temp.left; x < Temp.right; x++)
					{
					*/
				for (int y = Temp.top; y < (int)min((int)Temp.bottom, (int)DispProperties.cyHeight); y++)
				{
					for (int x = Temp.left; x < (int)min(Temp.right, (int)DispProperties.cxWidth); x++)
					{
						*(unsigned char*)WriteAddress = Value;
						WriteAddress += DispProperties.cbxPitch;
					}

					WriteAddressLineFeed += DispProperties.cbyPitch;
					WriteAddress = WriteAddressLineFeed;
				}

				return true;
			}
			break;

		default:
			break;
		}
	}
	return false;
}


// ----
bool	CGraphics::DrawSprite(	int				_XPos, 
								int				_YPos,
								CGraphicsData*	_GraphicsData,
								RECT&			_Region,
								int				_Flip)
{
#ifdef DISABLE_GRAPHICS
	return true;
#endif

	if (SoundSystemRef)
		SoundSystemRef->Process();

	if (/*	(GraphicsMemAddress)				// No pointer to display memory.
		&&*/	(_GraphicsData)						// No graphical data
		)
	{
		void*	WriteBuffer;
		if (ActiveScreen == 0)
		{
			WriteBuffer = GraphicsMemAddress;
		}
		else
		{
			if (DispProperties.cbyPitch < 0)
				WriteBuffer = (char*)Screens[ActiveScreen] + ((DispProperties.cyHeight - 1) * (-DispProperties.cbyPitch));
			else
				WriteBuffer = Screens[ActiveScreen];
		}

		if (!WriteBuffer)
			return false;

		int		ReadOffsetX, ReadOffsetY;
		int		ReadDirectionX, ReadDirectionY;
		int		x, y;

		if (_Flip)
		{
			if (_Flip & SPRITE_ROTATE_180)
			{
				ReadOffsetX	= _Region.right -1;
				ReadOffsetY	= _Region.bottom -1;
				ReadDirectionX = -1;
				ReadDirectionY = -1;
			}
			else if (_Flip & SPRITE_FLIP_VERTICAL)
			{
				ReadOffsetX	= _Region.left;
				ReadOffsetY	= _Region.bottom -1;
				ReadDirectionX = 1;
				ReadDirectionY = -1;
			}
			else if (_Flip & SPRITE_FLIP_HORIZONTAL)
			{
				ReadOffsetX	= _Region.right - 1;
				ReadOffsetY	= _Region.top;

				ReadDirectionX = -1;
				ReadDirectionY = 1;
			}
			else
			{
				ASSERT(0);
				return false;
			}
		}
		else
		{
			ReadOffsetX	= _Region.left;
			ReadOffsetY	= _Region.top;
			ReadDirectionX = 1;
			ReadDirectionY = 1;
		}

		switch (DispProperties.cBPP)
		{
		case 32:	// 32 bit display.
			if (DispProperties.ffFormat & kfDirect888)
			{
				if (DispProperties.cbxPitch == 3)	// 24 bit
				{
					RENDERSETUP(DWORD, 32);
					case EO_Portrait_Normal:
						ORIENTATION_TOP(DWORD, cxWidth, cyHeight)
						ORIENTATION_CORE(Portrait_Normal)
								&&	(*ReadAddress != Trans))
								{
									*WriteAddress		= ((*ReadAddress) >> 16);
									*(WriteAddress + 1)	= ((*ReadAddress) >> 8);
									*(WriteAddress + 2)	= (*ReadAddress);
								}
						ORIENTATION_TAIL(DWORD, +=, cbxPitch);
						break;

					case EO_Portrait_Flipped:
						break;

					case EO_Landscape_Normal:
						ORIENTATION_TOP(DWORD, cyHeight, cxWidth)
						ORIENTATION_CORE(Landscape_Normal)
								&&	(*ReadAddress != Trans))
								{
									*WriteAddress		= ((*ReadAddress) >> 16);
									*(WriteAddress + 1)	= ((*ReadAddress) >> 8);
									*(WriteAddress + 2)	= (*ReadAddress);
								}
						ORIENTATION_TAIL(DWORD, +=, cbyPitch);
						break;

					case EO_Landscape_Flipped:
						ORIENTATION_TOP(DWORD, cyHeight, cxWidth)
						ORIENTATION_CORE(Landscape_Flipped)
								&&	(*ReadAddress != Trans))
								{
									*WriteAddress		= ((*ReadAddress) >> 16);
									*(WriteAddress + 1)	= ((*ReadAddress) >> 8);
									*(WriteAddress + 2)	= (*ReadAddress);
								}
						ORIENTATION_TAIL(DWORD, -=, cbyPitch);
						break;	
					}
					return true;
				}
				else if (DispProperties.cbxPitch == 4) // 32 bit. (much easier)
				{
					RENDERSETUP(DWORD, 32);
					case EO_Portrait_Normal:
						ORIENTATION_TOP(DWORD, cxWidth, cyHeight)
						ORIENTATION_CORE(Portrait_Normal)
						RENDER_TEST_TRANSPARENCY_AND_WRITE(DWORD)
						ORIENTATION_TAIL(DWORD, +=, cbxPitch);
						break;

					case EO_Portrait_Flipped:
						break;

					case EO_Landscape_Normal:
						ORIENTATION_TOP(DWORD, cyHeight, cxWidth)
						ORIENTATION_CORE(Landscape_Normal)
						RENDER_TEST_TRANSPARENCY_AND_WRITE(DWORD)
						ORIENTATION_TAIL(DWORD, +=, cbyPitch);
						break;

					case EO_Landscape_Flipped:
						ORIENTATION_TOP(DWORD, cyHeight, cxWidth)
						ORIENTATION_CORE(Landscape_Flipped)
						RENDER_TEST_TRANSPARENCY_AND_WRITE(DWORD)
						ORIENTATION_TAIL(DWORD, -=, cbyPitch);
						break;
					}
					return true;
				}
			}
			break;

		case 16:	// 16 bit display.
			if (	(DispProperties.ffFormat & kfDirect555)
				||	(DispProperties.ffFormat & kfDirect565)
				)
			{
				RENDERSETUP(WORD, 16);
				case EO_Portrait_Normal:
					ORIENTATION_TOP(WORD, cxWidth, cyHeight)
					ORIENTATION_CORE(Portrait_Normal)
					RENDER_TEST_TRANSPARENCY_AND_WRITE(WORD)
					ORIENTATION_TAIL(WORD, +=, cbxPitch);
					break;

				case EO_Portrait_Flipped:
					break;

				case EO_Landscape_Normal:
					ORIENTATION_TOP(WORD, cyHeight, cxWidth)
					ORIENTATION_CORE(Landscape_Normal)
					RENDER_TEST_TRANSPARENCY_AND_WRITE(WORD)
					ORIENTATION_TAIL(WORD, +=, cbyPitch);
					break;

				case EO_Landscape_Flipped:
					ORIENTATION_TOP(WORD, cyHeight, cxWidth)
					ORIENTATION_CORE(Landscape_Flipped)
					RENDER_TEST_TRANSPARENCY_AND_WRITE(WORD)
					ORIENTATION_TAIL(WORD, -=, cbyPitch);
					break;
				}
				return true;
			}
			break;

		case 8:		// 8 bit display.
			RENDERSETUP(unsigned char, 8);
			case EO_Portrait_Normal:
				ORIENTATION_TOP(unsigned char, cxWidth, cyHeight)
				ORIENTATION_CORE(Portrait_Normal)
				RENDER_TEST_TRANSPARENCY_AND_WRITE(unsigned char)
				ORIENTATION_TAIL(unsigned char, +=, cbxPitch);
				break;

			case EO_Portrait_Flipped:
				break;

			case EO_Landscape_Normal:
				ORIENTATION_TOP(unsigned char, cyHeight, cxWidth)
				ORIENTATION_CORE(Landscape_Normal)
				RENDER_TEST_TRANSPARENCY_AND_WRITE(unsigned char)
				ORIENTATION_TAIL(unsigned char, +=, cbyPitch);
				break;

			case EO_Landscape_Flipped:
				ORIENTATION_TOP(unsigned char, cyHeight, cxWidth)
				ORIENTATION_CORE(Landscape_Flipped)
				RENDER_TEST_TRANSPARENCY_AND_WRITE(unsigned char)
				ORIENTATION_TAIL(unsigned char, -=, cbyPitch)
				break;
			}
			return true;
		}
	}
	return false;
}

// ----
// Draws a sprite, but ignores any transparency info it may contain.
bool	CGraphics::DrawNonTransSprite(	int				_XPos, 
										int				_YPos,
										CGraphicsData*	_GraphicsData,
										RECT&			_Region,
										int				_Flip)
{
#ifdef DISABLE_GRAPHICS
	return true;
#endif

	if (SoundSystemRef)
		SoundSystemRef->Process();

	if (/*	(GraphicsMemAddress)				// No pointer to display memory.
		&&	*/(_GraphicsData)						// No graphical data
		)
	{
		void*	WriteBuffer;
		if (ActiveScreen == 0)
		{
			WriteBuffer = GraphicsMemAddress;
		}
		else
		{
			if (DispProperties.cbyPitch < 0)
				WriteBuffer = (char*)Screens[ActiveScreen] + ((DispProperties.cyHeight - 1) * (-DispProperties.cbyPitch));
			else
				WriteBuffer = Screens[ActiveScreen];
		}

		if (!WriteBuffer)
			return false;

		int		ReadOffsetX, ReadOffsetY;
		int		ReadDirectionX, ReadDirectionY;
		int		x, y;

		if (_Flip)
		{
			if (_Flip & SPRITE_ROTATE_180)
			{
				ReadOffsetX	= _Region.right -1;
				ReadOffsetY	= _Region.bottom -1;
				ReadDirectionX = -1;
				ReadDirectionY = -1;
			}
			else if (_Flip & SPRITE_FLIP_VERTICAL)
			{
				ReadOffsetX	= _Region.left;
				ReadOffsetY	= _Region.bottom -1;
				ReadDirectionX = 1;
				ReadDirectionY = -1;
			}
			else if (_Flip & SPRITE_FLIP_HORIZONTAL)
			{
				ReadOffsetX	= _Region.right - 1;
				ReadOffsetY	= _Region.top;

				ReadDirectionX = -1;
				ReadDirectionY = 1;
			}
			else
			{
				ASSERT(0);
				return false;
			}
		}
		else
		{
			ReadOffsetX	= _Region.left;
			ReadOffsetY	= _Region.top;
			ReadDirectionX = 1;
			ReadDirectionY = 1;
		}

		switch (DispProperties.cBPP)
		{
		case 32:	// 32 bit display.
			if (DispProperties.ffFormat & kfDirect888)
			{
				if (DispProperties.cbxPitch == 3)	// 24 bit
				{
					RENDERSETUP(DWORD, 32);
					case EO_Portrait_Normal:
						ORIENTATION_TOP(DWORD, cxWidth, cyHeight)
						ORIENTATION_CORE(Portrait_Normal)
								)
								{
									*WriteAddress		= ((*ReadAddress) >> 16);
									*(WriteAddress + 1)	= ((*ReadAddress) >> 8);
									*(WriteAddress + 2)	= (*ReadAddress);
								}
						ORIENTATION_TAIL(DWORD, +=, cbxPitch);
						break;

					case EO_Portrait_Flipped:
						break;

					case EO_Landscape_Normal:
						ORIENTATION_TOP(DWORD, cyHeight, cxWidth)
						ORIENTATION_CORE(Landscape_Normal)
								)
								{
									*WriteAddress		= ((*ReadAddress) >> 16);
									*(WriteAddress + 1)	= ((*ReadAddress) >> 8);
									*(WriteAddress + 2)	= (*ReadAddress);
								}
						ORIENTATION_TAIL(DWORD, +=, cbyPitch);
						break;

					case EO_Landscape_Flipped:
						ORIENTATION_TOP(DWORD, cyHeight, cxWidth)
						ORIENTATION_CORE(Landscape_Flipped)
								)
								{
									*WriteAddress		= ((*ReadAddress) >> 16);
									*(WriteAddress + 1)	= ((*ReadAddress) >> 8);
									*(WriteAddress + 2)	= (*ReadAddress);
								}
						ORIENTATION_TAIL(DWORD, -=, cbyPitch);
						break;	
					}
					return true;
				}
				else if (DispProperties.cbxPitch == 4) // 32 bit. (much easier)
				{
					RENDERSETUP(DWORD, 32);
					case EO_Portrait_Normal:
						ORIENTATION_TOP(DWORD, cxWidth, cyHeight)
						ORIENTATION_CORE(Portrait_Normal)
						RENDER_ALWAYS_WRITE(DWORD)
						ORIENTATION_TAIL(DWORD, +=, cbxPitch);
						break;

					case EO_Portrait_Flipped:
						break;

					case EO_Landscape_Normal:
						ORIENTATION_TOP(DWORD, cyHeight, cxWidth)
						ORIENTATION_CORE(Landscape_Normal)
						RENDER_ALWAYS_WRITE(DWORD)
						ORIENTATION_TAIL(DWORD, +=, cbyPitch);
						break;

					case EO_Landscape_Flipped:
						ORIENTATION_TOP(DWORD, cyHeight, cxWidth)
						ORIENTATION_CORE(Landscape_Flipped)
						RENDER_ALWAYS_WRITE(DWORD)
						ORIENTATION_TAIL(DWORD, -=, cbyPitch);
						break;
					}
					return true;
				}
			}
			break;

		case 16:	// 16 bit display.
			if (	(DispProperties.ffFormat & kfDirect555)
				||	(DispProperties.ffFormat & kfDirect565)
				)
			{
				RENDERSETUP(WORD, 16);
				case EO_Portrait_Normal:
					ORIENTATION_TOP(WORD, cxWidth, cyHeight)
					ORIENTATION_CORE(Portrait_Normal)
					RENDER_ALWAYS_WRITE(WORD)
					ORIENTATION_TAIL(WORD, +=, cbxPitch);
					break;

				case EO_Portrait_Flipped:
					break;

				case EO_Landscape_Normal:
					ORIENTATION_TOP(WORD, cyHeight, cxWidth)
					ORIENTATION_CORE(Landscape_Normal)
					RENDER_ALWAYS_WRITE(WORD)
					ORIENTATION_TAIL(WORD, +=, cbyPitch);
					break;

				case EO_Landscape_Flipped:
					ORIENTATION_TOP(WORD, cyHeight, cxWidth)
					ORIENTATION_CORE(Landscape_Flipped)
					RENDER_ALWAYS_WRITE(WORD)
					ORIENTATION_TAIL(WORD, -=, cbyPitch);
					break;
				}
				return true;
			}
			break;

		case 8:		// 8 bit display.
			RENDERSETUP(unsigned char, 8);
			case EO_Portrait_Normal:
				ORIENTATION_TOP(unsigned char, cxWidth, cyHeight)
				ORIENTATION_CORE(Portrait_Normal)
				RENDER_ALWAYS_WRITE(unsigned char)
				ORIENTATION_TAIL(unsigned char, +=, cbxPitch);
				break;

			case EO_Portrait_Flipped:
				break;

			case EO_Landscape_Normal:
				ORIENTATION_TOP(unsigned char, cyHeight, cxWidth)
				ORIENTATION_CORE(Landscape_Normal)
				RENDER_ALWAYS_WRITE(unsigned char)
				ORIENTATION_TAIL(unsigned char, +=, cbyPitch);
				break;

			case EO_Landscape_Flipped:
				ORIENTATION_TOP(unsigned char, cyHeight, cxWidth)
				ORIENTATION_CORE(Landscape_Flipped)
				RENDER_ALWAYS_WRITE(unsigned char)
				ORIENTATION_TAIL(unsigned char, -=, cbyPitch)
				break;
			}
			return true;
		}
	}
	return false;
}


// ----
// Draws a string.
#if (defined(PROJECT) && (PROJECT == SPEEDBALL2))
bool	CGraphics::DrawString(	CFont*		_Font,
								CString*	_String,
								int			_XPos,
								int			_YPos,
								DWORD		_StyleBits,
								POINT*		_Point,
								bool*		_PointIsInDrawRegion)
#else
bool	CGraphics::DrawString(	CFont*		_Font,
								CString*	_String,
								int			_XPos,
								int			_YPos,
								DWORD		_StyleBits,
								int			_Right,
								int			_Bottom,
								POINT*		_Point,
								bool*		_PointIsInDrawRegion)
#endif
{
#ifdef DISABLE_GRAPHICS
	return true;
#endif

//	if (SoundSystemRef)
//		SoundSystemRef->Process();

	int		i, Width;
	RECT	bob;
	RECT	CharRegion;
	int		XDest, YDest;
	CString LineString;
	CString	RemainingString;
	CString	Token;
#ifdef INCLUDE_UNFINISHED_CODE
	bool	PIIDRRet = false;
#endif

	if (_Font)
	{
		CGraphicsData* FontGraphicHandle;
				
		switch (_StyleBits)
		{
		case TEXT_LEFT:	
		case (TEXT_RIGHT | TEXT_CENTRED):	// Both right and centre set
			// Text should use the x, y values as hard left values. 
			// That is, the text should appear to the right of the x, y values.
			bob.top		= _YPos;
			bob.left	= _XPos;
			bob.bottom	= _YPos + _Font->GetLineHeight();
			bob.right	= _XPos + _Font->GetStringWidth(_String);

			XDest = bob.left;
			YDest = bob.top;
			break;

		case TEXT_RIGHT:
			// x is hard right
			bob.top		= _YPos;
			bob.left	= _XPos - _Font->GetStringWidth(_String);
			bob.bottom	= _YPos + _Font->GetLineHeight();
			bob.right	= _XPos;

			XDest = bob.left;
			YDest = bob.top;
			break;

		case TEXT_CENTRED:
			bob.top		= _YPos;
			bob.left	= _XPos - (_Font->GetStringWidth(_String) >> 1);
			bob.bottom	= _YPos + _Font->GetLineHeight();
			bob.right	= _XPos + (_Font->GetStringWidth(_String) >> 1);
			
			XDest = bob.left;
			YDest = bob.top;
			break;

			// New for projects after Speedball2
#if (!defined(PROJECT) || (PROJECT != SPEEDBALL2))
#ifdef INCLUDE_UNFINISHED_CODE
		case TEXT_BOXED:
			if (	(_Right <= _XPos)
				||	(_Bottom <= _YPos))
				return false;

			// Now, take the string we have and see how 
			// many words we can fit on a line - then send 
			// those words recursive to this method
			
			RemainingString = _String;
			LineString.Clear();
			Token.Clear();
			
			YDest = _YPos;
			while (		(RemainingString.GetString() || Token.GetString())
					&&	((YDest + _Font->GetLineHeight()) < _Bottom)
					)
			{
				while (		(RemainingString.GetString() || Token.GetString())
						&&	(!Token.DoesStringContainNewLine())
						&&	(	(	_Font->GetStringWidth(&LineString) 
								+	_Font->GetStringWidth(&Token)
								)	< (_Right - _XPos)
							)
						)
				{
					if (Token.GetString())
					{
						LineString += Token;
						LineString += " ";
					}
					RemainingString.MoveFirstWord(&Token);
				}

				ODCSN((&LineString));

				DrawString(	_Font, 
							&LineString, 
							_XPos, 
							YDest, 
							TEXT_LEFT,
							-1, 
							-1,
							_Point,
							_PointIsInDrawRegion);

				if (_PointIsInDrawRegion)
					PIIDRRet |= *_PointIsInDrawRegion;

				YDest += _Font->GetLineHeight();

				// Make sure the next word goes out on the next line.
				LineString = Token;
				LineString += " ";
				Token.Clear();
			}
			
			if (_PointIsInDrawRegion)
				*_PointIsInDrawRegion = PIIDRRet;

			// All done
			return true;
			break;
#endif
#endif
		default: 
			return false;
		}

		int OriginX = XDest;

		// Now we've worked out the start rendering positions 
		// we had better actually render the string.
		for (i = 0; i < (int)_String->GetStringLength(); i++)
		{
			_Font->GetCharData(_String->GetString()[i], &FontGraphicHandle, &CharRegion, &Width);
			if (_String->GetString()[i] == '\n') 
			{
				// New line
				XDest = OriginX;
				YDest += (_Font->GetLineHeight());
				bob.bottom += (_Font->GetLineHeight());	// Move the bottom bound down.
			}
			else
			{
				if (FontGraphicHandle)
					DrawSprite(XDest, YDest, FontGraphicHandle, CharRegion);
				XDest += Width;
			}
		}
		
		if (	(_Point)
			&&	(_PointIsInDrawRegion)
			)
		{
			if (	(_Point->x >= 0)			// Is the mouse on the screen?
				&&	(_Point->y >= 0)

				&&	(_Point->x > bob.left)		// Yes, so, is the mouse in the rectangle?
				&&	(_Point->x < bob.right) 
				&&	(_Point->y > bob.top) 
				&&	(_Point->y < bob.bottom) 
				)
				*_PointIsInDrawRegion = true;	// Yes.
			else
				*_PointIsInDrawRegion = false;	// No, to one of the above.
		}


		return true;//DrawRect(&bob, 0, 0, 0);
	}

	return false;
}

// ----
// This method is basically a hack to implement the 
// functionality of a function in Populous. 
// It is not intended for any other use.
bool	CGraphics::ToggleIcon(	int				_XPos, 
								int				_YPos,
								CGraphicsData*	_GraphicsData,
								RECT&			_Region,
								int				_Flip)
{
#ifdef DISABLE_GRAPHICS
	return true;
#endif

	if (SoundSystemRef)
		SoundSystemRef->Process();

	if (/*	(GraphicsMemAddress)				// No pointer to display memory.
		&&*/	(_GraphicsData)						// No graphical data
		)
	{
		void*	WriteBuffer;
		if (ActiveScreen == 0)
		{
			WriteBuffer = GraphicsMemAddress;
		}
		else
		{
			if (DispProperties.cbyPitch < 0)
				WriteBuffer = (char*)Screens[ActiveScreen] + ((DispProperties.cyHeight - 1) * (-DispProperties.cbyPitch));
			else
				WriteBuffer = Screens[ActiveScreen];
		}

		if (!WriteBuffer)
			return false;

		int		ReadOffsetX, ReadOffsetY;
		int		ReadDirectionX, ReadDirectionY;
		int		x, y;

		if (_Flip)
		{
			if (_Flip & SPRITE_ROTATE_180)
			{
				ReadOffsetX	= _Region.right -1;
				ReadOffsetY	= _Region.bottom -1;
				ReadDirectionX = -1;
				ReadDirectionY = -1;
			}
			else if (_Flip & SPRITE_FLIP_VERTICAL)
			{
				ReadOffsetX	= _Region.left;
				ReadOffsetY	= _Region.bottom -1;
				ReadDirectionX = 1;
				ReadDirectionY = -1;
			}
			else if (_Flip & SPRITE_FLIP_HORIZONTAL)
			{
				ReadOffsetX	= _Region.right - 1;
				ReadOffsetY	= _Region.top;

				ReadDirectionX = -1;
				ReadDirectionY = 1;
			}
			else
			{
				ASSERT(0);
				return false;
			}
		}
		else
		{
			ReadOffsetX	= _Region.left;
			ReadOffsetY	= _Region.top;
			ReadDirectionX = 1;
			ReadDirectionY = 1;
		}

		switch (DispProperties.cBPP)
		{
		case 32:	// 32 bit display.
			if (DispProperties.ffFormat & kfDirect888)
			{
				if (DispProperties.cbxPitch == 3)	// 24 bit
				{
					RENDERSETUP(DWORD, 32);
					case EO_Portrait_Normal:
						ORIENTATION_TOP(DWORD, cxWidth, cyHeight)
						ORIENTATION_CORE(Portrait_Normal)
								&&	(*ReadAddress == Trans))											\
							{		
								*WriteAddress		= (*(WriteAddress + 0)) ^ 0xFF;
								*(WriteAddress + 1)	= (*(WriteAddress + 1)) ^ 0xFF;
								*(WriteAddress + 2) = (*(WriteAddress + 2)) ^ 0xFF;
							}
						ORIENTATION_TAIL(DWORD, +=, cbxPitch);
						break;

					case EO_Portrait_Flipped:
						break;

					case EO_Landscape_Normal:
						ORIENTATION_TOP(DWORD, cyHeight, cxWidth)
						ORIENTATION_CORE(Landscape_Normal)
								&&	(*ReadAddress == Trans))											\
							{		
								*WriteAddress		= (*(WriteAddress + 0)) ^ 0xFF;
								*(WriteAddress + 1)	= (*(WriteAddress + 1)) ^ 0xFF;
								*(WriteAddress + 2) = (*(WriteAddress + 2)) ^ 0xFF;
							}
						ORIENTATION_TAIL(DWORD, +=, cbyPitch);
						break;

					case EO_Landscape_Flipped:
						ORIENTATION_TOP(DWORD, cyHeight, cxWidth)
						ORIENTATION_CORE(Landscape_Flipped)
								&&	(*ReadAddress == Trans))											\
							{		
								*WriteAddress		= (*(WriteAddress + 0)) ^ 0xFF;
								*(WriteAddress + 1)	= (*(WriteAddress + 1)) ^ 0xFF;
								*(WriteAddress + 2) = (*(WriteAddress + 2)) ^ 0xFF;
							}
						ORIENTATION_TAIL(DWORD, -=, cbyPitch);
						break;	
					}
					return true;
				}
				else if (DispProperties.cbxPitch == 4) // 32 bit. (much easier)
				{
					RENDERSETUP(DWORD, 32);
					case EO_Portrait_Normal:
						ORIENTATION_TOP(DWORD, cxWidth, cyHeight)
						ORIENTATION_CORE(Portrait_Normal)
						RENDER_TOGGLE_DATA(DWORD)
						ORIENTATION_TAIL(DWORD, +=, cbxPitch);
						break;

					case EO_Portrait_Flipped:
						break;

					case EO_Landscape_Normal:
						ORIENTATION_TOP(DWORD, cyHeight, cxWidth)
						ORIENTATION_CORE(Landscape_Normal)
						RENDER_TOGGLE_DATA(DWORD)
						ORIENTATION_TAIL(DWORD, +=, cbyPitch);
						break;

					case EO_Landscape_Flipped:
						ORIENTATION_TOP(DWORD, cyHeight, cxWidth)
						ORIENTATION_CORE(Landscape_Flipped)
						RENDER_TOGGLE_DATA(DWORD)
						ORIENTATION_TAIL(DWORD, -=, cbyPitch);
						break;
					}
					return true;
				}
			}
			break;

		case 16:	// 16 bit display.
			if (	(DispProperties.ffFormat & kfDirect555)
				||	(DispProperties.ffFormat & kfDirect565)
				)
			{
				RENDERSETUP(WORD, 16);
				case EO_Portrait_Normal:
					ORIENTATION_TOP(WORD, cxWidth, cyHeight)
					ORIENTATION_CORE(Portrait_Normal)
					RENDER_TOGGLE_DATA(WORD)
					ORIENTATION_TAIL(WORD, +=, cbxPitch);
					break;

				case EO_Portrait_Flipped:
					break;

				case EO_Landscape_Normal:
					ORIENTATION_TOP(WORD, cyHeight, cxWidth)
					ORIENTATION_CORE(Landscape_Normal)
					RENDER_TOGGLE_DATA(WORD)
					ORIENTATION_TAIL(WORD, +=, cbyPitch);
					break;

				case EO_Landscape_Flipped:
					ORIENTATION_TOP(WORD, cyHeight, cxWidth)
					ORIENTATION_CORE(Landscape_Flipped)
					RENDER_TOGGLE_DATA(WORD)
					ORIENTATION_TAIL(WORD, -=, cbyPitch);
					break;
				}
				return true;
			}
			break;

		case 8:		// 8 bit display.
			RENDERSETUP(unsigned char, 8);
			case EO_Portrait_Normal:
				ORIENTATION_TOP(unsigned char, cxWidth, cyHeight)
				ORIENTATION_CORE(Portrait_Normal)
				RENDER_TOGGLE_DATA(unsigned char)
				ORIENTATION_TAIL(unsigned char, +=, cbxPitch);
				break;

			case EO_Portrait_Flipped:
				break;

			case EO_Landscape_Normal:
				ORIENTATION_TOP(unsigned char, cyHeight, cxWidth)
				ORIENTATION_CORE(Landscape_Normal)
				RENDER_TOGGLE_DATA(unsigned char)
				ORIENTATION_TAIL(unsigned char, +=, cbyPitch);
				break;

			case EO_Landscape_Flipped:
				ORIENTATION_TOP(unsigned char, cyHeight, cxWidth)
				ORIENTATION_CORE(Landscape_Flipped)
				RENDER_TOGGLE_DATA(unsigned char)
				ORIENTATION_TAIL(unsigned char, -=, cbyPitch)
				break;
			}
			return true;
		}
	}
	return false;


}

// ----
void CGraphics::SaveScreen(CString* _FileName)
{
#ifdef DISABLE_GRAPHICS
	return;
#endif

	unsigned char	*ReadAddress	= ((unsigned char*)GraphicsMemAddress + (0 * DispProperties.cbyPitch) + (0 * DispProperties.cbxPitch));
	//WORD			*ReadAddress	= (WORD*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);
	WORD			Value, RValue, GValue, BValue;
	unsigned char	R, G, B;

	TGAHeader	Header;
	/*
	struct TGAHeader
	{
		BYTE	mIDLength;
		BYTE	mColourMapType;
		BYTE	mImageType;
		BYTE	mColourMapSpec[5];
		WORD	mOriginX;
		WORD	mOriginY;
		WORD	mSizeX;
		WORD	mSizeY;
		BYTE	mPixelDepth;
		BYTE	mImageDescriptor;
	};
	*/
	
	memset((void*)&Header, 0, sizeof(Header));
	Header.mImageType	= 2;
	Header.mSizeX		= DispProperties.cxWidth;
	Header.mSizeY		= DispProperties.cyHeight;
	Header.mPixelDepth	= 24;
		
	// Allocate image memory.
	char	*OutputTGA	= new char[sizeof(Header) + 3 * DispProperties.cxWidth * DispProperties.cyHeight];
	memcpy((void*)OutputTGA, (void*)&Header, sizeof(Header));
	
	char	*WriteAddress = OutputTGA + sizeof(Header);
	for (int y = 0; y < (int)DispProperties.cyHeight; y++)
	{
		for (int x = 0; x < (int)DispProperties.cxWidth; x++)
		{
			Value = *(WORD*)ReadAddress;
			ReadAddress += DispProperties.cbxPitch;
			
			// Process data into 8 bit values
			RValue = (Value & 0xF800);	R = RValue >> 8;	// [11111 000] 000 00000
			GValue = (Value & 0x7E0);	G = GValue >> 3;	// [111111 00] 000
			BValue = (Value & 0x1F);	B = BValue << 3;	// [11111 (000)]

			*WriteAddress = B; WriteAddress++;
			*WriteAddress = G; WriteAddress++;
			*WriteAddress = R; WriteAddress++;
		}
		
		// Prepare for next line.
		ReadAddress	= ((unsigned char*)GraphicsMemAddress + ((y + 1) * DispProperties.cbyPitch));
	}

	FILE* FH = FOPEN(_FileName->GetString(), "wb");
	if (FH)
	{
		fwrite(OutputTGA, sizeof(Header) + 3 * DispProperties.cxWidth * DispProperties.cyHeight, 1, FH);
		fclose(FH);
	}
	delete[] OutputTGA;
}

// ----
UINT		CGraphics::Match8BitPalette(COLORREF	_a)
{
	UINT	mapcol;
	for (int i = NumberColoursMatched; i--;)
	{
		if (ColourMatchArray[i].In == _a)
			return ColourMatchArray[i].Out;
	}

	mapcol = GetNearestPalette(_a);//GetNearestPaletteIndex(Palette, _a);
	if (mapcol != CLR_INVALID)
	{
		if (NumberColoursMatched < 108)
		{
			ColourMatchArray[NumberColoursMatched].In = _a;
			ColourMatchArray[NumberColoursMatched].Out = mapcol;
			NumberColoursMatched++;
		}
		else
		{
			// Free the top end of the array.
			NumberColoursMatched = 16;
			ColourMatchArray[NumberColoursMatched].In = _a;
			ColourMatchArray[NumberColoursMatched].Out = mapcol;
			NumberColoursMatched++;
		}
		
		return mapcol;
	}
	return 0;
}


// ----
UINT	CGraphics::GetNearestPalette(COLORREF _Colour)
{
	int	i, ret;
	ret = 0;
	unsigned char	R, G, B;
	unsigned char	RD, GD, BD;

	if (	(!SystemPaletteCount) 
		||	(!SystemPaletteRef))
		return CLR_INVALID;

	R = (_Colour >> 0) & 0xFF;
	G = (_Colour >> 8) & 0xFF;
	B = (_Colour >> 16) & 0xFF;

	RD = abs((SystemPaletteRef + ret)->peRed - R);
	GD = abs((SystemPaletteRef + ret)->peGreen - G);
	BD = abs((SystemPaletteRef + ret)->peBlue - B);

	for (i = 0; i < SystemPaletteCount; i++)
	{
		if (	(abs((SystemPaletteRef + i)->peRed - R) <= RD)
			&&	(abs((SystemPaletteRef + i)->peGreen - G) <= GD)
			&&	(abs((SystemPaletteRef + i)->peBlue - B) <= BD))
		{
			ret = i;
			RD = abs((SystemPaletteRef + ret)->peRed - R);
			GD = abs((SystemPaletteRef + ret)->peGreen - G);
			BD = abs((SystemPaletteRef + ret)->peBlue - B);
		}
	}
	return ret;
}

