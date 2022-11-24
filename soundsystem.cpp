#include	"SoundSystem.h"
#include	"SafeProgramming.h"
#include	"Object.h"
#include	"GameDefines.h"
#include	<windows.h>
#include	<mmsystem.h>
#include	"misc.h"

//#pragma optimize("", off)	// We're having problems...

// ----
#define	GET_INSTRUMENT(a)	(((a & 0xF0000000) >> 24) | ((a & 0xF000) >> 12))
#define	GET_PITCH(a)		((a & 0xFFF0000) >> 16)
#define	GET_EFFECT_CODE(a)	((a & 0xFFF) >> 8)
#define	GET_EFFECT_PARAM(a)	(a & 0xFF)

#define	AUDIO_THRESHOLD			8500

// Static initialisation.
int				CSoundSystem::SoundSystemCount			= 0;	

// Sample format.
//#define SOUND_SAMPLE_FORMAT				WAVE_FORMAT_2S16 // Defined in "GameDefines.h"
#define	SOUND_BUFFER_LENGTH_IN_SECONDS	0.12f

#ifdef COMPAQ_SOUND
// IPaqs are slow to declare a buffer finished with. Need fewer, but longer buffers.
// Longer buffers result in a time lag between the code
// issuing a sample play command and it actually being 
// played. This configuration seems to be the best. 
// Short or longer buffers cause stutter.
#define	SOUND_BUFFER_LENGTH				(3072) 
#define	NUMBER_OF_QUEUED_BUFFERS		2
#else
// All other machines are much better about the sound code
//  and have a response time half that of the IPaq.
#define	SOUND_BUFFER_LENGTH				(1024)
#define	NUMBER_OF_QUEUED_BUFFERS		3
#endif


//#define	MOD_NOTES_PER_SEC				3
#define	MOD_FRAME						((1323 * ModTempo) / 6) /* 11025 / (4 * (125 / 60)) */


#define	RIGHT_VOLUME					0xFF
#define LEFT_VOLUME						0xFF
#define	MASTER_VOLUME					0x3F//0x7F

#define	ASSUMED_MOD_SAMPLE_FREQ	8278

// ----
CSoundSystem::CSoundSystem()
{
	int i;
	ModTempo	= 6;	// Mods normally play back at a tempo of 6.

	SoundSystemID = SoundSystemCount;
	SoundSystemCount++;
	
	PlatformRef				= NULL;

	// Device info.
	DeviceID = -1;	// Assume that no device will be found.
	ZeroMemory(&DeviceCaps, sizeof(DeviceCaps));

	// Sample playback info.
	for (i = MAX_SOUND_BUFFERS; i--;)
	{
		BufferArray[i]		= NULL;
	}

	for (i = MAX_SIMULTANEOUS_SAMPLES; i--;)
	{
		PlayData[i].SampleRef = NULL;
		PlayData[i].Index = 0;
	}

	NextBuffer = NUMBER_OF_QUEUED_BUFFERS;

	// MOD
	// Position info
	for (i = MAX_POSITIONS; i--;)
		PositionArray[i]	= 0;

	NumberOfPositions		= 0;
	NumberOfPatterns		= 0;
//	CurrentPosition			= 0;
	
	// Note info.
	//memset((void*)PatternArray, 0, (sizeof(PatternStruct) * MAX_POSITIONS));

	// Samples
	for (i = NUM_OF_MOD_SAMPLES; i--;)
		ModSampleArray[i] = NULL;
	
	for (i = 4; i--;)
	{
		ModPlayData[i].SampleRef = NULL;		// Pointer to sample object
		ModPlayData[i].Index = 0;				// Pointer to where we are up to playing
		ModPlayData[i].Count = 0;				// The number of times the sample is to be played.
		ModPlayData[i].Pause  = 0.0f;				// Delay time.
		ModPlayData[i].PauseRemaining = 0;		// Amount of pause time remaining.

		ModPlayData[i].Volume = 0;				// Volume for this 'channel'
		ModPlayData[i].Pitch = 0;				// Current sample pitch.

//		ModPlayData[i].TimeUntilNextNote = 0;	// Kind of global property, but then again not.
//		ModPlayData[i].CurrentPosition = 0;	// Array in to PositionArray, which is in turn an index into PatternArray.
//		ModPlayData[i].CurrentNote = 0;
	}

	GotOldVolume = false;
	ModPlaying = false;
}

// ----
CSoundSystem::~CSoundSystem()
{
	PlaySound(NULL, NULL, NULL);		// Erm...

	int i;

	// Delete all play buffer.
	for (i = MAX_SOUND_BUFFERS; i--;)
		SAFELY_DELETE_ARRAY(BufferArray[i]);

	// Delete any and all mod samples
	for (i = NUM_OF_MOD_SAMPLES; i--;)
		SAFELY_DELETE(ModSampleArray[i]);

	if (DeviceID != -1) 
	{
		if (OldVolume)
		{
			// Restore old volume.
			if (waveOutSetVolume(DeviceHandle, OldVolume) == MMSYSERR_NOERROR)
			{
				//OutputDebugString(TEXT("Set old volume\n"));
			}
		}

		// Kill the device.
		waveOutReset(DeviceHandle);
		waveOutClose(DeviceHandle);
	}
}

