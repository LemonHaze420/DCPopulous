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

void	CPopulous::draw_mouth(int _Screen, int _XPos, int _YPos, int _Height, int _Mouth)
{
	/*
;       r0      screen address
;       r1      xpos
;       r2      ypos
;       r3      height
;       sp+00   mouth_number

        LDR     r4,[sp,#14*4]           ;       get mouth number
        ADD     r0,r0,r1,ASL#2
        MOV     r5,#320/2
        MLA     r0,r2,r5,r0             ;       screen address
		MOV     r5,#6*4
        LDR     r6,%0                   ;       pointer to map_steps
        MLA     r1,r4,r5,r6             ;       graphic address
        MOV     r2,r3                   ;       height
big_block_loop
        LDMIA   r1,{r3-r8}
        STMIA   r0,{r3-r8}
        ADD     r1,r1,#320/2
        ADD     r0,r0,#320/2
        SUBS    r2,r2,#1
        BNE     big_block_loop
        LDMFD   sp!,{r0-r12,pc} 
0
        DCD     map_steps

        END
		*/

	GraphicsRef->SetActiveScreen(_Screen);

	Region.top		= 0;
	Region.bottom	= 1;
	Region.left		= (_Mouth * 48);
	Region.right	= Region.left + 48;

	for (int i = 0; i < _Height; i++)
	{
		GraphicsRef->DrawSprite(_XPos << 3, _YPos + i, MouthGraphics, Region);
		Region.left		+= 320;
		Region.right	= Region.left + 48;
	}
}

// ----
// This method calls code to update our variables, such
// as the mouse position, and feed it through into the 
// way Populous works.
bool		CPopulous::UpdatePopulousData()
{
	int i;

	if (!TheGameWindow->Process())
		return false;

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

	POINT*	mouse;
	mouse = ControllerRef->GetMousePosition();

	if (	(mouse->x >= 0) && (mouse->x < 320)
		&&	(mouse->y >= 0) && (mouse->y < 200))
	{
		// Only update mouse info if the mouse
		// is in the game screen area.
		mousex = mouse->x;
		mousey = mouse->y;

		if (	(ControllerRef->IsButtonPressedDB(MouseSelect))
			&& !(ControllerRef->IsButtonPressed(Select)))
		{
			left_button = 0;	// Think this means mouse down.
//			MouseTimer->ClearNumberOfEvents();
		}

//		if (	(ControllerRef->IsButtonPressed(MouseSelect))
//			&&	!(ControllerRef->IsButtonPressed(Select))
//			&&	(MouseTimer->GetNumberOfEvents()))
//		{
//			left_button = 0;
//			MouseTimer->ClearNumberOfEvents();
//		}

		if (	(ControllerRef->IsButtonPressedDB(MouseSelect))
			&&	(ControllerRef->IsButtonPressed(Select)))
		{
			right_button = 0;
//			MouseTimer->ClearNumberOfEvents();
		}

//		if (	(ControllerRef->IsButtonPressed(MouseSelect))
//			&&	(ControllerRef->IsButtonPressed(Select))
//			&&	(MouseTimer->GetNumberOfEvents()))
//		{
//			right_button = 0;
//			MouseTimer->ClearNumberOfEvents();
//		}
	}
	keyboard();

	// For in game sound music and effects we fake the 
	// music callback being called on a timer basis.
	if (selected >= 0)
	{
		for (i = MusicTimer->GetNumberOfEvents(); i--;)
		{
			ProcessAudio();
		}
	}
	MusicTimer->ClearNumberOfEvents();

	// No process the sound through our interface.
	TheSoundSystem->Process();

	// A work around for devices with hardware double buffering.
	if (ThePreferences->HardwareDoubleBuffering)
	{
 		GraphicsRef->EndDraw();
		GraphicsRef->StartDraw();
	}

#if 0
#ifdef NOT_FINAL_BUILD
	if (ControllerRef->IsButtonPressed(DebugQuickExit))
		exit(0);
#endif
#endif

	return true;
}

