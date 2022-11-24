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

// ----
// Take a screen shot.
void		CPopulous::TakePicture()
{
	SYSTEMTIME picTime;
	GetSystemTime(&picTime);

	char	convert[256];
	sprintf(convert, 
			"%d%d%d-%d%d%d%d.tga", 
			picTime.wYear, 
			picTime.wMonth,
			picTime.wDay,
			picTime.wHour,
			picTime.wMinute,
			picTime.wSecond,
			picTime.wMilliseconds);
	CString	Path = ThePlatform->GetPath(EP_Snapshot);
	Path += convert;

//#ifdef NOT_FINAL_BUILD
//	GraphicsRef->StartDraw();
//	GraphicsRef->SaveScreen(&Path);
//	GraphicsRef->EndDraw();
//#endif
}

int			CPopulous::newrand()
{
	long r0 = seed;
	
	r0 = 9377 * r0;
	r0 = r0 + 9439;
	r0 = r0 & 0x7FFF;
	seed = r0;
	return r0;
}

void		CPopulous::SetRandomNumbers()
{

	//Re-seeds the random number table at launch based on the system time
	SYSTEMTIME Time;

	GetSystemTime(&Time);

	int TimeInSecs = 0;

	TimeInSecs += ((Time.wYear			* 60 * 60 * 24 * 32 * 14));
	TimeInSecs += ((Time.wMonth			* 60 * 60 * 24 * 32));
	TimeInSecs += ((Time.wDay			* 60 * 60 * 24));
	TimeInSecs += ((Time.wHour			* 60 * 60));
	TimeInSecs += ((Time.wMinute		* 60));
	TimeInSecs += ((Time.wSecond));

	srand(TimeInSecs);

	for (int i = 0; i < 300; i++)
	{
		RandomArray[i] = rand(); // Values in the range 0->255. //thank you are ........
	}
	RandomArrayLoc = 0;

}

int		CPopulous::check_life(int player, int position)
{
//;       r0      player
//;       r1      at position
//;       returns
//;       r0      town type

	long r0, r1, r2, r3, r4, r5, r6, r7;
	UBYTE	*r10, *r11;
	SHORT	*r12;

	// Re name our parameters
	r0 = player;
	r1 = position;


	//	MOV	   r2,r1
	r2 = r1;	// Ok

	//	MOV 	r4,#0
	//	MOV 	r5,#0
	//	ADD 	r6,r0,#FARM_BLOCK
	r4 = 0;	// Ok
	r5 = 0;	// Ok
	r6 = r0 + FARM_BLOCK;	// Ok

	//	LDR 	r10,%0					;		pointer to map_blk
	//	LDR 	r11,%1					;		pointer to map_bk2
	//	LDR 	r12,%2					;		pointer to vector
	r10 = map_blk;	// Ok
	r11 = map_bk2;	// Ok
	r12 = offset_vector;	// Ok

	//	MOV 	r7,#0
	r7 = 0;	// Ok

	//	LDR 	r8,%3					;		pointer to all_of_city var
	//	STRB	r7,[r8,#0]
	//	STRB	r7,[r8,#1]
	all_of_city = 0;	// Ok

	//	LDR 	r8,%4					;		pointer to a_flat_block var
	//	STRB	r7,[r8,#0]
	//	STRB	r7,[r8,#1]
	a_flat_block = 0;	// Ok

life_loop:
	//	LDRB	r7,[r12],#1
	//	LDRB	r8,[r12],#1
	//	ORR 	r3,r7,r8,LSL#8
	//	MOV 	r3,r3,ASL#16
	//	MOV 	r3,r3,ASR#16
	r3 = *r12;			// Ok
	r12++;				// Ok
//	r3 = r3 << 16;		// Ok
//	r3 = r3 >> 16;		// Ok
	if ((r3 >> 15) & 0x01)
		r3 = 0xFFFF0000 | (r3 & 0xFFFF);

	//	MOV 	r0,r2
	//	MOV 	r1,r3
	r0 = r2;	// Ok
	r1 = r3;	// Ok

	//	BL		valid_move
	r0 = valid_move(r0, r1);	// Ok

	//	TEQ 	r0,#0
	//	BEQ 	land_ok
	if ((r0 ^ 0) == 0)		// Ok
		goto land_ok;		// Ok

	//	CMP 	r0,#2
	//	BNE 	end_of_loop
	if (r0 != 2)			// Ok
		goto end_of_loop;	// Ok

	//	ADD 	r4,r4,#ROCK_LAND_FOOD
	r4 += ROCK_LAND_FOOD;	// Ok

	//	B		end_of_loop
	goto end_of_loop;	// Ok

land_ok:
	//	ADD 	r3,r3,r2
	r3 += r2;	// Ok

	//	LDRB	r7,[r10,r3]
	r7 = *(r10 + r3);	// Ok

	//	CMP 	r6,r7
	//	BEQ 	add_farm_land
	if (r6 == r7)	// Ok
		goto add_farm_land;	// Ok

	//	CMP 	r7,#FLAT_BLOCK
	//	BNE 	no_add_flat_land
	if (r7 != FLAT_BLOCK)	// Ok
		goto no_add_flat_land;	// Ok

	//	MOV 	r7,#1
	r7 = 1;	// Ok

	//	LDR 	r8,%4					;		pointer to a_flat_block var
	//	STRB	r7,[r8,#0]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r8,#1]
	a_flat_block = r7;	// Ok

add_farm_land:
	//	TEQ 	r4,#0
	//	BNE 	food_already_set
	if ((r4 ^ 0) != 0) 	// Ok
		goto food_already_set;	// Ok

	//	MOV 	r4,#START_FOOD
	r4 = START_FOOD;	// Ok

food_already_set:
	//	ADD 	r4,r4,#FLAT_LAND_FOOD
	r4 += FLAT_LAND_FOOD;	// Ok

	//	B		check_for_city			  
	goto check_for_city;	// Ok

no_add_flat_land:	
	//	TEQ 	r5,#0
	//	BNE 	check_for_city
	if ((r5 ^ 0) != 0)	// Ok
		goto check_for_city;	// Ok

	//	MOVS	r0,#0
	//	LDMFD	sp!,{r1-r12,pc}
	return 0;	// Ok

check_for_city:
	//	LDRB	r1,[r11,r3]
	r1 = *(r11 + r3);	// Ok

	//	CMP 	r5,#9
	//	BGE 	no_city_here
	if (r5 >= 9)	// Ok
		goto no_city_here;	// Ok

	//	LDRB	r7,[r11,r2]
	r7 = *(r11 + r2);	// Ok

	//	CMP 	r7,#CITY_CENTRE
	//	BNE 	no_city_here
	if (r7 != CITY_CENTRE)	// Ok
		goto no_city_here;	// Ok

	//	CMP 	r1,#CITY_TOWER
	//	BLT 	no_city_here
	if (r1 < CITY_TOWER)	// Ok
		goto no_city_here;	// Ok

	//	CMP 	r1,#CITY_WALL2
	//	BGT 	no_city_here
	if (r1 > CITY_WALL2)	// Ok
		goto no_city_here;	// Ok

	//	LDR 	r8,%3					;		pointer to all_of_city var
	//	LDRB	r7,[r8,#0]
	//	LDRB	r8,[r8,#1]
	//	ORR 	r7,r7,r8,LSL#8
	//	MOV 	r7,r7,ASL#16
	//	MOV 	r7,r7,ASR#16
	//	ADD 	r7,r7,#1
	//	LDR 	r8,%3					;		pointer to all_of_city var
	//	STRB	r7,[r8,#0]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r8,#0]
	all_of_city++;	// Ok

	//	B		end_of_loop
	goto end_of_loop;	// Ok

no_city_here:
	//	TEQ 	r5,#0
	//	BEQ 	end_of_loop
	if ((r5 ^ 0) == 0)	// Ok
		goto end_of_loop;	// Ok

	//	CMP 	r1,#FIRST_TOWN
	//	BLE 	end_of_loop
	if (r1 <= FIRST_TOWN)	// are: corrected	// Ok
		goto end_of_loop;	// Ok

	//	CMP 	r1,#CITY_WALL2
	//	BGT 	end_of_loop
	if (r1 > CITY_WALL2)	// Ok
		goto end_of_loop;	// Ok

	//	MOVS	r0,#0
	//	LDMFD	sp!,{r1-r12,pc}
	return 0;	// Ok

end_of_loop:	   
	//	ADD 	r5,r5,#1
	r5++;	// Ok

	//	CMP 	r5,#17
	//	BNE 	life_loop
	if (r5 != 17)	// Ok
		goto life_loop;	// Ok

	//	CMP 	r4,#START_FOOD-FLAT_LAND_FOOD
	//	BGE 	food_not_small
	if (r4 >= (START_FOOD - FLAT_LAND_FOOD))	// Ok
		goto food_not_small;	// Ok

	//	MOV 	r4,#0
	r4 = 0;	// Ok

food_not_small:
	//	LDR 	r7,%5
	r7 = MAX_FOOD;	// Ok

	//	CMP 	r4,r7
	//	BNE 	not_a_city
	if (r4 != r7)	// Ok
		goto not_a_city;	// Ok

	//	LDR 	r4,%6
	r4 = CITY_FOOD;	// Ok

not_a_city:
	//	MOVS	r0,r4
	//	LDMFD	sp!,{r1-r12,pc}
	return r4;	// Ok
/*
0
		DCD 	map_blk
1
		DCD 	map_bk2
2
		DCD 	offset_vector
3
		DCD 	all_of_city
4
		DCD 	a_flat_block
5
		DCD 	MAX_FOOD
6
		DCD 	CITY_FOOD
*/

	/*
	int amountOfFood = 0;
	int PosToCheck = 0;
	int validMoveRet = 0;
	
	all_of_city = 0;
	a_flat_block = 0;

	for(int r5 = 0; r5<17; r5++)
	{
		PosToCheck = abs(offset_vector[r5] + position);
		validMoveRet = valid_move(position, PosToCheck);

		if(validMoveRet != 0)
		{	
			if(validMoveRet == 2)
			{
				amountOfFood += ROCK_LAND_FOOD;
			}
		}
		else
		{
		//land ok
			if(map_blk[PosToCheck] != FARM_BLOCK)
			{
				if(map_blk[PosToCheck] == FLAT_BLOCK)
				{
					a_flat_block = 1;
				}
				else
				{
					if(r5 == 0)
					{
						amountOfFood = 0;
					}
				}
			

				if(amountOfFood == 0)
				{
					amountOfFood = START_FOOD;
				}

				amountOfFood += FLAT_LAND_FOOD;
			}
			else
			{
				if(amountOfFood == 0)
				{
					amountOfFood = START_FOOD;
				}

				amountOfFood += FLAT_LAND_FOOD;
			}

			//check for city
			if(		(r5 < 9)
				&&	(map_bk2[PosToCheck] == CITY_CENTRE)
				&&	(map_bk2[position] > CITY_TOWER)
				&&	(map_bk2[position] < CITY_WALL2) )
			{
				abs(PosToCheck++);
				all_of_city = 1;
			}
			else
			{
				if(		(r5 != 0)
					&&	(map_bk2[PosToCheck] != FIRST_TOWN)
					&&	(map_bk2[PosToCheck] != CITY_WALL2) )
				{
					amountOfFood += FLAT_LAND_FOOD;
				}
			}
		}
	}

	if(amountOfFood <= START_FOOD-FLAT_LAND_FOOD)
	{
		amountOfFood = FLAT_LAND_FOOD;
	}

	if(amountOfFood <= MAX_FOOD)
	{
		amountOfFood = CITY_FOOD;
	}

	return amountOfFood;

	*/
}

