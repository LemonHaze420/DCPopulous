#include	"SafeProgramming.h"
#include	"GraphicsClasses.h"
#include	"Object.h"
#include	"BasicTypes.h"
#include	<windows.h>
#include	"String.h"
#include	"Misc.h"

#ifdef ULTRA_FAST_OBJECT
	#pragma message("WARNING: You MUST use font data created with an equal level of optamisation!!!")
#endif

// ----
CFont::CFont()
{
	FontGraphics	= NULL;
}


// ----
CFont::~CFont()
{
	SAFELY_DELETE(FontGraphics);
}


// ----
// Init the object.
bool	CFont::Init(CPlatform*	_PlatformRef,
					CGraphics*	_GraphicsObject,
					CString*	_FileName)
{
	// Clear memory and release any old graphics data. 
	memset((void*)&FontData, 0 , sizeof(FontData));
	SAFELY_DELETE(FontGraphics);

	if (	(_PlatformRef)
		&&	(_GraphicsObject)
		&&	(_FileName) 
		&&	(_FileName->GetStringLength() > 0)
		)
	{
		FILE*	FH;
		CString	DataPath;
		DataPath = _PlatformRef->GetPath(EP_Fonts);
		DataPath += _FileName;
		DataPath += ".dat";

		CString	ImageData;
		ImageData = _PlatformRef->GetPath(EP_Fonts);
		ImageData += _FileName;

		FH = FOPEN(DataPath.GetString(), "rb");
		if (FH)
		{
			if (fread((void*)&FontData, sizeof(FontData), 1, FH) == 1)
			{
				fclose(FH);
				FontData.SetCreation("FontData", "\n\nThis object was loaded\n\n", "CFont", __LINE__);

				// Now for the graphics.
				NEW(FontGraphics, CGraphicsData());
				return SAFELY_RETURN(FontGraphics, Init(_GraphicsObject, &ImageData, NULL));
			}

			// Close file even if read incorrect number of bytes.
			fclose(FH);
		}
	}	// _FileName && _FileName->GetStringLength() > 0

	return false;
}

// ----
// This method loads and processes (Archemedes) 
// Populous format graphics. (This should really be an 
// overloading of Init - in a sub class).
#if (defined(PROJECT) && (PROJECT == POPULOUS))
bool	CFont::InitAsPopulous(	CGraphics*	_GraphicsObject, 
								FILE*		_FH,
								int			_DataLength,
								int			_ImageWidth)
{
	int i;

	// Clear the lot
	for (i = 0; i < MAX_CHARS; i++)
	{
		FontData.CharDataArray[i].Region.left	= 0;
		FontData.CharDataArray[i].Region.top	= 0;
		FontData.CharDataArray[i].Region.right	= 0;
		FontData.CharDataArray[i].Region.bottom	= 0;
		FontData.CharDataArray[i].Width			= 0;
	}
	SAFELY_DELETE(FontGraphics);

	// Define all the standard chars
	for (i = 0; i < 110; i++)
	{
		FontData.CharDataArray[32 + i].Region.left		= 0;
		FontData.CharDataArray[32 + i].Region.top		= i * _ImageWidth;
		FontData.CharDataArray[32 + i].Region.right		= 8;
		FontData.CharDataArray[32 + i].Region.bottom	= (i + 1) * _ImageWidth;
		FontData.CharDataArray[32 + i].Width			= _ImageWidth;
	}

	// Some how the naming has got a little confused.
	FontData.LineHeight = 7;

	// Finally, load the graphics.
	NEW(FontGraphics, CGraphicsData());
	return SAFELY_RETURN(FontGraphics, InitAsPopulous(_GraphicsObject, _FH, _DataLength, _ImageWidth));
}
#endif


// ----
// Obtain the information require to draw a single character.
// Unknown characters are ignored, and the method will return false.
bool	CFont::GetCharData(	TCHAR			_Char,			// in
							CGraphicsData**	_GraphicsData,	// out
							RECT*			_Region,		// out
							int*			_Width)			// out
{
	UINT	Index = (UINT)_Char;

	if (Index < MAX_CHARS)
	{
#if HACKING_ABOUT_WITH_THE_FONTS
#define CHARWIDTH 12

		// This is a bit hacked to work with a test bitmap.
		_Region->top = 0;	
		_Region->bottom = 16;	

		if (Index == 32)
		{
			_Region->top	= 0;
			_Region->left	= 0;
			_Region->bottom	= 0;
			_Region->right	= 0; 
			*_GraphicsData	= NULL;
			*_Width			= CHARWIDTH;
			return true;
		}
		else if ((Index >= 48)  && (Index <= 58))
		{
			// Numeric char
			_Region->left = (25 + Index - 48) * 16;
		}
		else if ((Index >= 65)  && (Index <= 90))
		{
			// upper case
			_Region->left = (Index - 65) * 16;
		}
		else if ((Index >= 97)  && (Index <= 122))
		{
			// lower case
			_Region->left = (Index - 97) * 16;
		}
		else
		{
			_Region->top	= 0;
			_Region->left	= 0;
			_Region->bottom	= 0;
			_Region->right	= 0; 
			*_GraphicsData	= NULL;
			*_Width			= 0;
			return false;
		}

		_Region->right = _Region->left + CHARWIDTH;
		*_GraphicsData	= FontGraphics;
		*_Width = CHARWIDTH;

#else

		*_GraphicsData		= FontGraphics;
		SmallRect* RectPtr	= &(FontData.CharDataArray[Index].Region);
		_Region->top		= RectPtr->top;
		_Region->bottom		= RectPtr->bottom;
		_Region->left		= RectPtr->left;
		_Region->right		= RectPtr->right;
		*_Width				= FontData.CharDataArray[Index].Width;
#endif
		
		return true;	
	}

	// Otherwise, return everything as NULL or 0.
	*_GraphicsData	= NULL;
	_Region->top	= 0;
	_Region->bottom	= 0;
	_Region->left	= 0;
	_Region->right	= 0;
	*_Width			= 0;

	return false;
}


// ----
// Gets the width of the string text in pixels. 
// Unknown characters are ignored, and the method will return zero.
int		CFont::GetStringWidth(CString*	_String)
{
	int ret = 0;
	if (_String)
	{	
		for (int i = _String->GetStringLength(); i--;)
		{
			UINT Index = (UINT)(_String->GetString()[i]);
			if (Index < MAX_CHARS)
			{
				ret += FontData.CharDataArray[Index].Width;
			}
		}
	}
	return ret;
}


// ----
int		CFont::GetLineHeight()
{
	//return 16;
	return FontData.LineHeight;
}