// ----
void	CPopulous::toggle_icon(int _Screen, int _XPos, int _YPos, int _Bank)
{
	Region.left		= 0;
	Region.top		= 0;
	Region.right	= 32;
	Region.bottom	= 16;

	/*
;       r0      pointer to screen address
;       r1      xpos
;       r2      ypos
;       r3      screen offset

	  MOV     r4,#320/2*8             ;       convert the x,y and offset
      ADD     r4,r4,#8                ;       values in to a screen address
      MUL     r1,r4,r1
      SUB     r4,r4,#16
      MUL     r2,r4,r2
      ADD     r0,r0,r1
      ADD     r0,r0,r2
      ADD     r0,r0,r3,ASL#2
	*/

	long r4 = 1280 + 8;
	long r1 = r4 * _XPos;
	r4 -= 16;
	long r2 = r4 * _YPos;
	long ScreenAddr = r1 + r2 + (_Bank << 2);

	GraphicsRef->SetActiveScreen(_Screen);
	GraphicsRef->ToggleIcon((ScreenAddr % 160) * 2,
							(ScreenAddr / 160), 
							ToggleIconGraphics, 
							Region);
}

// ----
void	CPopulous::draw_s32(long _Screen, int _X, int _Y, int _Frame)
{
	GraphicsRef->SetActiveScreen(_Screen);

	Region.left		= 0;
	Region.top		= _Frame * 32;
	Region.right	= 16;
	Region.bottom	= Region.top + 32;

	GraphicsRef->DrawSprite(_X, _Y, BigFont, Region);
}

// ----
void	CPopulous::res_head(long r0, long r1, long r2)
{
	int x, y, Screen;

	if (r0 > b_screen)
	{
		Screen = r0 - 128;
		x = 128 * 2;
		y = 0;
	}
	else
	{
		Screen = r0;
		x = 0;
		y = 0;
	}

	GraphicsRef->SetActiveScreen(Screen);
	for (int j = 0; j < 64; j++)
	{
		for (int i = 0; i < 64; i++)
		{
			//if (!((((unsigned char)res_data[r2 + (i % 6)]) >> (i % 8)) & 0x01))
			if (!(((unsigned char)res_data[(r2 << 1) + (i % 6)] >> ((j + i) % 8)) & 0x01))
			{
				Region.left		= i;
				Region.top		= ((r1 / 2048) * 64) + j; // Guess at the height
				Region.right	= Region.left + 1;
				Region.bottom	= Region.top + 1;
				GraphicsRef->DrawSprite(x + i, y + j, HeadGraphics, Region);
			}
		}
	}
}

/*
// ----
void	CPopulous::DrawController(int _X, int _Y)
{
	if (!PDAGraphics)
	{
		TempString = "pda";
		NEW(PDAGraphics, CGraphicsData());
		PDAGraphics->Init(GraphicsRef, &TempString, ThePlatform);
	}
	
	if (!PDAKeysGraphics)
	{
		TempString = "keys";
		NEW(PDAKeysGraphics, CGraphicsData());
		PDAKeysGraphics->Init(GraphicsRef, &TempString, ThePlatform);
	}

	// Draw the device image
	Region.left		= 0;
	Region.top		= 0;
	Region.right	= 100;
	Region.bottom	= 75;
	GraphicsRef->DrawSprite(_X, 
							_Y, 
							PDAGraphics, 
							Region, 
							(GraphicsRef->GetOrientation() == EO_Landscape_Normal)?0:SPRITE_ROTATE_180);


	int ii;
	switch (ThePlatform->GetMachineManufacturer())
	{
	case EM_HP_Generic:
		ii = 2;
		break;

	case EM_Compaq_Generic:
		ii = 0;
		break;

	case EM_Casio_Generic:
	default:
		ii = 1;	// Looks the most like a general pda layout
		break;
	}

	Region.left		= 0;
	Region.top		= 75 * ii;
	Region.right	= 100;
	Region.bottom	= Region.top + 75;
	GraphicsRef->DrawSprite(_X, 
							_Y, 
							PDAKeysGraphics, 
							Region, 
							(GraphicsRef->GetOrientation() == EO_Landscape_Normal)?0:SPRITE_ROTATE_180);	


}*/


