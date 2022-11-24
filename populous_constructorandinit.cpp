#include	"Populous.h"
#include	"Object.h"
#include	"SafeProgramming.h"
#include	<windows.h>

#include	"GraphicsClasses.h"
#include	"SoundSystem.h"
#include	"Controls.h"
#include	"Platform.h"
#include	"Preferences.h"
#include	"Localization.h"

#include	"Extern.h"

#define		KEYBOARD_IMAGE	"keyboard"
#define		CORNER_IAMGE	"MenuCorner"	// Move this to Help();
#define		HELP_IMAGE		"helpicons"		// ditto

CPopulous::CPopulous()
{
	int i;

	// Low-level objects.
	GameState		= GameError;
	HInstance		= NULL;	
	FileMissing		= false;
	
	ThePlatform		= NULL;
	TheGameWindow	= NULL;
	TheSoundSystem	= NULL;
	ThePreferences	= NULL;
	TheLocalization	= NULL;
	GraphicsRef		= NULL;
	ControllerRef	= NULL;
	
	// Game oriented data.
	RandomArrayLoc = 0;

	for(i = 0; i < MAX_RANDOM_NUMBERS; i++)
		RandomArray[i] = NULL;

	// Set everything to NULL.
	Timer50ths								= NULL;
	Timer5ths								= NULL;
	GameFrameLimiter						= NULL;
	IntroFrameLimiter						= NULL;
	MusicTimer								= NULL;
	BatteryTimer							= NULL;
//	MouseTimer								= NULL;

											
	BackGroundImage							= NULL;
	BlockGraphics							= NULL;
	SpriteGraphics							= NULL;
	Sprite32Graphics						= NULL;
	IconGraphics							= NULL;
	MouthGraphics							= NULL;
	ToggleIconGraphics						= NULL;
	TheKeyboardGraphic						= NULL;
	HeadGraphics							= NULL;
	BigFont									= NULL;
	TheMenuCornerGraphics					= NULL;
	HelpIconGraphics						= NULL;
//	PDAGraphics								= NULL;
//	PDAKeysGraphics							= NULL;

	StandardFont							= NULL;

	for (i = MAX_NUM_OF_SAMPLES; i--;)
		SampleArray[i].SampleRef = NULL;

	measln		= new SHORT[128 / 2];	// 128 bytes.
	measures	= new UBYTE[16384];		// 16384 bytes (16.0 Kb)
}

CPopulous::~CPopulous()
{
	int i;

	// Timers
	SAFELY_DELETE(Timer50ths);
	SAFELY_DELETE(Timer5ths);
	SAFELY_DELETE(GameFrameLimiter);
	SAFELY_DELETE(IntroFrameLimiter);
	SAFELY_DELETE(MusicTimer);
	SAFELY_DELETE(BatteryTimer);
//	SAFELY_DELETE(MouseTimer);
	
	// Graphics
	SAFELY_DELETE(BackGroundImage);
	SAFELY_DELETE(BlockGraphics);
	SAFELY_DELETE(SpriteGraphics);
	SAFELY_DELETE(Sprite32Graphics);
	SAFELY_DELETE(IconGraphics);
	SAFELY_DELETE(MouthGraphics);
	SAFELY_DELETE(ToggleIconGraphics);
	SAFELY_DELETE(TheKeyboardGraphic);
	SAFELY_DELETE(HeadGraphics);
	SAFELY_DELETE(BigFont);
	SAFELY_DELETE(TheMenuCornerGraphics);
	SAFELY_DELETE(HelpIconGraphics);
//	SAFELY_DELETE(PDAGraphics);
//	SAFELY_DELETE(PDAKeysGraphics);

	// Font
	SAFELY_DELETE(StandardFont);

	// Now for the low level objects
	SAFELY_DELETE(TheSoundSystem);
	SAFELY_DELETE(TheGameWindow);
	SAFELY_DELETE(ThePlatform);
	SAFELY_DELETE(ThePreferences);
	SAFELY_DELETE(TheLocalization);

	SAFELY_DELETE(SampleArray[0].SampleRef);
	SAFELY_DELETE(SampleArray[26].SampleRef);
	SAFELY_DELETE(SampleArray[52].SampleRef);
	SAFELY_DELETE(SampleArray[66].SampleRef);
	SAFELY_DELETE(SampleArray[67].SampleRef);
	SAFELY_DELETE(SampleArray[69].SampleRef);
	SAFELY_DELETE(SampleArray[70].SampleRef);
	SAFELY_DELETE(SampleArray[71].SampleRef);
	SAFELY_DELETE(SampleArray[73].SampleRef);
	SAFELY_DELETE(SampleArray[74].SampleRef);
	SAFELY_DELETE(SampleArray[75].SampleRef);
	SAFELY_DELETE(SampleArray[76].SampleRef);
	SAFELY_DELETE(SampleArray[104].SampleRef);
	SAFELY_DELETE(SampleArray[105].SampleRef);
	SAFELY_DELETE(SampleArray[106].SampleRef);
	SAFELY_DELETE(SampleArray[107].SampleRef);
	SAFELY_DELETE(SampleArray[109].SampleRef);
	SAFELY_DELETE(SampleArray[110].SampleRef);
	SAFELY_DELETE(SampleArray[111].SampleRef);
	SAFELY_DELETE(SampleArray[112].SampleRef);
	SAFELY_DELETE(SampleArray[113].SampleRef);

	for (i = MAX_NUM_OF_SAMPLES; i--;)
		SampleArray[i].SampleRef = NULL;

	SAFELY_DELETE_ARRAY(measln);
//	SAFELY_DELETE_ARRAY(seqlen);
//	SAFELY_DELETE_ARRAY(sequn);
	SAFELY_DELETE_ARRAY(measures);
}

