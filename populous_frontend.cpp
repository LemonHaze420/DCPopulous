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

//#include "os.h"
//#include "swis.h"
//#include "computer.h"
//#include "title.h"
//#include "extern.h"
#include <stdio.h>
#include <stdlib.h>
//#include "os.h"
//#include "swis.h"
#include "computer.h"
//#include "populous.h"
#include "extern.h"


int 	CPopulous::title()
{
#ifdef TEST_IN_EMULATION
	return 1;
#endif
	int eger;

	while(1)
	{
		eger = title_page();
        switch(eger)
		{
		case    0:
		case    1:
		case    2:
		case    3:
			selected = eger;
			TheSoundSystem->StopMod();
			return(selected);
		
			// Our escape values
		case   -10:
			return(-10);

		case	-11:
			return (-11);
		
		case	-12:
			return -12;

		case -13:
			return -13;

		default:
			display();
			break;
		}
	}
}


int		CPopulous::title_page()
{
	TheSoundSystem->Process();
	CFont	*RegFont = NULL;
//	SHORT   c1;
	LONG    count;
	selected = -2;
	
	GraphicsRef->StartDraw();	
	int tttt = GraphicsRef->GetActiveScreen();
	for (int i = 0; i < 4; i++)
	{
		GraphicsRef->SetActiveScreen(i);
		GraphicsRef->ClearScreen(0, 0, 0);
	}
	GraphicsRef->SetActiveScreen(tttt);

	//os_swi0(OS_WriteI+22);
	//os_swi0(OS_WriteI+13);
	//os_swi0(OS_RemoveCursors);
	open_screen();
	load_screen((UBYTE*)"load");



	//copy_screen_256((char *)o_screen,(char *)screen_pointer[0]);
	GraphicsRef->CopyScreen(o_screen,d_screen);//screen_pointer[0]);

#ifndef BOXEDSALE
	if (!ThePreferences->Registered)
	{
		// add registration text
		NEW(RegFont, CFont());
		if (RegFont->Init(ThePlatform, GraphicsRef, &CString(REGISTRATION_FONT)) == false)
		{
			delete RegFont;
			RegFont = NULL;
		}
	}
#endif

#ifdef	CASSIO
	GraphicsRef->EndDraw();
	GraphicsRef->StartDraw();
#endif

	count=0;
	create_mouse();

	// Clear the timer.
	Timer50ths->ClearNumberOfEvents();
	Redraw = true;
	while(1)
	{
		UpdatePopulousData();

		if (Redraw)
		{
			GraphicsRef->CopyScreen(o_screen,d_screen);
			if (RegFont)
			{
				GraphicsRef->DrawString(RegFont, 
										&CString("Unregistered version."),
										120, 
										214,
										TEXT_CENTRED);			
				GraphicsRef->DrawString(RegFont, 
										&CString("Tap here to register!"),
										120, 
										226,
										TEXT_CENTRED);	
				GraphicsRef->DrawString(RegFont, 
										&CString("XXXXXXXXXXXX"),
										232, 
										156,
										TEXT_LEFT);
				GraphicsRef->DrawString(RegFont, 
										&CString("XXXXXXXXXXXX"),
										232, 
										156 + 28,
										TEXT_LEFT);
			}
#ifdef	CASSIO
			GraphicsRef->EndDraw();
			GraphicsRef->StartDraw();
#endif
		}

		Timer50ths->BlockUntilClock();
		count += Timer50ths->GetNumberOfEvents();
		Timer50ths->ClearNumberOfEvents();
		if (count > (20 * 50))
			break;

		POINT *mouse = ControllerRef->GetMousePosition();

		if (ControllerRef->IsButtonPressedDB(MouseSelect) == true)
		{
			if (	(mouse->x >= 230) 
				&&	(mouse->x < 320)
				&&	(mouse->y >= (152)) 
				&&	(mouse->y <	240))
			{
				switch ((mouse->y - 152) / 14)
				{
				case 0:
					if (ThePreferences->Registered)
					{
						if (RegFont) delete RegFont; RegFont = NULL;
						return 3;	// Tutorial
					}
					break;

				case 1:	
					if (RegFont) delete RegFont; RegFont = NULL;		
					return 2;	// Conquest
				case 2:	
					if (ThePreferences->Registered)
					{
						if (RegFont) delete RegFont; RegFont = NULL;		
						return 1;	// Custom
					}
					break;
				case 3:
					if (RegFont) delete RegFont; RegFont = NULL;
					return -10;	// Options
				case 4:
					if (RegFont) delete RegFont; RegFont = NULL;
					return -11;	// Help
				case 5:
				case 6:
					if (RegFont) delete RegFont; RegFont = NULL;
					return -12;	// Quit
				default:
					break;
				}
			}

#ifndef BOXEDSALE
			if (!ThePreferences->Registered)
			{
				if (	(mouse->x >= 0)
					&&	(mouse->x < 222)
					&&	(mouse->y > 200)
					&&	(mouse->y < 240)
					)
				{
					if (RegFont) delete RegFont; RegFont = NULL;
					return -13;	// Registration!
				}
			}
#endif
		
			if (RegFont) delete RegFont; RegFont = NULL;
			return -1;
		}
	}
	if (RegFont) delete RegFont; RegFont = NULL;
	return(selected);
}

// ----
#define	OPTIONS_IMAGE						"options"
#define	DONE_BUTTON_TOP						205
#define	DONE_BUTTON_BOTTOM					(DONE_BUTTON_TOP + 30)
#define	DONE_BUTTON_LEFT					145
#define	DONE_BUTTON_RIGHT					(DONE_BUTTON_LEFT + 78)

