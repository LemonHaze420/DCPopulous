#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include	"SafeProgramming.h"
#include	"Object.h"
#include	<windows.h>
#include	"String.h"
#include	"Platform.h"
#include	"Preferences.h"
#include	"Misc.h"
#include	"BasicTypes.h"
#include	"SoundSystem.h"
#include	"GameDefines.h"
#include	<gx.h>
#include	"creation.h"



// ----
// TGA file header format
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

struct ColourMatch8
{
	COLORREF	In;
	UINT		Out;
};


// ----
// Class declarations.
class CGraphicsData;
class CFont;

// These modes should be mutually exclusive - if they are 
// not used as such then the default (left) shall be adopted.
#define	TEXT_LEFT				0		// Default
#define	TEXT_RIGHT				(1 << 0)	
#define	TEXT_CENTRED			(1 << 1)
#define	TEXT_BOXED				(1 << 2)
								
#define	SPRITE_NORMAL			(0 << 12)
#define SPRITE_FLIP_VERTICAL	(1 << 12)
#define	SPRITE_FLIP_HORIZONTAL	(2 << 12)
#define	SPRITE_ROTATE_180		(4 << 12)

class CGraphics : public CObject
{
public:
	CGraphics();
	~CGraphics();

	// ----
	// Init the object.
	bool	Init(HWND _WindowHandle, HPALETTE _Palette, CPreferences* _PrefRef, CPlatform*	_PlatformRef);

	// ----
	void	AssignSysPalette(PALETTEENTRY		*SystemPalette, int PaletteCount) {SystemPaletteRef = SystemPalette; SystemPaletteCount = PaletteCount;}
	UINT	GetNearestPalette(COLORREF _Colour);

	// ----
	void	AssignPlatform(CPlatform* _Platform);

	// ----
	// Populous work arounds.
	void			SetActivePalette(int _a);
	void			SetActiveScreen(int _a);
	int				GetActiveScreen() {return ActiveScreen;}
	void			CopyScreen(int _Source, int _Destination);
	DWORD			PopulousToPal(int _Index);

	// ---- 
	// Allows the system to do on-the-fly conversion of 
	// coordinates into the new screen orientation.
	void			SetOrientation(EOrientation	_Mode);

	// ----
	EOrientation	GetOrientation();

	// ----
	// If you assign a sound system object then the class 
	// will update the sound buffer everytime a graphics 
	// method is called. This is 'nice' as it should 
	// prevent audio stutter when drawing a large amount 
	// of graphics.
	void	AssignSoundSystem(CSoundSystem*	_SoundSystemRef);

	// ----
	CPlatform*				GetPlatform();

	// ----
	GXDisplayProperties*	GetDisplay();

	// ----
	bool	StartDraw();
	bool	EndDraw();

	// ----
	// Copies the back buffer to the front buffer.
	void	SwapScreens();

	// ----
	void	ForceRedraw();

	// ----
	bool	ClearScreen(UINT R, UINT G, UINT B);

	// ----
	bool	DrawPixel(int _XPos, int _YPos, UINT R, UINT G, UINT B);

	// ----
	bool	DrawRect(RECT* _Region, UINT R, UINT G, UINT B);

	// ----
	bool	DrawSprite(	int				_XPos, 
						int				_YPos,
						CGraphicsData*	_GraphicsData, 
						RECT&			_Region,
						int				_Flip = SPRITE_NORMAL);


	// ----
	// Draw a sprite - with no check for transparency.
	bool	DrawNonTransSprite(	int				_XPos, 
								int				_YPos,
								CGraphicsData*	_GraphicsData,
								RECT&			_Region,
								int				_Flip = SPRITE_NORMAL);


	// ----
	// Draws string
#if (defined(PROJECT) && (PROJECT == SPEEDBALL2))
	bool	DrawString(	CFont*		_Font, 
						CString*	_String,
						int			_XPos,
						int			_YPos,
						DWORD		_StyleBits				= TEXT_LEFT,
						POINT*		_Point					= NULL,
						bool*		_PointIsInDrawRegion	= NULL);
#else
	bool	DrawString(	CFont*		_Font, 
						CString*	_String,
						int			_XPos,
						int			_YPos,
						DWORD		_StyleBits				= TEXT_LEFT,
						int			_Right					= -1,
						int			_Bottom					= -1,
						POINT*		_Point					= NULL,
						bool*		_PointIsInDrawRegion	= NULL);
#endif

	// ----
	// This method is basically a hack to implement the 
	// functionality of a function in Populous. 
	// It is not intended for any other use.
	bool	ToggleIcon(	int				_XPos, 
						int				_YPos,
						CGraphicsData*	_GraphicsData, 
						RECT&			_Region,
						int				_Flip = SPRITE_NORMAL);