int		CPopulous::valid_move(int Pos, int PosToCheck)
{
	long	r0, r1, r2, r3, r4, r5;
	UBYTE	*r12;

	r0 = Pos;
	r1 = PosToCheck;

//; 	  r0	  at position
//; 	  r1	  to position
//; 	  returns
//; 	  r0	  move number
	//	MOV 	r2,r0
	//	MOV 	r3,r1
	//	MOV 	r0,#0
	r2 = r0;
	r3 = r1;
	r0 = 0;

	//	TEQ 	r3,#0
	//	BEQ 	the_end
	if ((r3 ^ 0) == 0)
		goto the_end;

	//	MOV 	r4,r2
	r4 = r2;

	//	ADDS	r4,r4,r3
	r4 += r3;

	//	BLT 	return_one
	if (r4 < 0)
		goto return_one;

	//	CMP 	r4,#MAP_WIDTH*MAP_HEIGHT
	//	BGE 	return_one
	if (r4 >= (MAP_WIDTH*MAP_HEIGHT))
		goto return_one;
	
	//	AND 	r3,r3,#63
	r3 = r3 & 63;

	//ODI(r3);
	//	CMP 	r3,#3
	//	BLE 	no_to_change
	if (r3 <= 3)
		goto no_to_change;

	//	ORR 	r3,r3,#2_11000000
	r3 = r3 | 0xC0;//11000000

	//	MOV 	r3,r3,ASL#24
	//	MOV 	r3,r3,ASR#24
	// Hmmm.
	//r3 = r3 << 24;
	//r3 = r3 >> 24;
	r3 = 0xFFFFFF00 | (r3 & 0xFF);

no_to_change:
	//	AND 	r2,r2,#63
	r2 = r2 & 63;

	//	ADDS	r3,r3,r2
	r3 += r2;

	//	BLT 	return_one
	if (r3 < 0)		// Erm...
		goto return_one;

	//	CMP 	r3,#63
	//	BGT 	return_one
	if (r3 > 63)
		goto return_one;

	//	LDR 	r12,%0					;		pointer to map_blk
	r12 = map_blk;

	//	MOV 	r4,r4,ASL#16
	//	MOV 	r4,r4,ASR#16
	//	ADD 	r12,r12,r4
	r12 += r4;

	//	LDRB	r5,[r12]
	r5 = *r12;

	//	CMP 	r5,#ROCK_BLOCK
	//	BEQ 	return_two
	if (r5 == ROCK_BLOCK)
		goto return_two;

	//	CMP 	r5,#WATER_BLOCK
	//	BEQ 	return_three
	if (r5 == WATER_BLOCK)
		goto return_three;

	//	B		the_end
	goto the_end;

return_one:
	//	MOV 	r0,#1
	r0 = 1;
	
	//	B		the_end
	goto the_end;

return_two:
	//	MOV 	r0,#2
	r0 = 2;

	//	B		the_end
	goto the_end;

return_three:
	//	MOV 	r0,#3
	r0 = 3;

the_end:
	//	TEQ 	r0,#0
	//	LDMFD	sp!,{r1-r12,pc}
	return r0;

/*
0
		DCD 	map_blk
*/

	/*
	if(PosToCheck == 0)
	{
		return 0;
	}

	if(	(Pos + PosToCheck) < Pos)
	{
		return 1;
	}

	if(PosToCheck >= MAP_WIDTH*MAP_HEIGHT)
	{
		return 1;
	}

	if(Pos < 0)
	{
		return 1;
	}

	if(map_blk[PosToCheck] == ROCK_BLOCK)
	{
		return 2;
	}

	if(map_blk[PosToCheck] == WATER_BLOCK)
	{
		return 3;
	}

	return 0;
	*/
}


// ----
// This method is effectively the DrawString of the 
// Populous world. This is now implemented as a wrapper to 
// our underlying graphical routines.
void	CPopulous::text(int		_Screen, 
						int		_XPos,
						int		_YPos, 
						UBYTE	*_String)
{
	if (!_String)
		return;

	GraphicsRef->SetActiveScreen(_Screen);

	
	//ODS("0. String: ");	ODSAN(_String);
	//ODS("0. Path:   ");	ODCSN(ThePlatform->GetPath(EP_Data));

	// Convert string and render it.
	TempString.AssignAsData((char*)_String);
	//ODS("1. String: ");	ODSAN(_String);
	//ODS("1. Path:   ");	ODCSN(ThePlatform->GetPath(EP_Data));

	GraphicsRef->DrawString(StandardFont,
							&TempString,
							_XPos,
							_YPos);
	//ODS("2. String: ");	ODSAN(_String);
	//ODS("2. Path:   ");	ODCSN(ThePlatform->GetPath(EP_Data));
}

// ----
// Not exactly sure what this is supposed to do, or how I'm
// gonna change the code around where it is called.
int		CPopulous::keyboard()
{
	POINT*	mouse;
	TCHAR	Char;
	DWORD	KeyData;
	
	inkey = 0;
	asckey = 0;
	
	if (ControllerRef->GetChar(&Char, &KeyData))
	{
		// Eat up all the key presses, bar the very last one.
		while (ControllerRef->GetChar(&Char, &KeyData));

		asckey = Char & 0xFF;

		// Convert lower to upper case.
		if (asckey >= (65 + 32))
			asckey -= 32;	

		// Eliminate nonalpha chars.
		if (	(asckey < 65) 
			||	(asckey >= (65 + 26)))
			asckey = 0;	

		// Want enter key
		if (Char == 13)
			asckey = Char;
	}
	else
	{
		mouse = ControllerRef->GetMousePosition();
		if (	(ControllerRef->IsButtonPressedDB(MouseSelect))
			&&	(mouse->y >= 200)
			&&	(mouse->y < 240))
		{
			if (mouse->y < 220)
			{
				if ((mouse->x / 20) < 13)
					asckey = 'A' + (mouse->x / 20);
				else
				{
					if (	((mouse->x / 20) == 13)
						||	((mouse->x / 20) == 14))
						asckey = 127;
					else
						asckey = 13;
				}
			}
			else 
			{
				if ((mouse->x / 20) < 13)
					asckey = 'N' + (mouse->x / 20);
				else
				{
					if (	((mouse->x / 20) == 13)
						||	((mouse->x / 20) == 14))
						shift = true;
					else
						asckey = 13;
				}
			}
		}
	}

	return asckey;
}

// ----
// A wrapper that allows for the plotting of a single pixel.
void	CPopulous::pixel(int _Screen, int _XPos, int _YPos, int _Colour)
{
	unsigned char	R, G, B;
	DWORD	Colour;

	Colour = GraphicsRef->PopulousToPal(_Colour);
	R = (Colour >> 16) & 0xFF;
	G = (Colour >> 8) & 0xFF;
	B = Colour & 0xFF;

	GraphicsRef->SetActiveScreen(_Screen);
	GraphicsRef->DrawPixel(_XPos, _YPos, R, G, B);
}

// ----
// Draw a 16x16 sprite - this a wrapper.
void	CPopulous::draw_sprite(	LONG _Screen, 
								int _XPos, 
								int _YPos, 
								int _SpriteIndex)
{
	RECT	Region;

	if (_SpriteIndex > 142)
	{
		ODSN("Frame number too large - not going to draw sprite.");
		return;
	}

	Region.left		= 0;
	Region.top		= 16 * _SpriteIndex;
	Region.right	= 16;
	Region.bottom	= 16 * (_SpriteIndex + 1);

	GraphicsRef->SetActiveScreen(_Screen);
	GraphicsRef->DrawSprite(_XPos, _YPos, SpriteGraphics, Region);
}

// ----
// Draw a 32x32 sprite - this a wrapper.
void	CPopulous::draw_s_32(	LONG _Screen, 
								int _XPos, 
								int _YPos, 
								int _SpriteIndex)
{
	RECT	Region;

	if (_SpriteIndex > 255)
	{
		ODSN("Large sprite index");
	}

	Region.left		= 0;
	Region.top		= 32 * _SpriteIndex;
	Region.right	= 32;
	Region.bottom	= 32 * (_SpriteIndex + 1);

	GraphicsRef->SetActiveScreen(_Screen);
	GraphicsRef->DrawSprite(_XPos, _YPos, Sprite32Graphics, Region);
}


// ----
void	CPopulous::clr_wsc()
{
	GraphicsRef->CopyScreen(back_scr, w_screen);
}

// ----
void	CPopulous::bbc_cls()
{
	// Don't want to clear the screen really. GraphicsRef->ClearScreen(0,0,0);
}


void	CPopulous::wait_for_disc()
{
	GameState = GameError;
	ODSN("Error locating file.");
	exit(0);
}

// ----
// Copies a back buffer screen to the foreground.
void	CPopulous::copy_screen(int a, int b)
{
	/*
	RECT Region;

	Region.left		= 0;
	Region.top		= 0;
	Region.right	= 320;
	Region.bottom	= 200;

	GraphicsRef->DrawSprite(0, 0, BackGroundImage, Region);
	*/
	GraphicsRef->CopyScreen(a, b); // source and destination.
}

// ----
void	CPopulous::draw_bar(int _Screen, int _XPos, int _YPos, int _MaxHeight, int _CurrentHeight, int _Colour)
{
	unsigned char	R, G, B;
	DWORD			Colour;

	GraphicsRef->SetActiveScreen(_Screen);

	// Draw bar area.
	Region.left		= _XPos * 8 + 2;
	Region.top		= _YPos - min(_CurrentHeight + 1, _MaxHeight);
	Region.right	= _XPos * 8 + 6;
	Region.bottom	= _YPos + 1;

	Colour = GraphicsRef->PopulousToPal(_Colour);
	R = (Colour >> 16) & 0xFF;
	G = (Colour >> 8) & 0xFF;
	B = Colour & 0xFF;
	GraphicsRef->DrawRect(&Region, R, G, B);

	// Clear remaing area
	Region.bottom	= Region.top;
	Region.top		= _YPos - _MaxHeight;
	Colour = GraphicsRef->PopulousToPal(2);
	R = (Colour >> 16) & 0xFF;
	G = (Colour >> 8) & 0xFF;
	B = Colour & 0xFF;

	// The code above results in illegal screen co-ordinates.
//	GraphicsRef->DrawRect(&Region, R, G, B);			
}

void	CPopulous::put_block(	int	_XPos,
								int	_YPos,
								int	_ZPos,
								int	_Block)
{
	int ScreenX, ScreenY;
	int ScreenPos;

	ScreenPos = screen_origin;
	ScreenX = _XPos << 3;
	ScreenY = _YPos << 3;
	ScreenPos += ScreenX;
	ScreenPos -= ScreenY;

	ScreenPos = ScreenX * 160 + ScreenPos;
	ScreenPos = ScreenY * 160 + ScreenPos;
	ScreenPos -= 160 * _ZPos;

	ScreenX = (ScreenPos % 160) * 2;	// Back track to x and y co-ordinates.
	ScreenY = ScreenPos / 160;

	Region.left		= 0;
	Region.top		= _Block * 24;		// Block height
	Region.right	= 32;				// Block width.
	Region.bottom	= Region.top + 24;	

	GraphicsRef->DrawSprite(ScreenX, ScreenY, BlockGraphics, Region);
}

// ----
// Draw a sprite on screen, given a screen address and a pointer to some graphics memory.
void	CPopulous::draw_fixed_sprite(int x, int y, int z, LONG	GraphicsPointer)
{
	/*
	int	ScreenX, ScreenY;

	ScreenX	= (ScreenAddress % 160) * 2;
	ScreenY	= ScreenAddress / 160;

  */
	//
	int ScreenX, ScreenY;
	int ScreenPos;

	ScreenPos = screen_origin;
	ScreenX = x << 3;
	ScreenY = y << 3;
	ScreenPos += ScreenX;
	ScreenPos -= ScreenY;

	ScreenPos = ScreenX * 160 + ScreenPos;
	ScreenPos = ScreenY * 160 + ScreenPos;
	ScreenPos -= 1280 * z;//160 * z;

	ScreenX = (ScreenPos % 160) * 2;	// Back track to x and y co-ordinates.
	ScreenY = ScreenPos / 160;

	//
	Region.left		= 0;
	Region.top		= GraphicsPointer / 10;// >> 6;	// 1024 bytes per sprite, over 16 lines. 1024 / 16 =  64 :. >> 6
	Region.right	= 16;
	Region.bottom	= Region.top + 16;

	if(ScreenX >= 320)
	{
		ODS("\n");
	}

	if(ScreenY >= 200)
	{
		ODS("\n");
	}
	GraphicsRef->DrawSprite(ScreenX, ScreenY, SpriteGraphics, Region);
}

// ----
void	CPopulous::draw_icon(int _Screen, int _XPos, int _YPos, int _Icon)
{
	int	ScreenAddr = 0;
	int ScreenX, ScreenY;

	ScreenAddr	= _YPos * 160 + ScreenAddr;
	ScreenAddr	= ScreenAddr + (_XPos << 3);

	ScreenX		= (ScreenAddr % 160) * 2;
	ScreenY		= ScreenAddr / 160;

	Region.left		= 0;
	Region.top		= _Icon * 16;
	Region.right	= 16;				// Not sure about this!
	Region.bottom	= Region.top + 16;

	GraphicsRef->SetActiveScreen(_Screen);
	GraphicsRef->DrawSprite(ScreenX, ScreenY, IconGraphics, Region);
}

