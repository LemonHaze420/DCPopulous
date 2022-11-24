#include	"SafeProgramming.h"
#include	"GraphicsClasses.h"
#include	"Object.h"
#include	<windows.h>
#include	"GameDefines.h"


CGraphicsData::CGraphicsData()
{
	Data = NULL;
	DataWidth = 0;
	DataHeight = 0;
	PixelSize = 0;
	// FileName is pre-initialised by its constructor.
}

CGraphicsData::~CGraphicsData()
{
	SAFELY_DELETE_ARRAY(Data);
}

#define	MASK_COLOUR		((0xFF << 16) | 0xFF)

// ----
// For loading a tga resource.
//#define IMAGE_EXTENSION			".tga"
//#define IMAGE_DATA_EXTENSION	".tgd"	// targa description.


// ----
// Load data / auto dither down to supported display depth.
// Passing in a valid CPlatform object instructs the method to prepend the images directory to the file name. 
// If you do not wish the file name to be modified, pass NULL instead. (This is the case for the CFonr::Init method).
bool	CGraphicsData::Init(CGraphics*		_GraphicsObject, 
							CString*		_FileName,
							CPlatform*		_PlatformRef)
{
	FILE*	FH;
	char*	TempDataBuffer;

	if (	(_GraphicsObject)
		&&	(_FileName) 
		&&	(_FileName->GetStringLength() > 0))
	{
		CString	ImagePath;

		if (_PlatformRef)
			ImagePath = _PlatformRef->GetPath(EP_ImageData);
		ImagePath				+= _FileName;
		ImagePath				+= ".cbd";

		// Load the data, etc.	
		FH = FOPEN(ImagePath.GetString(), "rb");
		if (FH)
		{
			fread(&ImageInfo, sizeof(ImageInfo), 1, FH);
		
			if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect565)
			{
				int R, G, B;

				R = ((ImageInfo.TransparentKey >> 16) & 0xFF) >> 3;
				G = ((ImageInfo.TransparentKey >> 8	) & 0xFF) >> 2;
				B = ((ImageInfo.TransparentKey		) & 0xFF) >> 3;

				ImageInfo.TransparentKey = (R << 11) + (G << 5) + B;
			}
			else if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect555)
			{
				int R, G, B;

				R = ((ImageInfo.TransparentKey >> 16) & 0xFF) >> 3;
				G = ((ImageInfo.TransparentKey >> 8	) & 0xFF) >> 3;
				B = ((ImageInfo.TransparentKey		) & 0xFF) >> 3;

				ImageInfo.TransparentKey = (R << 10) + (G << 5) + B;
			}
			else if (_GraphicsObject->GetDisplay()->cBPP == 8)
			{
				int R, G, B;

				R = (ImageInfo.TransparentKey >> 16	)	& 0xFF;
				G = (ImageInfo.TransparentKey >> 8	)	& 0xFF;
				B = (ImageInfo.TransparentKey		)	& 0xFF;

				if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect)
				{
					R = R >> 5;
					G = G >> 5;
					B = B >> 6;

					// Update for palettized display
					ImageInfo.TransparentKey = (R << 5) + (G << 2) + B;
				}
				else
				{
					COLORREF InColour;
					InColour = (B << 16) + (G << 8) + R;

					// Making the assumption that the palette is a maximum of 256 entries!!!
					ImageInfo.TransparentKey = _GraphicsObject->Match8BitPalette(InColour);//GetNearestPaletteIndex(_GraphicsObject->GetPalette(), InColour);
				}
			}
		
			// Read header
			TGAHeader	header;
			fread(&header, sizeof(header), 1, FH);

			// Store width and height info
			DataWidth	= header.mSizeX;
			DataHeight	= header.mSizeY;

			int	TotalCount;
			fread(&TotalCount, sizeof(TotalCount), 1, FH);
	
			// Read data into buffer, before processing.
			TempDataBuffer = new char[TotalCount * 4];
			if (TempDataBuffer)
			{
				fread(TempDataBuffer, TotalCount * 4, 1, FH);
				fclose(FH);
					
				Data = NULL;
				switch (_GraphicsObject->GetDisplay()->cBPP)
				{
				case 8:
					Data = new char[DataWidth * DataHeight * 1];
					PixelSize = 1;
					break;
				case 16:
					Data = new char[DataWidth * DataHeight * 2];
					PixelSize = 2;
					break;
				case 24:
				case 32:
					Data = new char[DataWidth * DataHeight * 4]; // The data is stored in the format xRGB, reguardless of whether the screen is 24 or 32 bit.
					PixelSize = 4;
					break;
				default:
					Data = NULL;
					PixelSize = 0;
					return false;
				}

				if (Data)
				{
					// Right, got source and destination buffers. 
					// Time to process data.

					char*			Source	= TempDataBuffer;
					//Source	+= (DataHeight - 1) * DataWidth * 3;
					char*			Dest	= Data;
					unsigned char	R, G, B;
					unsigned char	Count;
					EMachine		MType;

					int ReadIn = 0;

					if (_GraphicsObject->GetPlatform())
						MType = _GraphicsObject->GetPlatform()->GetMachineType();
					else
						return false;

					for (int Index = TotalCount; Index--;)
					{
						// Read pixel.
						Count	= *Source;	Source++;
						R		= *Source;	Source++;
						G		= *Source;	Source++;
						B		= *Source;	Source++;

						ReadIn += Count;

						int Loop;

						// Process pixels.
						if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect555)
						{
							R = (R >> 3);
							G = (G >> 3);
							B = (B >> 3);
									
							WORD Value = (R << 10) + (G << 5) + B;
							for (Loop = Count; Loop--;)
							{
								*(WORD*)Dest = Value;
								Dest += sizeof(WORD);
							}
						}
						else if(_GraphicsObject->GetDisplay()->ffFormat & kfDirect565)
						{
							// 565 is in the format RRRRRxGGGGGBBBBB - (This 
							// information was not provided with the 
							// documentation, but is in an article on Microsoft's 
							// website). - Unless, of course, you don't stick 
							// to that format and use another one, such as 
							// Casio. They use RRRRRGGGGGGBBBBB. (Which is what
							// you would expect, but counter to what Microsoft
							// say on their website. God know which is correct.
							R = (R >> 3);
							G = (G >> 2);	// Don't really care about the lowest bit too much.
							B = (B >> 3);

							WORD Value = (R << 11) + (G << 5) + B;

							for (Loop = Count; Loop--;)
							{
								*(WORD*)Dest = Value;
								Dest += sizeof(WORD);
							}
						}
						else if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect888)
						{
							DWORD Value = (R << 16) + (G << 8) + B;

							for (Loop = Count; Loop--;)
							{
								*(DWORD*)Dest = Value;
								Dest += sizeof(DWORD);
							}
						}
						else if (_GraphicsObject->GetDisplay()->cBPP == 8)
						{
							if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect)
							{
								// Direct 8 bit mode. 
								R = (R >> 5);
								G = (G >> 5);
								B = (B >> 6);

								unsigned char Value = (R << 5) + (G << 2) + B;
								for (Loop = Count; Loop--;)
								{
									*Dest = Value;	
									Dest++;
								}
							}
							else
							{
								// Palettized 8 bit mode.
								COLORREF InColour;
								InColour = (B << 16) + (G << 8) + R;

								// Making the assumption that the palette is a maximum of 256 entries!!!
								UINT mapcol = _GraphicsObject->Match8BitPalette(InColour);
									
								//if (mapcol != CLR_INVALID)
								{
									for (Loop = Count; Loop--;)
									{
										*(unsigned char*)Dest = mapcol;
										Dest++;
									}
								}
								/*else
								{
									for (Loop = Count; Loop--;)
									{
										*Dest = 0;
										Dest++;
									}
								}*/
							}
						}
						else
						{
							// This display mode is currently not supported.
							return false;
						}
					}

					SAFELY_DELETE_ARRAY(TempDataBuffer);
					ErrorCode = OBJECT_NO_ERROR;

					ASSERT(ReadIn == (DataWidth * DataHeight));
					return true;
				}
				SAFELY_DELETE_ARRAY(TempDataBuffer);
			}
			else
			{
				fclose(FH);
			}
		}
		else
		{
			ODS("Failed to load image: ");
			ODCSN((&ImagePath));
		}
	}
	
	ErrorCode = GetLastError();
	return false;
}