#define	MUSIC_ON_BUTTON_TOP					67
#define	MUSIC_ON_BUTTON_BOTTOM				(MUSIC_ON_BUTTON_TOP + 28)
#define	MUSIC_ON_BUTTON_LEFT				35
#define	MUSIC_ON_BUTTON_RIGHT				(MUSIC_ON_BUTTON_LEFT + 44)
#define	MUSIC_OFF_BUTTON_TOP				66
#define	MUSIC_OFF_BUTTON_BOTTOM				(MUSIC_OFF_BUTTON_TOP + 28)
#define	MUSIC_OFF_BUTTON_LEFT				90
#define	MUSIC_OFF_BUTTON_RIGHT				(MUSIC_OFF_BUTTON_LEFT + 44)

#define	EFFECTS_ON_BUTTON_TOP				67
#define	EFFECTS_ON_BUTTON_BOTTOM			(EFFECTS_ON_BUTTON_TOP + 28)
#define	EFFECTS_ON_BUTTON_LEFT				192
#define	EFFECTS_ON_BUTTON_RIGHT				(EFFECTS_ON_BUTTON_LEFT + 44)
#define	EFFECTS_OFF_BUTTON_TOP				66
#define	EFFECTS_OFF_BUTTON_BOTTOM			(EFFECTS_OFF_BUTTON_TOP + 28)
#define	EFFECTS_OFF_BUTTON_LEFT				247
#define	EFFECTS_OFF_BUTTON_RIGHT			(EFFECTS_OFF_BUTTON_LEFT + 44)

#define	VOLUME_UP_BUTTON_TOP				171
#define	VOLUME_UP_BUTTON_BOTTOM				(VOLUME_UP_BUTTON_TOP + 31)
#define	VOLUME_UP_BUTTON_LEFT				98
#define	VOLUME_UP_BUTTON_RIGHT				(VOLUME_UP_BUTTON_LEFT + 31)
#define	VOLUME_DOWN_BUTTON_TOP				171
#define	VOLUME_DOWN_BUTTON_BOTTOM			(VOLUME_DOWN_BUTTON_TOP + 31)
#define	VOLUME_DOWN_BUTTON_LEFT				37
#define	VOLUME_DOWN_BUTTON_RIGHT			(VOLUME_DOWN_BUTTON_LEFT + 31)

#define	ORIENTATION_BUTTON_TOP				133
#define	ORIENTATION_BUTTON_BOTTOM			(ORIENTATION_BUTTON_TOP + 89)
#define	ORIENTATION_BUTTON_LEFT				170
#define	ORIENTATION_BUTTON_RIGHT			(ORIENTATION_BUTTON_LEFT + 134)

