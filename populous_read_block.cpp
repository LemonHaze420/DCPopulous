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

// Original files.
#include	<stdio.h>
//#include	"bbc.h"
#include	"Computer.h"
#include	"God.h"
#include	"Extern.h"


void	CPopulous::read_back_scr()
{
	RECT	Region;

	TempString	= ThePlatform->GetPath(EP_ImageData);
	TempString	+= "qaz";

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

	// Draw the back ground screen
	Region.left		= 0;
	Region.top		= 0;
	Region.right	= 320;
	Region.bottom	= 200;
	GraphicsRef->DrawSprite(0, 0, BackGroundImage, Region);

	SAFELY_DELETE(BackGroundImage);
	NEW(BackGroundImage, CGraphicsData());
	TempString = "exit";
	BackGroundImage->Init(GraphicsRef, &TempString, ThePlatform);
	Region.left		= 0;
	Region.top		= 0;
	Region.right	= 30;
	Region.bottom	= 15;
	GraphicsRef->DrawSprite(290, 185, BackGroundImage, Region);


	//draw the keyboard at the bottom of the screen
	Region.left		= 0;
	Region.top		= 0;
	Region.right	= 320;
	Region.bottom	= 40;
	GraphicsRef->DrawSprite(0, 200, TheKeyboardGraphic, Region);


//	GraphicsRef->EndDraw();
	//fread((char *)back_scr,1,SCREEN_SIZE,fp);
	
	bbc_cls();
	//screen_pointer[2]=back_scr;
	//change_palette(palette,16); 
}


// ----
void 	CPopulous::read_sprites(SHORT number)
{
	FILE	*fp;
	//UBYTE	*fname;
	//fname="<Populous$Dir>.dat.sprites0";
	//fname[26]='0'+number;
	sprites_in=number;

	// Generate path.
	TempString = ThePlatform->GetPath(EP_ImageData);
	sprintf(convert, "sprites%d", number);
	TempString += convert;

	do
	{
		fp=FOPEN(TempString.GetString(),"rb");
		if(!fp) wait_for_disc();
	}
	while(!fp);
	SAFELY_DELETE(SpriteGraphics);
	NEW(SpriteGraphics, CGraphicsData());
	SpriteGraphics->InitAsPopulous(GraphicsRef, fp, SPRITE_DATA, 16);
//	fread((char *)sprite_data,1,SPRITE_DATA,fp);
	fclose(fp);
	bbc_cls();
	//make_arc_sprites_16((char *)sprite_data,SPRITE_DATA);

	TempString = ThePlatform->GetPath(EP_ImageData);
	sprintf(convert, "spr_32%d", number);
	TempString += convert;

	//fname="<Populous$Dir>.dat.spr_320";
	//fname[25]='0'+number;
	do
	{
		fp=FOPEN(TempString.GetString(),"rb");
		if(!fp) wait_for_disc();
	}
	while(!fp);

	// CE code.
	SAFELY_DELETE(Sprite32Graphics);
	NEW(Sprite32Graphics, CGraphicsData());
	change_palette(0);
	//Sprite32Graphics->InitAsPopulous(GraphicsRef, fp, SPRITE_DATA_32, 16, true);
	Sprite32Graphics->InitAsPopulous32(GraphicsRef, fp, SPRITE_DATA_32);
	//fread((char *)sprite_data_32,1,SPRITE_DATA_32,fp);
	fclose(fp);
	bbc_cls();

	change_palette(0);
	//make_arc_sprites_16((char *)sprite_data_32,SPRITE_DATA_32);
}