// ----
bool	CSoundSystem::Init(CPlatform	*_PlatformRef, int _Volume)
{
#ifdef DISABLE_AUDIO
	return true;
#endif

	WAVEOUTCAPS EnumeratedDeviceCaps;
	TCHAR		buffer[1024];
	int			i;

	if (SoundSystemID > 1)
		return false;

	if (	(_Volume < 0)
		||	(_Volume > 10))
		return false;

	// ----
	// Store imported data
	PlatformRef = _PlatformRef;

	// Store volume
	MasterVolume = _Volume;
	
	// Reset and restore any old device we opened.
	if (DeviceID != -1)
	{
		// Restore old volume.
		if (waveOutSetVolume(DeviceHandle, OldVolume) == MMSYSERR_NOERROR)
		{
			//OutputDebugString(TEXT("Set old volume\n"));
		}

		waveOutReset(DeviceHandle);
		waveOutClose(DeviceHandle);
	}


	// ----
	// MOD
	// Position info
	for (i = MAX_POSITIONS; i--;)
		PositionArray[i]	= 0;
	NumberOfPositions		= 0;
	NumberOfPatterns		= 0;
//	CurrentPosition			= 0;
	
	// Note info.
	//memset((void*)&PatternArray, 0, (sizeof(PatternStruct) * MAX_POSITIONS));

	// Samples
	for (i = NUM_OF_MOD_SAMPLES; i--;)
		SAFELY_DELETE(ModSampleArray[i]);

	// ----
	// Device info.
	DeviceID = -1;	// Assume that no device will be found.
	memset((void*)&DeviceCaps, 0,  sizeof(DeviceCaps));

	// Sample playback info.
	for (i = MAX_SOUND_BUFFERS; i--;)
		BufferArray[i]		= NULL;

	for (i = MAX_SIMULTANEOUS_SAMPLES; i--;)
	{	
		PlayData[i].SampleRef		= NULL;
		PlayData[i].Index			= 0;
	
		ModPlayData[i].SampleRef	= NULL;
		ModPlayData[i].Index		= 0;
	}

	NextBuffer = NUMBER_OF_QUEUED_BUFFERS - 1;

	
	// ----
	// Loop through all the available devices, attempting to 
	// find the bext one (what does 'best' mean?)
	for (i = waveOutGetNumDevs(); i--;)
	{
		if (waveOutGetDevCaps(i, &EnumeratedDeviceCaps, sizeof(EnumeratedDeviceCaps)) == MMSYSERR_NOERROR)
		{
			/*
			WORD		wMid;                  // manufacturer ID 
			WORD		wPid;                  // product ID 
			MMVERSION	vDriverVersion;		   // version of the driver 
			TCHAR		szPname[MAXPNAMELEN];  // product name (NULL terminated string) 
			DWORD		dwFormats;             // formats supported 
			WORD		wChannels;             // number of sources supported
			WORD		wReserved1;            // packing 
			DWORD		dwSupport;             // functionality supported by driver 
			*/
			
			/*
			WAVE_FORMAT_1M08	=>	11.025 kHz, Mono,   8-bit 
			WAVE_FORMAT_1S08	=>	11.025 kHz, Stereo, 8-bit 
			WAVE_FORMAT_1M16	=>	11.025 kHz, Mono,   16-bit
			WAVE_FORMAT_1S16	=>	11.025 kHz, Stereo, 16-bit
			WAVE_FORMAT_2M08	=>	22.05  kHz, Mono,   8-bit 
			WAVE_FORMAT_2S08	=>	22.05  kHz, Stereo, 8-bit 
			WAVE_FORMAT_2M16	=>	22.05  kHz, Mono,   16-bit
			WAVE_FORMAT_2S16	=>	22.05  kHz, Stereo, 16-bit
			*/

			swprintf(	buffer, 
						TEXT("Device %d\n\tManufacturer %d\n\tProduct %d\n\tDriver Version %d\n\tProduct Name %s\n\tFormats %d\n\tNumber of channels %d \n\tReserved %d\n\tFunctionality %d\n"), 
						i,
						EnumeratedDeviceCaps.wMid,
						EnumeratedDeviceCaps.wPid,
						EnumeratedDeviceCaps.vDriverVersion,
						EnumeratedDeviceCaps.szPname,
						EnumeratedDeviceCaps.dwFormats,
						EnumeratedDeviceCaps.wChannels,
						EnumeratedDeviceCaps.wReserved1,
						EnumeratedDeviceCaps.dwSupport);

			OutputDebugString(buffer);

			if (DeviceID > -1)
			{
				// Is this device better than the one we've already got?
				if (	(EnumeratedDeviceCaps.wChannels > DeviceCaps.wChannels) 
					&&	(EnumeratedDeviceCaps.dwSupport & WAVE_FORMAT_1M16))
				{
					DeviceID = i;
					memcpy((void*)&DeviceCaps, (void*)&EnumeratedDeviceCaps, sizeof(DeviceCaps));
				}
			}
			else
			{
				// NEW: Assign the first sound device that comes along. 
				// Will try and get the favoured device if we can.

				// Assign first enumerated device that supports the required sample format.
				//if (EnumeratedDeviceCaps.dwSupport & SOUND_SAMPLE_FORMAT)
				//{
					DeviceID = i;
					memcpy((void*)&DeviceCaps, (void*)&EnumeratedDeviceCaps, sizeof(DeviceCaps));
				//}
			}
		}
	}

	// If we enumerated a device then lets create two buffers.
	if (DeviceID > -1)
	{
		WAVEFORMATEX	fmt;

		/*
		WORD        wFormatTag; 
		WORD        nChannels;       
		DWORD       nSamplesPerSec;  
		DWORD       nAvgBytesPerSec; 
		WORD        nBlockAlign;     
		WORD        wBitsPerSample;  
		WORD        cbSize;   
		*/
		fmt.wFormatTag		= 1;
		fmt.nChannels		= 2;
		fmt.nSamplesPerSec	= 11025;
		fmt.nAvgBytesPerSec	= 11025 * 2 * 2; // 16 bit stereo
		fmt.nBlockAlign		= 4;
		fmt.wBitsPerSample	= 16;
		fmt.cbSize			= 0;
		
		// Open wave device
		if (waveOutOpen(&DeviceHandle,
						DeviceID,		
						&fmt,			
						NULL, 
						(DWORD)this,	
						WAVE_MAPPED) == MMSYSERR_NOERROR)
		{
			//OutputDebugString(TEXT("Managed to open sound out device.\n"));
			if (waveOutPause(DeviceHandle) == MMSYSERR_NOERROR)
			{
				//OutputDebugString(TEXT("Managed to pause sound out device.\n"));

				// Set volume
				DWORD	Volume;
				Volume =	(	(((RIGHT_VOLUME * _Volume * 25) & 0xFFFF) << 16)
							|	((LEFT_VOLUME * _Volume * 25) & 0xFFFF));

				if (!GotOldVolume) 
				{
					if (waveOutGetVolume(DeviceHandle, &OldVolume) != MMSYSERR_NOERROR)
					{
						OutputDebugString(TEXT("Unable to store old volume.\n"));
					}
					else
						GotOldVolume = true;
				}

				if (waveOutSetVolume(DeviceHandle, Volume) == MMSYSERR_NOERROR)
				{
					//OutputDebugString(TEXT("Set volume.\n"));
				}
			
				// Create our play buffers
				for (int i = MAX_SOUND_BUFFERS; i--;)
				{
					BufferArray[i] = new short[SOUND_BUFFER_LENGTH]; 
					memset(BufferArray[i], 0, SOUND_BUFFER_LENGTH); 
					
					//OutputDebugString(TEXT("Created buffer.\n"));
				}

				for (i = NUMBER_OF_QUEUED_BUFFERS; i--;)
				{
					WaveHeaderArray[i].lpData			= (char*)BufferArray[i];
					WaveHeaderArray[i].dwBufferLength	= 2 * SOUND_BUFFER_LENGTH;
					WaveHeaderArray[i].dwFlags			= 0;
					WaveHeaderArray[i].dwUser			= 0;
					WaveHeaderArray[i].dwLoops			= 1;

					if (waveOutPrepareHeader(DeviceHandle, &WaveHeaderArray[i], sizeof(WAVEHDR)) == MMSYSERR_NOERROR)
					{
						//if (
							waveOutWrite(DeviceHandle, &WaveHeaderArray[i], sizeof(WAVEHDR));
							//== MMSYSERR_NOERROR)
							//OutputDebugString(TEXT("Wrote out sound.\n"));
					}
				}

				//if (
					waveOutRestart(DeviceHandle);// == MMSYSERR_NOERROR)
					//OutputDebugString(TEXT("Managed to restart sound out device.\n"));
			}
		}
		else
		{
			DeviceID = -1;		// Fail to open, thus give up.
		//	return false;
		}
	}

	return true;
}