void	CPopulous::OptionsScreen()
{	
	// Clear all the buffers
	for (int count = 4; count--;)
	{
		GraphicsRef->SetActiveScreen(count);
		GraphicsRef->ClearScreen(0, 0, 0);
	}

	SAFELY_DELETE(BackGroundImage);
	NEW(BackGroundImage, CGraphicsData());
	TheSoundSystem->Process();
	SAFELY_CONTROL(if, BackGroundImage, Init(GraphicsRef, &CString(OPTIONS_IMAGE), ThePlatform), ==, false)
		return;

	POINT	*mousePosition;
//	bool	
	Redraw = true;
	while (true)
	{
		TheGameWindow->Process();
		TheSoundSystem->Process();
#if 0
#ifdef NOT_FINAL_BUILD
		if (ControllerRef->IsButtonPressed(DebugQuickExit))
		{
			exit(0);
		}
#endif
#endif
		// Force a screen update (in some places).
		if (TheGameWindow->GetJustActivated())
		{
			RedrawCount = 10;
			Redraw = true;
		}

		if (RedrawCount >= 0)
		{
			RedrawCount--;
		}
		else
		{
			RedrawCount = -1;
		}

		if (!RedrawCount)
		{
			Redraw = true;
		}

		mousePosition = ControllerRef->GetMousePosition();
		int y = mousePosition->y;
		int x = mousePosition->x;
		if (ControllerRef->IsButtonPressedDB(MouseSelect))
		{
			// Exit
			if (	(y > DONE_BUTTON_TOP)
				&&	(y < DONE_BUTTON_BOTTOM)
				&&	(x > DONE_BUTTON_LEFT)
				&&	(x < DONE_BUTTON_RIGHT))
			{
				break;
			}

			// Music on.
			if (	(y > MUSIC_ON_BUTTON_TOP)
				&&	(y < MUSIC_ON_BUTTON_BOTTOM)
				&&	(x > MUSIC_ON_BUTTON_LEFT)
				&&	(x < MUSIC_ON_BUTTON_RIGHT)
				&&	(!ThePreferences->MusicEnabled))
			{
				ThePreferences->MusicEnabled = true;
				TheSoundSystem->PlayMod();
				Redraw = true;
			}

			// Music off.
			if (	(y > MUSIC_OFF_BUTTON_TOP)
				&&	(y < MUSIC_OFF_BUTTON_BOTTOM)
				&&	(x > MUSIC_OFF_BUTTON_LEFT)
				&&	(x < MUSIC_OFF_BUTTON_RIGHT)
				&&	(ThePreferences->MusicEnabled))
			{
				ThePreferences->MusicEnabled = false;
				TheSoundSystem->StopMod();
				Redraw = true;
			}

			// Effects on.
			if (	(y > EFFECTS_ON_BUTTON_TOP)
				&&	(y < EFFECTS_ON_BUTTON_BOTTOM)
				&&	(x > EFFECTS_ON_BUTTON_LEFT)
				&&	(x < EFFECTS_ON_BUTTON_RIGHT)
				&&	(!ThePreferences->EffectsEnabled))
			{
				ThePreferences->EffectsEnabled = true;
				Redraw = true;
			}

			// Effects off.
			if (	(y > EFFECTS_OFF_BUTTON_TOP)
				&&	(y < EFFECTS_OFF_BUTTON_BOTTOM)
				&&	(x > EFFECTS_OFF_BUTTON_LEFT)
				&&	(x < EFFECTS_OFF_BUTTON_RIGHT)
				&&	(ThePreferences->EffectsEnabled))
			{
				ThePreferences->EffectsEnabled = false;
				TheSoundSystem->StopAllSamples();
				Redraw = true;
			}

			// Volume on.
			if (	(y > VOLUME_UP_BUTTON_TOP)
				&&	(y < VOLUME_UP_BUTTON_BOTTOM)
				&&	(x > VOLUME_UP_BUTTON_LEFT)
				&&	(x < VOLUME_UP_BUTTON_RIGHT)
				&&	(ThePreferences->Volume < 10))
			{
				ThePreferences->Volume++;
				TheSoundSystem->SetVolume(ThePreferences->Volume);
				Redraw = true;
			}

			// Volume off.
			if (	(y > VOLUME_DOWN_BUTTON_TOP)
				&&	(y < VOLUME_DOWN_BUTTON_BOTTOM)
				&&	(x > VOLUME_DOWN_BUTTON_LEFT)
				&&	(x < VOLUME_DOWN_BUTTON_RIGHT)
				&&	(ThePreferences->Volume > 0))
			{
				ThePreferences->Volume--;
				TheSoundSystem->SetVolume(ThePreferences->Volume);
				Redraw = true;
			}

			// Swap orientation.
			if (	(y > ORIENTATION_BUTTON_TOP)
				&&	(y < ORIENTATION_BUTTON_BOTTOM)
				&&	(x > ORIENTATION_BUTTON_LEFT)
				&&	(x < ORIENTATION_BUTTON_RIGHT))
			{
				if (ThePreferences->PreferredOrientation == EO_Landscape_Normal)
					ThePreferences->PreferredOrientation = EO_Landscape_Flipped;
				else
					ThePreferences->PreferredOrientation = EO_Landscape_Normal;

				GraphicsRef->SetOrientation(ThePreferences->PreferredOrientation);
				ControllerRef->SetOrientation(ThePreferences->PreferredOrientation);

				Redraw = true;
			}
		}

		if (Redraw)
		{
			GraphicsRef->StartDraw();
			GraphicsRef->SetActiveScreen(0);

			Region.left = 0;
			Region.top = 0;
			Region.right = 320;
			Region.bottom = 240;
			GraphicsRef->DrawSprite(0, 0, BackGroundImage, Region);

			sprintf(convert, "%d", ThePreferences->Volume);
			TempString = convert;
			GraphicsRef->DrawString(StandardFont, &TempString, 67 + 15, 171 + ((31 - 8) >> 1), TEXT_CENTRED);

			if (ThePreferences->EffectsEnabled)
			{
				Region.left		= EFFECTS_ON_BUTTON_LEFT;
				Region.top		= EFFECTS_ON_BUTTON_BOTTOM + 8;
				Region.right	= EFFECTS_ON_BUTTON_RIGHT;
				Region.bottom	= EFFECTS_ON_BUTTON_BOTTOM + 10;
			}
			else
			{
				Region.left		= EFFECTS_OFF_BUTTON_LEFT;
				Region.top		= EFFECTS_OFF_BUTTON_BOTTOM + 8;
				Region.right	= EFFECTS_OFF_BUTTON_RIGHT;
				Region.bottom	= EFFECTS_OFF_BUTTON_BOTTOM + 10;
			}
			GraphicsRef->DrawRect(&Region, 221, 169, 46);

			if (ThePreferences->MusicEnabled)
			{
				Region.left		= MUSIC_ON_BUTTON_LEFT;
				Region.top		= MUSIC_ON_BUTTON_BOTTOM + 8;
				Region.right	= MUSIC_ON_BUTTON_RIGHT;
				Region.bottom	= MUSIC_ON_BUTTON_BOTTOM + 10;
			}
			else
			{
				Region.left		= MUSIC_OFF_BUTTON_LEFT;
				Region.top		= MUSIC_OFF_BUTTON_BOTTOM + 8;
				Region.right	= MUSIC_OFF_BUTTON_RIGHT;
				Region.bottom	= MUSIC_OFF_BUTTON_BOTTOM + 10;
			}
			GraphicsRef->DrawRect(&Region, 221, 169, 46);


			// All done (this is required for hardware 
			// double-buffered devices, such as the 
			// Casio-thing-a-peia).
			GraphicsRef->EndDraw();
			Redraw = false;
		}
	}
	ThePreferences->SavePreferences();

	GraphicsRef->StartDraw();
	for (count = 4; count--;)
	{
		GraphicsRef->SetActiveScreen(count);
		GraphicsRef->ClearScreen(0, 0, 0);
	}

	return;
}