#if (defined(PROJECT) && (PROJECT == POPULOUS))
// ----
// Load up Populous specific data - yes it would be better
// to split this functionality off into a DLL, but I ain't 
// got the time.
bool	CGraphicsData::InitAsPopulous(	CGraphics*	_GraphicsObject, 
										FILE*		_FH,
										int			_DataLength,
										int			_ImageWidth,
										bool		_DataHasMask)
{
	char*			TempBuffer;
	char*			Index;
	LONGLONG		Plane[4];
	int				i, j;
	unsigned char	R, G, B;
	char*			Dest;
	int				StripSize;
	int				StripWidth;

	if (_DataHasMask)
		StripWidth = 5;
	else
		StripWidth = 4;

	// These comments only apply to 32 pixel wide images

	// File format looks like this... (as far as i can see...).
	// Long block of data.
	
	// 4 bytes for mask (4 * 8 = 32 - 32 pixels).
	// 4 bytes for plane0
	// 4 bytes for plane1
	// 4 bytes for plane2
	// 4 bytes for plane3

	// Repeat until data runs out. (i.e. _DataLength -= (5 * 4);)

	SAFELY_DELETE_ARRAY(Data);

	if (	(_GraphicsObject)
		&&	(_FH)
		&&	(	(_ImageWidth == 64) 
			||	(_ImageWidth == 32) 
			||	(_ImageWidth == 16)
			||	(_ImageWidth == 8))
		)
	{
		// StripSize is the number of bytes per plane, 
		// per horizontal line of the sprite.
		StripSize = (_ImageWidth >> 3);

		TempBuffer = new char[_DataLength];
		if (!TempBuffer)
			return false;

		DataWidth = _ImageWidth;
		DataHeight = _DataLength / (StripSize * StripWidth);

		// Create end buffer.
		Data = NULL;
		switch (_GraphicsObject->GetDisplay()->cBPP)
		{
		case 8:
			Data = new char[DataWidth * DataHeight * 1];
			PixelSize = 1;
			break;
		case 16:
			Data = new char[DataWidth * DataHeight * 2];
			PixelSize = 2;
			break;
		case 24:
		case 32:
			Data = new char[DataWidth * DataHeight * 4]; // The data is stored in the format xRGB, reguardless of whether the screen is 24 or 32 bit.
			PixelSize = 4;
			ImageInfo.TransparentKey = (0xFF << 16) + 0xFF;		// Magenta
			break;
		default:
			Data = NULL;
			PixelSize = 0;
			return false;
		}

		// Hmmm, this file format uses masks, which doesn't
		// fit my way of doing things - could change the 
		// graphics data on loading (i.e. convert the mask
		// bit to a mask colour...) (This is not implemented).

		// Magenta (or colours very close to it) are always
		// going to be transparent
		ImageInfo.TransparentKey = MASK_COLOUR;	
		if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect565)
		{
			int R, G, B;

			R = ((ImageInfo.TransparentKey >> 16) & 0xFF) >> 3;
			G = ((ImageInfo.TransparentKey >> 8	) & 0xFF) >> 2;
			B = ((ImageInfo.TransparentKey		) & 0xFF) >> 3;

			ImageInfo.TransparentKey = (R << 11) + (G << 5) + B;
		}
		else if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect555)
		{
			int R, G, B;

			R = ((ImageInfo.TransparentKey >> 16) & 0xFF) >> 3;
			G = ((ImageInfo.TransparentKey >> 8	) & 0xFF) >> 3;
			B = ((ImageInfo.TransparentKey		) & 0xFF) >> 3;

			ImageInfo.TransparentKey = (R << 10) + (G << 5) + B;
		}
		else if (_GraphicsObject->GetDisplay()->cBPP == 8)
		{
			int R, G, B;

			R = (ImageInfo.TransparentKey >> 16	)	& 0xFF;
			G = (ImageInfo.TransparentKey >> 8	)	& 0xFF;
			B = (ImageInfo.TransparentKey		)	& 0xFF;

			if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect)
			{
				R = R >> 5;
				G = G >> 5;
				B = B >> 6;

				// Update for palettized display
				ImageInfo.TransparentKey = (R << 5) + (G << 2) + B;
			}
			else
			{
				COLORREF InColour;
				InColour = (B << 16) + (G << 8) + R;

				// Making the assumption that the palette is a maximum of 256 entries!!!
				ImageInfo.TransparentKey = _GraphicsObject->Match8BitPalette(InColour);//GetNearestPaletteIndex(_GraphicsObject->GetPalette(), InColour);
			}
		}

		if (!Data)
			return false;

		// Load the data, etc.	
		if (fread(TempBuffer, _DataLength, 1, _FH) != 1)
			return false;

		// ----
		Dest	= Data;
		Index = TempBuffer;
		while (_DataLength >= (StripSize * StripWidth))
		{
			LONGLONG Mask = 0;
			if (_DataHasMask)
			{
				for (i = 0; i < StripSize; i++)
				{
					Mask = (Mask << 8) | *(unsigned char*)Index;
					Index++;
				}
			}

			for (i = 0; i < 4; i++)	// Data is in 4 planes
			{
				Plane[i] = 0;
				for (j = 0; j < StripSize; j++)	// Count across the image width.
				{
					Plane[i] = ((Plane[i] << 8) | (*(unsigned char*)Index));
					Index++;
				}
			}

			// We now have 32 pixels worth of data, in four planes (16 bit colour).
			// Need to map from plane colour to direct mode colour.

			// Now loop through extracting / mapping plane info.
			for (i = _ImageWidth; i--;)
			{
				bool	MaskBit = (Mask >> i) & 0x01;
				DWORD	Colour;

				if (!MaskBit)
				{
					int PaletteIndex =	(	(((Plane[3] >> i) & 0x01) << 3)
										|	(((Plane[2] >> i) & 0x01) << 2)
										|	(((Plane[1] >> i) & 0x01) << 1)
										|	(((Plane[0] >> i) & 0x01) << 0)
										);

					// Now map the PaletteIndex to a colour, via the palette.
					Colour = _GraphicsObject->PopulousToPal(PaletteIndex);
				}
				else
					Colour = MASK_COLOUR;	// Mask by colour.

				// Read pixel.
				R		= (Colour >> 16) & 0xFF;
				G		= (Colour >> 8) & 0xFF;
				B		= Colour & 0xFF;

				// Process pixels.
				if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect555)
				{
					R = (R >> 3);
					G = (G >> 3);
					B = (B >> 3);
							
					WORD Value = (R << 10) + (G << 5) + B;
					*(WORD*)Dest = Value;
					Dest += sizeof(WORD);
				}
				else if(_GraphicsObject->GetDisplay()->ffFormat & kfDirect565)
				{
					R = (R >> 3);
					G = (G >> 3);
					B = (B >> 3);

					WORD Value = (R << 11) + (G << 6) + B;

					*(WORD*)Dest = Value;
					Dest += sizeof(WORD);
				}
				else if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect888)
				{
					DWORD Value = (R << 16) + (G << 8) + B;

					*(DWORD*)Dest = Value;
					Dest += sizeof(DWORD);
				}
				else if (_GraphicsObject->GetDisplay()->cBPP == 8)
				{
					if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect)
					{
						// Direct 8 bit mode. 
						R = (R >> 5);
						G = (G >> 5);
						B = (B >> 6);

						unsigned char Value = (R << 5) + (G << 2) + B;
						*Dest = Value;	
						Dest++;
					}
					else
					{
						// Palettized 8 bit mode.
						COLORREF InColour;
						InColour = (B << 16) + (G << 8) + R;

						// Making the assumption that the palette is a maximum of 256 entries!!!
						UINT mapcol = _GraphicsObject->Match8BitPalette(InColour);
							
						//if (mapcol != CLR_INVALID)
						{
							*(unsigned char*)Dest = mapcol;
							Dest++;
						}
					}
				}
				else
				{
					// This display mode is currently not supported.
					return false;
				}
			}

			// Increment / decrement counters 
			//DataHeight++;
			_DataLength -= (StripSize * StripWidth);
		}
		return true;
	}
	return false;
}