// ----
int 	CPopulous::load_ground(SHORT num, SHORT ok_cancel)
{
	FILE	*fp;
	//UBYTE	*fname="<Populous$Dir>.dat.land0";
	//fname[23]='0'+num;
	SHORT	c1,c2,c3;

	CString	TempString;
	TempString = ThePlatform->GetPath(EP_Data);
	sprintf(convert, "land%d", num);
	TempString += convert;

	do
	{
		fp=FOPEN(TempString.GetString(),"rb");
		if(!fp) wait_for_disc();
	}
	while(!fp);
	fread(&walk_death,1,2L,fp);
	fread(population_add,1,22L,fp);
	fread(mana_add,1,22L,fp);
	fread(weapons_add,1,22L,fp);
	fread(battle_add1,1,22L,fp);
	fread(battle_add2,1,6L,fp);
	fread(map_colour,1,16L,fp);
	fread(&c1,1,2L,fp);

	// CE code to load graphics.
	SAFELY_DELETE(BlockGraphics);
	NEW(BlockGraphics, CGraphicsData());
	SAFELY_CONTROL(if, BlockGraphics, InitAsPopulous(GraphicsRef, fp, BLK_DATA, 32), ==, false)
	{
		fclose(fp);
		return false;
	}

	//fread((char*)blk_data,1,BLK_DATA,fp);
	fclose(fp);
	bbc_cls();
	//make_arc_sprites_32((char *)blk_data,BLK_DATA);
	{
		int 	i;
		int 	t1;
		int 	t2;
		t1 = (walk_death << 8) & 0xff00;
		t2 = (walk_death >> 8) & 0x00ff;
		walk_death = t1 | t2;
		for(i=0;i!=11;i++)
		{
			t1 = (population_add[i] << 8) & 0xff00;
			t2 = (population_add[i] >> 8) & 0x00ff;
			population_add[i] = t1 | t2;
		}
		for(i=0;i!=11;i++)
		{
			t1 = (mana_add[i] << 8) & 0xff00;
			t2 = (mana_add[i] >> 8) & 0x00ff;
			mana_add[i] = t1 | t2;
		}
		for(i=0;i!=11;i++)
		{
			t1 = (weapons_add[i] << 8) & 0xff00;
			t2 = (weapons_add[i] >> 8) & 0x00ff;
			weapons_add[i] = t1 | t2;
		}
		for(i=0;i!=11;i++)
		{
			t1 = (battle_add1[i] << 8) & 0xff00;
			t2 = (battle_add1[i] >> 8) & 0x00ff;
			battle_add1[i] = t1 | t2;
		}
		for(i=0;i!=3;i++)
		{
			t1 = (battle_add2[i] << 8) & 0xff00;
			t2 = (battle_add2[i] >> 8) & 0x00ff;
			battle_add2[i] = t1 | t2;
		}
		t1 = (c1 << 8) & 0xff00;
		t2 = (c1 >> 8) & 0x00ff;
		c1 = t1 | t2;
	}
	
	if(c1 < 0 || c1 > 3)
		c1=0;
	
	if(c1 != sprites_in)
		read_sprites(c1);
	for(c1=0;c1<16;c1++)
		map_colour[c1+16]=map_colour[c1];
	for(c1=0;c1<11;c1++)
		weapons_order[c1]=weapons_add[c1];
	for(c1=0;c1<11;c1++)
		for(c2=0;c2<11;c2++)
			if(weapons_order[c1] < weapons_order[c2])
			{
				c3=weapons_order[c1];
				weapons_order[c1]=weapons_order[c2];
				weapons_order[c2]=c3;
			}
			return(1);
}

// ----
int 	CPopulous::read_lord()
{
	FILE	*fp;
	
	CString	TempString;
	TempString = ThePlatform->GetPath(EP_ImageData);
	TempString += "lord";

	SAFELY_DELETE(BackGroundImage);
	NEW(BackGroundImage, CGraphicsData);
	do
	{
		fp=FOPEN(TempString.GetString(),"rb");
		if(!fp) wait_for_disc();
	}
	while(!fp);
	
	BackGroundImage->InitAsPopulousScreen(GraphicsRef, fp);
	//fread((char *)w_screen,1,SCREEN_SIZE,fp);
	fclose(fp);
	
	Region.left		= 0;
	Region.top		= 0;
	Region.right	= 320;
	Region.bottom	= 200;
	GraphicsRef->DrawSprite(0, 0, BackGroundImage, Region);

	bbc_cls();
	return(1);
}


// ----
void 	CPopulous::read_mouth()
{
	FILE	*fp;

	TempString = ThePlatform->GetPath(EP_ImageData);
	TempString += "mouths";

	SAFELY_DELETE(MouthGraphics);
	NEW(MouthGraphics, CGraphicsData);
	do
	{
		fp=FOPEN(TempString.GetString(),"rb");
		if(!fp) wait_for_disc();
	}
	while(!fp);
	MouthGraphics->InitAsPopulousScreen(GraphicsRef, fp, 5600, 5600);	// This is now a hack.

	//fread((char *)map_steps,1,5600L,fp);
	fclose(fp);
	bbc_cls();
}