// ----
bool	CSoundSystem::PlaySample(	CSample*	_Sound,
									int			_Channel,
									int			_Frequency,
									int			_Count,	
									float		_Pause)

{
#ifdef DISABLE_AUDIO
	return true;
#endif
	/*if (_Unique)
	{
		// Check that is sample isn't already playing
		for (int i = MAX_SIMULTANEOUS_SAMPLES; i--;)
		{
			if (PlayData[i].SampleRef == _Sound)
				return false;
		}
	}*/

	// Check if room to play another sample
	//for (int i = MAX_SIMULTANEOUS_SAMPLES; i--;)
	{
		//if (!PlayData[i].SampleRef)
		if ((_Channel >= 0) && (_Channel < MAX_SIMULTANEOUS_SAMPLES))
		{
			// Found a slot.
			PlayData[_Channel].SampleRef		= _Sound;
			PlayData[_Channel].Index			= 0;
			PlayData[_Channel].Count			= _Count;
			if (_Pause < 0.0f)
				PlayData[_Channel].Pause		= -_Pause;
			else
				PlayData[_Channel].Pause		= _Pause;
			PlayData[_Channel].PauseRemaining	= 0;
			PlayData[_Channel].Pitch			= _Frequency;
			return true;
		}
	}
	return false;
}


// ----
// Loads all the samples, and assorted data.
struct	TSI_Struct
{
//	char	Name[22];
	int		Length;		// In Bytes
	int		FTune;		// Don't care
	int		Volume;		// Why?
	int		RStart;		// Repeat start point in bytes
	int		RLength;	// Repeat length in bytes
};

// ----
bool	CSoundSystem::LoadMod(CString	*_FileName)
{
#ifdef DISABLE_AUDIO
	return true;
#endif

	FILE*	FH;
	char	Buffer[32];
	int		i, j, k;

	// Need some temp space to hold info about sample positions, etc
	TSI_Struct	TempSampleInfo[NUM_OF_MOD_SAMPLES];


	if (	(_FileName) 
		&&	(_FileName->GetString() != 0))
	{
		CString	ModPath;
		ModPath = PlatformRef->GetPath(EP_AudioData);
		ModPath += _FileName;

		// Note info.
		memset((void*)PatternArray, 0, (sizeof(PatternStruct) * MAX_POSITIONS));

		// Get rid of any previous info.
		for (i = NUM_OF_MOD_SAMPLES; i--;)
			SAFELY_DELETE(ModSampleArray[i]);
		
		for (i = MAX_POSITIONS; i--;)
			PositionArray[i] = 0;

		NumberOfPositions = 0;
		NumberOfPatterns = 0;
		//memset(PatternArray, 0, sizeof(NotesStruct) * MAX_POSITIONS * 64);

		for (i = 4; i--;)
		{
			ModPlayData[i].SampleRef = NULL;
			ModPlayData[i].Index = 0;
			ModPlayData[i].CurrentNote = 0;
			ModPlayData[i].CurrentPosition = 0;
			ModPlayData[i].TimeUntilNextNote = MOD_FRAME;
			ModPlayData[i].Pitch = 0;
		}

		// Now load new mod
		FH = FOPEN(ModPath.GetString(), "rb");
		if (FH)
		{
			// Now what to do first?

			// Feature					Start	Length
			// Read song name			[0]		[20]
			// Instrument 0	Name		[20]	[22]
			//				Length		[42]	[2]	
			//				FineTuning	[44]	[1]
			//				Volume		[45]	[1]
			//				RepeatStart	[46]	[2]
			//				RepeatLen	[48]	[2]

			// Seek to the start of the first sample description.
			fread(Buffer, 20, 1, FH);

			//if (fseek(FH, 20, SEEK_SET))
			//{
			//	fclose(FH);
			//	return false;
			//}

			// 1. Strip out sample info.
			for (i = 0; i < NUM_OF_MOD_SAMPLES; i++)
			{
				// Read the 30 bytes we want
				fread(Buffer, 30, 1, FH);

				//TempSampleInfo[i].Name
				short cB;
				
				cB = *((WORD*)(Buffer + 22));
				TempSampleInfo[i].Length	= 2 * EndianInt16(cB);

				TempSampleInfo[i].FTune		= *(Buffer + 24);
				TempSampleInfo[i].Volume	= *(Buffer + 25);

				cB = *((WORD*)(Buffer + 26));
				TempSampleInfo[i].RStart	= 2 * EndianInt16(cB);

				cB = *((WORD*)(Buffer + 28));
				TempSampleInfo[i].RLength	= 2 * EndianInt16(cB);
			}
		}
		else
			return false;

		// Now need to read pattern data.
		// File pointer should be in the correct place.
		fread(&NumberOfPositions,	1,		1, FH);	// NumberOfPositions  ????????????????
		fread(Buffer,				1,		1, FH);	// Ignored	(127)
		fread(PositionArray,		128,	1, FH);	// Positions
		fread(Buffer,				4,		1, FH);	// Ignored (M!K!).

		CString con;
		con = Buffer;
		OutputDebugString(con.GetString());

		// Calculate the highest pattern number.
		NumberOfPatterns = -1;
		for (i = 0; i < NumberOfPositions; i++)
		{
			if (	(NumberOfPatterns == -1) 
				||	(PositionArray[i] > NumberOfPatterns))
				NumberOfPatterns = PositionArray[i];
		}

		// Read in position info
		for (i = 0; i <= NumberOfPatterns; i++)
		{
			fread((void*)&PatternArray[i],	1024,	1, FH);
	
			// Swap the data's endian-ness.  (I'll make words up if I wish).
			for (j = 0; j < 64; j++)
			{
				for (k = 0; k < 4; k++)
				{
					PatternArray[i].NotesArray[j].Note[k] = EndianInt32(PatternArray[i].NotesArray[j].Note[k]); 
				}
			}
		}

		// Load the samples.
		// Position file pointer first. - might be in the right place, depends on how I read the data above.
		for (i = 0; i < NUM_OF_MOD_SAMPLES; i++)
		{
			// The file pointer will incremented by the 'ModInit' method.
			if (TempSampleInfo[i].Length > 0)
			{
				NEW(ModSampleArray[i], CSample());
				ODS("Sample ");
				ODI(i);
				ODS(" is ");
				ODI(TempSampleInfo[i].Length);
				ODSN(" bytes long.");
				//sprintf(coo, TEXT("Sample %d is %d bytes long."), i, TempSampleInfo[i].Length);
				
				if (!(ModSampleArray[i]->ModInit(	FH,
													TempSampleInfo[i].Length,
													TempSampleInfo[i].RStart, 
													TempSampleInfo[i].RStart + TempSampleInfo[i].RLength)))
				{
					// Delete if it failed to init correctly.
					SAFELY_DELETE(ModSampleArray[i]);
				
					OutputDebugString(TEXT("Incorrect sample data!\n"));
					fclose(FH);
					return false;
				}
				else
				{
					ODS("Sample ");
					ODI(i);
					ODS("s repeat is ");
					ODI(TempSampleInfo[i].RLength);
					ODSN(" bytes long.");
					//wsprintf(coo, TEXT("Sample %ds repeat is %d bytes long"), i, TempSampleInfo[i].RStart + TempSampleInfo[i].RLength);
				}
			}
		}

		// Close the file.
		fclose(FH);

		// Init the play back data...
		for (i =0; i < 4; i++)
		{
			// Update note info here.
			ModPlayData[i].CurrentNote		= 0;
			ModPlayData[i].CurrentPosition	= 0;
			ModPlayData[i].Volume			= 64; // Default volume.

			DWORD ThisColumnNote = PatternArray[PositionArray[ModPlayData[i].CurrentPosition]].NotesArray[ModPlayData[i].CurrentNote].Note[i];
			int Instrument = GET_INSTRUMENT(ThisColumnNote);
			
			// Process instrument info.
			if (	(Instrument > 0)
				&&	(Instrument <= NUM_OF_MOD_SAMPLES))
			{
				ModPlayData[i].SampleRef		= ModSampleArray[Instrument - 1];
				ModPlayData[i].Index			= 0;	// Assumption!
				ModPlayData[i].Count			= 1;	// This too.
				ModPlayData[i].PauseRemaining	= 0;
			}
			else
			{
				if (Instrument != 0)
				{
					OutputDebugString(TEXT("Invalid instrument\n"));
				}
			}

			// Process pitch info.
			if (GET_PITCH(ThisColumnNote) > 0)
			{
				ModPlayData[i].Pitch = GET_PITCH(ThisColumnNote);
			}

			for (int l = 0; l < 4; l++) // Scan across all four channels for effects. 
			{
				DWORD ColumnSearch = PatternArray[PositionArray[ModPlayData[i].CurrentPosition]].NotesArray[ModPlayData[i].CurrentNote].Note[l];

				// Process effects.
				switch (GET_EFFECT_CODE(ColumnSearch))
				{
				case 0x0A:
					if (l == i)	// this effect is localised
					{
						if (GET_EFFECT_PARAM(ColumnSearch) < 0x0F)
						{
							ModPlayData[i].Volume += (GET_EFFECT_PARAM(ColumnSearch) >> 4);
						}
						else
						{
							ModPlayData[i].Volume -= GET_EFFECT_PARAM(ColumnSearch);
						}
					}
					break;
				
				case 0x0B:
					// Jump position.
					ModPlayData[i].CurrentPosition = GET_EFFECT_PARAM(ColumnSearch);
					if (ModPlayData[i].CurrentPosition >= NumberOfPositions)
						ModPlayData[i].CurrentPosition = 0;
					ModPlayData[i].CurrentNote = 0;
					break;

				case 0x0C:
					if (l == i)	// this effect is localised
					{
						// Change volume - for the particular column.
						ModPlayData[i].Volume = GET_EFFECT_PARAM(ColumnSearch);
					}
					break;

				case 0x0D:
					// Break pattern. 
					ModPlayData[i].CurrentPosition++;
					if (ModPlayData[i].CurrentPosition >= NumberOfPositions)
						ModPlayData[i].CurrentPosition = 0;
					ModPlayData[i].CurrentNote = GET_EFFECT_PARAM(ColumnSearch);
					break;

				case 0x0F:
					ModTempo = GET_EFFECT_PARAM(ColumnSearch) - 1;
					break;

				case 0:
					break;
				default:
					break;
				}
			}
 
			// Reset the 'timer'.
			ModPlayData[i].TimeUntilNextNote = MOD_FRAME; // Blah, blah, blah... (this needs to be finished :o)
		}

		return true;
	}
	return false;
}