	// ----
	void		SaveScreen(CString* _FileName);

	// ----
	UINT		Match8BitPalette(COLORREF	_a);

	// ----
	HPALETTE	GetPalette() {return Palette;}

protected:
	HWND				WindowHandle;
	HPALETTE			Palette;
	void				*GraphicsMemAddress;
	void				*BackBuffer;
	void				*PrimaryBuffer;

	unsigned long		ScreenMemSize;

	// This is here to mimic Populous. 
	// Gonna be bit of a hack I feel.
	void				*ScreenOne;
	void				*ScreenTwo; 
	void				*Screens[4];			
	int					ActiveScreen;

	bool				ScreenOpen;
	bool				ForceFullRedraw;
	CPlatform			*PlatformRef;
	CPreferences		*PreferencesRef;
	CSoundSystem		*SoundSystemRef;

	// Using GetNearestPaletteEntry is very slow, so we 
	// cache the results of previous colour lookups in this
	// array.
	ColourMatch8		ColourMatchArray[1024];
	PALETTEENTRY		*SystemPaletteRef;
	int					SystemPaletteCount;
	int					NumberColoursMatched;

	GXDisplayProperties	DispProperties;
	EOrientation		Orientation;


	// Populous additions
	int					ActivePalette;
};


// ----
struct ImageDataStruct
{
	DWORD TransparentKey;
};

class CGraphicsData : public CObject
{
public:
	CGraphicsData();
	~CGraphicsData();

	// ----
	// Load data / auto dither down to supported display depth.
	virtual bool	Init(	CGraphics*		_GraphicsObject, 
							CString*		_FileName,
							CPlatform*		_PlatformRef);

	// Populous specific code.
#if (defined(PROJECT) && (PROJECT == POPULOUS))
	// ----
	// This method loads and processes (Archemedes) 
	// Populous format graphics. (This should really be an 
	// overloading of Init - in a sub class).

	bool			InitAsPopulous(	CGraphics*	_GraphicsObject, 
									FILE*		_FH,
									int			_DataLength,
									int			_ImageWidth	= 32,
									bool		_DataHasMask = true);

	// ----
	bool			InitAsPopulous32(	CGraphics*	_GraphicsObject, 
										FILE*		_FH,
										int			_DataLength);

	// ----
	// Loads and processes a Populous screenm graphic.
	bool			InitAsPopulousScreen(	CGraphics*	_GraphicsObject, 
											FILE*		_FH,
											int			_DataLength		= 32000,
											int			_ImageWidth		= 320,
											int			_ColourDepth	= 16);

	// ----
	// Converts a populous palette index to a colour.
	DWORD			PopulousToPal(int _Index);
#endif

	// ----
	// Will clip offsets to outer bounds of data.
	virtual char*	GetAddress(int _XOffset, int _OffsetY);

	// ----
	virtual	DWORD	GetTransKey32();
	virtual	WORD	GetTransKey16();
	virtual	BYTE	GetTransKey8();

	virtual int		GetWidth() {return DataWidth;}
	virtual int		GetHeight() {return DataHeight;}


protected:
	char*			Data;
	CString			FileName;		// Might not need this - would save memory usage if removed.
	int				DataHeight, DataWidth;
	int				PixelSize;
	ImageDataStruct	ImageInfo;
};



#define	MAX_CHARS		256

// ----
// Store data for a single char
class	CCharData
{
public:
	SmallRect	Region;
	UINT		Width;	// So we can take the width of an item into account, even if we have no graphical data. - i.e. the space char.
};


// ----
// Hold data about 256 chars.
class	CFontData : public CObject
{
public:
	int			LineHeight;
	CCharData	CharDataArray[MAX_CHARS];
};


// ----
class CFont : public CObject
{
public:
	CFont();
	~CFont();

	// ----
	// Init the object.
	bool	Init(	CPlatform*	_PlatformRef,
					CGraphics*	_GraphicsObject,
					CString*	_FileName);

#if (defined(PROJECT) && (PROJECT == POPULOUS))
	// Create a very simple font 
	bool	InitAsPopulous(	CGraphics*	_GraphicsObject, 
							FILE*		_FH,
							int			_DataLength,
							int			_FontWidth	= 8);
#endif

	// ----
	// Obtain the information require to draw a single character.
	// Unknown characters are ignored, and the method will return false.
	bool	GetCharData(TCHAR			_Char,			// in
						CGraphicsData**	_GraphicsData,	// out
						RECT*			_Region,		// out
						int*			_Width);		// out

	// ----
	// Gets the width of the string text in pixels. 
	// Unknown characters are ignored, and the method will return zero.
	int		GetStringWidth(CString*	_String);
	

	// ----
	int		GetLineHeight();

protected:

	CFontData		FontData;
	CGraphicsData*	FontGraphics;
};


#endif