void	CPopulous::draw_it(int xOff, int yOff)
{
//;       r0      xoffset
//;       r1      yoffset

//	int mapGridRef = ((yOff * MAP_WIDTH) + xOff) + (yCount * 64);

	long	r0, r1, r2, r3, r4, r5;
	UBYTE	*r9, *r10, *r11, *r12;

	r0 = xOff;
	r1 = yOff;

	//	LDR 	r9,%0					;		pointer to map_blk
	//	LDR 	r10,%1					;		pointer to map_bk2
	//	LDR 	r11,%2					;		pointer to map_alt
	//	LDR 	r12,%3					;		pointer to map_who
	r9 = map_blk;
	r10 = map_bk2;
	r11 = map_alt;
	r12 = map_who;

	//	MOV 	r2,#MAP_WIDTH
	r2 = MAP_WIDTH;

	//	MLA 	r0,r1,r2,r0
	r0 = r1 * r2 + r0;

	//	ADD 	r9,r9,r0
	//	ADD 	r10,r10,r0
	//	ADD 	r11,r11,r0
	//	ADD 	r12,r12,r0				;		offset into arrays
	r9 += r0;
	r10 += r0;
	r11 += r0;
	r12 += r0;

	//	STMFD	sp!,{r11}				;		save altitudes
//	stackR11 = r11;

	//	MOV 	r1,#0
	r1 = 0;

yloop:
	//	MOV 	r0,#0
	r0 = 0;

xloop:
	//	LDRB	r2,[r11]
	r2 = *r11;

	//	MOV 	r2,r2,LSL#3 			;		convert altitude to x pos
	r2 = r2 << 3;

	//	LDRB	r3,[r9]
	r3 = *r9;

	//	TEQ 	r3,#0
	//	BNE 	notwater
	if ((r3 ^ 0) != 0)
		goto notwater;

	//	LDR 	r5,%4					;		pointer to toggle var
	//	LDRB	r4,[r5,#0]
	//	LDRB	r5,[r5,#1]
	//	ORRS	r4,r4,r5,LSL#8
	//	BNE 	notwater
	r4 = toggle;

	if (r4 != 0)
		goto notwater;

	//	MOV 	r3,#16					;		animate water
	r3 = 16;

notwater:
	//	BL		put_block
	put_block(r0, r1, r2, r3);

	//	ADD 	r2,r2,#8
	r2 += 8;

	//	LDRB	r3,[r10]
	r3 = *r10;

	//	TEQ 	r3,#0
	//	BEQ 	no_block2
	if ((r3 ^ 0) == 0)
		goto no_block2;

	//	BL		put_block
	put_block(r0, r1, r2, r3);

no_block2:
	//	LDRB	r3,[r12]
	r3 = *r12;

	//	TEQ 	r3,#0
	//	BEQ 	no_seq
	if ((r3 ^ 0) == 0)
		goto no_seq;

	//	LDR 	r4,%5					;		pointer to screen_origin var
	//	LDR 	r4,[r4]
	r4 = screen_origin;

	//	LDR 	r5,%6
	r5 = ORIGIN;

	//	CMP 	r4,r5
	//	BNE 	no_seq
	if (r4 != r5)
		goto no_seq;

	//	BL		move_sprite    
	move_sprite(r0, r1, r2, r9, r11, r12);//&peeps[map_who[mapGridRef]], mapGridRef);

	//((yOff * MAP_WIDTH) + xOff) + (r1 * 64);


no_seq:
	//	LDR 	r4,%0					;		pointer to map_blk
	r4 = (long)map_blk;

	//	SUB 	r4,r9,r4
	r4 = (long)r9 - r4;

	//	LDR 	r6,%7					;		pointer to devil_magnet var
	//	LDRB	r5,[r6,#0]
	//	LDRB	r6,[r6,#1]
	//	ORRS	r5,r5,r6,LSL#8
	r5 = devil_magnet;

	//	BEQ 	the_devil_magnet_not_here
	if (r5 == 0)
		goto the_devil_magnet_not_here;

	//	CMP 	r4,r5
	//	BNE 	the_devil_magnet_not_here
	if (r4 != r5)
		goto the_devil_magnet_not_here;

	//	MOV 	r3,#DEVILS_MAGNET
	r3 = DEVILS_MAGNET;
	
	//	BL		put_block
	put_block(r0, r1, r2, r3);

the_devil_magnet_not_here:
	//	LDR 	r6,%8					;		pointer to god_magnet var
	//	LDRB	r5,[r6,#0]
	//	LDRB	r6,[r6,#1]
	//	ORRS	r5,r5,r6,LSL#8
	r5 = god_magnet;

	//	BEQ 	the_gods_magnet_not_here
	if (r5 == 0)
		goto the_gods_magnet_not_here;

	//	CMP 	r4,r5
	//	BNE 	the_gods_magnet_not_here
	if (r4 != r5)
		goto the_gods_magnet_not_here;

	//	MOV 	r3,#GODS_MAGNET
	r3 = GODS_MAGNET;

	//	BL		put_block
	put_block(r0, r1, r2, r3);

the_gods_magnet_not_here:
	//	ADD 	r9,r9,#1
	//	ADD 	r10,r10,#1
	//	ADD 	r11,r11,#1
	//	ADD 	r12,r12,#1
	r9++;
	r10++;
	r11++;
	r12++;

	//	ADD 	r0,r0,#1
	r0++;

	//	CMP 	r0,#8
	//	BNE 	xloop
	if (r0 != 8)
		goto xloop;

	//	ADD 	r9,r9,#56
	//	ADD 	r10,r10,#56
	//	ADD 	r11,r11,#56
	//	ADD 	r12,r12,#56
	r9 += 56;
	r10 += 56;
	r11 += 56;
	r12 += 56;

	//	ADD 	r1,r1,#1
	r1++;

	//	CMP 	r1,#8
	//	BNE 	yloop
	if (r1 != 8)
		goto yloop;

/*


	//	LDR 	r0,%9					;		pointer to work_screen var
	//	LDR 	r0,[r0]
	r0 = work_screen;

	//	LDR 	r1,%5					;		pointer to screen_origin var
	//	LDR 	r1,[r1]
	r1 = screen_origin;

		ADD 	r0,r0,r1
		LDR 	r1,%10					;		screen offset
		ADD 	r0,r0,r1
		LDR 	r1,%11					;		pointer to sprite_data
		LDR 	r1,[r1]
		LDR 	r2,%12					;		graphic offset
		ADD 	r1,r1,r2
		LDMFD	sp!,{r2}				;		altitudes
		ADD 	r2,r2,#7
		MOV 	r3,#8
left_loop
		STMFD	sp!,{r0}				;		save screen pointer
		LDRB	r4,[r2] 				;		get altitude
		TEQ 	r4,#0
		BEQ 	end_left_loop
left_column_loop	
		BL		draw_fixed_sprite
		SUB 	r0,r0,#320/2*8
		SUBS	r4,r4,#1
		BNE 	left_column_loop
end_left_loop	 
		LDMFD	sp!,{r0}
		ADD 	r0,r0,#320/2*8
		SUB 	r0,r0,#8
		ADD 	r2,r2,#64
		SUBS	r3,r3,#1
		BNE 	left_loop
		SUB 	r0,r0,#320/2*8
		LDR 	r1,%11					;		pointer to sprite_data
		LDR 	r1,[r1]
		LDR 	r3,%13					;		graphic offset
		ADD 	r1,r1,r3
		SUB 	r2,r2,#64
		MOV 	r3,#8
right_loop
		STMFD	sp!,{r0}				;		save screen pointer
		LDRB	r4,[r2] 				;		get altitude
		TEQ 	r4,#0
		BEQ 	end_right_loop
right_column_loop	 
		BL		draw_fixed_sprite
		SUB 	r0,r0,#320/2*8
		SUBS	r4,r4,#1
		BNE 	right_column_loop
end_right_loop	  
		LDMFD	sp!,{r0}
		SUB 	r0,r0,#320/2*8
		SUB 	r0,r0,#8
		SUB 	r2,r2,#1
		SUBS	r3,r3,#1
		BNE 	right_loop
		LDMFD	sp!,{r0-r12,pc}
*/
	int altCount, altPos, myAlt;

	// "Left"-hand wall
	for (altCount = 0; altCount < 8; altCount++)
	{
		altPos = ((yOff * MAP_WIDTH) + xOff + 7) + (altCount * 64);
		for (myAlt = map_alt[altPos]; myAlt > 0; myAlt--)
		{
			draw_fixed_sprite(8, altCount, myAlt - 1, SIDE_WALL1 * 160);
		}
	}
	
	// "Right"-hand wall
	for (altCount = 0; altCount < 8; altCount++)
	{
		altPos = (yOff + 7) * MAP_WIDTH + xOff + altCount;	
		for (myAlt = map_alt[altPos]; myAlt > 0; myAlt--)
		{
			draw_fixed_sprite(altCount + 1, 8, myAlt, SIDE_WALL2 * 160);
		}
	}

/*
0
        DCD     map_blk
1
        DCD     map_bk2
2
        DCD     map_alt
3
        DCD     map_who
4
        DCD     toggle
5
        DCD     screen_origin
6
        DCD     ORIGIN
7
        DCD     devil_magnet
8
        DCD     god_magnet
9
        DCD     work_screen
10
        DCD     LEFT_WALL-ORIGIN
11
        DCD     sprite_data
12
        DCD     SIDE_WALL1*160
13
        DCD     SIDE_WALL2*160
*/
	/*
	int mapGridRef = 0;
	int xCount;
	int	yCount;
	int altCount;
	int	myAlt, altPos;

	mapGridRef = (yOff * MAP_WIDTH) + xOff;

	//yloop
	for (yCount = 0; yCount < 8; yCount++)
	{
		mapGridRef = ((yOff * MAP_WIDTH) + xOff) + (yCount * 64);//(yCount + 64);//56);
		//xloop
		for (xCount = 0; xCount < 8; xCount++)
		{
			if(mapGridRef > 4095)
				break;	// No point continuing.

			int myalt = map_alt[mapGridRef]<<3;
			int squareType = map_blk[mapGridRef];

			if(squareType == 0) //if it is water
			{
				if(toggle)// || (toggle<<1))	// you what? i.e. (1) or (1 << 1), makes no difference.
				{
					squareType = 16; //changes water tile..
				}
			}
			//not water

			put_block(xCount, yCount, myalt, squareType); //puts the block up

			myalt += 8;
			squareType = map_bk2[mapGridRef];
			//TEQ     r3,#0				; r3 exclusive-or 0. 
			//BEQ     no_seq			; branch if z flag is set. (the value is zero)
			//if (!(squareType ^ 0))	// then jump over the put_block...
			if (squareType)				// here, we negate the state again.
			{
				put_block(xCount, yCount, myalt, squareType);//puts a building on...
			}
			
			//no_block2
			
			squareType = map_who[mapGridRef];
			if (squareType) //if there is someone there
			{
				p_peeps person = peeps[map_who[mapGridRef]];
				if(screen_origin <= ORIGIN)
				{
					move_sprite(xCount, yCount, myalt, &person, mapGridRef);
				}
			}
			//no seq
			if ((devil_magnet == mapGridRef))
			{
				squareType = DEVILS_MAGNET;
				put_block(xCount, yCount, myalt, squareType);

			}

			if ((god_magnet == mapGridRef))
			{
				squareType = GODS_MAGNET;
				put_block(xCount, yCount, myalt, squareType);

			}

			mapGridRef++;// = xCount;	// Surely, this should be at the end of the loop?
		}
	}

	// "Left"-hand wall
	for (altCount = 0; altCount < 8; altCount++)
	{
		altPos = ((yOff * MAP_WIDTH) + xOff + 7) + (altCount * 64);
		for (myAlt = map_alt[altPos]; myAlt > 0; myAlt--)
		{
			draw_fixed_sprite(8, altCount, myAlt - 1, SIDE_WALL1 * 160);
		}
	}
	
	// "Right"-hand wall
	for (altCount = 0; altCount < 8; altCount++)
	{
		altPos = (yOff + 7) * MAP_WIDTH + xOff + altCount;	
		for (myAlt = map_alt[altPos]; myAlt > 0; myAlt--)
		{
			draw_fixed_sprite(altCount + 1, 8, myAlt, SIDE_WALL2 * 160);
		}
	}*/
}

//move sprite......
//this function basically finds the correct frame for the sprite and also finds the correct offset for when a peep is walking
//uphill or whatever...
//The orignal ASM is very convuluted and I don't think I need to do half the stuff it needs to....
//If only I had a file with all the graphics in it. I could find all the correct offsets and everything.
//
//I just worked out that I can use the screen offset... r5... to find the x/y for drawing