// ----
void	CSoundSystem::PlayMod()
{
	// Init the play back data...
	for (int i =0; i < 4; i++)
	{
		// Update note info here.
		ModPlayData[i].CurrentNote		= 0;
		ModPlayData[i].CurrentPosition	= 0;
		ModPlayData[i].Volume			= 64; // Default volume.

		DWORD ThisColumnNote = PatternArray[PositionArray[ModPlayData[i].CurrentPosition]].NotesArray[ModPlayData[i].CurrentNote].Note[i];
		int Instrument = GET_INSTRUMENT(ThisColumnNote);
		
		// Process instrument info.
		if (	(Instrument > 0)
			&&	(Instrument <= NUM_OF_MOD_SAMPLES))
		{
			ModPlayData[i].SampleRef		= ModSampleArray[Instrument - 1];
			ModPlayData[i].Index			= 0;	// Assumption!
			ModPlayData[i].Count			= 1;	// This too.
			ModPlayData[i].PauseRemaining	= 0;
		}
		else
		{
			if (Instrument != 0)
			{
				OutputDebugString(TEXT("Invalid instrument\n"));
			}
		}

		// Process pitch info.
		if (GET_PITCH(ThisColumnNote) > 0)
		{
			ModPlayData[i].Pitch = GET_PITCH(ThisColumnNote);
		}

		for (int l = 0; l < 4; l++) // Scan across all four channels for effects. 
		{
			DWORD ColumnSearch = PatternArray[PositionArray[ModPlayData[i].CurrentPosition]].NotesArray[ModPlayData[i].CurrentNote].Note[l];

			// Process effects.
			switch (GET_EFFECT_CODE(ColumnSearch))
			{
			case 0x0A:
				if (l == i)	// this effect is localised
				{
					if (GET_EFFECT_PARAM(ColumnSearch) < 0x0F)
					{
						ModPlayData[i].Volume += (GET_EFFECT_PARAM(ColumnSearch) >> 4);
					}
					else
					{
						ModPlayData[i].Volume -= GET_EFFECT_PARAM(ColumnSearch);
					}
				}
				break;
			
			case 0x0B:
				// Jump position.
				ModPlayData[i].CurrentPosition = GET_EFFECT_PARAM(ColumnSearch);
				if (ModPlayData[i].CurrentPosition >= NumberOfPositions)
					ModPlayData[i].CurrentPosition = 0;
				ModPlayData[i].CurrentNote = 0;
				break;

			case 0x0C:
				if (l == i)	// this effect is localised
				{
					// Change volume - for the particular column.
					ModPlayData[i].Volume = GET_EFFECT_PARAM(ColumnSearch);
				}
				break;

			case 0x0D:
				// Break pattern. 
				ModPlayData[i].CurrentPosition++;
				if (ModPlayData[i].CurrentPosition >= NumberOfPositions)
					ModPlayData[i].CurrentPosition = 0;
				ModPlayData[i].CurrentNote = GET_EFFECT_PARAM(ColumnSearch);
				break;

			case 0x0F:
				ModTempo = GET_EFFECT_PARAM(ColumnSearch) - 1;
				break;

			case 0:
				break;
			default:
				break;
			}
		}

		// Reset the 'timer'.
		ModPlayData[i].TimeUntilNextNote = MOD_FRAME; // Blah, blah, blah... (this needs to be finished :o)
	}


	ModPlaying = true;
}

