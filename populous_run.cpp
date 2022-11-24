#include	"Populous.h"
#include	"Object.h"
#include	"SafeProgramming.h"
#include	<windows.h>
#include	"GameDefines.h"

#include	"GraphicsClasses.h"
#include	"SoundSystem.h"
#include	"Controls.h"
#include	"Platform.h"
#include	"Preferences.h"
#include	"Localization.h"

// Original include files.
#include	"God.h"
#include	"Extern.h"


#define	SAMPLE_NAME_0_10__15		"compwindy2.dat"
#define	SAMPLE_NAME_26_51__77		"voicesaah.dat"
#define	SAMPLE_NAME_52_53			"complongdr.dat"
#define	SAMPLE_NAME_66				"swamped.dat"
#define	SAMPLE_NAME_67_68			"sword.dat"
#define	SAMPLE_NAME_69				"knighting.dat"
#define	SAMPLE_NAME_70				"crackfire.dat"
#define	SAMPLE_NAME_71_72			"heart4.dat"
#define	SAMPLE_NAME_73				"laugh.dat"
#define	SAMPLE_NAME_74__78			"compthunde.dat"
#define	SAMPLE_NAME_75__79			"compvolcan.dat"
#define	SAMPLE_NAME_76__80			"rumble.dat"
#define	SAMPLE_NAME_104				"well_done.dat"
#define	SAMPLE_NAME_105				"greater.dat"
#define	SAMPLE_NAME_106				"deity.dat"
#define	SAMPLE_NAME_107				"eternal.dat"
#define	SAMPLE_NAME_109				"immortal.dat"
#define	SAMPLE_NAME_110				"god.dat"
#define	SAMPLE_NAME_111				"deva.dat"
#define	SAMPLE_NAME_112				"being.dat"
#define	SAMPLE_NAME_113				"mortal.dat"

// ----
// Function call as Structured Exception Handling (SEH) 
// exception filter.
int FilterQuit(LPEXCEPTION_POINTERS _a)
{
	if (	(_a)
		&&	(_a->ExceptionRecord->ExceptionCode == QUIT_EXCEPTION))
		return EXCEPTION_EXECUTE_HANDLER;
	else
		return EXCEPTION_CONTINUE_SEARCH;
}

