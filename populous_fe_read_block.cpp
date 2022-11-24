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


#include	<stdio.h>
#include	<string.h>
#include	"Computer.h"
#include	"God.h"
#include	"Extern.h"

SHORT	CPopulous::read_back_scr(UBYTE	*fname)
{
	TempString	= ThePlatform->GetPath(EP_ImageData);
	TempString	+= (char*)fname;

	FILE	*fp;
	do
	{
		fp=FOPEN(TempString.GetString(),"rb");
		if(!fp) wait_for_disc();
	}
	while(!fp);
	SAFELY_DELETE(BackGroundImage);
	NEW(BackGroundImage, CGraphicsData());
	change_palette(0);
	BackGroundImage->InitAsPopulousScreen(GraphicsRef, fp);
	fclose(fp);

	// Now render it to the screen
//	GraphicsRef->StartDraw();

	GraphicsRef->SetActiveScreen(back_scr);
	GraphicsRef->ClearScreen(0, 0, 0);
	Region.left		= 0;
	Region.top		= 0;
	Region.right	= 320;
	Region.bottom	= 200;
	GraphicsRef->DrawSprite(0, 0, BackGroundImage, Region);

	/*
	//draw the keyboard at the bottom of the screen
	Region.left		= 0;
	Region.top		= 0;
	Region.right	= 320;
	Region.bottom	= 40;
	GraphicsRef->DrawSprite(0, 200, TheKeyboardGraphic, Region);
	*/

//	GraphicsRef->EndDraw();
	//fread((char *)back_scr,1,SCREEN_SIZE,fp);
	
	bbc_cls();
	//screen_pointer[2]=back_scr;
	//change_palette(palette,16); 

	return 0;
}


void	CPopulous::load_graphics()
{
	TempString = ThePlatform->GetPath(EP_Data);
	TempString += "land0";
	char	dummy[256];

	FILE	*fp;
	do
	{
		fp=FOPEN(TempString.GetString(),"rb");
		if(!fp) wait_for_disc();
	}
	while(!fp);
	fread((char *)dummy,1,114L,fp);	// Ditch first 114 bytes. (Jump data about blocks).

	// CE code to load graphics.
	SAFELY_DELETE(BlockGraphics);
	NEW(BlockGraphics, CGraphicsData());
	SAFELY_CONTROL(if, BlockGraphics, InitAsPopulous(GraphicsRef, fp, BLK_DATA, 32), ==, false)
	{
		fclose(fp);
		return;
	}
	//fread((char *)blk_data,1,33600L,fp);
	fclose(fp);
	bbc_cls();
	//make_arc_sprites_32((char *)blk_data,33600);

	TempString = ThePlatform->GetPath(EP_ImageData);
	TempString += "sprites0";
	do
	{
		fp=FOPEN(TempString.GetString(),"rb");
		if(!fp) wait_for_disc();
	}
	while(!fp);
	SAFELY_DELETE(SpriteGraphics);
	NEW(SpriteGraphics, CGraphicsData());
	SpriteGraphics->InitAsPopulous(GraphicsRef, fp, SPRITE_DATA, 16);
	//fread((char *)sprite_data,1,SPRITE_DATA_32,fp);
	fclose(fp);
	bbc_cls();
	//make_arc_sprites_16((char *)sprite_data,SPRITE_DATA);

	/* Code moved to Init
	do
	{
		fp=FOPEN(TempString.GetString(),"rb");
		if(!fp) wait_for_disc();
	}
	while(!fp);
	fread(font,1,4400L,fp);
	fclose(fp);
	bbc_cls();
	make_arc_sprites_8((char *)font,4400);
	*/
}


void	CPopulous::load_screen(UBYTE	*fname)
{
//	TempString = ThePlatform->GetPath(EP_ImageData);
//	TempString += fname;

//	FILE	*fp;
//	do
//	{
//		fp=FOPEN(TempString.GetString(),"rb");
//		if(!fp) wait_for_disc();
//	}
//	while(!fp);
	//fread((char *)o_screen,1,64000L,fp);

	SAFELY_DELETE(BackGroundImage);
	NEW(BackGroundImage, CGraphicsData());
	change_palette(0);
	TempString = (char*)fname;
	BackGroundImage->Init(GraphicsRef, &TempString, ThePlatform); //AsPopulousScreen(GraphicsRef, fp, 64000, 320, 256);

//	GraphicsRef->StartDraw();
	GraphicsRef->SetActiveScreen(o_screen);
	GraphicsRef->ClearScreen(0, 0, 0);
	Region.left		= 0;
	Region.top		= 0;
	Region.right	= 320;
	Region.bottom	= 240;
	GraphicsRef->DrawSprite(0, 0, BackGroundImage, Region);

	TheGameWindow->Process();
	TheSoundSystem->Process();
	Sleep(30);
	TheGameWindow->Process();
	GraphicsRef->DrawSprite(0, 0, BackGroundImage, Region);
	/*
	// ----
	// Now for the second part of the file.
	SAFELY_DELETE(BackGroundImage);
	NEW(BackGroundImage, CGraphicsData());
	BackGroundImage->InitAsPopulousScreen(GraphicsRef, fp);
	fclose(fp);
	
	GraphicsRef->SetActiveScreen(b_screen);
	GraphicsRef->ClearScreen(0, 0, 0);
	Region.left		= 0;
	Region.top		= 0;
	Region.right	= 320;
	Region.bottom	= 200;
	GraphicsRef->DrawSprite(0, 0, BackGroundImage, Region);
	*/
//	GraphicsRef->EndDraw();

	bbc_cls();
}