// ----
bool	CSoundSystem::Process()
{
#ifdef DISABLE_AUDIO
	return true;
#endif

	if (SoundSystemID != 0)
		return false;

	bool Continue = true;
	while (Continue)
	{
		// Free old buffer.
		int unprep = NextBuffer - NUMBER_OF_QUEUED_BUFFERS + 1;
		while (unprep < 0)
			unprep += MAX_SOUND_BUFFERS;

		if (WaveHeaderArray[unprep].dwFlags & WHDR_DONE)
		{
			// Free cleaned buffer
			waveOutUnprepareHeader(	DeviceHandle, 
									&WaveHeaderArray[unprep], 
									sizeof(WAVEHDR));

			// Increment buffer to fill.
			NextBuffer++;
			if (NextBuffer >= MAX_SOUND_BUFFERS) 
				NextBuffer = 0;

			// Clear buffer.
			memset(BufferArray[NextBuffer], 0, 2 * SOUND_BUFFER_LENGTH); 

			// ----
			// Do the individual sound mixing routines.
			ProcessAudio();
			ProcessMod();

			// ----
			// Prepare header
			WaveHeaderArray[NextBuffer].lpData			= (char*)BufferArray[NextBuffer];
			WaveHeaderArray[NextBuffer].dwBufferLength	= 2 * SOUND_BUFFER_LENGTH;
			WaveHeaderArray[NextBuffer].dwFlags			= 0;
			WaveHeaderArray[NextBuffer].dwUser			= 0;
			WaveHeaderArray[NextBuffer].dwLoops			= 1;

			if (waveOutPrepareHeader(DeviceHandle, &WaveHeaderArray[NextBuffer], sizeof(WAVEHDR)) == MMSYSERR_NOERROR)
			{
				if (waveOutWrite(DeviceHandle, &WaveHeaderArray[NextBuffer], sizeof(WAVEHDR)) == MMSYSERR_NOERROR)
				{
					//OutputDebugString(TEXT("Primary CSoundSystem: Successfully queued next chunk...\n"));
				}
			}
		}
		else
		{
			Continue = false;
		}
	}
	return true;
}

// ----
// Stop playback of all samples (so that it is safe to 
bool	CSoundSystem::StopAllSamples()
{
#ifdef DISABLE_AUDIO
	return true;
#endif

	for (int i = MAX_SIMULTANEOUS_SAMPLES; i--;)
	{
		PlayData[i].SampleRef	= NULL;
		PlayData[i].Index		= 0;
	}

	return (waveOutReset(DeviceHandle) == MMSYSERR_NOERROR);
}

// ----
void	CSoundSystem::StopMod()
{
#ifdef DISABLE_AUDIO
	return;
#endif

	ModPlaying = false;
	/*
	for (int i =0; i < 4; i++)
	{
		// Update note info here.
		ModPlayData[i].CurrentNote		= 0;
		ModPlayData[i].CurrentPosition	= 0;
		ModPlayData[i].Volume			= 64; // Default volume.

		DWORD ThisColumnNote = PatternArray[PositionArray[ModPlayData[i].CurrentPosition]].NotesArray[ModPlayData[i].CurrentNote].Note[i];
		int Instrument = GET_INSTRUMENT(ThisColumnNote);
		
		// Process instrument info.
		if (	(Instrument > 0)
			&&	(Instrument <= NUM_OF_MOD_SAMPLES))
		{
			ModPlayData[i].SampleRef		= ModSampleArray[Instrument - 1];
			ModPlayData[i].Index			= 0;	// Assumption!
			ModPlayData[i].Count			= 1;	// This too.
			ModPlayData[i].PauseRemaining	= 0;
		}
		else
		{
			if (Instrument != 0)
			{
				OutputDebugString(TEXT("Invalid instrument\n"));
			}
		}

		// Process pitch info.
		if (GET_PITCH(ThisColumnNote) > 0)
		{
			ModPlayData[i].Pitch = GET_PITCH(ThisColumnNote);
		}
		
		// Reset the 'timer'.
		ModPlayData[i].TimeUntilNextNote = MOD_FRAME; 
	}*/
}

// ----
bool	CSoundSystem::SetVolume(int _Volume)
{
#ifdef DISABLE_AUDIO
	return true;
#endif
	
	if (	(_Volume < 0)
		||	(_Volume > 10))
		return false;

	// Set volume
	DWORD	Volume;
	Volume =	(	(((RIGHT_VOLUME * _Volume * 25) & 0xFFFF) << 16)
				|	((LEFT_VOLUME * _Volume * 25) & 0xFFFF));

	return (waveOutSetVolume(DeviceHandle, Volume) == MMSYSERR_NOERROR);
}