// ----
void	CPopulous::game_quit()
{
	SHORT   c1,setup=1;
	SHORT   old_serial_off=serial_off;
	SHORT   old_paint_map=paint_map;
	SHORT   old_player=player;
	SHORT   old_type=stats[player].type;
	SHORT   old_in_conquest=in_conquest;
//	SHORT   c2;
	
	bring_up_two=0;
	saved=0;
	while(1)
    {
		if(setup)
        {
			requester(d_screen, 80, 76, 160, 48,&end_ok,&end_cancel);
			setup=0;
        }

		text(d_screen, 80 + 16,	76 + 8 + 4,		(UBYTE*)"ARE YOU SURE YOU");
		text(d_screen, 80 + 24,	76 + 16 + 4,	(UBYTE*)"WANT TO QUIT?");

		left_button=2;
		while(left_button) 
		{	
			UpdatePopulousData();	// This could be wrong
			if (Redraw)
			{
				copy_screen(w_screen,d_screen);
				requester(d_screen, 80, 76, 160, 48,&end_ok,&end_cancel);
				text(d_screen, 80 + 16,	76 + 8 + 4,		(UBYTE*)"ARE YOU SURE YOU");
				text(d_screen, 80 + 24,	76 + 16 + 4,	(UBYTE*)"WANT TO QUIT?");
				Redraw = false;
			}

			waittof();
		}
		
		if(c1=check_cancel(&end_ok,&end_cancel))
        {
			if(c1 == 2)
            {
				// Cancel
				serial_off=old_serial_off;
				paint_map=old_paint_map;
				if(player != old_player)
                {
					set_tend_icons(-1,-1);
					set_mode_icons(-1,-1);
					player=old_player;
                }
				not_player=!player;
				stats[player].type=old_type;
				in_conquest=old_in_conquest;
            }
			else
			{
				// Okay
				// This is a bit of a nasty way of doing 
				// things, but it does allow us to maintain
				// control over the program execution, and 
				// let us do the saving and cleaning up we 
				// need (want) to.
				RaiseException(QUIT_EXCEPTION, 0, 0, NULL); 
			}
			break;
        }
    }
the_end:;
		return_to_game();

}


// ----
//;       r0      pointer to string buffer
//;       r1      pointer to screen address
//;       r2      mask number
void	CPopulous::show_text(long _TextLine, long _Screen, long _Qaz)
{
	CString theCredits[55];

	theCredits[0] = "POPULOUS";
	theCredits[1] = "FOR THE";
	theCredits[2] = "POCKET PC";
	theCredits[3] = "";
	theCredits[4] = "BROADSWORD INTERACTIVE";
	theCredits[5] = "";
	theCredits[6] = "MANAGEMENT";
	theCredits[7] = "";
	theCredits[8] = "JOHN JONES-STEELE";
	theCredits[9] = "DAVID ROWE";
	theCredits[10] = "";
	theCredits[11] = "LEAD PROGRAMMERS";
	theCredits[12] = "";
	theCredits[13] = "ANDREW EVANS";
	theCredits[14] = "ANTHONY DUNN";
	theCredits[15] = "";
	theCredits[16] = "ADDITIONAL ARTWORK"; 
	theCredits[17] = "";
	theCredits[18] = "PHILIP CURTIS";
	theCredits[19] = ""; 
    theCredits[20] = "";
	theCredits[21] = "ORIGINAL PROGRAMMING"; 
	theCredits[22] = "AND DESIGN";
	theCredits[23] = "";
	theCredits[24] = "GLENN CORPES";
	theCredits[25] = "AND PETER MOLYNEUX";
	theCredits[26] = "WITH TECHNICAL";
	theCredits[27] = "HELP FROM";
	theCredits[28] = "KEVIN DONKIN";
	theCredits[29] = "";
	theCredits[30] = "ORIGINAL GRAPHICS";
	theCredits[31] = "";
	theCredits[32] = "GLENN CORPES";
	theCredits[33] = "DAN WHEELER";
	theCredits[34] = "SEAN COOPER";
	theCredits[35] = "ANDY JONES";
	theCredits[36] = "AND PETER MOLYNEUX";
	theCredits[37] = "";
	theCredits[38] = "ORIGINAL GAME";
	theCredits[39] = "SOUND AND MUSIC";
	theCredits[40] = "DAVE HANLON";
	theCredits[41] = "";
	theCredits[42] = "ORIGINAL MUSIC IDEA";
	theCredits[43] = "LES EDGAR";
	theCredits[44] = "";
	theCredits[45] = "ORIGINAL TITLE MUSIC";
	theCredits[46] = "ROB HUBBARD";
	theCredits[47] = "";
	theCredits[48] = "COPYRIGHT 1989";
	theCredits[49] = "BULLFROG PRODUCTIONS";
	theCredits[50] = "";
	theCredits[51] = "POCKET PC VERSION";
	theCredits[52] = "COPYRIGHT 2001";
	theCredits[53] = "BROADSWORD INTERACTIVE";
	theCredits[54] = "";

	if (_Screen > 3)
	{
		GraphicsRef->SetActiveScreen(w_screen);
	}
	else
	{
		GraphicsRef->SetActiveScreen(_Screen);
	}

	for (int i = 0; i < 55; i++)
	{
		long yPos = _TextLine + (i * 15);
		if (yPos > 40)
			continue;

		GraphicsRef->DrawString(StandardFont, &(theCredits[i]), 160, yPos, TEXT_CENTRED);
	}
}