void	CPopulous::move_sprite(int x, int y, int alt, UBYTE*	r9, UBYTE*	r11, UBYTE*	r12)//p_peeps *who, int mapGridRef)
{
	//UBYTE	r1;
	long r1, r2, r3, r4, r5, r6, /*oldR3,*/ r7, r0;
	p_peeps *r10;
	long	Stack[1024], Sp;
	Sp = 0;


	r0 = x;						// are: correct
	r1 = y;
	r2 = alt;


	//	MOV 	r0,r0,LSL#3
	//	MOV 	r1,r1,LSL#3
	r0 = r0 << 3;				// are: correct
	r1 = r1 << 3;				// are: correct
	
	//	MOV 	r5,r0
	//	ADD 	r5,r5,r1
	//	SUB 	r5,r5,r2
	r5 = r0 + r1 - r2;			// are: correct
	
	//	MOV 	r0,r0,LSL#1
	//	MOV 	r1,r1,LSL#1
	r0 = r0 << 1;				// are: correct
	r1 = r1 << 1;				// are: correct

	//	MOV 	r4,r0
	//	SUB 	r4,r4,r1
	r4 = r0 - r1;				// are: correct

	//	LDRB	r3,[r12]
	r3 = *r12;					// are: correct

	//	SUB 	r3,r3,#1
	r3--;						// are: correct

	//	STMFD	sp!,{r3}
	Stack[Sp++] = r3;			// are: assumed

	//	MOV 	r7,#SIZEOF_PEEP
	//	MUL 	r3,r7,r3
	//	LDR 	r10,%0					;		pointer to peeps
	//	LDR 	r8,[r10]				// This appears to be redundant
	//	ADD 	r10,r10,r3
//	r8 = *r10;					// are: redundant
	r10 = peeps + r3;			// are: correct

	//	LDRB	r6,[r10,#FLAGS]
	//	AND 	r6,r6,#&f8
	r6 = r10->flags & 0xF8;		// are: correct

	//	LDRB	r3,[r10,#PLAYER]
	//	MOV 	r3,r3,ASL#24
	//	MOV 	r3,r3,ASR#24
	r3 = r10->player;			// are: correct
	//r3 = r3 << 24;				// are: correct
	//r3 = r3 >> 24;				// are: correct
	if ((r3 >> 7) & 0x01)
		r3 = 0xFFFFFF00 | (r3 & 0xFF);

	//	LDR 	r8,%1					;		pointer to player var
	//	LDRB	r7,[r8,#0]
	//	LDRB	r8,[r8,#1]
	//	ORR 	r7,r7,r8,LSL#8
	r7 = player;				// are: correct

	//	MOV 	r7,r7,ASL#16
	//	MOV 	r7,r7,ASR#16
//	r7 = r7 << 16;				// are: correct
//	r7 = r7 >> 16;				// are: correct
	if ((r7 >> 15) & 0x01)
		r7 = 0xFFFF0000 | (r7 & 0xFFFF);


	//	CMP 	r3,r7
	//	BNE 	this_is_not_the_player
	if (r3 != r7)				// are: correct
		goto this_is_not_the_player;	// are: correct
		
	//	LDRB	r6,[r10,#FLAGS]
	r6 = r10->flags & 0xFF;		// are: correct

	//	LDRB	r7,[r10,#IN_OUT]
	//	LDRB	r8,[r10,#IN_OUT+1]
	//	ORRS	r7,r7,r8,LSL#8
	//	BEQ 	person_out
	if ((r10->in_out ^ 0) == 0)	// are: correct
		goto person_out;		// are: correct

	//	ORR 	r6,r6,#IN_TOWN
	r6 = r6 | IN_TOWN;			// are: correct

person_out:
this_is_not_the_player:
	//	LDMFD	sp!,{r7}
	r7 = Stack[--Sp];			// are: correct

	//	CMP 	r7,#FUNNY
	//	BGE 	this_is_funny
	if (r7 >= FUNNY)			// are: correct
		goto this_is_funny;		// are: correct

	//	LDR 	r8,%2					;		pointer to ok_to_build var
	//	LDRB	r7,[r8,#0]
	//	LDRB	r8,[r8,#1]
	//	ORR 	r7,r7,r8,LSL#8
	//	MOV 	r7,r7,ASL#16
	//	MOV 	r7,r7,ASR#16
	//	ORR 	r7,r7,r6
	//	LDR 	r8,%2
	//	STRB	r7,[r8,#0]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r8,#1]
	ok_to_build = ok_to_build | r6;	// are: correct
	r7 = ok_to_build;				// are: correct

this_is_funny:
	//	LDRB	r3,[r10,#FLAGS]
	r3 = r10->flags;				// are: correct
	//	CMP 	r3,#ON_MOVE
	//	BEQ 	move_people
	if (r3 == ON_MOVE)				// are: correct
		goto move_people;			// are: correct
	
	//	CMP 	r3,#IN_TOWN
	//	BEQ 	in_the_town
	if (r3 == IN_TOWN)				// are: correct
		goto in_the_town;			// are: correct

	//	TST 	r3,#1:SHL:IN_WATER
	//	BNE 	move_people
	if ((r3 & IN_WATER) != 0)		// are: correct
		goto move_people;			// are: correct

	//	TST 	r3,#1:SHL:WAIT_FOR_ME
	//	BNE 	wait_for_me
	if ((r3 & WAIT_FOR_ME) != 0)	// are: correct
		goto wait_for_me;			// are: correct

	//	TST 	r3,#1:SHL:I_AM_WAITING
	//	BNE 	wait_for_me
	if ((r3 & I_AM_WAITING) != 0)	// are: correct
		goto wait_for_me;			// are: correct

	//	TST 	r3,#1:SHL:IN_BATTLE
	//	BNE 	in_a_battle
	if ((r3 & IN_BATTLE) != 0)		// are: correct
		goto in_a_battle;			// are: correct

	//	TST 	r3,#1:SHL:IN_RUIN
	//	BNE 	in_a_ruin
	if ((r3 & IN_RUIN) != 0)		// are: correct
		goto in_a_ruin;				// are: correct

	//	ADD 	r4,r4,#320-120-8
	r4 += 320-120-8;				// are: correct

	//	ADD 	r5,r5,#64
	r5 += 64;						// are: correct
	
	//	LDRB	r6,[r10,#FRAME]
	//	LDRB	r7,[r10,#FRAME+1]
	//	ORR 	r6,r6,r7,LSL#8
	//	MOV 	r6,r6,ASL#16
	//	MOV 	r6,r6,ASR#16
	r6 = r10->frame;				// are: correct
	//r6 = r6 << 16;					// are: correct
	//r6 = r6 >> 16;					// are: correct
	if ((r6 >> 15) & 0x01)
		r6 = 0xFFFF0000 | (r6 & 0xFFFF);


	//	LDRB	r7,[r10,#HEAD_FOR]
	//	LDRB	r8,[r10,#HEAD_FOR+1]
	//	ORR 	r7,r7,r8,LSL#8
	//	LDRB	r8,[r10,#HEAD_FOR+2]
	//	ORR 	r7,r7,r8,LSL#16
	//	LDRB	r8,[r10,#HEAD_FOR+3]
	//	ORRS	r7,r7,r8,LSL#24
	//	BEQ 	knight_not_in_effect
	if (r10->head_for == 0)
		goto knight_not_in_effect;
		
	//	ADD 	r6,r6,#KNIGHT_WIN_SPRITE-VICTORY_SPRITE
	r6 += KNIGHT_WIN_SPRITE - VICTORY_SPRITE;

knight_not_in_effect:
	//	LDRB	r7,[r10,#PLAYER]
	r7 = r10->player;				// are: correct

	//	TEQ 	r7,#0
	//	BEQ 	effect_not_god
	if ((r7 ^ 0) == 0)				// are: correct
		goto effect_not_god;		// are: correct

	//	ADD 	r6,r6,#4
	r6 += 4;						// are: correct

effect_not_god:
	//	LDRB	r1,[r12]
	r1 = *r12;						// are: correct

	//	BL		place_in_sprite
	place_in_sprite(r1, r4, r5, r6, r10);	// are: think so.

	//	LDMFD	sp!,{r0-r12,pc}
	return;

in_a_ruin:
	//	LDR 	r7,%3					;		pointer to sound_effect var
	//	LDRB	r8,[r7,#0]
	//	ORR 	r8,r8,#1:SHL:IN_RUIN
	//	STRB	r8,[r7,#0]
	sound_effect = sound_effect | IN_RUIN;	// are: correct

	//	ADD 	r4,r4,#320-120-16
	r4 += 320-120-16;						// are: correct

	//	ADD 	r5,r5,#64
	r5 += 64;								// are: correct

	//	LDR 	r7,%4					;		pointer to game_turn var
	//	LDRB	r6,[r7,#0]
	//	LDRB	r7,[r7,#1]
	//	ORR 	r6,r6,r7,LSL#8
	//	AND 	r6,r6,#3
	//	ADD 	r6,r6,#FIRE_SPRITE
	r6 = game_turn & 0x03;					// are: correct
	r6 += FIRE_SPRITE;						// are: correct

	//	LDRB	r1,[r12]
	r1 = *r12;								// are: correct

	//	BL		place_in_sprite
	place_in_sprite(r1, r4, r5, r6, r10);	// are: think so.

	//	LDMFD	sp!,{r0-r12,pc}
	return;

wait_for_me:
	//	ADD 	r4,r4,#320-120-8
	//	ADD 	r5,r5,#64
	r4 += 320-120-8;				// are: correct
	r5 += 64;						// are: correct

	//	LDRB	r6,[r10,#FRAME]
	//	LDRB	r7,[r10,#FRAME+1]
	//	ORR 	r6,r6,r7,LSL#8
	//	MOV 	r6,r6,ASL#16
	//	MOV 	r6,r6,ASR#16
	r6 = r10->frame;				// are: correct
	//r6 = r6 << 16;					// are: correct
	//r6 = r6 >> 16;					// are: correct
	if ((r6 >> 15) & 0x01)
		r6 = 0xFFFF0000 | (r6 & 0xFFFF);

	//	LDRB	r7,[r10,#HEAD_FOR]
	//	LDRB	r8,[r10,#HEAD_FOR+1]
	//	ORR 	r7,r7,r8,LSL#8
	//	LDRB	r8,[r10,#HEAD_FOR+2]
	//	ORR 	r7,r7,r8,LSL#16
	//	LDRB	r8,[r10,#HEAD_FOR+3]
	//	ORRS	r7,r7,r8,LSL#24
	//	BEQ 	wait_not_knight
	if (r10->head_for == 0)			// are: correct
		goto wait_not_knight;		// are: correct
		
	//	ADD 	r6,r6,#KNIGHT_WAITS_SPRITE-WAIT_SPRITE
	r6 += KNIGHT_WAITS_SPRITE - WAIT_SPRITE;	// are: correct

wait_not_knight:
	//	LDRB	r7,[r10,#PLAYER]
	//	TEQ 	r7,#0
	//	BEQ 	wait_not_god
	if ((r10->player ^ 0) == 0)		// are: correct
		goto wait_not_god;			// are: correct

	//	ADD 	r6,r6,#2
	r6 += 2;						// are: correct

wait_not_god:
	//	LDRB	r1,[r12]
	//	BL		place_in_sprite
	r1 = *r12;						// are: correct
	place_in_sprite(r1, r4, r5, r6, r10);	// are: think so

	//	LDMFD	sp!,{r0-r12,pc}
	return;							// are: correct

in_a_battle:
	//	LDR 	r7,%3					;		pointer to sound_effect var
	//	LDRB	r8,[r7,#0]
	//	ORR 	r8,r8,#1:SHL:IN_BATTLE
	//	STRB	r8,[r7,#0]
	sound_effect = sound_effect | IN_BATTLE;// are: correct

	//	ADD 	r4,r4,#320-120-8
	//	ADD 	r5,r5,#64
	r4 += 320-120-8;				// are: correct
	r5 += 64;						// are: correct

	//	LDRB	r6,[r10,#FRAME]
	//	LDRB	r7,[r10,#FRAME+1]
	//	ORR 	r6,r6,r7,LSL#8
	//	MOV 	r6,r6,ASL#16
	//	MOV 	r6,r6,ASR#16
	r6 = r10->frame;				// are: correct
	//r6 = r6 << 16;					// are: correct
	//r6 = r6 >> 16;					// are: correct
	if ((r6 >> 15) & 0x01)
		r6 = 0xFFFF0000 | (r6 & 0xFFFF);
	
	//	LDRB	r1,[r12]
	//	BL		place_in_sprite
	r1 = *r12;						// are: correct
	place_in_sprite(r1, r4, r5, r6, r10);	// are: think so.

	//	LDMFD	sp!,{r0-r12,pc}
	return;							// are: correct

in_the_town:
	//	ADD 	r4,r4,#320-120-8
	//	ADD 	r5,r5,#64
	r4 += 320-120-8;				// are: correct
	r5 += 64;						// are: correct

	//	MOV 	r6,#FLAG_SPRITE
	r6 = FLAG_SPRITE;				// are: correct

	//	LDR 	r8,%5					;		pointer to toggle var
	//	LDRB	r7,[r8,#0]
	//	LDRB	r8,[r8,#1]
	//	ORR 	r7,r7,r8,LSL#8
	r7 = toggle;					// are: correct

	//	MOV 	r7,r7,ASL#16
	//	MOV 	r7,r7,ASR#16
	//r7 = r7 << 16;					// are: correct
	//r7 = r7 >> 16;					// are: correct
	if ((r7 >> 15) & 0x01)
		r7 = 0xFFFF0000 | (r7 & 0xFFFF);

	//	ADD 	r6,r6,r7
	r6 += r7;						// are: correct

	//	LDRB	r7,[r10,#PLAYER]
	r7 = r10->player;				// are: correct

	//	TEQ 	r7,#0
	//	BEQ 	gods_in_town
	if ((r7 ^ 0) == 0)				// are: correct
		goto gods_in_town;			// are: correct

	//	ADD 	r6,r6,#2
	r6 += 2;						// are: correct

gods_in_town:
	//	LDRB	r1,[r12]
	r1 = *r12;						// are: correct

	//	BL		place_in_sprite
	place_in_sprite(r1, r4, r5, r6, r10);	// are: think so.

	//	LDMFD	sp!,{r0-r12,pc}
	return;							// are: correct

move_people:
	//	LDRB	r7,[r10,#FRAME]
	//	LDRB	r8,[r10,#FRAME+1]
	//	ORR 	r7,r7,r8,LSL#8
	//	MOV 	r7,r7,ASL#16
	//	MOV 	r7,r7,ASR#16
	//	CMP 	r7,#8
	//	BLT 	peep_ok
	if (r10->frame < 8)				// are: correct
		goto peep_ok;				// are: correct

	//	LDMFD	sp!,{r0-r12,pc}
	return;							// are: correct

peep_ok:
	//	LDRB	r6,[r11]
	r6 = *r11;						// are: correct

	//	LDRB	r7,[r10,#DIRECTION]
	//	LDRB	r8,[r10,#DIRECTION+1]
	//	ORR 	r3,r7,r8,LSL#8
	//	MOV 	r3,r3,ASL#16
	//	MOV 	r3,r3,ASR#16
	r3 = r10->direction;			// are: corrected
	//r3 = r3 << 16;					// are: correct
	//r3 = r3 >> 16;					// are: correct
	if ((r3 >> 15) & 0x01)
		r3 = 0xFFFF0000 | (r3 & 0xFFFF);

	//	ADD 	r11,r11,r3
	r11 += r3;						// are: correct

	//	LDRB	r7,[r11]
	r7 = *r11;						// are: correct

	//	SUB 	r6,r6,r7
	r6 -= r7;						// are: correct

	//	MOV 	r6,r6,ASL#24
	//	MOV 	r6,r6,ASR#24
//	r6 = r6 << 24;					// are: correct
//	r6 = r6 >> 24;					// are: correct
	if ((r6 >> 7) & 0x01)
		r6 = 0xFFFFFF00 | (r6 & 0xFF);

	//	LDRB	r7,[r10,#FRAME]
	//	LDRB	r8,[r10,#FRAME+1]
	//	ORR 	r7,r7,r8,LSL#8
	//	MOV 	r7,r7,ASL#16
	//	MOV 	r7,r7,ASR#16
	r7 = r10->frame;				// are: correct

	//	MUL 	r6,r7,r6
	r6 *= r7;						// are: correct

	//	ADD 	r5,r5,r6
	r5 += r6;						// are: correct

	//	LDRB	r7,[r9]
	r7 = *r9;						// are: correct

	//	CMP 	r7,#15
	//	BEQ 	not_a_flat_block
	if (r7 == 15)					// are: correct
		goto not_a_flat_block;		// are: correct

	//	ADD 	r5,r5,#4
	r5 += 4;						// are: correct

not_a_flat_block:
	//	LDRB	r7,[r10,#FRAME]
	//	LDRB	r8,[r10,#FRAME+1]
	//	ORR 	r7,r7,r8,LSL#8
	//	MOV 	r7,r7,ASL#16
	//	MOV 	r7,r7,ASR#16
	r7 = r10->frame;				// are: correct

	//	CMP 	r3,#-65
	//	BNE 	not_up
	if (r3 != -65)					// are: correct
		goto not_up;				// are: correct

	//	CMP 	r7,#4
	//	BLE 	up_no_clip
	if (r7 <= 4)					// are: correct
		goto up_no_clip;			// are: correct

	//	TEQ 	r0,#0
	//	BEQ 	end_move_sprite
	if ((r0 ^ 0) == 0)				// are: correct
		goto end_move_sprite;		// are: correct

	//	TEQ 	r1,#0				
	//	BEQ 	end_move_sprite
	if ((r1 ^ 0) == 0)				// are: correct
		goto end_move_sprite;		// are: correct

up_no_clip:
	//	MOV 	r0,#0
	//	MOV 	r1,#-2
	//	MOV 	r6,#0
	r0 = 0;							// are: correct
	r1 = -2;						// are: correct
	r6 = 0;							// are: correct

	//	B		end_of_tests
	goto end_of_tests;				// are: correct

not_up:
	//	CMP 	r3,#-64
	//	BNE 	not_up_right
	if (r3 != -64)					// are: correct
		goto not_up_right;			// are: correct

	//	CMP 	r7,#4
	//	BLE 	up_right_no_clip
	if (r7 <= 4)					// are: correct
		goto up_right_no_clip;		// are: correct

	//	TEQ 	r1,#0
	//	BEQ 	end_move_sprite
	if ((r1 ^ 0) == 0)				// are: correct
		goto end_move_sprite;		// are: correct

up_right_no_clip:
	//	MOV 	r0,#2
	//	MOV 	r1,#-1
	//	MOV 	r6,#2
	r0 = 2;							// are: correct
	r1 = -1;						// are: correct
	r6 = 2;							// are: correct

	//	B		end_of_tests
	goto end_of_tests;				// are: correct

not_up_right:
	//	CMP 	r3,#-63
	//	BNE 	not_right
	if (r3 != -63)					// are: correct
		goto not_right;				// are: correct

	//	CMP 	r7,#4
	//	BLE 	right_no_clip
	if (r7 <= 4)					// are: correct
		goto right_no_clip;			// are: correct

	//	CMP 	r0,#16*7
	//	BEQ 	end_move_sprite
	if (r0 == (16*7))				// are: correct
		goto end_move_sprite;		// are: correct

	//	TEQ 	r1,#0
	//	BEQ 	end_move_sprite
	if ((r1 ^ 0) == 0)				// are: correct
		goto end_move_sprite;		// are: correct

right_no_clip:
	//	MOV 	r0,#4
	//	MOV 	r1,#0
	//	MOV 	r6,#4
	r0 = 4;							// are: correct
	r1 = 0;							// are: correct
	r6 = 4;							// are: correct

	//	B		end_of_tests
	goto end_of_tests;				// are: correct

not_right:
	//	CMP 	r3,#1
	//	BNE 	not_down_right
	if (r3 != 1)					// are: correct
		goto not_down_right;		// are: correct

	//	CMP 	r7,#4
	//	BLE 	down_right_no_clip
	if (r7 <= 4)					// are: correct
		goto down_right_no_clip;	// are: correct

	//	CMP 	r0,#16*7
	//	BEQ 	end_move_sprite
	if (r0 == (16 * 7))				// are: correct
		goto end_move_sprite;		// are: correct

down_right_no_clip:
	//	MOV 	r0,#2
	//	MOV 	r1,#1
	//	MOV 	r6,#6
	r0 = 2;							// are: correct
	r1 = 1;							// are: correct
	r6 = 6;							// are: correct

	//	B		end_of_tests
	goto end_of_tests;				// are: correct

not_down_right:
	//	CMP 	r3,#65
	//	BNE 	not_down
	if (r3 != 65)					// are: correct
		goto not_down;				// are: correct

	//	CMP 	r7,#4
	//	BLE 	down_no_clip
	if (r7 <= 4)					// are: correct
		goto down_no_clip;			// are: correct

	//	CMP 	r0,#16*7
	//	BEQ 	end_move_sprite
	if (r0 == (16 * 7))				// are: correct
		goto end_move_sprite;		// are: correct

	//	CMP 	r1,#16*7
	//	BEQ 	end_move_sprite
	if (r1 == (16 * 7))				// are: correct
		goto end_move_sprite;		// are: correct

down_no_clip:
	//	MOV 	r0,#0
	//	MOV 	r1,#2
	//	MOV 	r6,#8
	r0 = 0;		// are: correct
	r1 = 2;		// are: correct
	r6 = 8;		// are: correct

	//	B		end_of_tests
	goto end_of_tests;	// are: correct

not_down:
	//	CMP 	r3,#64
	//	BNE 	not_down_left
	if (r3 != 64)			// are: correct
		goto not_down_left;	// are: correct

	//	CMP 	r7,#4
	//	BLE 	down_left_no_clip
	if (r7 <= 4)				// are: correct
		goto down_left_no_clip;	// are: correct

	//	CMP 	r1,#16*7
	//	BEQ 	end_move_sprite
	if (r1 == (16 * 7))			// are: correct
		goto end_move_sprite;	// are: correct

down_left_no_clip:
	//	MOV 	r0,#-2
	//	MOV 	r1,#1
	//	MOV 	r6,#10
	r0 = -2;	// are: correct
	r1 = 1;		// are: correct
	r6 = 10;	// are: correct

	//	B		end_of_tests
	goto end_of_tests;	// are: correct

not_down_left:
	//	CMP 	r3,#63
	//	BNE 	not_left
	if (r3 != 63)		// are: correct
		goto not_left;	// are: correct

	//	CMP 	r7,#4
	//	BLE 	left_no_clip
	if (r7 <= 4)			// are: correct
		goto left_no_clip;	// are: correct

	//	TEQ 	r0,#0
	//	BEQ 	end_move_sprite
	if ((r0 ^ 0) == 0)			// are: correct
		goto end_move_sprite;	// are: correct

	//	CMP 	r1,#16*7
	//	BEQ 	end_move_sprite
	if (r1 == (16 *7))			// are: correct
		goto end_move_sprite;	// are: correct

left_no_clip:
	//	MOV 	r0,#-4
	//	MOV 	r1,#0
	//	MOV 	r6,#12
	r0 = -4;	// are: correct
	r1 = 0;		// are: correct
	r6 = 12;	// are: correct

	//	B		end_of_tests
	goto end_of_tests;	// are: correct

not_left:
	//	CMP 	r3,#-1
	//	BNE 	not_up_left
	if (r3 != -1)			// are: correct
		goto not_up_left;	// are: correct

	//	CMP 	r7,#4
	//	BLE 	up_left_no_clip
	if (r7 <= 4)				// are: correct
		goto up_left_no_clip;	// are: correct

	//	TEQ 	r0,#0
	//	BEQ 	end_move_sprite
	if ((r0 ^ 0) == 0)			// are: correct
		goto end_move_sprite;	// are: correct

up_left_no_clip:
	//	MOV 	r0,#-2
	//	MOV 	r1,#-1
	//	MOV 	r6,#14
	r0 = -2;	// are: correct
	r1 = -1;	// are: correct
	r6 = 14;	// are: correct

	//	B		end_of_tests
	goto end_of_tests;	// are: correct
	
not_up_left:
	//	MOV 	r0,#0
	//	MOV 	r1,#0
	//	MOV 	r6,#8
	r0 = 0;	// are: correct
	r1 = 0;	// are: correct
	r6 = 8;	// are: correct

end_of_tests:
	//	LDRB	r3,[r12]
	r3 = *r12;	// are: correct

	//	CMP 	r3,#FUNNY
	//	BLT 	normal_sprite
	if (r3 < FUNNY)			// are: corrected
		goto normal_sprite;	// are: correct

	//	LDRB	r6,[r10,#BATTLE_POPULATION]
	//	LDRB	r7,[r10,#BATTLE_POPULATION+1]
	//	ORR 	r6,r6,r7,LSL#8
//	r6 = r10->battle_population;	// are: correct
	// This code would appear to cause the game to fail. 
	// We guess that the code should be referencing frame
	r6 = r10->frame;

	//	B		all_sprites
	goto all_sprites;				// are: correct

normal_sprite:
	//	LDRB	r3,[r10,#FLAGS]
	r3 = r10->flags;				// are: correct

	//	TST 	r3,#1:SHL:IN_WATER
	//	BEQ 	people_not_in_water
	if ((r3 & IN_WATER) == 0)		// are: correct
		goto people_not_in_water;	// are: correct

	//	LDR 	r7,%4					;		pointer to game_turn var
	//	LDRB	r6,[r7,#0]
	//	LDRB	r7,[r7,#1]
	//	ORR 	r6,r6,r7,LSL#8
	//	AND 	r6,r6,#3
	//	ADD 	r6,r6,#WATER_SPRITE
	r6 = game_turn & 0x03;	// are: correct
	r6 += WATER_SPRITE;		// are: correct

	//	LDRB	r7,[r10,#HEAD_FOR]
	//	LDRB	r8,[r10,#HEAD_FOR+1]
	//	ORR 	r7,r7,r8,LSL#8
	//	LDRB	r8,[r10,#HEAD_FOR+2]
	//	ORR 	r7,r7,r8,LSL#16
	//	LDRB	r8,[r10,#HEAD_FOR+3]
	//	ORRS	r7,r7,r8,LSL#24
	//	BEQ 	water_not_knight
	if (r10->head_for == 0)		// are: correct
		goto water_not_knight;	// are: correct

	//	ADD 	r6,r6,#KNIGHT_DROWNS-WATER_SPRITE
	r6 += KNIGHT_DROWNS - WATER_SPRITE;	// are: correct

water_not_knight:
	//	LDRB	r7,[r10,#PLAYER]
	r7 = r10->player;			// are: correct

	//	TEQ 	r7,#0
	//	BEQ 	all_sprites
	if ((r7 ^ 0) == 0)			// are: correct
		goto all_sprites;		// are: correct

	//	ADD 	r6,r6,#4
	r6 += 4;					// are: correct

	//	B		all_sprites
	goto all_sprites;			// are: correct

people_not_in_water: 
	//	LDRB	r7,[r10,#HEAD_FOR]
	//	LDRB	r8,[r10,#HEAD_FOR+1]
	//	ORR 	r7,r7,r8,LSL#8
	//	LDRB	r8,[r10,#HEAD_FOR+2]
	//	ORR 	r7,r7,r8,LSL#16
	//	LDRB	r8,[r10,#HEAD_FOR+3]
	//	ORRS	r7,r7,r8,LSL#24
	//	BEQ 	normal_people
	if (r10->head_for == 0)	// are: correct
		goto normal_people;	// are: correct

	//	ADD 	r6,r6,#KNIGHT_PEOPLE
	r6 += KNIGHT_PEOPLE;	// are: correct

normal_people:
	//	LDRB	r7,[r10,#PLAYER]
	r7 = r10->player;			// are: correct

	//	TEQ 	r7,#0
	//	BEQ 	this_is_god
	if ((r7 ^ 0) == 0)			// are: correct
		goto this_is_god;		// are: correct

	//	ADD 	r6,r6,#BAD_PEOPLE
	r6 += BAD_PEOPLE;			// are: correct

this_is_god:
	//	LDR 	r8,%5					;		pointer to toggle var
	//	LDRB	r7,[r8,#0]
	//	LDRB	r8,[r8,#1]
	//	ORR 	r7,r7,r8,LSL#8
	//	MOV 	r7,r7,ASL#16
	//	MOV 	r7,r7,ASR#16
	//	ADD 	r6,r6,r7
	r6 += toggle;	// are: correct

all_sprites:
	//	LDRB	r7,[r10,#FRAME]
	//	LDRB	r8,[r10,#FRAME+1]
	//	ORR 	r7,r7,r8,LSL#8
	//	MOV 	r7,r7,ASL#16
	//	MOV 	r7,r7,ASR#16
	r7 = r10->frame;		// are: correct

	//	MUL 	r0,r7,r0
	r0 *= r7;				// are: correct

	//	MUL 	r1,r7,r1
	r1 *= r7;				// are: correct

	//	ADD 	r4,r4,r0
	r4 += r0;				// are: correct

	//	ADD 	r5,r5,r1
	r5 += r1;				// are: correct

	//	ADD 	r4,r4,#320-128-8
	//	ADD 	r5,r5,#64
	r4 += 320-128-8;		// are: correct
	r5 += 64;				// are: correct

	//	LDRB	r1,[r12]
	r1 = *r12;				// are: correct

	//	BL		place_in_sprite
	place_in_sprite(r1, r4, r5, r6, r10);	// are: think so.

end_move_sprite:
	return;
	//	LDMFD	sp!,{r0-r12,pc}
//0
//		DCD 	peeps
//1
//		DCD 	player
//2
//		DCD 	ok_to_build
//3
//		DCD 	sound_effect
//4
//		DCD 	game_turn
//5
//		DCD 	toggle

		/*





	int xPos = x<<3;
	int yPos = y<<3;

	int r5 = (xPos + yPos) - alt; //this must be the position on the map to draw.. In the asm it gets various vaslues added
											   //to it.. but mostly 64, which would take the position down one block..

	xPos = xPos<<1;
	yPos = yPos<<1;

	int r4 = xPos - yPos; //this is called r4 because I don't know what it is supposed to represent

	int r3,r6,r7,r8 = 0;
	int spriteOffset = 0;	//this is the sprite offset into the file
	int a,b =0;

	r6 = who->flags;
	r7 = who->in_out;
	r8 = who->in_out +1;

	r3 = who->direction;

	//ok now try to get the action they are performing.

	if(who->in_out)
	{
		who->flags = IN_TOWN;
	}

	if(who->flags != FUNNY)
	{
		//in the asm some maths stuff goes here I don't think I need it... but this space is here... just in case I'm wrong
		r7 = ok_to_build;
	}
	
	//Now this switch statement is going to have to have a lot of code.... just beacuse I loathe gotos.......
	switch(who->flags)
	{
	case ON_MOVE:
	case IN_WATER:

		if(who->frame >= 8)
			return;

		//this works out the screen_offset.. ie. r5
		//I'm gonna translate this directly from asm so sorry if it doesn't make any sense.. It doesn't to me either
		r6 = alt;
		r3 = who->direction;
		alt = alt + r3;

		r7 = alt;

		r6 = r6 - r7;

		r6 = abs(r6);

		r7 = who->frame;
		r6 = r7 * r6;
		r5 = r5 + r6;

		if(map_blk[mapGridRef] != 15)
			r5 += 4;

		//  Ok sorry bout that... I'll try to make the rest understandable.... to a point..

//not a flat block
		//I think this next line is correct.
		r7 = who->frame;


	//	Ok I am soooo sorry about this but the translation rquires jumps that will be a bitch in
	//	proper c++ so I'm gonna have to use a *few* gotos..... God I hate assembly......

		if(r3 != -65)
			goto not_up;

		if(r7 <= 4)
			goto up_no_clip;

		if(xPos == 0)
			return;

		if(yPos == 0)
			return;
up_no_clip:
		xPos = 0;
		yPos = -2;
		alt	= 0;
		goto end_of_tests;

not_up:
		if(r3 != -64)
			goto not_up_right;

		if(r7 <= 4)
			goto up_right_no_clip;

		if(yPos == 0)
			return;

up_right_no_clip:
		
		xPos = 0;
		yPos = -1;
		alt = 2;
		goto end_of_tests;
not_up_right:
		if(r3 != -63)
			goto not_right;

		if(r7 <= 4)
			goto right_no_clip;

		if(xPos == (16*7))
			return;

		if(yPos == 0)
			return;

right_no_clip:

		xPos = 4;
		yPos = 0;
		alt = 4;
		goto end_of_tests;

not_right:

		if(r3 != 1)
			goto not_down_right;

		if(r7 <= 4)
			goto down_right_no_clip;
		
		if(xPos == (16*7) )
			return;

down_right_no_clip:

		xPos = 2;
		yPos = 1;
		alt = 6;
		goto end_of_tests;

not_down_right:

		if(r3 != 65)
			goto not_down;

		if(r7 <= 4)
			goto down_no_clip;

		if(xPos == (16*7) )
			return;

		if(yPos == (16*7) )
			return;

down_no_clip:
		
		xPos = 0;
		yPos = 2;
		alt = 8;
		goto end_of_tests;

not_down:
	
		if( r3 != 64)
			goto not_down_left;

		if(r7 <= 4)
			goto down_left_no_clip;

		if(yPos == (16 * 7) )
			return;

down_left_no_clip:

		xPos = -2;
		yPos = 1;
		alt = 10;
		goto end_of_tests;

not_down_left:
		
		if( r3 != 63)
			goto not_left;

		if(r7 <= 4)
			goto left_no_clip;

		if(xPos == 0)
			return;

		if(yPos == (16 * 7) )
			return;

left_no_clip:
		
		xPos = -4;
		yPos = 0;
		alt = 12;
		goto end_of_tests;

not_left:

		if( r3 != -1)
			goto not_up_left;

		if(r7 <= 4)
			goto up_left_no_clip;

		if(xPos == 0)
			return;

up_left_no_clip:
	
		xPos = -2;
		yPos = -1;
		alt = 14;
		goto end_of_tests;

not_up_left:
		xPos = 0;
		yPos = 0;
		alt = 8;
end_of_tests:
		
		if( who->flags >= FUNNY)
		{
			r6 = who->battle_population;
			r7 = who->battle_population<<1;

			r6 = r6 | r7;

			goto all_sprites;
		}

//normal sprite
		if(who->flags == IN_WATER)
			goto people_not_in_water;

		r6 = game_turn;

		r6 = r6 & 3;

		r6 = r6 + WATER_SPRITE;

		if(who->status == 0)
			goto water_not_knight;

		r6 += (KNIGHT_DROWNS-WATER_SPRITE);

water_not_knight:

		if(who->player)
			goto all_sprites;

		r6 += 4;

		goto all_sprites;
people_not_in_water:
		
		if(who->status == 0)
			goto normal_people;

		r6 += KNIGHT_PEOPLE;

normal_people:

		if(who->player == 0)
			goto this_is_god;

		r6 += BAD_PEOPLE;

this_is_god:

		r6 += toggle;

all_sprites:
		r7 = who->frame;
		xPos = xPos * r7;
		yPos = yPos * r7;

		r4 += xPos;
		r5 += yPos;

		r4 += (320-128-8);
		r5 += 64;

		place_in_sprite(yPos, r4, r5, r6, r7, who);

		break;
	case IN_TOWN:

		r5 += 64; //no I don't have a clue now either
		r4 += 320-120-8; //well I don't know what this is supposed to do....

		//sprite_offset = FLAG_SPRITE;
		
		who->frame = !who->frame;

		if(who->player == 1)
		{
			//spriteOffset += 2;
		}

		place_in_sprite(yPos, r4, r5, r6, r7, who);

		break;
	case WAIT_FOR_ME:
	case I_AM_WAITING:
		
		r5 += 64; //no I don't have a clue now either
		r4 += 320-120-8; //well I don't know what this is supposed to do....

		//it then seems to alternate the frames. I'm gonna do this in a different way
		who->frame = !who->frame;

		if(who->status == 1)
		{
			spriteOffset += KNIGHT_WAITS_SPRITE-WAIT_SPRITE;
		}
		//now check wether the player is blue or red

		if(who->player == 1)
		{
			spriteOffset += 2;
		}

		//then place that sprite ready to draw....
		place_in_sprite(yPos, r4, r5, r6, r7, who); //this is just a guess at what needs to be passed in

		break;
	case IN_BATTLE:
		r5 += 64; //no I don't have a clue now either
		r4 += 320-120-8; //well I don't know what this is supposed to do....

		sound_effect = IN_BATTLE;

		who->frame = !who->frame;

		place_in_sprite(yPos, r4, r5, r6, r7, who);
		break;
	case IN_RUIN:

		r5 += 64; //no I don't have a clue now either
		r4 += 320-120-8; //well I don't know what this is supposed to do....

		//have to play the ruin sound effect here
		sound_effect = IN_RUIN;

		a = game_turn;
		a = a & 3;

		spriteOffset = FIRE_SPRITE + a;

		place_in_sprite(yPos, r4, r5, r6, r7, who);

		break;
	default:

		r5 += 64; //no I don't have a clue now either
		r4 += 320-120-8; //well I don't know what this is supposed to do....
		//first work out if this is a knight. when a person is knighted it sets thier status to 1 so I'm going to use this
		if(who->status == 1)
		{
			spriteOffset += KNIGHT_WAITS_SPRITE-WAIT_SPRITE;
		}

		//now check wether the player is blue or red

		if(who->player == 1)
		{
			spriteOffset += 2;
		}

		//then place that sprite ready to draw....
		place_in_sprite(yPos, r4, r5, r6, r7, who); //this is just a guess at what needs to be passed in
		break;
	}

	return;
*/	
}