// ----
#define	PITCH_SCALE			(3546894.6f * 0.75f)
#define	MUSIC_THRESHOLD		(8000)
#define	PITCH_NEM			(33075)
bool	CSoundSystem::ProcessAudio()
{
#ifdef DISABLE_AUDIO
	return true;
#endif

	short*	MixingBufferIndex;
	short*	PlayBufferIndex = NULL;
	int		AmountToWrite;

	short	existing;
	long	additiveL;
	long	result;

	int		StepSize;
	int		Step;
	int		TotalSteps;
	int		i;

	// For each channel.
	for (i = 0; i < MAX_SIMULTANEOUS_SAMPLES; i++)
	{
		// Grab pointer to main mixing buffer.
		MixingBufferIndex	= (short*)BufferArray[NextBuffer];
		int MixRemaining	= SOUND_BUFFER_LENGTH >> 1;

		// While room in that buffer...
		while (MixRemaining > 0)
		{
			// Some play back ratio (i.e that affects the pitch).
			if (PlayData[i].Pitch > 0)
			{
				StepSize = /*PITCH_NEM /*/ PlayData[i].Pitch;
			}
			//else
			//	StepSize = 75;

			if (StepSize < 1)
				StepSize = 75;

			// If there is data to play then play it.
			if (	(PlayData[i].SampleRef) 
				&&	((((PlayData[i].SampleRef->GetDataLength() - PlayData[i].Index) * 100) / StepSize) > 3)) // hack
			{
				// Is the sample going to be paused for the rest of the remaining mix time?
				if ((PlayData[i].PauseRemaining - MixRemaining) >= 0)
				{
					// Yes. Simple get out.

					// Decrement pause counter.
					PlayData[i].PauseRemaining -= MixRemaining;

					// No need to process anymore mixing.
					MixRemaining = 0;
					continue;
				}
				else
				{
					// There is either no pause, or it is shorter 
					// than the remaining mix time.

					// Step forward in time, until the end of 
					// the pause period. Will continue to mix from there!
					MixRemaining				-= PlayData[i].PauseRemaining;
					MixingBufferIndex			+= PlayData[i].PauseRemaining;
					PlayData[i].PauseRemaining	= 0;		// No pause now.

					// So, I want to mix up until the end
					// of the mix buffer, the end of the 
					// sample, or until the point I need 
					// to change the note / pitch.
					AmountToWrite = min((int)MixRemaining,					
										(int)(((PlayData[i].SampleRef->GetDataLength() - PlayData[i].Index) * 100) / StepSize)	// Amount of data left in sample.
										);
										
					/*
					//AmountToWrite = min((int)AmountToWrite,
					//					(int)((PlayData[i].TimeUntilNextNote * 100) / StepSize));

					// Must ensure that the net result of 
					// StepSize and AmountToWrite does not
					// cause a buffer over run (on reading
					// the sample data that is).
					*/
				}
						
				// Right then. Time to start mixing...

				// Get a handle to the data we should be mixing...
				PlayBufferIndex	= (short*)(PlayData[i].SampleRef->GetData());
				PlayBufferIndex	+= PlayData[i].Index;
				Step			= 0;
				TotalSteps		= 0;
				int StepCount;

				short		Prev = AUDIO_THRESHOLD + 1;
				// ... and then mix the data
				// This loop will cost!!!
				for (int j = AmountToWrite; j--;)
				{
					// Sit in a loop gathering data to merge.
					Step		+= StepSize;					// Add
					
					// Only clear value if we plan to calculate a new one!!!
					if (Step > 100)
						additiveL	= 0;
					else
						additiveL = *PlayBufferIndex;

					StepCount = 0;
					while (Step >= 100)							// Test
					{
						additiveL += *PlayBufferIndex;
						
						PlayBufferIndex++;
						Step -= 100;
						TotalSteps++;
						StepCount++;
					}

					if (StepCount > 0)
						additiveL /= StepCount;

					if (additiveL > AUDIO_THRESHOLD)			// Test
						additiveL = AUDIO_THRESHOLD;			// Set
					else 
					if (additiveL < -AUDIO_THRESHOLD)			// Test
						additiveL = -AUDIO_THRESHOLD;			// Set					

					// Low level pass.
					if (Prev > AUDIO_THRESHOLD)
						Prev = additiveL;

					additiveL = (additiveL	>> 1) + 
								(additiveL	>> 2) + 
								(additiveL	>> 3) + 
								(additiveL	>> 4) + 
								(Prev		>> 5) +
								(Prev		>> 6) +
								(Prev		>> 7);
					Prev = additiveL;

					if (i < 2)
					{
						existing	= *MixingBufferIndex;		// Read
						result		= existing + additiveL;		// Add

						if (result > AUDIO_THRESHOLD)			// Test
							result = AUDIO_THRESHOLD;			// Set
						else 
						if (result < -AUDIO_THRESHOLD)			// Test
							result = -AUDIO_THRESHOLD;			// Set
						*MixingBufferIndex = result;			// Write


						MixingBufferIndex++;					// Add
						existing	= *MixingBufferIndex;		// Read
						result		= existing + (additiveL >> 1);		// Add

						if (result > AUDIO_THRESHOLD)			// Test
							result = AUDIO_THRESHOLD;			// Set
						else 
						if (result < -AUDIO_THRESHOLD)			// Test
							result = -AUDIO_THRESHOLD;			// Set
						*MixingBufferIndex = result;			// Write
					}
					else
					{
						existing	= *MixingBufferIndex;		// Read
						result		= existing + (additiveL >> 1);		// Add

						if (result > AUDIO_THRESHOLD)			// Test
							result = AUDIO_THRESHOLD;			// Set
						else 
						if (result < -AUDIO_THRESHOLD)			// Test
							result = -AUDIO_THRESHOLD;			// Set
						*MixingBufferIndex = result;			// Write
						MixingBufferIndex++;					// Add
						
						existing	= *MixingBufferIndex;		// Read
						result		= existing + additiveL;		// Add

						if (result > AUDIO_THRESHOLD)			// Test
							result = AUDIO_THRESHOLD;			// Set
						else 
						if (result < -AUDIO_THRESHOLD)			// Test
							result = -AUDIO_THRESHOLD;			// Set
						*MixingBufferIndex = result;			// Write
					}
					MixingBufferIndex++;						// Add
				}
				
				// Update index to reflect the amount of data we have read.
				PlayData[i].Index				+= TotalSteps;
				MixRemaining					-= AmountToWrite;
				//PlayData[i].TimeUntilNextNote	-= AmountToWrite;

				// Now, we are at one of the following cases.
				// case 1. Middle of a sample.
				// case 2. Run out of sample data, and there is no pause.
				// case 3. At the end of a looping sample (no pause)
				// case 4. At end of looping sample (pause between loops).

				// Case 1. No action required.
				// Case 2, 3, 4. Need to detect end of sample state.
				if (PlayData[i].Index >= PlayData[i].SampleRef->GetDataLength())
				{
					// Okay, at the end of a sample.
					// Case 2 - play once only.
					if (PlayData[i].Count == 1)
					{
						// Remove from chain.
						PlayData[i].SampleRef = NULL;
						MixRemaining = 0;
					}
					else
					{	
						// Case 3, 4. Need to loop. 
						// Decrement loop count
						if (PlayData[i].Count > 0)
							PlayData[i].Count --;
									
						// Calculate pause.
						PlayData[i].PauseRemaining = PlayData[i].Pause * 11025;
				
						// Set index back to beginning of sample.
						PlayData[i].Index = 0;
					}	
				} 
			}
			else
			{
				// No sample, or ran out of data.
				PlayData[i].Count--;
				
				if (!PlayData[i].Count)
				{
					PlayData[i].SampleRef	= NULL;
					PlayData[i].Index		= 0;
				}
				else
				{
					if (PlayData[i].Count < 0)
						PlayData[i].Count = -1;
				}
				
				// Simply take remainder of timeuntil...
				//int MinMixnNextNote = min((int)PlayData[i].TimeUntilNextNote, (int)MixRemaining);
				//PlayData[i].TimeUntilNextNote -= MinMixnNextNote;
				//MixRemaining -= MinMixnNextNote;
				MixRemaining = 0;
			}
		}
	}

	return true;
}