#ifndef	BOXEDSALE
void	CPopulous::RegistrationScreen()
{	
	#define	REGMESSAGELINES		7

	CString		Temp;
	CString		*RegMessage[REGMESSAGELINES];
	RECT		all;
	RegMessage[0] = new CString("The gameplay in this demo version of");
	RegMessage[1] = new CString("Populous is limited. To register and ");
	RegMessage[2] = new CString("experience the full game visit");
	RegMessage[3] = new CString("www.portable-games.com/populous");
	RegMessage[4] = new CString(" ");
	RegMessage[5] = new CString("Your username is: ");
	RegMessage[6] = new CString("");

	// Clear all the buffers
	for (int count = 4; count--;)
	{
		GraphicsRef->SetActiveScreen(count);
		GraphicsRef->ClearScreen(0, 0, 0);
	}

	SAFELY_DELETE(BackGroundImage);
	NEW(BackGroundImage, CGraphicsData());
	TheSoundSystem->Process();
	SAFELY_CONTROL(if, BackGroundImage, Init(GraphicsRef, &CString(REGISTRATION_IMAGE), ThePlatform), ==, false)
		return;

	CFont	*RegFont;
	NEW(RegFont, CFont());
	if (RegFont->Init(ThePlatform, GraphicsRef, &CString(REGISTRATION_FONT)) == false)
		return;

	CGraphicsData		*RegKeyboardGraphic;
	NEW(RegKeyboardGraphic, CGraphicsData());
	SAFELY_CONTROL(if, RegKeyboardGraphic, Init(GraphicsRef, &CString(REGKEYBOARD_IMAGE), ThePlatform), ==, false)
		return;

	#define KEY_SIZE			20
	#define KEYBOARD_START_X	(160 - 6 * KEY_SIZE)
	#define KEYBOARD_START_Y	(148)

	POINT	*mousePosition;
//	bool	
	Redraw = true;
	unsigned long	Tempcode	= ThePreferences->GetRegistrationCode();
	bool			EnteredCode = false;
	bool			Mistake		= false;
	while (true)
	{
		TheGameWindow->Process();
		TheSoundSystem->Process();

		// Force a screen update (in some places).
		if (TheGameWindow->GetJustActivated())
		{
			RedrawCount = 10;
			Redraw = true;
		}

		if (RedrawCount >= 0)
		{
			RedrawCount--;
		}
		else
		{
			RedrawCount = -1;
		}

		if (!RedrawCount)
		{
			Redraw = true;
		}

		mousePosition = ControllerRef->GetMousePosition();
		int y = mousePosition->y;
		int x = mousePosition->x;
		if (ControllerRef->IsButtonPressedDB(MouseSelect))
		{
			// Exit
			if (	(y > DONE_BUTTON_TOP)
				&&	(y < DONE_BUTTON_BOTTOM)
				&&	(x > DONE_BUTTON_LEFT)
				&&	(x < DONE_BUTTON_RIGHT))
			{
				if (Mistake)
					break;

				if (ThePreferences->Registered)
					break;

				// If the user entered/modified a code...
				if (!EnteredCode)
					break;

				// Attempt registration
				CString	TempName;
				if (!GetRegistrationString(&TempName))
					break;	// Invalid username catch could go here!

				// Good name, so lets test the name against the key the user entered.
				if (ValidateRegistrationDetails(&TempName, Tempcode, PRODUCT_ACTIVATION_RPN_STRING))
				{
					// Successfully registered
					ThePreferences->Registered = true;
					ThePreferences->SetRegistrationCode(Tempcode);

					unsigned long codecrypt = EncryptCode(ThePreferences->GetRegistrationCode());
					SaveToRegistry(	HKEY_LOCAL_MACHINE,							// Existing key or default root.
									&CString(REG_PREFERENCE_PATH),				// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
									&CString(REG_ACTIVATION_CODE),				// Name of the value to store.
									REG_BINARY,									// Specifies the format for storing the data.
									(char*)&codecrypt,							// A pointer to the data to store.
									sizeof(codecrypt));

					for (int iiiii = REGMESSAGELINES; iiiii--;)
					{
						(*RegMessage[iiiii]) = " ";
					}
					(*RegMessage[0]) = "Thank you for purchasing Populous.";
					(*RegMessage[2]) = "Visit www.portable-games.com again soon";
					(*RegMessage[3]) = "for news of more amazing handheld games.";
					Redraw = true;
				}
				else
				{
					for (int iiiii = REGMESSAGELINES; iiiii--;)
					{
						(*RegMessage[iiiii]) = " ";
					}
					(*RegMessage[0]) = "Sorry, the product key you entered does";
					(*RegMessage[1]) = "not match your username.";
					(*RegMessage[2]) = "Please try again, or contact ";
					(*RegMessage[3]) = "www.portable-games.com for advice.";
					Mistake = true;
					Redraw = true;
				}
			}

			if (	(!ThePreferences->Registered)
				&&	(!Mistake))
			{
				// Keyboard check - check numbers
				if (	(y > KEYBOARD_START_Y)
					&&	(y < KEYBOARD_START_Y + KEY_SIZE)
					&&	(x > KEYBOARD_START_X)
					&&	(x < KEYBOARD_START_X + KEY_SIZE * 10)
					)
				{
					if (Tempcode < 10000)
					{
						Tempcode *= 10;
						Tempcode += (((x - KEYBOARD_START_X) / KEY_SIZE) + 1) % 10;
						EnteredCode = true;
					}
					Redraw = true;
				}
			
				// Check delete
				if (	(y > KEYBOARD_START_Y)
					&&	(y < KEYBOARD_START_Y + KEY_SIZE)
					&&	(x > KEYBOARD_START_X + KEY_SIZE * 10)
					&&	(x < KEYBOARD_START_X + KEY_SIZE * 12)
					)
				{
					Tempcode /= 10;	// Drop off the last digit!
					EnteredCode = true;
					Redraw = true;
				}
			}
		}

		if (Redraw)
		{
			GraphicsRef->StartDraw();
			GraphicsRef->SetActiveScreen(0);

			Region.left = 0;
			Region.top = 0;
			Region.right = 320;
			Region.bottom = 240;
			GraphicsRef->DrawSprite(0, 0, BackGroundImage, Region);

			for (int i = 0; i < REGMESSAGELINES; i++)
			{
				GraphicsRef->DrawString(RegFont, 
										RegMessage[i],
										20, 
										60 + 10 * i,
										TEXT_LEFT);
			}

			if (	(!ThePreferences->Registered)
				&&	(!Mistake)
				)
			{
				GetRegistrationString(&Temp);
				GraphicsRef->DrawString(RegFont, 
										&Temp,
										160, 
										60 + 10 * 6,
										TEXT_CENTRED);

				CString TempStr;
				char	TempChars[32];
				sprintf(TempChars, "%05d", Tempcode);
				TempStr = TempChars;
				GraphicsRef->DrawString(RegFont, 
										&TempStr,
										160, 
										KEYBOARD_START_Y - 12,
										TEXT_CENTRED);

				// draw the numbers
				all.left = 0;
				all.top = 0;
				all.right = all.left + KEY_SIZE * 12;
				all.bottom = all.top + KEY_SIZE;					
				SAFELY_CALL(GraphicsRef, DrawSprite(KEYBOARD_START_X, KEYBOARD_START_Y, RegKeyboardGraphic, all));

				/*
				all.left = 10 * 16;
				all.top = 0;
				all.right = all.left + 16;
				all.bottom = all.top + 32;		
				SAFELY_CALL(GraphicsRef, DrawSprite(KEYBOARD_START_X + 10 * KEY_SIZE, KEYBOARD_START_Y, RegKeyboardGraphic, all));
				*/
			}

			// All done (this is required for hardware 
			// double-buffered devices, such as the 
			// Casio-thing-a-peia).
			GraphicsRef->EndDraw();
			Redraw = false;
		}
	}

	delete RegFont;
	RegFont = NULL;

	delete RegKeyboardGraphic;
	RegKeyboardGraphic = NULL;
	
	GraphicsRef->StartDraw();
	for (count = 4; count--;)
	{
		GraphicsRef->SetActiveScreen(count);
		GraphicsRef->ClearScreen(0, 0, 0);
	}

	return;
}