//by the way yPos isn't actually yPos......... I was wrong... so shoot me....
// _MapWhoRef, the new name for yPos, is a pointer into map_who
//void	CPopulous::place_in_sprite(int _MapWhoRef, int r4, int r5, int r6, int r1,p_peeps *who)
void	CPopulous::place_in_sprite(long r1, long r4, long r5, long r6, p_peeps	*r10)
{
	if (r6 > 255)
	{
		ODSN("Large sprite index");
	}

	long		r0, r7;
	s_sprite	*r11;
	m_magnet	*r9;


	//	LDR 	r8,%0					;		pointer to no_sprites var
	//	LDRB	r7,[r8,#0]
	//	LDRB	r8,[r8,#1]
	//	ORR 	r0,r7,r8,LSL#8
	r0 = no_sprites;

	//	MOV 	r0,r0,ASL#16
	//	MOV 	r0,r0,ASR#16
	//r0 = r0 << 16;
	//r0 = r0 >> 16;
	
	if ((r0 >> 15) & 0x01)
		r0 = 0xFFFF0000 | (r0 & 0xFFFF);

	/*
	//	MOV 	r7,#S_SIZEOF_SPRITE
	r7 = S_SIZEOF_SPRITE;

	//	MUL 	r0,r7,r0
	r0 *= r7;
	*/

	//	LDR 	r11,%1					;		pointer to sprite
	//	ADD 	r11,r11,r0
	r11 = sprite + r0;

	//	MOV 	r7,r4
	//	STRB	r7,[r11,#S_X]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r11,#S_X+1]
	r11->x = r4;
	
	//	MOV 	r7,r5
	//	STRB	r7,[r11,#S_Y]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r11,#S_Y+1]
	r11->y = r5;

	//	MOV 	r7,r6
	//	STRB	r7,[r11,#S_FRAME]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r11,#S_FRAME+1]
	r11->frame = r6;

	//	MOV 	r7,r1
	//	STRB	r7,[r11,#S_WHO]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r11,#S_WHO+1]
	r11->who = r1;

	//	LDR 	r8,%0					;		pointer to no_sprites var
	//	LDRB	r7,[r8,#0]
	//	LDRB	r8,[r8,#1]
	//	ORR 	r7,r7,r8,LSL#8
	r7 = no_sprites;

	//	MOV 	r7,r7,ASL#16
	//	MOV 	r7,r7,ASR#16
	//r7 = r7 << 16;
	//r7 = r7 >> 16;
	
	if ((r7 >> 15) & 0x01)
		r7 = 0xFFFF0000 | (r7 & 0xFFFF);

	//	ADD 	r7,r7,#1
	r7++;

	//	LDR 	r8,%0					;		pointer to no_sprites var
	//	STRB	r7,[r8,#0]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r8,#1]
	no_sprites = r7;

	//	LDR 	r8,%2					;		pointer to view_who var
	//	LDRB	r7,[r8,#0]
	//	LDRB	r8,[r8,#1]
	//	ORR 	r7,r7,r8,LSL#8
	r7 = view_who;

	//	MOV 	r7,r7,ASL#16
	//	MOV 	r7,r7,ASR#16
	//r7 = r7 << 16;
	//r7 = r7 >> 16;
	
	if ((r7 >> 15) & 0x01)
		r7 = 0xFFFF0000 | (r7 & 0xFFFF);
	//	CMP 	r1,r7
	//	BNE 	there_is_no_shield
	if (r1 != r7)
		goto there_is_no_shield;

	//	ADD 	r11,r11,#S_SIZEOF_SPRITE
	r11++;

	//	ADD 	r7,r4,#8
	r7 = r4 + 8;

	//	STRB	r7,[r11,#S_X]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r11,#S_X+1]
	r11->x = r7;

	//	MOV 	r7,r5
	r7 = r5;

	//	STRB	r7,[r11,#S_Y]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r11,#S_Y+1]
	r11->y = r7;

	//	MOV 	r7,#SHIELD_SPRITE
	r7 = SHIELD_SPRITE;

	//	STRB	r7,[r11,#S_FRAME]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r11,#S_FRAME+1]
	r11->frame = r7;

	//	MOV 	r7,r1
	r7 = r1;

	//	STRB	r7,[r11,#S_WHO]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r11,#S_WHO+1]
	r11->who = r7;

	//	LDR 	r8,%0					;		pointer to no_sprites var
	//	LDRB	r7,[r8,#0]
	//	LDRB	r8,[r8,#1]
	//	ORR 	r7,r7,r8,LSL#8
	r7 = no_sprites;

	//	MOV 	r7,r7,ASL#16
	//	MOV 	r7,r7,ASR#16
	//r7 = r7 << 16;
	//r7 = r7 >> 16;
	
	if ((r7 >> 15) & 0x01)
		r7 = 0xFFFF0000 | (r7 & 0xFFFF);

	//	ADD 	r7,r7,#1
	r7++;

	//	LDR 	r8,%0					;		pointer to no_sprites var
	//	STRB	r7,[r8,#0]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r8,#1]
	no_sprites = r7;

there_is_no_shield:
	//	LDRB	r6,[r10,#PLAYER]
	r6 = r10->player;

	//	MOV 	r6,r6,ASL#24
	//	MOV 	r6,r6,ASR#24
	//r6 = r6 << 24;
	//r6 = r6 >> 24;
	if ((r6 >> 7) & 0x01)
		r6 = 0xFFFFFF00 | (r6 & 0xFF);

	//	LDR 	r9,%3					;		pointer to magnet
	r9 = magnet;

	//	TEQ 	r6,#0
	//	BEQ 	player_must_be_god
	if ((r6 ^ 0) == 0)
		goto player_must_be_god;

	//	ADD 	r9,r9,#M_SIZEOF_MAGNET
	r9++;

player_must_be_god:
	//	LDRB	r7,[r9,#M_CARRIED]
	//	LDRB	r8,[r9,#M_CARRIED+1]
	//	ORR 	r7,r7,r8,LSL#8
	r7 = r9->carried;

	//	MOV 	r7,r7,ASL#16
	//	MOVS	r7,r7,ASR#16
	//r7 = r7 << 16;
	//r7 = r7 >> 16;
	
	if ((r7 >> 15) & 0x01)
		r7 = 0xFFFF0000 | (r7 & 0xFFFF);

	//	BEQ 	end_of_place_in_sprite
	if (r7 == 0)
		goto end_of_place_in_sprite;

	//	CMP 	r1,r7
	//	BNE 	end_of_place_in_sprite
	if (r1 != r7)
		goto end_of_place_in_sprite;

	//	ADD 	r6,r6,#MAGNET_SPRITE
	r6 += MAGNET_SPRITE;

	//	ADD 	r11,r11,#S_SIZEOF_SPRITE
	r11++;

	//	ADD 	r4,r4,#8
	r4 += 8;

	//	MOV 	r7,r4
	//	STRB	r7,[r11,#S_X]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r11,#S_X+1]
	r11->x = r4;

	//	MOV 	r7,r5
	//	STRB	r7,[r11,#S_Y]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r11,#S_Y+1]
	r11->y = r5;

	//	MOV 	r7,r6
	//	STRB	r7,[r11,#S_FRAME]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r11,#S_FRAME+1]
	r11->frame = r6;

	//	MOV 	r7,r1
	//	STRB	r7,[r11,#S_WHO]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r11,#S_WHO+1]
	r11->who = r1;

	//	LDR 	r8,%0					;		pointer to no_sprites var
	//	LDRB	r7,[r8,#0]
	//	LDRB	r8,[r8,#1]
	//	ORR 	r7,r7,r8,LSL#8
	r7 = no_sprites;

	//	MOV 	r7,r7,ASL#16
	//	MOV 	r7,r7,ASR#16
	//r7 = r7 << 16;
	//r7 = r7 >> 16;
	
	if ((r7 >> 15) & 0x01)
		r7 = 0xFFFF0000 | (r7 & 0xFFFF);

	//	ADD 	r7,r7,#1
	r7++;

	//	LDR 	r8,%0					;		pointer to no_sprites var
	//	STRB	r7,[r8,#0]
	//	MOV 	r7,r7,LSR#8
	//	STRB	r7,[r8,#1]
	no_sprites = r7;

end_of_place_in_sprite:
	//	LDMFD	sp!,{r0-r12,pc}
	return;

/*
0
        DCD     no_sprites
1
        DCD     sprite
2
        DCD     view_who
3
        DCD     magnet
*/
	/*
	if (no_sprites >= 64)	// Safe guard.
		return;

	s_sprite *newSprite = &sprite[no_sprites];

	newSprite->x = r4;
	newSprite->y = r5; 
	newSprite->frame = r6;	
	newSprite->who = r1;	

	no_sprites++;

	newSprite = &sprite[no_sprites];
	if(_MapWhoRef == view_who)
	{
		newSprite->x = 10;//r4;
		newSprite->y = 0;//r5;
		newSprite->frame = SHIELD_SPRITE;
		newSprite->who = r1;

		no_sprites++;
		newSprite = &sprite[no_sprites];
	}

	m_magnet theMagnet = magnet[0];

	if(who->player != 0)
	{
		theMagnet = magnet[1];
	}

	if(theMagnet.carried)
	{
		if(_MapWhoRef == theMagnet.carried)
		{
			r4 += 4;
			r6 += MAGNET_SPRITE;
			
			newSprite->x = 20;//r4;
			newSprite->y = 0;//r5;
			newSprite->frame = r6;
			newSprite->who = r1;

			no_sprites++;
		}
	}*/
}