// ----
bool			CGraphicsData::InitAsPopulous32(CGraphics*	_GraphicsObject, 
												FILE*		_FH,
												int			_DataLength)
{
	char*			TempBuffer;
	char*			Index;
	LONGLONG		Plane[4];
	int				i, j;
	unsigned char	R, G, B;
	char*			Dest;
	int				StripSize;
	int				StripWidth;

	StripWidth = 5;
	int ImageWidth = 32;
	
	SAFELY_DELETE_ARRAY(Data);
	if (	(_GraphicsObject)
		&&	(_FH))
	{
		// StripSize is the number of bytes per plane, 
		// per horizontal line of the sprite.
		StripSize = (ImageWidth >> 4);

		TempBuffer = new char[_DataLength];
		if (!TempBuffer)
			return false;

		DataWidth = ImageWidth;
		DataHeight = _DataLength / (StripSize * StripWidth);

		// Create end buffer.
		Data = NULL;
		switch (_GraphicsObject->GetDisplay()->cBPP)
		{
		case 8:
			Data = new char[DataWidth * DataHeight * 1];
			PixelSize = 1;
			break;
		case 16:
			Data = new char[DataWidth * DataHeight * 2];
			PixelSize = 2;
			break;
		case 24:
		case 32:
			Data = new char[DataWidth * DataHeight * 4]; // The data is stored in the format xRGB, reguardless of whether the screen is 24 or 32 bit.
			PixelSize = 4;
			ImageInfo.TransparentKey = (0xFF << 16) + 0xFF;		// Magenta
			break;
		default:
			Data = NULL;
			PixelSize = 0;
			return false;
		}

		// Hmmm, this file format uses masks, which doesn't
		// fit my way of doing things - could change the 
		// graphics data on loading (i.e. convert the mask
		// bit to a mask colour...) (This is not implemented).

		// Magenta (or colours very close to it) are always
		// going to be transparent
		ImageInfo.TransparentKey = MASK_COLOUR;	
		if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect565)
		{
			int R, G, B;

			R = ((ImageInfo.TransparentKey >> 16) & 0xFF) >> 3;
			G = ((ImageInfo.TransparentKey >> 8	) & 0xFF) >> 2;
			B = ((ImageInfo.TransparentKey		) & 0xFF) >> 3;

			ImageInfo.TransparentKey = (R << 11) + (G << 5) + B;
		}
		else if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect555)
		{
			int R, G, B;

			R = ((ImageInfo.TransparentKey >> 16) & 0xFF) >> 3;
			G = ((ImageInfo.TransparentKey >> 8	) & 0xFF) >> 3;
			B = ((ImageInfo.TransparentKey		) & 0xFF) >> 3;

			ImageInfo.TransparentKey = (R << 10) + (G << 5) + B;
		}
		else if (_GraphicsObject->GetDisplay()->cBPP == 8)
		{
			int R, G, B;

			R = (ImageInfo.TransparentKey >> 16	)	& 0xFF;
			G = (ImageInfo.TransparentKey >> 8	)	& 0xFF;
			B = (ImageInfo.TransparentKey		)	& 0xFF;

			if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect)
			{
				R = R >> 5;
				G = G >> 5;
				B = B >> 6;

				// Update for palettized display
				ImageInfo.TransparentKey = (R << 5) + (G << 2) + B;
			}
			else
			{
				COLORREF InColour;
				InColour = (B << 16) + (G << 8) + R;

				// Making the assumption that the palette is a maximum of 256 entries!!!
				ImageInfo.TransparentKey = _GraphicsObject->Match8BitPalette(InColour);//GetNearestPaletteIndex(_GraphicsObject->GetPalette(), InColour);
			}
		}

		if (!Data)
			return false;

		// Load the data, etc.	
		if (fread(TempBuffer, _DataLength, 1, _FH) != 1)
			return false;

		// ----
		Dest	= Data;
		Index = TempBuffer;
		while (_DataLength >= ((StripSize * StripWidth)))
		{
		//	ODS("Processing Sprite32 chuck at offset ");
		//	ODIN((int)(Index - TempBuffer));
		//	Sleep(1);

			LONGLONG Mask = 0;
			for (i = 0; i < StripSize; i++)
			{
				Mask = (Mask << 8) | *(unsigned char*)Index;
				Index++;
			}
			/*
			for (i = 0; i < 4; i++)
			{
				Plane[i] = 0;
				for (j = 0; j < StripSize; j++)	// Count across the image width.
				{
					Plane[i] = ((Plane[i] << 8) | (*(unsigned char*)Index));
					Index++;
				}
			}*/

			// We now have 32 pixels worth of data, in four planes (16 bit colour).
			// Need to map from plane colour to direct mode colour.

			// Now loop through extracting / mapping plane info.
			for (i = ImageWidth >> 1; i--;)
			{
				bool	MaskBit = (Mask >> i) & 0x01;
				DWORD	Colour;

				if ((i > 0) && !((i + 1) % 8))
				{
					for (j = 0; j < 4; j++)
					{
						Plane[j] = (*(unsigned char*)Index);
						Index++;
					}
				}
				
				/*
				if ((i) && (i % 2))
				{
					Plane[0] = (*(unsigned char*)Index);
					Index++;
				}
				else
				{
					Plane[0] = Plane[0] >> 4;
				}*/

				if (!MaskBit)
				{
					int pia = (i % 8);
					int piaa[4];
					piaa[0] = Plane[0] >> pia;
					piaa[1] = Plane[1] >> pia;
					piaa[2] = Plane[2] >> pia;
					piaa[3] = Plane[3] >> pia;

					int PaletteIndex =	(	((piaa[3] & 0x01) << 3)
										|	((piaa[2] & 0x01) << 2)
										|	((piaa[1] & 0x01) << 1)
										|	((piaa[0] & 0x01) << 0)
										) /*Plane[0] & 0x0F*/;

					// Now map the PaletteIndex to a colour, via the palette.
					Colour = _GraphicsObject->PopulousToPal(PaletteIndex);
				}
				else
					Colour = MASK_COLOUR;	// Mask by colour.

				// Read pixel.
				R		= (Colour >> 16) & 0xFF;
				G		= (Colour >> 8) & 0xFF;
				B		= Colour & 0xFF;

				// Process pixels.
				if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect555)
				{
					R = (R >> 3);
					G = (G >> 3);
					B = (B >> 3);
							
					WORD Value = (R << 10) + (G << 5) + B;
					*(WORD*)Dest = Value;
					Dest += sizeof(WORD);
				}
				else if(_GraphicsObject->GetDisplay()->ffFormat & kfDirect565)
				{
					R = (R >> 3);
					G = (G >> 3);
					B = (B >> 3);

					WORD Value = (R << 11) + (G << 6) + B;

					*(WORD*)Dest = Value;
					Dest += sizeof(WORD);
				}
				else if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect888)
				{
					DWORD Value = (R << 16) + (G << 8) + B;

					*(DWORD*)Dest = Value;
					Dest += sizeof(DWORD);
				}
				else if (_GraphicsObject->GetDisplay()->cBPP == 8)
				{
					if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect)
					{
						// Direct 8 bit mode. 
						R = (R >> 5);
						G = (G >> 5);
						B = (B >> 6);

						unsigned char Value = (R << 5) + (G << 2) + B;
						*Dest = Value;	
						Dest++;
					}
					else
					{
						// Palettized 8 bit mode.
						COLORREF InColour;
						InColour = (B << 16) + (G << 8) + R;

						// Making the assumption that the palette is a maximum of 256 entries!!!
						UINT mapcol = _GraphicsObject->Match8BitPalette(InColour);
							
						//if (mapcol != CLR_INVALID)
						{
							*(unsigned char*)Dest = mapcol;
							Dest++;
						}
					}
				}
				else
				{
					// This display mode is currently not supported.
					return false;
				}
			}

			// Increment / decrement counters 
			//DataHeight++;
			_DataLength -= (StripSize * StripWidth);
		}
		return true;
	}
	return false;
}


