#ifndef _SOUNDSYSTEM_H_
#define _SOUNDSYSTEM_H_

#include	"Object.h"
#include	"Platform.h"
#include	"Sample.h"
#include	<windows.h>
#include	<windowsx.h>
#include	<Mmsystem.h>

//#pragma optimize("", off)

#define	MAX_SOUND_BUFFERS					6
#define	MAX_SIMULTANEOUS_SAMPLES			4
#define	MAX_SIMULTANEOUS_SAMPLES_INC_MOD	8

struct PlayDataStruct
{
	CSample*	SampleRef;			// Pointer to sample object
	DWORD		Index;				// Pointer to where we are up to playing
	int			Count;				// The number of times the sample is to be played.
	float		Pause;				// Delay time.
	int			PauseRemaining;		// Amount of pause time remaining.

	// Addiotnal items for mod playback.
	int			Volume;				// Volume for this 'channel'
	int			Pitch;				// Current sample pitch.

	int			TimeUntilNextNote;	// Kind of global property, but then again not.
	int			CurrentPosition;	// Array in to PositionArray, which is in turn an index into PatternArray.
	int			CurrentNote;
};

// ----
// MOD
// ----

// ----
// There are four simultaneous channels - 
// each channel requires 4 bytes of data.
struct	NotesStruct
{
	DWORD		Note[4];
};

// ----
// Each pattern contains 64 NotesStructs.
struct	PatternStruct
{
	NotesStruct	NotesArray[64];
};

struct	PopulousInstrumentStruct
{
	CSample	*SampleRef;
	int		Frequency;
};

// ----
// This class encapsulates an audio mixer. 
// WARNING: THERE CAN ONLY EVER BE ONE INSTANCE OF THIS CLASS. 
//			Operations based on subsequent object will always fail!!!
class CSoundSystem : public CObject
{
public:
	CSoundSystem();
	~CSoundSystem();

	// ----
	// GENERIC
	// ----

	// ----
	bool	Init(CPlatform	*_PlatformRef, int _Volume);

	// ----
	int		GetSoundSystemID()	{return SoundSystemID;}

	// ----
	bool	Process();		

	// ----
	bool	SetVolume(int _Volume);


	// ----
	// SAMPLES
	// ----

	// ----
	bool	PlaySample(	CSample*	_Sound,
						int			_Channel,
						int			_Frequency		= 100,
						int			_Count			= 1,	// negative to loop.
						float		_Pause			= 0.0f);

	// ----
	// Stop playback of all samples (so that it is safe to 
	bool	StopAllSamples();


	// ---- 
	// MUSIC
	// ----

	// ----
	bool	LoadMod(CString	*_FileName);	// Load mod data.
	void	PlayMod();						// Starts mod playing.
	void	StopMod();						// Stops the mod playback.


	// ----
	// I must be a hack as I'm not protected!
	bool	ModPlaying;

	// ----
	// This checks wether a channel has finished playing it's data
	bool	CheckChannel(int ChanNum);

protected:
	// ----
	// These methods do the hard work...
	bool	ProcessAudio();	
	void	ProcessMod();


	// ----
	// Only allow one object.
	static	int				SoundSystemCount;	
			int				SoundSystemID;	
	
	// Store old volume level, so we can restore it.
	DWORD					OldVolume;
	bool					GotOldVolume;

	int						MasterVolume;

	// Store imported ref.
	CPlatform*				PlatformRef;

	// Device caps.
	HWAVEOUT				DeviceHandle;
	int						DeviceID;		// Id of the output device we are using.
	WAVEOUTCAPS				DeviceCaps;		// Caps of our chosen device.

	// Play queue buffers.
	WAVEHDR					WaveHeaderArray[MAX_SOUND_BUFFERS];
	short*					BufferArray[MAX_SOUND_BUFFERS];
	int						NextBuffer;

	// Store playing sample data.
	PlayDataStruct			PlayData[MAX_SIMULTANEOUS_SAMPLES];



	// ----
	// MOD
#define	MAX_POSITIONS		128
#define	NUM_OF_MOD_SAMPLES	31
	unsigned char			PositionArray[MAX_POSITIONS];
	int						NumberOfPositions;
	int						NumberOfPatterns;

	PatternStruct			PatternArray[MAX_POSITIONS];
	
	
	CSample					*ModSampleArray[NUM_OF_MOD_SAMPLES];	// Mod's have a maximum of 32 samples.

	// Store playing mod sample data.
	PlayDataStruct			ModPlayData[4];
	int						ModTempo;
};

//#pragma optimize("", on)

#endif