// ----
// This method is where the whole populous game runs.
// The method simply returns true to indicate that no
// errors occured, and false that something went wrong,
// and that we are to say sorry.
bool		CPopulous::Run()
{
	FILE	*FH;
	int		i;

	// Clear the screens to black, if only for effect.
	GraphicsRef->StartDraw();
	int tttt = GraphicsRef->GetActiveScreen();
	for (i = 0; i < 4; i++)
	{
		GraphicsRef->SetActiveScreen(i);
		GraphicsRef->ClearScreen(0,0,0);
	}
	GraphicsRef->SetActiveScreen(tttt);
	GraphicsRef->EndDraw();
	
	// Now, whatever needs to go here.
	int a=200;
	int x=3;
	int y;
	
	os_byte(a,&x,&y); // ?
	
//	os_read_var_val("Populous$GameType",(char *)&start_of_game,4);
//	start_of_game-='0';
	start_of_game = selected;

	// ----
	// Load icons
	TempString = ThePlatform->GetPath(EP_ImageData);
	TempString += "icon";
	FH = FOPEN(TempString.GetString(), "rb");
	if (!FH)
		return false;
	SAFELY_DELETE(IconGraphics);
	NEW(IconGraphics, CGraphicsData());
	SAFELY_CONTROL(if, IconGraphics, InitAsPopulous(GraphicsRef, FH, 3840, 16, false), ==, false)
		return false;
	fclose(FH);

	TempString = ThePlatform->GetPath(EP_ImageData);
	TempString += "iconmask";
	FH = FOPEN(TempString.GetString(), "rb");
	if (!FH)
		return false;
	SAFELY_DELETE(ToggleIconGraphics);
	NEW(ToggleIconGraphics, CGraphicsData());
	SAFELY_CONTROL(if, ToggleIconGraphics, InitAsPopulous(GraphicsRef, FH, 320, 32), ==, false)
		return false;
	fclose(FH);


	

	// ----
	// SAMPLE_NAME_0_10__15
	SAFELY_DELETE(SampleArray[0].SampleRef);
	NEW(SampleArray[0].SampleRef, CSample());
	if (!SampleArray[0].SampleRef->Init(&CString(SAMPLE_NAME_0_10__15), ThePlatform))
		return false;
	for (i = 0; i <= 10; i++)
		SampleArray[i] = SampleArray[0];
	SampleArray[15] = SampleArray[0];

	// SAMPLE_NAME_26_51__77
	SAFELY_DELETE(SampleArray[26].SampleRef);
	NEW(SampleArray[26].SampleRef, CSample());
	if (!SampleArray[26].SampleRef->Init(&CString(SAMPLE_NAME_26_51__77), ThePlatform))
		return false;
	for (i = 26; i <= 51; i++)
		SampleArray[i].SampleRef = SampleArray[26].SampleRef;
	SampleArray[77].SampleRef = SampleArray[26].SampleRef;

	// SAMPLE_NAME_52_53
	SAFELY_DELETE(SampleArray[52].SampleRef);
	NEW(SampleArray[52].SampleRef, CSample());
	if (!SampleArray[52].SampleRef->Init(&CString(SAMPLE_NAME_52_53), ThePlatform))
		return false;
	for (i = 52; i <= 53; i++)
		SampleArray[i].SampleRef = SampleArray[52].SampleRef;

	// SAMPLE_NAME_66
	SAFELY_DELETE(SampleArray[66].SampleRef);
	NEW(SampleArray[66].SampleRef, CSample());
	if (!SampleArray[66].SampleRef->Init(&CString(SAMPLE_NAME_66), ThePlatform))
		return false;

	// SAMPLE_NAME_67_68
	SAFELY_DELETE(SampleArray[67].SampleRef);
	NEW(SampleArray[67].SampleRef, CSample());
	if (!SampleArray[67].SampleRef->Init(&CString(SAMPLE_NAME_67_68), ThePlatform))
		return false;
	for (i = 67; i <= 68; i++)
		SampleArray[i].SampleRef = SampleArray[67].SampleRef;

	// SAMPLE_NAME_69
	SAFELY_DELETE(SampleArray[69].SampleRef);
	NEW(SampleArray[69].SampleRef, CSample());
	if (!SampleArray[69].SampleRef->Init(&CString(SAMPLE_NAME_69), ThePlatform))
		return false;

	// SAMPLE_NAME_70
	SAFELY_DELETE(SampleArray[70].SampleRef);
	NEW(SampleArray[70].SampleRef, CSample());
	if (!SampleArray[70].SampleRef->Init(&CString(SAMPLE_NAME_70), ThePlatform))
		return false;

	// SAMPLE_NAME_71_72
	SAFELY_DELETE(SampleArray[71].SampleRef);
	NEW(SampleArray[71].SampleRef, CSample());
	if (!SampleArray[71].SampleRef->Init(&CString(SAMPLE_NAME_71_72), ThePlatform))
		return false;
	for (i = 71; i <= 72; i++)
		SampleArray[i].SampleRef = SampleArray[71].SampleRef;

	// SAMPLE_NAME_73
	SAFELY_DELETE(SampleArray[73].SampleRef);
	NEW(SampleArray[73].SampleRef, CSample());
	if (!SampleArray[73].SampleRef->Init(&CString(SAMPLE_NAME_73), ThePlatform))
		return false;

	// SAMPLE_NAME_74__78
	SAFELY_DELETE(SampleArray[74].SampleRef);
	NEW(SampleArray[74].SampleRef, CSample());
	if (!SampleArray[74].SampleRef->Init(&CString(SAMPLE_NAME_74__78), ThePlatform))
		return false;
	SampleArray[78].SampleRef = SampleArray[74].SampleRef;

	// SAMPLE_NAME_75__79
	SAFELY_DELETE(SampleArray[75].SampleRef);
	NEW(SampleArray[75].SampleRef, CSample());
	if (!SampleArray[75].SampleRef->Init(&CString(SAMPLE_NAME_75__79), ThePlatform))
		return false;
	SampleArray[79].SampleRef = SampleArray[75].SampleRef;

	// SAMPLE_NAME_76__80
	SAFELY_DELETE(SampleArray[76].SampleRef);
	NEW(SampleArray[76].SampleRef, CSample());
	if (!SampleArray[76].SampleRef->Init(&CString(SAMPLE_NAME_76__80), ThePlatform))
		return false;
	SampleArray[80].SampleRef = SampleArray[76].SampleRef;

	// SAMPLE_NAME_104
	SAFELY_DELETE(SampleArray[104].SampleRef);
	NEW(SampleArray[104].SampleRef, CSample());
	if (!SampleArray[104].SampleRef->Init(&CString(SAMPLE_NAME_104), ThePlatform))
		return false;

	// SAMPLE_NAME_105
	SAFELY_DELETE(SampleArray[105].SampleRef);
	NEW(SampleArray[105].SampleRef, CSample());
	if (!SampleArray[105].SampleRef->Init(&CString(SAMPLE_NAME_105), ThePlatform))
		return false;

	// SAMPLE_NAME_106
	SAFELY_DELETE(SampleArray[106].SampleRef);
	NEW(SampleArray[106].SampleRef, CSample());
	if (!SampleArray[106].SampleRef->Init(&CString(SAMPLE_NAME_106), ThePlatform))
		return false;

	// SAMPLE_NAME_107
	SAFELY_DELETE(SampleArray[107].SampleRef);
	NEW(SampleArray[107].SampleRef, CSample());
	if (!SampleArray[107].SampleRef->Init(&CString(SAMPLE_NAME_107), ThePlatform))
		return false;

	// SAMPLE_NAME_109
	SAFELY_DELETE(SampleArray[109].SampleRef);
	NEW(SampleArray[109].SampleRef, CSample());
	if (!SampleArray[109].SampleRef->Init(&CString(SAMPLE_NAME_109), ThePlatform))
		return false;

	// SAMPLE_NAME_110
	SAFELY_DELETE(SampleArray[110].SampleRef);
	NEW(SampleArray[110].SampleRef, CSample());
	if (!SampleArray[110].SampleRef->Init(&CString(SAMPLE_NAME_110), ThePlatform))
		return false;

	// SAMPLE_NAME_111
	SAFELY_DELETE(SampleArray[111].SampleRef);
	NEW(SampleArray[111].SampleRef, CSample());
	if (!SampleArray[111].SampleRef->Init(&CString(SAMPLE_NAME_111), ThePlatform))
		return false;

	// SAMPLE_NAME_112
	SAFELY_DELETE(SampleArray[112].SampleRef);
	NEW(SampleArray[112].SampleRef, CSample());
	if (!SampleArray[112].SampleRef->Init(&CString(SAMPLE_NAME_112), ThePlatform))
		return false;

	// SAMPLE_NAME_113
	SAFELY_DELETE(SampleArray[113].SampleRef);
	NEW(SampleArray[113].SampleRef, CSample());
	if (!SampleArray[113].SampleRef->Init(&CString(SAMPLE_NAME_113), ThePlatform))
		return false;


	SampleArray[0].Frequency	= ((3546895/0x486)*256)/20833;
	SampleArray[1].Frequency	= ((3546895/0x460)*256)/20833;
	SampleArray[2].Frequency	= ((3546895/0x43B)*256)/20833;
	SampleArray[3].Frequency	= ((3546895/0x41A)*256)/20833;
	SampleArray[4].Frequency	= ((3546895/0x3FA)*256)/20833;
	SampleArray[5].Frequency	= ((3546895/0x3DC)*256)/20833;
	SampleArray[6].Frequency	= ((3546895/0x3C0)*256)/20833;
	SampleArray[7].Frequency	= ((3546895/0x3A6)*256)/20833;
	SampleArray[8].Frequency	= ((3546895/0x38C)*256)/20833;
	SampleArray[9].Frequency	= ((3546895/0x375)*256)/20833;
	SampleArray[10].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[11].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[12].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[13].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[14].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[15].Frequency	= ((3546895/0x408)*256)/20833;
	SampleArray[16].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[17].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[18].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[19].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[20].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[21].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[22].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[23].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[24].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[25].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[26].Frequency	= ((3546895/0x3B8)*256)/20833;
	SampleArray[27].Frequency	= ((3546895/0x382)*256)/20833;
	SampleArray[28].Frequency	= ((3546895/0x350)*256)/20833;
	SampleArray[29].Frequency	= ((3546895/0x320)*256)/20833;
	SampleArray[30].Frequency	= ((3546895/0x2F3)*256)/20833;
	SampleArray[31].Frequency	= ((3546895/0x2C9)*256)/20833;
	SampleArray[32].Frequency	= ((3546895/0x2A1)*256)/20833;
	SampleArray[33].Frequency	= ((3546895/0x27B)*256)/20833;
	SampleArray[34].Frequency	= ((3546895/0x257)*256)/20833;
	SampleArray[35].Frequency	= ((3546895/0x236)*256)/20833;
	SampleArray[36].Frequency	= ((3546895/0x216)*256)/20833;
	SampleArray[37].Frequency	= ((3546895/0x6A0)*256)/20833;
	SampleArray[38].Frequency	= ((3546895/0x3B8)*256)/20833;
	SampleArray[39].Frequency	= ((3546895/0x3B8)*256)/20833;
	SampleArray[40].Frequency	= ((3546895/0x3B8)*256)/20833;
	SampleArray[41].Frequency	= ((3546895/0x3B8)*256)/20833;
	SampleArray[42].Frequency	= ((3546895/0x3B8)*256)/20833;
	SampleArray[43].Frequency	= ((3546895/0x3B8)*256)/20833;
	SampleArray[44].Frequency	= ((3546895/0x3B8)*256)/20833;
	SampleArray[45].Frequency	= ((3546895/0x3B8)*256)/20833;
	SampleArray[46].Frequency	= ((3546895/0x3B8)*256)/20833;
	SampleArray[47].Frequency	= ((3546895/0x3B8)*256)/20833;
	SampleArray[48].Frequency	= ((3546895/0x3B8)*256)/20833;
	SampleArray[49].Frequency	= ((3546895/0x3B8)*256)/20833;
	SampleArray[50].Frequency	= ((3546895/0x3B8)*256)/20833;
	SampleArray[51].Frequency	= ((3546895/0x6A0)*256)/20833;
	SampleArray[52].Frequency	= ((3546895/0x3F8)*256)/20833;
	SampleArray[53].Frequency	= ((3546895/0x652)*256)/20833;
	SampleArray[54].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[55].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[56].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[57].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[58].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[59].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[60].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[61].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[62].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[63].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[64].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[65].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[66].Frequency	= ((3546895/0x28E)*256)/20833;
	SampleArray[67].Frequency	= ((3546895/0x2BD)*256)/20833;
	SampleArray[68].Frequency	= ((3546895/0x2CB)*256)/20833;
	SampleArray[69].Frequency	= ((3546895/0x2CA)*256)/20833;
	SampleArray[70].Frequency	= ((3546895/0x1E0)*256)/20833;
	SampleArray[71].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[72].Frequency	= ((3546895/0x145)*256)/20833;
	SampleArray[73].Frequency	= ((3546895/0xD27)*256)/20833;
	SampleArray[74].Frequency	= ((3546895/0x2F7)*256)/20833;
	SampleArray[75].Frequency	= ((3546895/0x6A8)*256)/20833;
	SampleArray[76].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[77].Frequency	= ((3546895/0x3B8)*256)/20833;
	SampleArray[78].Frequency	= ((3546895/0x2F7)*256)/20833;
	SampleArray[79].Frequency	= ((3546895/0x6A8)*256)/20833;
	SampleArray[80].Frequency	= ((3546895/0x18C)*256)/20833;
	SampleArray[81].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[82].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[83].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[84].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[85].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[86].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[87].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[88].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[89].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[90].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[91].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[92].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[93].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[94].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[95].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[96].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[97].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[98].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[99].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[100].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[101].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[102].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[103].Frequency	= ((3546895/0x165)*256)/20833;
	SampleArray[104].Frequency	= ((3546895/0x21E)*256)/20833;
	SampleArray[105].Frequency	= ((3546895/0x21E)*256)/20833;
	SampleArray[106].Frequency	= ((3546895/0x21E)*256)/20833;
	SampleArray[107].Frequency	= ((3546895/0x21E)*256)/20833;
	SampleArray[108].Frequency	= ((3546895/0x21E)*256)/20833;
	SampleArray[109].Frequency	= ((3546895/0x21E)*256)/20833;
	SampleArray[110].Frequency	= ((3546895/0x21E)*256)/20833;
	SampleArray[111].Frequency	= ((3546895/0x21E)*256)/20833;
	SampleArray[112].Frequency	= ((3546895/0x21E)*256)/20833;
	SampleArray[113].Frequency	= ((3546895/0x21E)*256)/20833;

#if 0
	// Output a list of those values.
	for (i = 0; i < 113; i++)
	{
		ODS("Sample ");
		ODI(i);
		ODS(" Frequency = ");
		ODIN(SampleArray[i].Frequency);
		Sleep(5);
	}
#endif

	// ----
	// Timers
	SAFELY_DELETE(Timer50ths);
	NEW(Timer50ths, CTimer(20));
	if (!Timer50ths)
		return false;
	Timer50ths->AssignSoundSystem(TheSoundSystem);

	SAFELY_DELETE(Timer5ths);
	NEW(Timer5ths, CTimer(200));
	if (!Timer5ths)
		return false;
	
	SAFELY_DELETE(GameFrameLimiter);
	NEW(GameFrameLimiter, CTimer(1000 / GAME_FRAME_RATE_LIMIT));
	if (!GameFrameLimiter)
		return false;
	GameFrameLimiter->AssignSoundSystem(TheSoundSystem);

	SAFELY_DELETE(IntroFrameLimiter);
	NEW(IntroFrameLimiter, CTimer(1000 / INTRO_FRAME_RATE_LIMIT));
	if (!IntroFrameLimiter)
		return false;
	IntroFrameLimiter->AssignSoundSystem(TheSoundSystem);

	SAFELY_DELETE(MusicTimer);
	NEW(MusicTimer, CTimer(20));
	if (!MusicTimer)
		return false;

	//make_arc_sprites_8((char *)font_data,4400L);
	//make_arc_sprites_16_nomask((char *)icon_data,3840L);

	//sprite_data=(LONG)malloc(SPRITE_DATA);
	//sprite_data_32=(LONG)malloc(SPRITE_DATA_32);
	//blk_data=(LONG)malloc(BLK_DATA);

	open_screen();
	//claim_sound_system();

	read_sprites(sprites_in);
	start_game();
	setup_serial_interrupt();
	baud_rate=serial.baud_rate=9600;
	set_serial();
	__try
	{
		animate();
	}
	__except(FilterQuit(GetExceptionInformation()))
	{
		// All okay, we were expecting this exception. It means quit.
		ODSN("Caught quit 'exception'");
	}
//error_end:;
	closeall();
	{
		int	a=200;
		int	x=0;
		int	y;
		os_byte(a,&x,&y);
	}

	return true;
}


void	CPopulous::closeall()
{
//	free((char *)blk_data);
	free((char *)screen_pointer[2]);
}

void	CPopulous::create_mouse()
{
	int 	a=106;
	int 	x=1;
	int 	y=0;
	os_byte(a,&x,&y);
	mouse_limit=399;

	left_button = 2;
	right_button = 2;
}


SHORT CPopulous::Random()
{
	return(rand());
}