#endif

//******************************************************************
// This code added from title_people.. just wanted to keep the front
// end code in the one place for simplicity.
//******************************************************************

#define NO_WALKERS          16
#define FIRST_WALKERS       4
#define BLUE_ONE            0
#define RED_ONE             16
#define BLUE_KNIGHT         32
#define RED_KNIGHT          48
#define BLUE_FLAG           64
#define RED_FLAG            66
#define END_OF_FIGHT        240

#define WALKER_BUILDS       100
#define WALKER_EFFECT       130
#define WALKER_WAITS        220
#define WALKER_END          255

#define WALKER_SETS         247
#define WALKER_ENDS         246
#define WALKER_FIGHT        249
#define WALKER_WAIT_HOLD    250
#define WALKER_WAIT0        251
#define WALKER_RES0         253
#define WALKER_RES1         254
#define WALKER_HOLD         252

SHORT  line_at[2];
SHORT   fight_count;

struct  w_walker    walker[NO_WALKERS]=
{
	{BLUE_ONE,0,0,8,38,0,0},
	{BLUE_ONE,0,0,8,44,0,0},
	{BLUE_ONE,0,0,8,15,0,0},
	{BLUE_ONE,0,0,8,15,0,0},
	{BLUE_ONE,0,0,8,15,0,0},
	{BLUE_ONE,0,0,8,15,0,0},
	{BLUE_KNIGHT,0,0,8,15,1,0},
	{RED_ONE,0,0,8,83,0,0},
	{RED_ONE,0,0,8,80,0,0},
	{RED_ONE,0,0,8,50,0,0},
	{RED_ONE,0,0,8,50,0,0},
	{RED_ONE,0,0,8,50,0,0},
	{RED_ONE,0,0,8,50,0,0},
	{RED_KNIGHT,0,0,8,50,0,0},
	{BLUE_FLAG,0,0,8,15,-1,1},
	{RED_FLAG,0,0,8,50,-1,1}
};
SHORT   start_at[]={38,44,15,15,15,15,15,83,80,50,50,50,50,50,15,50};

UBYTE   the_effect[]=
{
		64,65,66,67,68,70,71,72,73,75,76,85,86,87,88,89,90,91,92,93,94,95,96,
        97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,
        117,118,119,120,121,122,123,124,125,126,127,128,130,131,132,133,134,135,136,137,
        138,139,140,141,142,143,144,144,145,146
};

UBYTE   the_buildings[]=
{       
	33,34,35,36,37,38,39,40,41,42,43,44,54,55,56,57,58,59,60,61,62,63,64,65
};      


void	CPopulous::start_people()
{
	SHORT   bounce = 1;
	SHORT   c1;
	text_line[0]=(LONG)ts1;
	text_line[1]=(LONG)ts2;
	line_at[0]= 75;
	line_at[1]= 50;
	fight_count=0;
	for(c1=0;building[c1].where;c1++)
		building[c1].where=0;
	for(c1=0;c1<NO_WALKERS;c1++)
	{
		walker[c1].pos=0;
		walker[c1].where=start_at[c1];
		walker[c1].direction=8;
	}
	move_screen(o_screen,b_screen);
	init_extras(NO_WALKERS);
	while(bounce)
	{
		toggle=!toggle;
		bounce=process_extras(NO_WALKERS);
		move_screen(b_screen,w_screen);
		for(c1=0;c1<2;c1++)
			move_people(w_screen,walker[c1].where,walker[c1].direction,walker[c1].image,walker[c1].frame,extras[c1],0);
		for(c1=7;c1<9;c1++)
			move_people(w_screen,walker[c1].where,walker[c1].direction,walker[c1].image,walker[c1].frame,extras[c1],0);
		swap_screens();
	}
}