// ----
void	CSoundSystem::ProcessMod()
{
#ifdef DISABLE_AUDIO
	return;
#endif

	if(ModPlaying == false)
		return;

	short*	MixingBufferIndex;
	short*	PlayBufferIndex = NULL;
	int		AmountToWrite;

	short	existing;
	long	additiveL;
	long	result;

	int		StepSize;
	int		Step;
	int		TotalSteps;
	int		i, l;

	if (NumberOfPositions < 1)
		return;

	// For each channel.
	for (i = 0; i < MAX_SIMULTANEOUS_SAMPLES; i++)
	{
		// Grab pointer to main mixing buffer.
		MixingBufferIndex	= (short*)BufferArray[NextBuffer];
		int MixRemaining	= SOUND_BUFFER_LENGTH >> 1;

		// While room in that buffer...
		while (MixRemaining > 0)
		{
			// Some play back ratio (i.e that affects the pitch).
			if (ModPlayData[i].Pitch > 0)
			{
				StepSize = PITCH_NEM / ModPlayData[i].Pitch;
			}
			else
				StepSize = 75;

			if (StepSize < 1)
				StepSize = 1;

			// See if we need to process another note yet...
			if (((ModPlayData[i].TimeUntilNextNote * 100) / StepSize) <= 0)
			{
				// Update note info here.
				ModPlayData[i].CurrentNote++;
	
				// Reset volume.
				ModPlayData[i].Volume			= 64; // Default volume.

				// Safe guard
				if (ModPlayData[i].CurrentNote >= 64)	// Max length of pattern.
				{
					ModPlayData[i].CurrentNote = 0;
					ModPlayData[i].CurrentPosition++;
				}

				// Loop song...
				if (ModPlayData[i].CurrentPosition >= NumberOfPositions)
				{
					StopMod();
					PlayMod();
				}

				DWORD ThisColumnNote = PatternArray[PositionArray[ModPlayData[i].CurrentPosition]].NotesArray[ModPlayData[i].CurrentNote].Note[i];
				int Instrument = GET_INSTRUMENT(ThisColumnNote);
				// Process instrument info.
				if (	(Instrument > 0)
					&&	(Instrument <= NUM_OF_MOD_SAMPLES))
				{
					
					ModPlayData[i].SampleRef		= ModSampleArray[Instrument - 1];
					ModPlayData[i].Index			= 0;	// Assumption!
					ModPlayData[i].Count			= 1;	// This too.
					ModPlayData[i].PauseRemaining	= 0;
					ModPlayData[i].Pause			= 0.0f;
				}
				else
				{
					if (Instrument != 0)
					{
						OutputDebugString(TEXT("Invalid instrument\n"));
					}
				}

				// Process pitch info.
				if (GET_PITCH(ThisColumnNote) > 0)
				{
					ModPlayData[i].Pitch = GET_PITCH(ThisColumnNote);
				}

				for (l = 0; l < 4; l++) // Scan across all four channels for effects. 
				{
					DWORD ColumnSearch = PatternArray[PositionArray[ModPlayData[i].CurrentPosition]].NotesArray[ModPlayData[i].CurrentNote].Note[l];

					// Process effects.
					switch (GET_EFFECT_CODE(ColumnSearch))
					{
					case 0x0A:
						if (l == i)	// this effect is localised
						{
							if (GET_EFFECT_PARAM(ColumnSearch) < 0x0F)
							{
								ModPlayData[i].Volume += (GET_EFFECT_PARAM(ColumnSearch) >> 4);
							}
							else
							{
								ModPlayData[i].Volume -= GET_EFFECT_PARAM(ColumnSearch);
							}
						}
						break;
					
					case 0x0B:
						// Jump position.
						ModPlayData[i].CurrentPosition = GET_EFFECT_PARAM(ColumnSearch);
						if (ModPlayData[i].CurrentPosition >= NumberOfPositions)
							ModPlayData[i].CurrentPosition = 0;
						ModPlayData[i].CurrentNote = 0;
						break;

					case 0x0C:
						if (l == i)	// this effect is localised
						{
							// Change volume - for the particular column.
							ModPlayData[i].Volume = GET_EFFECT_PARAM(ColumnSearch);
						}
						break;

					case 0x0D:
						// Break pattern. 
						ModPlayData[i].CurrentPosition++;
						if (ModPlayData[i].CurrentPosition >= NumberOfPositions)
							ModPlayData[i].CurrentPosition = 0;
						ModPlayData[i].CurrentNote = GET_EFFECT_PARAM(ColumnSearch) - 1;
						break;

					case 0x0F:
						ModTempo = GET_EFFECT_PARAM(ColumnSearch) - 1;
						break;

					case 0:
						break;
					default:
						break;
					}
				}
				
				// Reset the 'timer'.
				ModPlayData[i].TimeUntilNextNote = MOD_FRAME; // Assignment
			}

			// Some play back ratio (i.e that affects the pitch).
			if (ModPlayData[i].Pitch > 0)
			{
				StepSize = PITCH_NEM / ModPlayData[i].Pitch;
			}
			else
				StepSize = 75;

			if (StepSize < 1)
				StepSize = 1;

			// If there is data to play then play it. (if not, then remember to decrement 'TimeUntilNextNote')
			if (	(ModPlayData[i].SampleRef) 
				&&	((((ModPlayData[i].SampleRef->GetDataLength() - ModPlayData[i].Index) * 100) / StepSize) > 0))
			{
				// Is the sample going to be paused for the rest of the remaining mix time?
				if ((ModPlayData[i].PauseRemaining - MixRemaining) >= 0)
				{
					// THIS CAN NOT HAPPEN WITH MOD PLAYBACK
					// Yes. Simple get out.

					// Decrement pause counter.
					ModPlayData[i].PauseRemaining -= MixRemaining;

					// No need to process anymore mixing.
					MixRemaining = 0;
					continue;
				}
				else
				{
					// There is either no pause, or it is shorter 
					// than the remaining mix time.

					// Step forward in time, until the end of 
					// the pause period. Will continue to mix from there!
					MixRemaining				-= ModPlayData[i].PauseRemaining;
					MixingBufferIndex			+= ModPlayData[i].PauseRemaining;
					ModPlayData[i].PauseRemaining	= 0;		// No pause now.

					// So, I want to mix up until the end
					// of the mix buffer, the end of the 
					// sample, or until the point I need 
					// to change the note / pitch.
					AmountToWrite = min((int)MixRemaining,					
										(int)(((ModPlayData[i].SampleRef->GetDataLength() - ModPlayData[i].Index) * 100) / StepSize)	// Amount of data left in sample.
										);
										
					AmountToWrite = min((int)AmountToWrite,
										(int)ModPlayData[i].TimeUntilNextNote); //(int)((ModPlayData[i].TimeUntilNextNote * 100) / StepSize));

					// Must ensure that the net result of 
					// StepSize and AmountToWrite does not
					// cause a buffer over run (on reading
					// the sample data that is).
					
				}
						
				// Right then. Time to start mixing...

				// Get a handle to the data we should be mixing...
				PlayBufferIndex	= (short*)(ModPlayData[i].SampleRef->GetData());
				PlayBufferIndex	+= ModPlayData[i].Index;
				Step			= 0;
				TotalSteps		= 0;
				int StepCount;

				short		Read;
				short		Prev = MUSIC_THRESHOLD + 1;
				// ... and then mix the data
				// This loop will cost!!!
				for (int j = AmountToWrite; j--;)
				{
					// Sit in a loop gathering data to merge.
					Step		+= StepSize;					// Add
					
					// Only clear value if we plan to calculate a new one!!!
					if (Step > 100)
						additiveL	= 0;
					else
						additiveL = *PlayBufferIndex;

					StepCount = 0;
					while (Step >= 100)							// Test
					{
						additiveL += *PlayBufferIndex;			// Read
						PlayBufferIndex++;						// Add
						Step -= 100;							// Sub
						TotalSteps++;
						StepCount++;
					}

					if (StepCount > 0)
						additiveL /= StepCount;

					additiveL  = additiveL >> 2; //3

					if (additiveL > MUSIC_THRESHOLD)			// Test
						additiveL = MUSIC_THRESHOLD;			// Set
					else 
					if (additiveL < -MUSIC_THRESHOLD)			// Test
						additiveL = -MUSIC_THRESHOLD;			// Set					

					// Low level pass.
					if (Prev > MUSIC_THRESHOLD)
						Prev = additiveL;

					additiveL = (additiveL	>> 1) + 
								(additiveL	>> 2) + 
								(additiveL	>> 3) + 
								(additiveL	>> 4) + 
								(Prev		>> 5) +
								(Prev		>> 6) +
								(Prev		>> 7);

					// Bit of a hack to set the volume to the two values specified in the mod.
					additiveL = additiveL >> (8 - (ModPlayData[i].Volume >> 3));
					Prev = additiveL;

					if (i < 2)
					{
						existing	= *MixingBufferIndex;		// Read
						result		= existing + additiveL;		// Add

						if (result > AUDIO_THRESHOLD)			// Test
							result = AUDIO_THRESHOLD;			// Set
						else 
						if (result < -AUDIO_THRESHOLD)			// Test
							result = -AUDIO_THRESHOLD;			// Set
						*MixingBufferIndex = result;			// Write


						MixingBufferIndex++;					// Add
						existing	= *MixingBufferIndex;		// Read
						result		= existing + (additiveL >> 1);		// Add

						if (result > AUDIO_THRESHOLD)			// Test
							result = AUDIO_THRESHOLD;			// Set
						else 
						if (result < -AUDIO_THRESHOLD)			// Test
							result = -AUDIO_THRESHOLD;			// Set
						*MixingBufferIndex = result;			// Write
					}
					else
					{
						existing	= *MixingBufferIndex;		// Read
						result		= existing + (additiveL >> 1);		// Add

						if (result > AUDIO_THRESHOLD)			// Test
							result = AUDIO_THRESHOLD;			// Set
						else 
						if (result < -AUDIO_THRESHOLD)			// Test
							result = -AUDIO_THRESHOLD;			// Set
						*MixingBufferIndex = result;			// Write
						MixingBufferIndex++;					// Add
						
						existing	= *MixingBufferIndex;		// Read
						result		= existing + additiveL;		// Add

						if (result > AUDIO_THRESHOLD)			// Test
							result = AUDIO_THRESHOLD;			// Set
						else 
						if (result < -AUDIO_THRESHOLD)			// Test
							result = -AUDIO_THRESHOLD;			// Set
						*MixingBufferIndex = result;			// Write
					}
					MixingBufferIndex++;						// Add
				}
				
				// Update index to reflect the amount of data we have read.
				ModPlayData[i].Index				+= TotalSteps;		// Again, this will have to change with playback pitch.

				MixRemaining						-= AmountToWrite;
				ModPlayData[i].TimeUntilNextNote	-= AmountToWrite;

				// Now, we are at one of the following cases.
				// case 1. Middle of a sample.
				// case 2. Run out of sample data, and there is no pause.
				// case 3. At the end of a looping sample (no pause)
				// case 4. At end of looping sample (pause between loops).

				// Case 1. No action required.
				// Case 2, 3, 4. Need to detect end of sample state.
				if (ModPlayData[i].Index >= ModPlayData[i].SampleRef->GetDataLength())
				{
					// Don't care, no such thing as pause or loop.
					// Simply clear sample from channel.

					if (ModPlayData[i].SampleRef->LoopEnd > 2)
					{
						ModPlayData[i].Index = ModPlayData[i].SampleRef->LoopStart;
						ODSN("Repeating sample");
					}
					else
					{
						ModPlayData[i].SampleRef	= NULL;
						ModPlayData[i].Index		= 0;

						// No more sample to play - lets cut the time until next note and mix remaining
						int MinMixnNextNote;
						MinMixnNextNote	= min(	(int)ModPlayData[i].TimeUntilNextNote, 
												(int)MixRemaining);
						ModPlayData[i].TimeUntilNextNote	-= MinMixnNextNote;
						MixRemaining						-= MinMixnNextNote;
					}

					// Okay, at the end of a sample.
				//	// Case 2 - play once only.
				//	if (PlayData[i].Count == 1)
				//	{
				//		// Remove from chain.
				//		PlayData[i].SampleRef = NULL;
				//		MixRemaining = 0;
				//	}
				//	else
				//	{	
				//		// Case 3, 4. Need to loop. 
				//		// Decrement loop count
				//		if (PlayData[i].Count > 0)
				//			PlayData[i].Count --;
				//					
				//		// Calculate pause.
				//		PlayData[i].PauseRemaining = PlayData[i].Pause * 11025;
				//
				//		// Set index back to beginning of sample.
				//		PlayData[i].Index = 0;
				//	}
				//	
					
				} 
			}
			else
			{
				if (	(ModPlayData[i].SampleRef)
					&&	(ModPlayData[i].SampleRef->LoopEnd > 2))
				{
					ModPlayData[i].Index = ModPlayData[i].SampleRef->LoopStart;
					ODSN("Repeating sample");
				}
				else
				{
					// No sample, or ran out of data.
					ModPlayData[i].Count--;
					
					if (!ModPlayData[i].Count)
					{
						ModPlayData[i].SampleRef	= NULL;
						ModPlayData[i].Index		= 0;
					}
					else
					{
						if (ModPlayData[i].Count < 0)
							ModPlayData[i].Count = -1;
					}

					// Simply take remainder of timeuntil...
					int MinMixnNextNote = min((int)ModPlayData[i].TimeUntilNextNote, (int)MixRemaining);
					ModPlayData[i].TimeUntilNextNote -= MinMixnNextNote;
					MixRemaining -= MinMixnNextNote;

				}	
			}
		}
	}

//	ODSN("Check point");
}

bool	CSoundSystem::CheckChannel(int ChanNum)
{
	if(PlayData[ChanNum].SampleRef != NULL)
		return true;
	else
		return false;
}

//#pragma optimize("", on)