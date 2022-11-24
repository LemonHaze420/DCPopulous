#include	"Sample.h"
#include	"Object.h"
#include	"SafeProgramming.h"
#include	"Platform.h"
#include	"String.h"
#include	"Misc.h"
#include	<windows.h>

#define UNSUPPORTED_NUMBER_OF_BITS_PER_SAMPLE	(1001)

// ----
CSample::CSample()
{
	PlatformRef	= NULL;
	Data		= NULL;
	DataLength	= 0;

	LoopStart	= NULL;
	LoopEnd		= NULL;
}

CSample::~CSample()
{
	SAFELY_DELETE_ARRAY(Data);
}

// ----
// WARNING: This code is NOT fully functional. 
// Some of the over sampling / under sampling code is most definately buggy!
// It works with the current data on the current batch of devices.
// Will revise later!
bool	CSample::Init(	CString*	_FileName,
						CPlatform*	_PlatformRef)
{
	CString		FilePath;
	FILE		*FH;
	char*		LoadedData;

	// Store imported value.
	PlatformRef = _PlatformRef;	

	// Find out where the file resides.
	if (PlatformRef)
	{
		FilePath = PlatformRef->GetPath(EP_AudioData);
	}
	FilePath += _FileName;

	DataLength = GetFileSize(&FilePath);
	if (DataLength <= 0)
	{
		DataLength = 0;
		return false;
	}

	FH = FOPEN(FilePath.GetString(), "rb");
	if (FH)
	{
		SAFELY_DELETE_ARRAY(Data);
		LoadedData = new char[DataLength];
		
		// Read data in before processing.
		if (fread(LoadedData, sizeof(char) * DataLength, 1, FH) == 1)
		{	
			fclose(FH);

			// Create actual data buffer of correct size
			// then process the data.
			Data = new short[DataLength];

			short*	DataIndex	= Data;
			char*	LoadedIndex	= LoadedData;
			if (DataIndex && LoadedIndex)
			{
				for (int i = DataLength; i--;)
				{
					*DataIndex = (short)((*LoadedIndex) << 8);
					LoadedIndex++;
					DataIndex++;
				}
				
				delete[] LoadedData;
				return true;
			}
			else
			{
				delete[] LoadedData;
				return false;
			}
		}
		fclose(FH);
	}
	
	return false;
}


// ----
// This method is dumb. 
// It assumes it is reading 8bit, mono, signed data. 
// It converts it to 16 bit mono
// It doesn't care about the frequency - that is gonna be a problem later.

// some code here
bool	CSample::ModInit(	FILE*			_FH,
							unsigned int	_ReadBytes,
							int				_LoopStart,
							int				_LoopEnd)
{
	char	*LoadData;
	int		NumberOfSamples;	// Sample conversion, thing.
	int		i;
	
	LoadData = new char[_ReadBytes];
	if (!LoadData)
		return false;

	NumberOfSamples = _ReadBytes;
	NumberOfSamples *= 2;	// 16 bit. (as opposed to the 8 bit I'm reading in).

	DataLength = _ReadBytes + 100;// True sample length + run off space (to stop clippnig).
	Data = new UNALIGNED short[NumberOfSamples];// << 1];
	if (!Data)
		return false;

	short*	DataIndex = Data;

	if (fread(LoadData, _ReadBytes, 1, _FH) != 1)
		return false;	// Failed to read all data.

	// Need to puff out data.
	// Start with a pointer aligned to the start of the sample data.
	char*	LoadedIndex = LoadData;

	// Perform processing then... (Nice comment!).
	for (i = _ReadBytes; i--;)
	{
		// This won't work if the data is sampled at a rate higher than 11025

		// Read byte and up rate to 16 bit value.
		short Sample1 = (*(char*)LoadedIndex);
		Sample1 = Sample1 << 8; 
		LoadedIndex++;

		// Mono
		*DataIndex = Sample1; 

		DataIndex++;		
	}

	// Clear sample run off space.
	for (i = 100; i--;)
	{
		*DataIndex = 0;
		DataIndex++;		
	}
	
	delete[] LoadData;
	
	if (_LoopEnd > 2) // A value less than two often indicates a 'silent' pause at the start of a sample.
	{
		DataLength = _LoopEnd;
	}

	// Copy over loop data.
	LoopStart = _LoopStart;
	LoopEnd = _LoopEnd;

	return true;
}