void	CPopulous::do_people()
{
	// This code allows me to see all the sprites.
	// Don't enable it in the release build!
	
	/*
	int tttt = GraphicsRef->GetActiveScreen();
	GraphicsRef->SetActiveScreen(d_screen);
	GraphicsRef->ClearScreen(0, 0, 0);
	for (int iii = 0; iii < 180; iii++)
	{
		Region.left		= 0;
		Region.top		= 16 * iii;
		Region.right	= 16;
		Region.bottom	= Region.top + 16;
		GraphicsRef->DrawSprite((iii % 20) * 16, 
								(iii / 20) * 16, 
								SpriteGraphics,
								Region);
	}

	left_button = 2;
	while (left_button)
	{
		UpdatePopulousData();
	}
	GraphicsRef->SetActiveScreen(tttt);
	*/

	SHORT  qaz=0;
	SHORT	x,y,dx,dy;
	SHORT	do_building=0;
	SHORT	res0_count=0;
	SHORT	res1_count=0;
	register struct  w_walker	 *walk;
	register SHORT	c1,c2,c3,c4,c5,c6;
	start_people();
	left_button = 2;
	while(1)
	{
		UpdatePopulousData();
		
		toggle=!toggle;
		keyboard();
		if (	(left_button == 0) 
			||	(asckey != 0))
			return;

		move_screen(b_screen,w_screen);
		for(c1=0;c1<2;c1++)
		{
			if(line_at[c1]==50)
				do_text((long*)text_line[c1]);
			qaz=line_at[c1];
			if(qaz>25)
				qaz=50-qaz;
			if(qaz<0)
				qaz=0;
			
			show_text(text_line[c1],w_screen+(line_at[c1])*160,qaz);

			text_line[c1]--;
			if(text_line[c1] < -1100)
				text_line[c1] = 40;

			line_at[c1]--;
			if(line_at[c1] < 0)
				line_at[c1]=50;
		}
		if(do_building)
		{
			move_screen(o_screen,b_screen);
			if(res1_count >=20)
				res_head(b_screen,0,res1_count);
			for(c1=0;building[c1].where;c1++)
			{
				draw_building(b_screen,building[c1].where,building[c1].which);
			}
			do_building=0;
		}
		for(c1=c6=0,walk=&walker[0];c1 < NO_WALKERS;c1++,walk++)
		{
			c2=(unsigned char)dest_array[(c1 * 162) + walk->pos];
			if(c2 < WALKER_BUILDS)
			{
				if(walk->where == c2)
				{
					walk->pos++;
					c2=(unsigned char)dest_array[(c1 * 162) + walk->pos];//dests[c1][walk->pos];
				}
				x=walk->where%10;
				y=walk->where/10;
				dx=sign(c2%10-x);
				dy=sign(c2/10-y);
				walk->direction=dx+dy*10;
				move_people(w_screen,walk->where,walk->direction,walk->image,walk->frame,extras[c1],walk->special);
				//if((walk->frame=(walk->frame+1) & 7)==0)
				walk->frame = ((walk->frame+1) & 7);
				if(walk->frame == 0)
					walk->where+=walk->direction;
			}
			else if(c2 >= WALKER_BUILDS && c2 < WALKER_EFFECT)
			{
				for(c3=0;building[c3].where;c3++);
				for(c4=0;c4<c3;c4++)
				{
					if(walk->where == building[c4].where)
					{
						building[c4].which=the_buildings[c2-WALKER_BUILDS];
						break;
					}
					else if(walk->where < building[c4].where)
					{
						for(c5=c3;c5 >= c4+1;c5--)
						{
							building[c5].where=building[c5-1].where;
							building[c5].which=building[c5-1].which;
						}
						building[c5].where=walk->where;
						building[c5].which=the_buildings[c2-WALKER_BUILDS];
						break;
					}
				}
				if(c3 == c4)
				{
					building[c3].where=walk->where;
					building[c3].which=the_buildings[c2-WALKER_BUILDS];
				}
				move_people(w_screen,walk->where,walk->direction,walk->image,walk->frame,extras[c1],0);
				walk->pos++;
				do_building=1;
			}
			else if(c2 >=WALKER_EFFECT && c2 <= WALKER_WAITS)
			{
				move_people(w_screen,walk->where,walk->direction,the_effect[c2-WALKER_EFFECT],walk->frame,extras[c1],walk->special);
				walk->pos++;
			}
			else if(c2 == WALKER_RES0)
			{
				if(res0_count>=19)
				{
					res_head(b_screen+32*4,2048,res0_count);
					walk->pos++;
				}
				res_head(w_screen+32*4,2048,res0_count);
				res0_count++;
			}
			else if(c2 == WALKER_RES1)
			{
				if(res1_count>=19)
				{
					res_head(b_screen,0,res1_count);
					walk->pos++;
				}
				res_head(w_screen,0,res1_count);
				res1_count++;
			}
			else if(c2 == WALKER_HOLD)
			{
				if(res0_count >= 20)
					walk->pos++;
			}
			else if(c2 == WALKER_WAIT0)
			{
				c3=27+toggle;
				if(c1>=7)
					c3+=2;
				if(Random() > 30000)
					c3=14+(c1>=7)*4;
				move_people(w_screen,walk->where,walk->direction,the_effect[c3],walk->frame,extras[c1],0);
			}
			else if(c2 == WALKER_WAIT_HOLD)
			{
				if(walk->count == -1) 
					move_people(w_screen,walk->where,walk->direction,walk->image,walk->frame,extras[c1],walk->special);
				else
				{
					walk->count++;
					if(walk->count == 4)
						walk->count=0;
					move_people(w_screen,walk->where,walk->direction,walk->image+walk->count,walk->frame,extras[c1],0);
				}
			}
			else if(c2 == WALKER_FIGHT)
			{
				if(walk->image == BLUE_KNIGHT)
				{
					fight_count++;
					if(fight_count == END_OF_FIGHT)
					{
						for(c3=0;c3<NO_WALKERS;c3++)
							walker[c3].pos++;
					}
					walk->frame+=walk->count;
					if(walk->frame==-1 || walk->frame == 8)
					{
						walk->count=-walk->count;
						walk->frame+=walk->count;
					}
					move_people(w_screen,walk->where,walk->direction,138+(fight_count&3),walk->frame,extras[c1],walk->special);
				}
			}
			else if(c2 == WALKER_SETS)
			{
				for(c3=0;c3<NO_WALKERS;c3++)
					if (((unsigned char)dest_array[(c3 * 162) + walker[c3].pos]) != WALKER_END) //(dests[c3][walker[c3].pos] != WALKER_END
						walker[c3].pos++;
			}
			else if(c2 == WALKER_ENDS)
			{
				fight_count++;
				move_people(w_screen,walk->where,walk->direction,121+(fight_count&3),walk->frame,extras[c1],0);
			}
		}
		swap_screens();

		// Stop us running too fast.
		IntroFrameLimiter->BlockUntilClock();
		IntroFrameLimiter->ClearNumberOfEvents();
	}
}