// ----
void	CPopulous::change_palette(int _a)
{
	GraphicsRef->SetActivePalette(_a);
}

void	CPopulous::open_screen()
{
	// Either state.
	d_screen = 0;
	w_screen = 1;
	
	// Main game
	back_scr = 2;

	// Titles
	o_screen = 2;
	b_screen = 3;

	GraphicsRef->SwapScreens();
	GraphicsRef->EndDraw();

	GraphicsRef->StartDraw();
}

#define SPRITE_NAME "helpsidebar"
#define		KEYBOARD_IMAGE	"keyboard"
void	CPopulous::Help()
{

	int SelectedMenu = 0;
	int Page		 = 0;
	int NumMenuPages = 4; //Remember to update for each page
	POINT *mousePosition;
//	RECT Region;
//	RECT CornerRegion;
	char buffer[40];
	CString TempString;
//	bool 
	Redraw		= true;

	GraphicsRef->SetActiveScreen(0);
	GraphicsRef->ClearScreen(0,0,0);

	Region.top = 0;
	Region.bottom = 240;
	Region.left = 0;
	Region.right = 80;

	SAFELY_DELETE(TheKeyboardGraphic);
	NEW(TheKeyboardGraphic, CGraphicsData());
	SAFELY_CONTROL(if, TheKeyboardGraphic, Init(GraphicsRef, &CString(SPRITE_NAME), ThePlatform), ==, false)
		return;
	
	GraphicsRef->DrawSprite(0,0, TheKeyboardGraphic, Region);
	
	//Now draw the page counter in the bottom corner
	Region.top = 0;
	Region.bottom = 35;
	Region.left = 0;
	Region.right = 96;
	GraphicsRef->DrawSprite(220, 200, TheMenuCornerGraphics, Region);

	bool	Continue  = true;
	while (Continue)
	{
		TheGameWindow->Process();
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

		if (selected >= 0)
		{
			for (int i = MusicTimer->GetNumberOfEvents(); i--;)
			{
				ProcessAudio();
			}
		}
		MusicTimer->ClearNumberOfEvents(); //	Clear ticks, as we simply don't care yet, but will when 'selected' changes

		// No process the sound through our interface.
		TheSoundSystem->Process();

#if 0
#ifdef NOT_FINAL_BUILD
		if (ControllerRef->IsButtonPressed(DebugQuickExit))
			exit(0);
#endif
#endif
		mousePosition = ControllerRef->GetMousePosition();
		int y = mousePosition->y;
		int x = mousePosition->x;

		if(ControllerRef->IsButtonPressedDB(MouseSelect))
		{
			if(x < 80)
			{
				switch (y / 40)
				{
				case 0:
					Redraw = true;
					NumMenuPages = 4;
					SelectedMenu = 0;
					Page = 0;
					break;

				case 1:
					Redraw = true;
					NumMenuPages = 2;
					SelectedMenu = 1;
					Page = 0;
					break;

				case 2:
					Redraw = true;
					NumMenuPages = 10; 
					SelectedMenu = 2;
					Page = 0;
					break;

				case 3:
					Redraw = true;
					NumMenuPages = 24;	// 23
					SelectedMenu = 3;
					Page = 0;
					break;

				case 4:
					Redraw = true;
					NumMenuPages = 11; // 10
					SelectedMenu = 4;
					Page = 0;
					break;

				case 5:
					Continue  = false;

				default:
					break;
				}
			}
			else
			{
				if(y >= 200)
				{
					if (	(x < (220 + 48))
						&&	(x > 220))
					{
						if (Page > 0)
						{
							Redraw = true;
							Page--;
						}
					}
					else if(	(x > (220 + 48))
							&&	(x < 320))
					{
						if (Page < (NumMenuPages - 1))
						{
							Redraw = true;
							Page++;
						}
					}
				}
			}
		}

		/*
			Ok this bit basically loads up a text file as determined by SelectedMenu.
			This is a help file on one of the parts of the game.

			Also this will draw graphical representations of the help to aid them.
			Where and what graphics are drawn are also determined by SelectedMenu
		*/

		switch(SelectedMenu)
		{
		case 0:
		case 1:
		case 2:
			break;
		case 3:
			//*****************
			//for this I must draw the correct icon on screen
			//*****************
			if(Page != 0)
			{
				RECT iconRegion;
				iconRegion.top = 20 * ( (Page - 1) / 6);
				iconRegion.bottom = iconRegion.top + 19;
				iconRegion.left = (30 * ((Page - 1) % 6)) + (10 * ((Page - 1) % 6));
				iconRegion.right = iconRegion.left + 30;

				GraphicsRef->DrawSprite(90, 10, HelpIconGraphics, iconRegion);
			}
			break;
		case 4:
		default:
			break;
		}

		/*****
		THIS CODE IS GOING HERE BECUSE EVERYTHING READS FROM A TEXT FILE.....
		*/////

		//This is the main menu thingy, it gives an overview of what the game is
		//about and other stuff.
		//no graphics on this one just load up the appropriate string.
		if(Redraw)
		{
			GraphicsRef->StartDraw();
			GraphicsRef->SetActiveScreen(0);

			Region.top = 0;
			Region.bottom = 240;
			Region.left = 0;
			Region.right = 80;
			GraphicsRef->DrawSprite(0,0, TheKeyboardGraphic, Region);

			Region.top = 0;
			Region.bottom = 240;
			Region.left = 80;
			Region.right = 320;
			GraphicsRef->DrawRect(&Region,50,50,50);

			FILE *text;

			TempString = ThePlatform->GetPath(EP_Text);
			sprintf(convert, "Help%d-%d.ctd", SelectedMenu, (Page + 1));// Put your file name here!
			TempString += convert;
			text = _wfopen(TempString.GetString(), TEXT("rb")); // No longer translated!

			CString theText;

			if(text != NULL)
			{
				//fseek( text, 0L, SEEK_SET ); // No need
				
				if (theText.LoadEncodedString(text))
				{
					GraphicsRef->DrawString(	StandardFont, 
												&(theText),
												84, 
												0);
				}
				fclose(text);
			}

			//Now draw the page counter in the bottom corner
			Region.top = 0;
			Region.bottom = 35;
			Region.left = 0;
			Region.right = 96;
			GraphicsRef->DrawSprite(220, 200, TheMenuCornerGraphics, Region);
			sprintf(buffer, "%d", (Page + 1));
			TempString = buffer;		
			GraphicsRef->DrawString(StandardFont, &TempString, 266, 214, TEXT_CENTRED);

			// ----
			// Anthony!!! Swap the bloody screens
			GraphicsRef->EndDraw();
			Redraw = false;
		}
	}

	TheGameWindow->Process();

	SAFELY_DELETE(TheKeyboardGraphic);
	NEW(TheKeyboardGraphic, CGraphicsData());
	SAFELY_CONTROL(if, TheKeyboardGraphic, Init(GraphicsRef, &CString(KEYBOARD_IMAGE), ThePlatform), ==, false)
		return;
}