// ----
// This method gets all the underlying systems up and running.
bool		CPopulous::Init(	HINSTANCE	_hInstance,
					LPTSTR		_lpCmdLine,
					int			_nCmdShow)
{
	FILE	*FH;
	
	// Start by assuming we're gonna fail - does that say something about the way I look at things?
	GameState = GameError;

	// Create an object to store the platform information.
	SAFELY_DELETE(ThePlatform);
	NEW(ThePlatform, CPlatform());
	SAFELY_CONTROL(if, ThePlatform, Init(), ==, false)
		return false;

	// Validate the files
	FileMissing = !ValidateFiles();
	if (FileMissing)
		return false;

	SAFELY_DELETE(ThePreferences);
	NEW(ThePreferences, CPreferences());
	SAFELY_CONTROL(if, ThePreferences, LoadPreferences(ThePlatform), ==, false)
		return false;

	// Load localization strings.
	SAFELY_DELETE(TheLocalization);
	NEW(TheLocalization, CLocalization(ThePlatform, ThePreferences));
	SAFELY_CONTROL(if, TheLocalization, LoadLocalizationStrings(), ==, false)
		return false;

	// Create app window + start GAPI.
	NEW(TheGameWindow, CGameWindow());
	SAFELY_CONTROL(if, TheGameWindow, Init(_hInstance, _lpCmdLine, _nCmdShow, ThePlatform, ThePreferences), ==, false)
		return false;

	// Time to create soundsystem object.
	SAFELY_DELETE(TheSoundSystem);
	NEW(TheSoundSystem, CSoundSystem());
	SAFELY_CONTROL(if, TheSoundSystem, Init(ThePlatform, ThePreferences->Volume), ==, false)
		return false;

	GraphicsRef = SAFELY_RETURN(TheGameWindow, GetGraphicsObject());
	if (!GraphicsRef)
		return false;
	
	SAFELY_CALL(GraphicsRef, AssignPlatform(ThePlatform));
	SAFELY_CALL(GraphicsRef, AssignSoundSystem(TheSoundSystem));
	SAFELY_CALL(GraphicsRef, SetOrientation(ThePreferences->PreferredOrientation));

	ControllerRef = SAFELY_RETURN(TheGameWindow, GetControlObject());
	if (!ControllerRef)
		return false;
	
	ControllerRef->SetOrientation(ThePreferences->PreferredOrientation);

	// Init all game data here.
	SetRandomNumbers();

	//load up the keyboard graphic
	SAFELY_DELETE(TheKeyboardGraphic);
	NEW(TheKeyboardGraphic, CGraphicsData());
	SAFELY_CONTROL(if, TheKeyboardGraphic, Init(GraphicsRef, &CString(KEYBOARD_IMAGE), ThePlatform), ==, false)
		return false;

	//TheMenuCornerGraphics

	// Move this to Help();
	SAFELY_DELETE(TheMenuCornerGraphics);
	NEW(TheMenuCornerGraphics, CGraphicsData());
	SAFELY_CONTROL(if, TheMenuCornerGraphics, Init(GraphicsRef, &CString(CORNER_IAMGE), ThePlatform), ==, false)
		return false;

	SAFELY_DELETE(HelpIconGraphics);
	NEW(HelpIconGraphics, CGraphicsData());
	SAFELY_CONTROL(if, HelpIconGraphics, Init(GraphicsRef, &CString(HELP_IMAGE), ThePlatform), ==, false)
		return false;

	// Big font
	SAFELY_DELETE(BigFont);
	NEW(BigFont, CGraphicsData());

	TempString = ThePlatform->GetPath(EP_ImageData);
	TempString += "bigfont";
	FH = FOPEN(TempString.GetString(), "rb");
	if (!FH)
		return false;

	SAFELY_CONTROL(if, BigFont, InitAsPopulous(GraphicsRef, FH, 2560, 16, true), ==, false)
	{
		fclose(FH);
		return false;
	}
	fclose(FH);

	// Load music data...
	TempString = ThePlatform->GetPath(EP_AudioData);
	TempString += "measln.dat";
	FH = FOPEN(TempString.GetString(), "rb");
	if (!FH)
		return false;
	fread(measln, 128, 1, FH);
	fclose(FH);

	TempString = ThePlatform->GetPath(EP_AudioData);
	TempString += "measures.dat";
	FH = FOPEN(TempString.GetString(), "rb");
	if (!FH)
		return false;
	fread(measures, 16384, 1, FH);
	fclose(FH);

	TempString = ThePlatform->GetPath(EP_AudioData);
	TempString += "seqlen.dat";
	FH = FOPEN(TempString.GetString(), "rb");
	if (!FH)
		return false;
	fread(&seqlen, 2, 1, FH);
	fclose(FH);

	TempString = ThePlatform->GetPath(EP_AudioData);
	TempString += "sequn.dat";
	FH = FOPEN(TempString.GetString(), "rb");
	if (!FH)
		return false;
	fread(sequn, 32, 1, FH);
	fclose(FH);

	// ----
	// Load font.
	TempString = ThePlatform->GetPath(EP_Fonts);
	TempString += "font";
	FH = FOPEN(TempString.GetString(), "rb");
	if (!FH)
		return false;
	SAFELY_DELETE(StandardFont);
	NEW(StandardFont, CFont());
	SAFELY_CONTROL(if, StandardFont, InitAsPopulous(GraphicsRef, FH, 4400, 8), ==, false)
		return false;
	fclose(FH);

	// ----
	// Timers
	SAFELY_DELETE(Timer50ths);
	NEW(Timer50ths, CTimer(20));
	if (!Timer50ths)
		return false;

	SAFELY_DELETE(Timer5ths);
	NEW(Timer5ths, CTimer(200));
	if (!Timer5ths)
		return false;
	
	SAFELY_DELETE(GameFrameLimiter);
	NEW(GameFrameLimiter, CTimer(1000 / GAME_FRAME_RATE_LIMIT));
	if (!GameFrameLimiter)
		return false;

	SAFELY_DELETE(IntroFrameLimiter);
	NEW(IntroFrameLimiter, CTimer(1000 / INTRO_FRAME_RATE_LIMIT));
	if (!IntroFrameLimiter)
		return false;

	SAFELY_DELETE(MusicTimer);
	NEW(MusicTimer, CTimer(20));
	if (!MusicTimer)
		return false;

	SAFELY_DELETE(BatteryTimer);
	NEW(BatteryTimer, CTimer(BATTERY_LOW_FLASH_TIME));
	if (!BatteryTimer)
		return false;
	
//	SAFELY_DELETE(MouseTimer);
//	NEW(MouseTimer, CTimer(ThePreferences->MouseRepeat));
//	if (!MouseTimer)
//		return false;
	
	// ----
	// Init stuff from the game that doesn't seem to be done anywhere else
	sprintf((char*)drawer, "%ls", ThePlatform->GetPath(EP_Saves)->GetString());

	SAFELY_DELETE(HeadGraphics);
	NEW(HeadGraphics, CGraphicsData());
	//SAFELY_CONTROL(if, HeadGraphics, InitAsPopulous(GraphicsRef, FH, 4096, 64, false), ==, false)
	TempString = "heads";
	SAFELY_CONTROL(if, HeadGraphics, Init(GraphicsRef, &TempString, ThePlatform), ==, false)
		return false;
	//fclose(FH);

	TempString = ThePlatform->GetPath(EP_Data);
	TempString += "intro";
	FH = FOPEN(TempString.GetString(), "rb");
	if (!FH)
		return false;
	fread(title_map_blk, 300, 1, FH);
	fclose(FH);

	// Load the title stuff
	load_graphics();

//	dests = (UBYTE**)dests_array;
	selected = -2;

	//TempString = ThePlatform->GetPath(EP_AudioData);
	TempString = "music.umd";
	TheSoundSystem->LoadMod(&TempString);

	// Assign in game variables based on our preferences.
	effect_off = 0;
	music_off = 0;
	if (ThePreferences->EffectsEnabled)
	{
		effect_off = 1;
	}
	if (ThePreferences->MusicEnabled)
	{
		music_off = 1;
		TheSoundSystem->PlayMod();
	}

	// Okay, we're ready, so set the game state to starting.
	GameState = GameStarting;
	RedrawCount = 1;
	return true;
}