int CPopulous::do_rock()
{
	SHORT  i;
	init_extras(18);
	left_button = 2;
	while(process_extras(18))
	{
		UpdatePopulousData();

		move_screen(b_screen,w_screen);
		draw_s32(w_screen,80,168-extras[0],5);
		for(i=1;i<=8;i++)
			draw_s32(w_screen,80+16*i,168-extras[i],6); 
			
		draw_s32(w_screen,224,168-extras[9],7);
		for(i=0;i<8;i++)
			draw_s32(w_screen,96+16*i,168-extras[i],game_name[i]);
		swap_screens();
		keyboard();
		if (	(left_button == 0)
			||	(asckey != 0))
			return(1);
	}
	for(i=0;i<18;i++)
	{
		extras[i]=160;
		e_vels[i]=Random()&7;
	}
	left_button = 2;
	while(process_extras(18))
	{
		UpdatePopulousData();

		move_screen(b_screen,w_screen);
		draw_s32(w_screen,80,8+extras[0],5);
		
		for(i=1;i<=8;i++)
			draw_s32(w_screen,80+16*i,8+extras[i],6);  
		draw_s32(w_screen,224,8+extras[9],7);
		for(i=0;i<8;i++)
			draw_s32(w_screen,96+16*i,8+extras[i],game_name[i]);
		swap_screens();
		keyboard();
		if (	(left_button == 0)
			||	(asckey != 0))
			return(1);
	}
	return(0);
}


void	CPopulous::init_extras(SHORT last)
{
	SHORT  i;
	for(i=0;i<last;i++)
	{
		extras[i] = 232+Random()%13;
		e_vels[i] = Random()%9;
	}
}


int		CPopulous::process_extras(SHORT last)
{
	register SHORT  i, vtemp, etemp;
	SHORT  ret = 0;
	for(i=0;i<last;i++)
	{
		vtemp = e_vels[i];
		etemp = extras[i];
		vtemp ++;
		etemp -=vtemp;
		if(etemp<0)
		{
			vtemp = -((vtemp)/2);
			etemp = 0;
		}
		if(!ret)
			ret = vtemp;
		extras[i]=etemp;
		e_vels[i]=vtemp;
	}
	return(ret);
}


void	CPopulous::display()
{
//	SHORT   c1;
	read_back_scr((UBYTE*)"demo");
	//if(screen_pointer[0])
	//	GraphicsRef->ClearScreen(0,0,0);
//	os_swi0(OS_WriteI+22);
//	os_swi0(OS_WriteI+9);
//	os_swi0(OS_RemoveCursors);
	open_screen();
//	change_palette(16);
	init_extras(100);
	s_size = 128;
	start_scroll();
	left_button = 2;

	while(process_extras(100))
	{
		UpdatePopulousData();

		move_screen(back_scr,w_screen);// b_screen
		draw_it();
		swap_screens();
		keyboard();
		if (	(left_button == 0) 
			||	(asckey != 0))
			goto the_end;
	}
	move_screen(d_screen,b_screen);
	move_screen(d_screen,o_screen);
	if(do_rock() == 0)
		do_people();
the_end:;
        old_copper=current_copper;
}


//********************************************************************************************
//        GET     h.equates

//        EXPORT  move_people

//        IMPORT  map_blk
//        IMPORT  toggle
//        IMPORT  draw_sprite

//        AREA    code,CODE


#define D_NW         -11
#define D_N          -10
#define D_NE         -9
#define D_W          -1
#define D_E          1
#define D_SW         9
#define D_S          10
#define D_SE         11


//;       r0      pointer to screen base
//;       r1      position in map
//;       r2      direction
//;       r3      image number
//;       sp+00   frame
//;       sp+04   bounce
//;       sp+08   special