// Loads and processes a Populous screenm graphic.
bool			CGraphicsData::InitAsPopulousScreen(CGraphics*	_GraphicsObject, 
													FILE*		_FH,
													int			_DataLength,
													int			_ImageWidth,
													int			_ColourDepth)
{
	char*			TempBuffer;
	char*			Index;
	int				i;
	unsigned char	R, G, B;
	char*			Dest;
	int				DataRemaining;

	// These comments only apply to 32 pixel wide images

	// File format looks like this... (as far as i can see...).
	// Long block of data.
	
	// 4 bytes for plane0
	// 4 bytes for plane1
	// 4 bytes for plane2
	// 4 bytes for plane3

	// Repeat until data runs out. (i.e. _DataLength -= (4 * 4);)

	SAFELY_DELETE_ARRAY(Data);

	if (	(_ColourDepth != 16) 
		&&	(_ColourDepth != 256))
		return false;

	if (	(_GraphicsObject)
		&&	(_FH))
	{
		DataWidth = _ImageWidth;
		DataHeight = (_DataLength / _ImageWidth) * 2;
		DataRemaining = _DataLength;

		TempBuffer = new char[DataRemaining];
		if (!TempBuffer)
			return false;

		// Create end buffer.
		Data = NULL;
		switch (_GraphicsObject->GetDisplay()->cBPP)
		{
		case 8:
			Data = new char[DataWidth * DataHeight * 1];
			PixelSize = 1;
			break;
		case 16:
			Data = new char[DataWidth * DataHeight * 2];
			PixelSize = 2;
			break;
		case 24:
		case 32:
			Data = new char[DataWidth * DataHeight * 4]; // The data is stored in the format xRGB, reguardless of whether the screen is 24 or 32 bit.
			PixelSize = 4;
			break;
		default:
			Data = NULL;
			PixelSize = 0;
			return false;
		}

		if (!Data)
			return false;

		// Load the data, etc.	
		if (fread(TempBuffer, DataRemaining, 1, _FH) != 1)
			return false;

		ImageInfo.TransparentKey = MASK_COLOUR;	
		if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect565)
		{
			int R, G, B;

			R = ((ImageInfo.TransparentKey >> 16) & 0xFF) >> 3;
			G = ((ImageInfo.TransparentKey >> 8	) & 0xFF) >> 2;
			B = ((ImageInfo.TransparentKey		) & 0xFF) >> 3;

			ImageInfo.TransparentKey = (R << 11) + (G << 5) + B;
		}
		else if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect555)
		{
			int R, G, B;

			R = ((ImageInfo.TransparentKey >> 16) & 0xFF) >> 3;
			G = ((ImageInfo.TransparentKey >> 8	) & 0xFF) >> 3;
			B = ((ImageInfo.TransparentKey		) & 0xFF) >> 3;

			ImageInfo.TransparentKey = (R << 10) + (G << 5) + B;
		}
		else if (_GraphicsObject->GetDisplay()->cBPP == 8)
		{
			int R, G, B;

			R = (ImageInfo.TransparentKey >> 16	)	& 0xFF;
			G = (ImageInfo.TransparentKey >> 8	)	& 0xFF;
			B = (ImageInfo.TransparentKey		)	& 0xFF;

			if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect)
			{
				R = R >> 5;
				G = G >> 5;
				B = B >> 6;

				// Update for palettized display
				ImageInfo.TransparentKey = (R << 5) + (G << 2) + B;
			}
			else
			{
				COLORREF InColour;
				InColour = (B << 16) + (G << 8) + R;

				// Making the assumption that the palette is a maximum of 256 entries!!!
				ImageInfo.TransparentKey = _GraphicsObject->Match8BitPalette(InColour);//GetNearestPaletteIndex(_GraphicsObject->GetPalette(), InColour);
			}
		}

		// ----
		Dest	= Data;
		Index = TempBuffer;
		int iii;
		iii = 1 + ((_ColourDepth == 16)?1:0);
		while (DataRemaining >= 1)
		{
			unsigned char TwoPixelColour = *(unsigned char*)Index;
			Index++;
			
			for (i = iii; i--;)
			{
				int PaletteIndex;

				if (_ColourDepth == 16)
				{	
					if (i)
						PaletteIndex = TwoPixelColour & 0x0F;
					else
						PaletteIndex = (TwoPixelColour & 0xF0) >> 4;
				}
				else
				{
					PaletteIndex = (TwoPixelColour & 0x0F);// - ((TwoPixelColour & 0xF0) >> 4);
				}
						
				// Now map the PaletteIndex to a colour, via the palette.
				DWORD	Colour = _GraphicsObject->PopulousToPal(PaletteIndex);

				// Read pixel.
				R		= (Colour >> 16) & 0xFF;
				G		= (Colour >> 8) & 0xFF;
				B		= Colour & 0xFF;

				// Process pixels.
				if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect555)
				{
					R = (R >> 3);
					G = (G >> 3);
					B = (B >> 3);
							
					WORD Value = (R << 10) + (G << 5) + B;
					*(WORD*)Dest = Value;
					Dest += sizeof(WORD);
				}
				else if(_GraphicsObject->GetDisplay()->ffFormat & kfDirect565)
				{
					R = (R >> 3);
					G = (G >> 2);
					B = (B >> 3);

					WORD Value = (R << 11) + (G << 5) + B;

					*(WORD*)Dest = Value;
					Dest += sizeof(WORD);
				}
				else if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect888)
				{
					DWORD Value = (R << 16) + (G << 8) + B;

					*(DWORD*)Dest = Value;
					Dest += sizeof(DWORD);
				}
				else if (_GraphicsObject->GetDisplay()->cBPP == 8)
				{
					if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect)
					{
						// Direct 8 bit mode. 
						R = (R >> 5);
						G = (G >> 5);
						B = (B >> 6);

						unsigned char Value = (R << 5) + (G << 2) + B;
						*Dest = Value;	
						Dest++;
					}
					else
					{
						// Palettized 8 bit mode.
						COLORREF InColour;
						InColour = (B << 16) + (G << 8) + R;

						// Making the assumption that the palette is a maximum of 256 entries!!!
						UINT mapcol = _GraphicsObject->Match8BitPalette(InColour);
							
						//if (mapcol != CLR_INVALID)
						{
							*(unsigned char*)Dest = mapcol;
							Dest++;
						}
					}
				}
				else
				{
					// This display mode is currently not supported.
					return false;
				}
			}

			// Decrement counters 
			DataRemaining -= 1;
		}
		return true;
	}
	return false;
}
#endif

// ----
// Will clip offsets to outer bounds of data.
char*	CGraphicsData::GetAddress(int _XOffset, int _OffsetY)
{
	if (Data)
		return (Data + (_OffsetY * DataWidth + _XOffset) * PixelSize);
	else
		return NULL;
}

// ----
DWORD	CGraphicsData::GetTransKey32()
{
	return (ImageInfo.TransparentKey);
}

// ----
WORD	CGraphicsData::GetTransKey16()
{
	return (ImageInfo.TransparentKey & 0xFFFF);
}

// ----
BYTE	CGraphicsData::GetTransKey8()
{
	return (ImageInfo.TransparentKey & 0xFF);
}