void	CPopulous::DPadScroll()
{
	int incFlip = 1;
	if(GraphicsRef->GetOrientation() == EO_Landscape_Flipped)
		incFlip = -1;

	if (ControllerRef->IsButtonPressed(Up))
	{
		if (ControllerRef->IsButtonPressed(Left))
		{
			xoff -= incFlip;
			yoff += incFlip;
			goto end;
			//return NW;
		}

		if (ControllerRef->IsButtonPressed(Right))
		{
			xoff-=incFlip;
			yoff-=incFlip;
			goto end;
			//return NE;
		}

		xoff-=incFlip;
		goto end;
		//return N;
	}
	
	if (ControllerRef->IsButtonPressed(Down))
	{
		if (ControllerRef->IsButtonPressed(Left))
		{
			xoff+=incFlip;
			yoff+=incFlip;
			goto end;
			//return SW;
		}

		if (ControllerRef->IsButtonPressed(Right))
		{
			xoff+=incFlip;
			yoff-=incFlip;
			goto end;
			//return SE;
		}

		xoff+=incFlip;
		goto end;
		//return S;
	}

	if (ControllerRef->IsButtonPressed(Left))
	{
		yoff+=incFlip;
		goto end;
			//return W;
	}

	if (ControllerRef->IsButtonPressed(Right))
	{
		yoff-=incFlip;
		goto end;
			//return E;
	}

end:
	if(xoff<0)
		xoff=0;

	if(xoff>MAP_WIDTH-8)
		xoff=MAP_WIDTH-8;

	if(yoff<0)
		yoff=0;

	if(yoff>MAP_HEIGHT-8)
		yoff=MAP_HEIGHT-8;
	//return STILL;
}