void CPopulous::move_people(long r0, long r1, long r2, long r3, long frame, long bounce, long special)
{
	long	r4, r5, r6, r7, r8, r9, r10, r11, r12;
	long	Stack[1024];
	long	Sp = 0;
	
	//	STMFD	sp!,{r0-r12,lr}
	
	Stack[Sp++] = frame;
	Stack[Sp++] = bounce;
	Stack[Sp++] = special;
	
	//	ADD 	r12,sp,#14*4
	// r12 should point to frame.
	r12 = (long)&(Stack[Sp - 3]);

	//	LDMFD	r12,{r4-r7}
	r4 = *(long*)(r12 + 0);//frame;
	r5 = *(long*)(r12 + 4);//bounce;
	r6 = *(long*)(r12 + 8);//special;
	r7 = 0;

	//	BL		move_people2
	//	LDMFD	sp!,{r0-r12,pc}	
	
move_people2:
	//	ROUT
	//	STMFD	sp!,{r1-r12,lr}
	Stack[Sp++] = r1;			//1
	Stack[Sp++] = r2;
	Stack[Sp++] = r3;
	Stack[Sp++] = r4;
	Stack[Sp++] = r5;
	Stack[Sp++] = r6;
	Stack[Sp++] = r7;
	Stack[Sp++] = r8;	// are: Not initialised
	Stack[Sp++] = r9;	// are: Not initialised
	Stack[Sp++] = r10;	// are: Not initialised
	Stack[Sp++] = r11;	// are: Not initialised
	Stack[Sp++] = r12;
	Stack[Sp++] = 0; // lr		// 13

	//	MOV 	r3,#0
	r3 = 0;

mp2_loop:
	//	SUBS	r1,r1,#10
	r1 -= 10;

	//	ADDPL	r3,r3,#1
	//	BPL 	mp2_loop
	if (r1 >= 0)
	{
		r3++;
		goto mp2_loop;
	}
		
	//	ADD 	r1,r1,#10
	//	MOV 	r2,r1
	//	SUB 	r1,r1,r3
	//	ADD 	r2,r2,r3
	r1 += 10;
	r2 = r1;
	r1 -= r3;
	r2 += r3;

	//	MOV 	r1,r1,ASL#4
	//	MOV 	r2,r2,ASL#3
	r1 = r1 << 4;
	r2 = r2 << 3;

	//	ADD 	r1,r1,#160-8
	//	ADD 	r2,r2,#40-12
	r1 += 160-8;
	r2 += 40-12;

	//	LDMFD	sp,{r3-r8}
	r3 = Stack[Sp - 13];//1];
	r4 = Stack[Sp - 12];//2];
	r5 = Stack[Sp - 11];//3];
	r6 = Stack[Sp - 10];//4];
	r7 = Stack[Sp - 9];//5];
	r8 = Stack[Sp - 8];//6];

	//	MOV 	r7,r7,ASL#16
	r7 = r7 << 16;

	//	SUB 	r2,r2,r7,LSR#16
	r2 -= abs(r7 >> 16);

	//	LDR 	r9,%0					;		pointer to map_blk
	//	ADD 	r9,r9,#200
	r9 = (long)(title_map_blk + 200);

	//	LDRB	r10,[r9,r3] 			;		get altitude of current block
	r10 = *(UBYTE*)(r9 + r3);

	//	MOV 	r10,r10,ASL#24
	//	MOV 	r10,r10,ASR#24
	// No.

	//	SUB 	r2,r2,r10				;		modify screen ypos
	//	ADD 	r3,r3,r4				;		add direction offset
	r2 -= r10;
	r3 += r4;

	//	LDRB	r11,[r9,r3] 			;		get altitude of next block
	r11 = *(UBYTE*)(r9 + r3);

	//	MOV 	r11,r11,ASL#24
	//	MOV 	r11,r11,ASR#24
	// No

	//	SUB 	r10,r10,r11 			;		get difference
	//	MUL 	r10,r6,r10				;		* frame
	r10 -= r11;
	r10 *= r6;

	//	ADD 	r2,r2,r10,ASR#3 		;		modify screen ypos
	r2 += r10 >> 3;

	//	CMP 	r4,#D_NW
	//	BNE 	not_up
	if (r4 != D_NW)
		goto not_up;

	//	MOV 	r9,#0
	//	MOV 	r10,#-2
	//	MOV 	r11,#0
	r9 = 0;
	r10 = -2;
	r11 = 0;

	//	B		end_of_tests
	goto end_of_tests;
		
not_up:
    //CMP     r4,#D_N
    //BNE     not_up_right
	if(r4 != D_N)
		goto not_up_right;

    //MOV     r9,#2
	r9 = 2;

    //MOV     r10,#-1
	r10 = -1;

    //MOV     r11,#2
	r11 = 2;

    //B       end_of_tests
	goto end_of_tests;

not_up_right:
    //CMP     r4,#D_NE
    //BNE     not_right
	if(r4 != D_NE)
		goto not_right;

    //MOV     r9,#4
	r9 = 4;

    //MOV     r10,#0
	r10 = 0;

    //MOV     r11,#4
	r11 = 4;

    //B       end_of_tests
	goto end_of_tests;

not_right:
    //CMP     r4,#D_E
    //BNE     not_down_right
	if(r4 != D_E)
		goto not_down_right;

    //MOV     r9,#2
	r9 = 2;

    //MOV     r10,#1
	r10 = 1;

    //MOV     r11,#6
	r11 = 6;

    //B       end_of_tests
	goto end_of_tests;

not_down_right:
    //CMP     r4,#D_SE
    //BNE     not_down
	if(r4 != D_SE)
		goto not_down;

    //MOV     r9,#0
	r9 = 0;

    //MOV     r10,#2
	r10 = 2;

    //MOV     r11,#8
	r11 = 8;

    //B       end_of_tests
	goto end_of_tests;

not_down:
    //CMP     r4,#D_S
    //BNE     not_down_left
	if(r4 != D_S)
		goto not_down_left;

    //MOV     r9,#-2
	r9 = -2;

    //MOV     r10,#1
	r10 = 1;

    //MOV     r11,#10
	r11 = 10;

    //B       end_of_tests
	goto end_of_tests;

not_down_left:
    //CMP     r4,#D_SW
    //BNE     not_left
	if(r4 != D_SW)
		goto not_left;

    //MOV     r9,#-4
	r9 = -4;

    //MOV     r10,#0
	r10 = 0;

    //MOV     r11,#12
	r11 = 12;

    //B       end_of_tests
	goto end_of_tests;

not_left:
    //CMP     r4,#D_W
    //BNE     not_up_left
	if(r4 != D_W)
		goto not_up_left;

    //MOV     r9,#-2
	r9 = -2;

    //MOV     r10,#-1
	r10 = -1;

    //MOV     r11,#14
	r11 = 14;

    //B       end_of_tests
	goto end_of_tests;

not_up_left:
    //MOV     r9,#0
	r9 = 0;

    //MOV     r10,#0
	r10 =0;

    //MOV     r11,#8
	r11 = 8;

end_of_tests:
    //MLA     r1,r6,r9,r1
	r1 += r6 * r9;

    //MLA     r2,r6,r10,r2
	r2 += r6 * r10;

    //TEQ     r8,#0
    //BNE     do_it_anyway
	if(r8 != 0)
		goto do_it_anyway;

    //CMP     r5,#64
    //BGE     not_a_walker
	if (r5 >= 64)
		goto not_a_walker;

    //ADD     r5,r5,r11
	r5 += r11;

do_it_anyway:
    //STMFD   sp!,{r6,r7}
	Stack[Sp++] = r6;
	Stack[Sp++] = r7;

    //LDR     r7,%1
    //LDRB    r6,[r7,#0]
    //LDRB    r7,[r7,#1]
    //ORR     r6,r6,r7,LSL#8
	r6 = toggle;

    //ADD     r5,r5,r6
	r5 += r6;

    //LDMFD   sp!,{r6,r7}
	r7 = Stack[--Sp];
	r6 = Stack[--Sp];

not_a_walker:   
    //MOV     r3,r5
	r3 = r5;
    //BL      draw_sprite

	// For some reason errors have creep into the code - 
	// meaning that all the sprites are too low, except 
	// the drowning peep, which is 20 pixels too high. 
	// Code doesn't crash, so much easier to hack it.
	if ((r3 >= WATER_SPRITE) && (r3 < WAIT_SPRITE))
		r2 = 156;// Hard coded, because the same code compiled for different CPUs gives different results
	else
		r2 -= 4;

	if (r3 == 143)
		r3 = 141;
	if (r3 == 144)
		r3 = 142;

	draw_sprite(r0, r1, r2, r3);
    //LDMFD   sp!,{r1-r12,pc}

//0
//        DCD     map_blk
//1
//        DCD     toggle
}