// ----
void	CPopulous::waittof()
{
	if (Timer50ths)
	{
		Timer50ths->BlockUntilClock();
		Timer50ths->ClearNumberOfEvents();
	}
}

// ----
void	CPopulous::PointToPrimary()
{
	POINT	PrimaryFireLocale;
	POINT	Edge, Start; //End;
	//int		Swapper; //Align;

	POINT	Test = ControllerRef->AccessKeyList()->ptA;
	ConvertCoordSpace(	Test, 
						&PrimaryFireLocale,
						GraphicsRef->GetOrientation());

	Edge.x = max(0, min(319, PrimaryFireLocale.x));
	Edge.y = max(0, min(239, PrimaryFireLocale.y));

	if (PrimaryFireLocale.y <= 0)
	{
		// Top
		Region.left		= max(0, (Edge.x - 1));
		Region.top		= Edge.y;
		Region.right	= min(319, (Region.left + 2));
		Region.bottom	= Region.top + 60;
		GraphicsRef->DrawRect(&Region, 220, 220, 220);

		TempString		= "HOLD DOWN THIS BUTTON";
		Region.top		= Region.bottom;
		if (Region.left > (319 - StandardFont->GetStringWidth(&TempString)))
			Region.left = 319 - StandardFont->GetStringWidth(&TempString);
		Region.right	= Region.left + StandardFont->GetStringWidth(&TempString);
		Region.bottom	+= ((StandardFont->GetLineHeight() * 3) + 1);
		GraphicsRef->DrawRect(&Region, 220, 220, 220);

		Start.x = (Region.left + Region.right) >> 1;
		Start.y = Region.top;
		GraphicsRef->DrawString(StandardFont,
								&TempString,
								Start.x, 
								Start.y + StandardFont->GetLineHeight() * 0,
								TEXT_CENTRED);

		TempString =	"AND TAP THE SCREEN";
		GraphicsRef->DrawString(StandardFont,
								&TempString,
								Start.x, 
								Start.y + StandardFont->GetLineHeight() * 1,
								TEXT_CENTRED);

		TempString =	"FOR 'RIGHT' CLICK.";
		GraphicsRef->DrawString(StandardFont,
								&TempString,
								Start.x, 
								Start.y + StandardFont->GetLineHeight() * 2,
								TEXT_CENTRED);
	}
	else if (PrimaryFireLocale.y >= 239)
	{
		// Bottom
		Region.left		= max(0, (Edge.x - 1));
		Region.top		= Edge.y - 60;
		Region.right	= min(319, (Region.left + 2));
		Region.bottom	= Edge.y;
		GraphicsRef->DrawRect(&Region, 220, 220, 220);

		TempString		= "HOLD DOWN THIS BUTTON";
		Region.bottom	= Region.top;
		Region.top		-= (StandardFont->GetLineHeight() * 3) + 1;
		if (Region.left > (319 - StandardFont->GetStringWidth(&TempString)))
			Region.left = 319 - StandardFont->GetStringWidth(&TempString);
		Region.right	= Region.left + StandardFont->GetStringWidth(&TempString);
		GraphicsRef->DrawRect(&Region, 220, 220, 220);

		Start.x = (Region.left + Region.right) >> 1;
		Start.y = Region.top;
		GraphicsRef->DrawString(StandardFont,
								&TempString,
								Start.x, 
								Start.y + StandardFont->GetLineHeight() * 0,
								TEXT_CENTRED);

		TempString =	"AND TAP THE SCREEN";
		GraphicsRef->DrawString(StandardFont,
								&TempString,
								Start.x, 
								Start.y + StandardFont->GetLineHeight() * 1,
								TEXT_CENTRED);

		TempString =	"FOR 'RIGHT' CLICK.";
		GraphicsRef->DrawString(StandardFont,
								&TempString,
								Start.x, 
								Start.y + StandardFont->GetLineHeight() * 2,
								TEXT_CENTRED);
	}
	else if (PrimaryFireLocale.x <= 0)
	{
		// Left
		Region.left		= Edge.x;
		Region.top		= max(0, (Edge.y - 1));
		Region.right	= Region.left + 60;
		Region.bottom	= min(319, (Region.top + 2));
		GraphicsRef->DrawRect(&Region, 220, 220, 220);

		TempString		= "HOLD DOWN THIS BUTTON";
		if (Region.top > (239 - ((StandardFont->GetLineHeight() * 3) + 1)))
			Region.top = 239 - ((StandardFont->GetLineHeight() * 3) + 1);
		Region.bottom	= Region.top + ((StandardFont->GetLineHeight() * 3) + 1);
		Region.left		= Region.right;
		Region.right	= Region.left + StandardFont->GetStringWidth(&TempString);
		GraphicsRef->DrawRect(&Region, 220, 220, 220);

		Start.x = (Region.left + Region.right) >> 1;
		Start.y = Region.top;
		GraphicsRef->DrawString(StandardFont,
								&TempString,
								Start.x, 
								Start.y + StandardFont->GetLineHeight() * 0,
								TEXT_CENTRED);

		TempString =	"AND TAP THE SCREEN";
		GraphicsRef->DrawString(StandardFont,
								&TempString,
								Start.x, 
								Start.y + StandardFont->GetLineHeight() * 1,
								TEXT_CENTRED);

		TempString =	"FOR 'RIGHT' CLICK.";
		GraphicsRef->DrawString(StandardFont,
								&TempString,
								Start.x, 
								Start.y + StandardFont->GetLineHeight() * 2,
								TEXT_CENTRED);
	}
	else if (PrimaryFireLocale.x >= 319)
	{
		// Right
		Region.left		= Edge.x - 60;
		Region.top		= max(0, (Edge.y - 1));
		Region.right	= Edge.x;
		Region.bottom	= min(319, (Region.top + 2));
		GraphicsRef->DrawRect(&Region, 220, 220, 220);

		TempString		= "HOLD DOWN THIS BUTTON";
		if (Region.top > (239 - ((StandardFont->GetLineHeight() * 3) + 1)))
			Region.top = 239 - ((StandardFont->GetLineHeight() * 3) + 1);
		Region.bottom	= Region.top + ((StandardFont->GetLineHeight() * 3) + 1);
		Region.right	= Region.left;
		Region.left		-= StandardFont->GetStringWidth(&TempString);
		GraphicsRef->DrawRect(&Region, 220, 220, 220);

		Start.x = (Region.left + Region.right) >> 1;
		Start.y = Region.top;
		GraphicsRef->DrawString(StandardFont,
								&TempString,
								Start.x, 
								Start.y + StandardFont->GetLineHeight() * 0,
								TEXT_CENTRED);

		TempString =	"AND TAP THE SCREEN";
		GraphicsRef->DrawString(StandardFont,
								&TempString,
								Start.x, 
								Start.y + StandardFont->GetLineHeight() * 1,
								TEXT_CENTRED);

		TempString =	"FOR 'RIGHT' CLICK.";
		GraphicsRef->DrawString(StandardFont,
								&TempString,
								Start.x, 
								Start.y + StandardFont->GetLineHeight() * 2,
								TEXT_CENTRED);
	}
}


// ----
// Flashy little battery low logo.
void	CPopulous::CheckBattery()
{
	SYSTEM_POWER_STATUS_EX	status;

	if (GetSystemPowerStatusEx(&status, false))
	{
		if (	(	(status.BatteryLifeTime	!=	0xFFFFFFFF)
				&&	(status.BatteryLifeTime	<=	(60 * 30)))	// 30 minutes
			||	(	(status.BatteryLifePercent != 255)
				&&	(status.BatteryLifePercent <= 25))		// 25%
			&&	(	(status.BatteryFlag	!=	8)				// And it is not on charge
				||	(status.BatteryFlag	!= 255)))			// or the status is unknown.
		{
			Region.left		= 0;
			Region.top		= 0;
			Region.right	= 32 - 7;
			Region.bottom	= 12;
			GraphicsRef->DrawRect(&Region, 0, 0, 0);

			Region.left		= 1;
			Region.top		= 1;
			Region.right	= 31 - 7;
			Region.bottom	= 11;
			GraphicsRef->DrawRect(&Region, 255, 255, 255);

			Region.left		= 4;
			Region.top		= 2;
			Region.right	= 30 - 7;
			Region.bottom	= 10;
			GraphicsRef->DrawRect(&Region, 0, 0, 0);
			
			Region.left		= 0;
			Region.top		= 0;
			Region.right	= 3;
			Region.bottom	= 3;
			GraphicsRef->DrawRect(&Region, 0, 0, 0);

			Region.left		= 0;
			Region.top		= 9;
			Region.right	= 3;
			Region.bottom	= 12;
			GraphicsRef->DrawRect(&Region, 0, 0, 0);

			if (BatteryTimer->GetNumberOfEvents() > 2)
				BatteryTimer->ClearNumberOfEvents();

			if (BatteryTimer->GetNumberOfEvents() % 2)
			{
				Region.left		= 31 - 7 - 5;
				Region.top		= 3;
				Region.right	= 31 - 7;
				Region.bottom	= 9;
				GraphicsRef->DrawRect(&Region, 255, 255, 255);
			}

			/*
			sprintf(convert, "%d\"", status.BatteryLifePercent);
			TempString = convert;
			GraphicsRef->DrawString(StandardFont,
									&TempString,
									(31 - 5) >> 1,
									2,
									TEXT_CENTRED);
			*/
		}
	}
}