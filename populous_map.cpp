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

void	CPopulous::setup_display(SHORT blank, SHORT landscape)
{
	//	SHORT	c1;
	
	if(in_conquest != -1)
	{
		start_seed=seed=conquest.seed+(in_conquest&7);
		if(ground_in != conquest.land); // are: Is this a bug? (Empty control statement).
		{
			ground_in=conquest.land;
			load_ground(ground_in,0);
		}
		clear_map();
		stats[1].computer_skill=conquest.skill;
		stats[1].computer_speed=conquest.speed;
		stats[1].computer_mode=(conquest.his_mode<<3)|0x07;
		stats[0].computer_mode=(conquest.my_mode<<3)|0x07;
		game_mode=conquest.game_mode;
	}
	else
	{
		start_seed=seed;
		if(seed)
		{
			if(newrand() & 1)
			{
				if(landscape == -1)
				{
					ground_in++;
					if(ground_in > 3)
						ground_in=0;
					load_ground(ground_in,0);
				}
			}
		}
		if(landscape != ground_in && landscape != -1)
		{
			load_ground(ground_in,0);
		}
		clear_map();
	}
	if(blank==0)
	{
		make_alt();
		make_map(0,0,63,63);
		make_woods_rocks();
	}
	Setscreen(back_scr,back_scr,-1); /* logical screen = background */
	copy_screen(back_scr,w_screen);
	copy_screen(back_scr,d_screen);
	draw_map(0,0,63,63); /* so you can see it drawing the map */
	Setscreen(w_screen,-1L,-1); 	/* logical screen = work_screen */
	place_first_people();
	
	seed++;
}

// ----
// Main game loop - as far as we've determined.
SHORT		CPopulous::animate()
{
	register SHORT	 c1;
	register struct s_sprite   *spr;
//	SHORT	toggle2=0;
//	SHORT	x;
//	SHORT	y;
//	SHORT	alt;
//	SHORT	block;
//	SHORT	fd;
//	SHORT	bb=0;
//	UBYTE	*aptr;
//	UBYTE tmp[100];
	struct	p_peeps *peep;
	
	xoff = 0;
	yoff = 0;
	setup_display(0,1);
	toggle_icon(back_scr,0,3,BANK2);
	toggle_icon(back_scr,0,4,BANK2);
	toggle_icon(back_scr,6,1,BANK1);
	toggle_icon(back_scr,4,3,BANK1);
	if(start_of_game == 2)
	{
		start_of_game=0;
		clr_wsc();
		swap_screens();
		while((in_conquest=show_world()) == -1);
		return_to_game();
		setup_display(0,-1);
	}

	TheGameWindow->Process();

	start_of_game=0;
	int CaptionCount = 0;

	while(1)
	{
#ifdef TEST_IN_EMULATION
		stats[0].type = COMPUTER_PLAYER;
		stats[1].type = COMPUTER_PLAYER;
#endif

		int 	a1=121;
		int 	x1=0x84;
		int 	y1;
		int 	a2=121;
		int 	x2=0xf0;
		int 	y2;
		os_byte(a1,&x1,&y1);
		os_byte(a2,&x2,&y2);
		if(x1==0xff && x2==0xff)
			break;
		
		// Limit frame rate.
		GameFrameLimiter->BlockUntilClock();
		GameFrameLimiter->ClearNumberOfEvents();

		if(magnet[player].population)
		{
			tempo=50-(magnet[!player].population/magnet[player].population)*2;
			if(tempo <7)
				tempo = 7;
			beat_two=tempo/3;
		}
		else
		{
			tempo=0;
			beat_two=1;
		}
		if(tempo_now > tempo)
			tempo_now=tempo;
//*****************************************************************************
		//heart beat effect..... this used to be done by the MOUSE!!!!
/*
		tempo_now++;
		if(tempo_now >= tempo)
		{
			TheSoundSystem->PlaySample(SampleArray[TUNE_HEART1]); 
			tempo_now = 0;
		}
		else
		{
			if( (beat_two - tempo) >= tempo_now )
			{
				TheSoundSystem->PlaySample(SampleArray[TUNE_HEART2]);
			}
		}
*/
//*****************************************************************************
		toggle = !toggle;
		clr_wsc();
		no_sprites=0;
		ok_to_build=0;
		sound_effect=0;
		if(mode == SCULPT)
			make_arc_mouse(pointer=player*3+1);
		move_mana();

		/*
		check the controller for a scroll
		*/
		DPadScroll();

		draw_it(xoff,yoff);
		
		DEBUG_CHECKPEEPS;

		// Update our systems. breaking the game if something fails.
		if (!UpdatePopulousData())
			break;


		for(c1=0,spr=&sprite[0];c1<no_sprites;c1++,spr++)
		{
			if(spr->who >= FUNNY)
			{
				draw_s_32(w_screen,spr->x-8,spr->y-16,spr->frame);
			}
			else	
				draw_sprite(w_screen,spr->x,spr->y,spr->frame);
		}
		draw_sprite(w_screen,64+(xoff+3)-(yoff+3)-3,((xoff+3)+(yoff+3))/2-3,CROSSHAIR_SPRITE);
		if(pause == 0 && paint_map == 0)
		{
			do_funny();
			move_peeps();
		}
		else
		{
			for(c1=0,peep=&peeps[0];c1<no_peeps;c1++,peep++)
			{
				if(peep->population)
				{
					if(peep->player == player || serial_off)
					{
						if(peep->flags & IN_TOWN)
							a_putpixel(peep->at_pos,stats[peep->player].house_color);
						if(peep->flags & ON_MOVE)
							a_putpixel(peep->at_pos,8+(peep->player == GOD)*7);
					}
				}
			}
		}
		if(view_timer)
		{
			view_timer--;
			if(left_button==0 || right_button==0 || view_timer == 0)
			{
				view_who=old_view_who;
			}
		}
		if(game_turn == 0x1000) // 0x1000
		{
		//	do_place_funny(start_seed&3);
		}
		show_the_shield();

		if (CaptionCount < (GAME_FRAME_RATE_LIMIT * 5)) // The frame rate will be limiting the game.
		{
			CaptionCount++;
			PointToPrimary();
		}

		// Flashy little battery low logo.
		CheckBattery();

		swap_screens();
		keyboard();
		if(inkey)
		{
			switch(inkey)
			{
			case	122:
				if(xoff)
					xoff--;
				break;
			case	27:
				if(xoff)
					xoff--;
				if(yoff)
					yoff--;
				break;				  
			case	42:
				if(yoff)
					yoff--;
				break;
			case	107:
				if(xoff)
					xoff--;
				if(yoff < MAP_HEIGHT-8)
					yoff++;
				break;
			case	43:
				if(xoff < MAP_WIDTH-8)
					xoff++;
				if(yoff)
					yoff--;
				break;
			case	124:
				if(yoff < MAP_HEIGHT-8)
					yoff++;
				break;
			case	108:
				if(xoff < MAP_WIDTH-8)
					xoff++;
				if(yoff < MAP_WIDTH-8)
					yoff++;
				break;
			case	26:
				if(xoff < MAP_WIDTH-8)
					xoff++;
				break;
			default:
				paint_the_map(stats[player].send);
				break;
			}
		}
		if(mode & (SCULPT | PUT_MAGNET | PUT_SWAMP))
			sculpt();
		else if(!left_button || !right_button)
			interogate();
		if(!left_button || !right_button)
		{
			zoom_map(!left_button);
		}

		left_button=(left_button == 2) + 1;
		right_button=(right_button == 2) + 1;
		get_message(0);
	}
	
	// are: What is the return value?
	return true;	// are: Assuming true
}


/**************************************************************************
*		Clear altitudes, map_alts, map_blks & map_bk2s					  *
**************************************************************************/
void CPopulous::clear_map()
{
	register SHORT	 c1;//,c2;
	if(!start_of_game)
	{
		set_mode_icons(6,1);
		set_tend_icons(4,3);
	}
	for(c1=0;c1 != END_OF_FUNNYS;c1++)
	{
		peeps[c1].population=0;
	}
	no_peeps=0; 		   
	make_arc_mouse(pointer=(player == DEVIL)*3+1);
	funny_done=0;
	score=0;
	for(c1=0;c1!=2;c1++)
	{
		magnet[c1].flags=SETTLE_MODE;
		magnet[c1].mana=399;
		god_magnet=devil_magnet=magnet[c1].go_to=MAP_WIDTH/2+MAP_WIDTH*(MAP_HEIGHT/2);
		magnet[c1].carried=0;
		battle_won[c1]=0;
		stats[c1].no_quakes=newrand()%3;
		stats[c1].no_swamps=stats[c1].no_quakes+1+newrand()%5;
		stats[c1].quake_count=0;	
		stats[c1].arrived=0;
	}
	clear_send();
	if(start_of_game==3)
		magnet[0].mana=mana_values[MANA_VOLCANO];
	if(in_conquest != -1 && level_number > 1235)
		magnet[1].mana=mana_values[MANA_QUAKE]+200;
	game_turn=0;
	mode = SCULPT;
	surender=-1;
	war=0;
	left_button=right_button=2;
	inkey=0;
	for(c1=0;c1<(MAP_WIDTH+1)*(MAP_HEIGHT+1);c1++)
		alt[c1] = 0;
	for(c1=0;c1<MAP_WIDTH*MAP_HEIGHT;c1++)
	{
		map_blk[c1]=0;
		map_alt[c1]=0;
		map_bk2[c1]=0;
		map_steps[c1]=0;
		map_who[c1]=0;
	}
}

SHORT CPopulous::raise_point(SHORT x, SHORT y)
{
	register SHORT	 *alt_xy,*alt_t;
	register SHORT	 tx,ty;
	
	tx = x;
	ty = y;
	if(tx > MAP_WIDTH || tx < 0 || ty > MAP_HEIGHT || ty < 0)
		return(0);
	alt_xy=&alt[tx+END_WIDTH*ty];
	if(*alt_xy < 8)
	{
		build_count++;
		(*alt_xy)++;
		alt_t=alt_xy+1;
		tx++;
		if(*alt_xy-*alt_t > 1)
			raise_point(tx,ty);
		alt_t+=END_WIDTH;
		ty++;
		if(*alt_xy-*alt_t > 1)
			raise_point(tx,ty);
		alt_t--;
		tx--;
		if(*alt_xy-*alt_t > 1)
			raise_point(tx,ty);
		alt_t--;		
		tx--;
		if(*alt_xy-*alt_t > 1)
			raise_point(tx,ty);
		alt_t-=END_WIDTH;
		ty--;
		if(*alt_xy-*alt_t > 1)
			raise_point(tx,ty);
		alt_t-=END_WIDTH;
		ty--;
		if(*alt_xy-*alt_t > 1)
			raise_point(tx,ty);
		alt_t++;
		tx++;
		if(*alt_xy-*alt_t > 1)
			raise_point(tx,ty);
		alt_t++;
		tx++;
		if(*alt_xy-*alt_t > 1)
			raise_point(tx,ty);
		if(x < xmin)
			xmin=x;
		if(x > xmax)
			xmax=x;
		if(y < ymin)
			ymin=y;
		if(y > ymax)
			ymax=y;
	}
	return(*alt_xy);
}

void CPopulous::make_alt()
{
	make_thing(2,4);
	make_thing(4,2);
	make_thing(3,3);
}

void CPopulous::make_thing(SHORT g_x, SHORT g_y)
{
	register SHORT	x,y;
	x=newrand()%MAP_WIDTH;
	y=newrand()%MAP_WIDTH;
	while(raise_point(x,y) != 6)
	{
		x += -g_x+newrand()%(g_x*2+1);
		y += -g_y+newrand()%(g_y*2+1);
		if(x < 0)
			x=0;
		if(x > MAP_WIDTH)
			x=MAP_WIDTH;
		if(y < 0)
			y=0;
		if(y >MAP_HEIGHT)
			y=MAP_HEIGHT;
	}
}

void CPopulous::make_map(SHORT x1, SHORT y1, SHORT x2, SHORT y2)
{
	register SHORT	 x,y,block,a,c1;
	SHORT	*alt_xy;
	for(x=x1;x<=x2;x++)
		for(y=y1;y<=y2;y++)
		{
			block=0;
			c1=x+y*MAP_WIDTH;
			alt_xy=&alt[x+END_WIDTH*y];
			a=(*alt_xy+*(alt_xy+1)+*(alt_xy+END_WIDTH)+*(alt_xy+END_WIDTH+1))>>2;
			if(*alt_xy > a)
				block++;
			if(*(alt_xy+1) > a)
				block+=2;
			if(*(alt_xy+END_WIDTH+1) > a)
				block+=4;
			if(*(alt_xy+END_WIDTH) > a)
				block +=8;
			if(map_blk[c1] != ROCK_BLOCK || (block == 0 && a == 0))
				map_blk[c1]=block;
			else
				block=map_blk[c1];
			if(a != 0 && block == 0)
			{
				a-=1;
				block=15;				 
			}
			if(a == 0 && block != FLAT_BLOCK && block != WATER_BLOCK)
				block+=16; 
			map_alt[c1] = a;
			if(map_blk[c1] != ROCK_BLOCK)
				map_blk[c1]=block;			  
			else
				block=ROCK_BLOCK;
			if(block == 0)
				map_bk2[c1]=0;
			map_steps[c1]=0;
		}
}

void CPopulous::draw_map(SHORT x1, SHORT y1, SHORT x2, SHORT y2)
{
	register SHORT	x,y;
	SHORT	c1;
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)
		{
			if((c1=map_colour[map_blk[x+MAP_WIDTH*y]]) == 25)
				c1=map_colour[31];
			pixel(back_scr,64+x-y,(x+y)>>1,c1); 
		}
	}
}

void CPopulous::mod_map(int x1, int y1, int x2, int y2)
{
	register SHORT	 x,y,c1,a,block;
	for(x=x1;x<=x2;x++)
		for(y=y1;y<=y2;y++)
		{
			c1=x+y*MAP_WIDTH;
			a=map_alt[c1];
			block=map_blk[c1];
			if(a != 0 && block == WATER_BLOCK)
			{
				a-=1;
				block=FLAT_BLOCK;				 
			}
			/* if we are a zero alt and we are on a slope convert to a costal block */			  
			if(a == 0 && block != FLAT_BLOCK && block != WATER_BLOCK)
				block+=16; 
			map_alt[c1]=a;
			if(map_blk[c1] != ROCK_BLOCK)
				map_blk[c1]=block;			  
			map_steps[c1]=0;
		}
}

void CPopulous::zoom_map(SHORT lr_button)
//SHORT   lr_button;		  /* 0 = left button pressed, 1 = right button pressed */
{
	SHORT	x,y;
	SHORT	c1;
	SHORT	c2;
//	SHORT	c3;
	UBYTE	*send;
	x=mousey+(mousex>>1)-32;
	y=mousey-(mousex>>1)+32;
	if(x>=0 && x<MAP_WIDTH && y>=0 && y<MAP_HEIGHT) /* Click on map */
	{
		x-=3;
		y-=3;
		if(x<0)
			x=0;
		if(x>MAP_WIDTH-8)
			x=MAP_WIDTH-8;
		if(y<0)
			y=0;
		if(y>MAP_HEIGHT-8)
			y=MAP_HEIGHT-8;
		xoff=x;
		yoff=y;
		goto the_end;
	}
	send=stats[player].send;
	if(x>274)
	{
		x=(x-272)/16;
		y=(y-32)/16;
		if(x == 0)
		{
			if(y == 0)
			{
				toggle_icon(d_screen,x,y,BANK2);
				send[S_EFFECT]=S_ACTION;
				send[S_Y]=SA_COMPUTER_OPTIONS;
				send[S_X]=lr_button;
			}
			else if(y == 3)
			{
				toggle_icon(back_scr,x,y,BANK2);
				music_off=!music_off;
				ThePreferences->MusicEnabled = !ThePreferences->MusicEnabled;
				ThePreferences->SavePreferences();
				if(music_off)
					kill_effect(0,4);
			}
			else if(y == 4)
			{
				toggle_icon(back_scr,x,y,BANK2);
				effect_off=!effect_off;
				ThePreferences->EffectsEnabled = !ThePreferences->EffectsEnabled;
				
				if (!ThePreferences->EffectsEnabled)
					TheSoundSystem->StopAllSamples();
				
				ThePreferences->SavePreferences();
				if(effect_off)
					kill_effect(0,4);
			}
		}
		else if(x == 1)
		{
			if(y == 1)
			{
				toggle_icon(d_screen,x,y,BANK2);
				send[S_Y]=SA_GAME_OPTIONS;
				send[S_EFFECT]=S_ACTION;
			}
			else if(y == 3)
			{
				send[S_EFFECT]=S_ACTION;
				send[S_Y]=SA_PAUSE;
			}
		}
		else if(x == 2)
		{
			if(y == 2)
			{
				// This code has been reactivated for an escape button.
				toggle_icon(d_screen,x,y,BANK2);
				send[S_Y]=SA_QUIT;
				send[S_EFFECT]=S_ACTION;
			}
		}
		goto the_end;
	}
	if(y>=146)					/* click on Control icons */
	{
		y =(y-144)/16;
		x =(x-96)/16;
		if(x <= 5 && x >= 3 && y>=0 && y<=2) /* direction icon */
		{
			toggle_icon(d_screen,x,y,BANK1);
			if(x == 4 && y == 1)
			{
				if(view_who)
				{
					xoff=GET_X(peeps[view_who-1].at_pos)-3;
					yoff=GET_Y(peeps[view_who-1].at_pos)-3;
				}
			}
			else
			{
				xoff = xoff + (x==5) - (x==3);
				yoff = yoff + (y==2) - (y==0);
				if(xoff<0)
					xoff=0;
				if(xoff>MAP_WIDTH-8)
					xoff=MAP_WIDTH-8;
				if(yoff<0)
					yoff=0;
				if(yoff>MAP_HEIGHT-8)
					yoff=MAP_HEIGHT-8;
			}
			goto the_end;
		}
		switch(x)
		{
		case	0:
			if(y == 0)			/* do a flood */
			{
				toggle_icon(d_screen,x,y,BANK1);
				send[S_EFFECT]=S_ACTION;
				send[S_Y]=SA_MANA_FLOOD;
			}
			break;
		case	1:
			if(y == 0)
			{
				if(paint_map == 0)
				{
					toggle_icon(d_screen,x,y,BANK1);
					send[S_EFFECT]=S_ACTION;
					send[S_Y]=SA_MANA_WAR;
				}
			}
			if(y == 1)	   /* do volcano */
			{
				toggle_icon(d_screen,x,y,BANK1);
				send[S_EFFECT]=MANA_VOLCANO;
				send[S_X]=xoff;
				send[S_Y]=yoff;
			}
			break;
		case	2:
			if(y == 0)			/* do a quake */
			{
				toggle_icon(d_screen,x,y,BANK1);
				send[S_EFFECT]=MANA_QUAKE;
				send[S_X]=xoff;
				send[S_Y]=yoff;
			}
			else if(y == 1) 	/* do a knight */
			{
				toggle_icon(d_screen,x,y,BANK1);
				send[S_EFFECT]=S_ACTION;
				send[S_Y]=SA_MANA_KNIGHT;				 
			}
			else if(y == 2) 	/* set a swamp */
			{
				toggle_icon(d_screen,x,y,BANK1);
				if((magnet[player].mana>mana_values[MANA_SWAMP] && (stats[player].computer_mode & C_SWAMP))|| paint_map)
				{
					set_mode_icons(x,y);
					mode=(mode & 3)|PUT_SWAMP;
					make_arc_mouse(pointer=5);
				}
			}
			break;
		case	3:
			if(y == 3)			/* go to paple magnet mode */
			{
				send[S_EFFECT]=S_ACTION;
				send[S_Y]=SA_MODE;
				send[S_X]=MAGNET_MODE;
			}
			break;
		case	4:
			if(y == 3)	   /* go to settle mode */	  
			{
				send[S_EFFECT]=S_ACTION;
				send[S_Y]=SA_MODE;
				send[S_X]=SETTLE_MODE;
			}
			else if(y == 4) 	/* go to fight mode */
			{
				send[S_EFFECT]=S_ACTION;
				send[S_Y]=SA_MODE;
				send[S_X]=FIGHT_MODE;
			}
			break;
		case	5:
			if(y == 3)			/* go to join mode */
			{
				send[S_EFFECT]=S_ACTION;
				send[S_Y]=SA_MODE;
				send[S_X]=JOIN_MODE;
			}
			break;
		case	6:
			if(y == 0 || y == 1)	  /* put blocks and sheld mode*/
			{
				set_mode_icons(x,y);
				mode=y+1;
				if(y == 1)
					make_arc_mouse(pointer=(player == DEVIL)*3+1);
				else
					make_arc_mouse(pointer=y);
			}
			else if(y == 2) 	   
			{
				set_mode_icons(x,y);
				mode=(mode & 3)|PUT_MAGNET;
				make_arc_mouse(pointer=player+2);
			}
			break;
		case	7:
			if(y == 0)		/* zoom to paple magnet */
			{
				toggle_icon(d_screen,x,y,BANK1);
				if(lr_button == 0)
				{
					if(magnet[player].go_to)
					{
						xoff=GET_X(magnet[player].go_to)-3;
						yoff=GET_Y(magnet[player].go_to)-3;
					}
				}
				else
				{
					if(magnet[player].carried)
					{
						xoff=GET_X(peeps[magnet[player].carried-1].at_pos)-3;
						yoff=GET_Y(peeps[magnet[player].carried-1].at_pos)-3;
						set_temp_view(magnet[player].carried);
					}
					else
					{
						xoff=GET_X(magnet[player].go_to)-3;
						yoff=GET_Y(magnet[player].go_to)-3;
					}
				}
			}
			else if(y == 1) 		/* zoom to fight */
			{
				toggle_icon(d_screen,x,y,BANK1);
				if(no_peeps)
				{
					if(view_fight >= no_peeps)
						view_fight=0;
					for(c1=view_fight+1,c2=0;;c1++)
					{
						if(c1 >= no_peeps)
							c1=0;
						if(c1 == view_fight+1 && c2)
							break;
						c2=1;
						if((peeps[c1].flags & IN_BATTLE) && peeps[c1].population)
						{
							xoff=GET_X(peeps[c1].at_pos)-3;
							yoff=GET_Y(peeps[c1].at_pos)-3;
							view_fight=c1;
							set_temp_view(c1+1);
							break;
						}
					}
				}
			}
			break;
		case	8:
			if(y == 0)		   /* zoom to village */
			{
				toggle_icon(d_screen,x,y,BANK1);
				if(lr_button == 0)
				{
				}
				if(no_peeps)
				{
					if(view_people >= no_peeps)
						view_people=0;
					for(c1=view_people+1,c2=0;;c1++)
					{
						if(c1 == view_people+1 && c2)
							break;
						if(c1 == no_peeps)
							c1=0;
						c2=1;
						if(peeps[c1].player == player && 
							peeps[c1].population > 0)
						{
							if(lr_button == 0 && peeps[c1].flags == IN_TOWN)
							{
								xoff=GET_X(peeps[c1].at_pos)-3;
								yoff=GET_Y(peeps[c1].at_pos)-3;
								view_people=c1;
								set_temp_view(c1+1);
								break;
							}
							else if(lr_button && peeps[c1].head_for)
							{
								xoff=GET_X(peeps[c1].at_pos)-3;
								yoff=GET_Y(peeps[c1].at_pos)-3;
								view_people=c1;
								set_temp_view(c1+1);
								break;
							}
						}
					}
				}
			}
			break;
		default:
			break;
		}
	}
the_end:;
		if(xoff<0)
			xoff=0;
		if(xoff>MAP_WIDTH-8)
			xoff=MAP_WIDTH-8;
		if(yoff<0)
			yoff=0;
		if(yoff>MAP_HEIGHT-8)
			yoff=MAP_HEIGHT-8;
		if(!left_button) left_button=2;
		if(!right_button) right_button=2;
}

void CPopulous::set_tend_icons(SHORT x, SHORT y)
{
	if(magnet[player].flags == MAGNET_MODE)
		toggle_icon(back_scr,3,3,BANK1);
	else if(magnet[player].flags == SETTLE_MODE)
		toggle_icon(back_scr,4,3,BANK1);
	else if(magnet[player].flags == JOIN_MODE)
		toggle_icon(back_scr,5,3,BANK1);
	else if(magnet[player].flags == FIGHT_MODE)
		toggle_icon(back_scr,4,4,BANK1);
	if(x>=0)
		toggle_icon(back_scr,x,y,BANK1);
}

void CPopulous::set_mode_icons(SHORT x, SHORT y)
{
	if(mode & PUT_MAGNET)
		toggle_icon(back_scr,6,2,BANK1);
	else if(mode & PUT_SWAMP)
		toggle_icon(back_scr,2,2,BANK1);
	else
		toggle_icon(back_scr,6,mode-1,BANK1);
	if(x>=0)
		toggle_icon(back_scr,x,y,BANK1);
}

SHORT CPopulous::lower_point(SHORT x, SHORT y)
{
	register SHORT	 *alt_xy,*alt_t;
	register SHORT	 tx,ty;
	
	tx = x;
	ty = y;
	if(tx > MAP_WIDTH || tx < 0 || ty > MAP_HEIGHT || ty < 0)
		return(0);
	alt_xy=&alt[tx+END_WIDTH*ty];
	if(*alt_xy)
	{
		build_count++;
		(*alt_xy)--;
		alt_t=alt_xy+1;
		tx++;
		if(*alt_t-*alt_xy > 1)
			lower_point(tx,ty);
		alt_t+=END_WIDTH;
		ty++;
		if(*alt_t-*alt_xy > 1)
			lower_point(tx,ty);
		alt_t--;
		tx--;
		if(*alt_t-*alt_xy > 1)
			lower_point(tx,ty);
		alt_t--;		
		tx--;
		if(*alt_t-*alt_xy > 1)
			lower_point(tx,ty);
		alt_t-=END_WIDTH;
		ty--;
		if(*alt_t-*alt_xy > 1)
			lower_point(tx,ty);
		alt_t-=END_WIDTH;
		ty--;
		if(*alt_t-*alt_xy > 1)
			lower_point(tx,ty);
		alt_t++;
		tx++;
		if(*alt_t-*alt_xy > 1)
			lower_point(tx,ty);
		alt_t++;
		tx++;
		if(*alt_t-*alt_xy > 1)
			lower_point(tx,ty);
		if(x < xmin)
			xmin=x;
		if(x > xmax)
			xmax=x;
		if(y < ymin)
			ymin=y;
		if(y > ymax)
			ymax=y;
	}
	return(*alt_xy);
}

void CPopulous::interogate()
{
	register struct  s_sprite	 *spr;
	register SHORT	 c1;
	
	for(c1=0,spr=sprite;c1<no_sprites;c1++,spr++)
		if(mousex >= spr->x && mousex <= spr->x + 12)
			if(mousey >= spr->y && mousey <= spr->y + 8)
			{
				if(mode == INTEROGATE)
				{
					if(right_button == 0)
						set_temp_view(spr->who);
					else 
					{
						view_timer=0;
						view_who=spr->who;
					}
				}
				break;
			}
}

void CPopulous::show_the_shield()
{
	
	register SHORT		 c1;
	SHORT		c2;
	USHORT		c3;
	USHORT		 life;
	struct	p_peeps *g_battle;
	struct	p_peeps *d_battle;
	register struct  p_peeps *peep;
	if(view_who)
	{
		peep=&peeps[view_who-1];
		if(peep->population <= 0)
		{
			view_who=0;
			return;
		}
		draw_icon(w_screen,17,4,peep->player);
		for(c1=1;weapons_order[c1]!=peep->weapons && c1<11;c1++);
		if(c1<11)
			draw_icon(w_screen,18,4,c1+1);
		if(peep->flags & IN_BATTLE)
		{
			draw_sprite(w_screen,272,22,peep->frame);
			if(peep->player == GOD)
			{
				g_battle=peep;
				d_battle=&peeps[peep->battle_population];
			}
			else
			{
				g_battle=&peeps[peep->battle_population];
				d_battle=peep;
			}
			c1=(LONG)((LONG)g_battle->population*16L)/(LONG)(g_battle->population+d_battle->population);
			c2=(LONG)((LONG)d_battle->population*16L)/(LONG)(g_battle->population+d_battle->population);
			draw_bar(w_screen,36,37,16,c1,15);
			draw_bar(w_screen,37,37,16,c2,8);
		}
		else if(peep->flags == IN_TOWN)
		{
			draw_sprite(w_screen,268,22,FLAG_SPRITE+toggle+peep->player*2);
			life=check_life(peep->player,peep->at_pos);
			if(life == 0) life=1;
			if(life == CITY_FOOD)
				draw_bar(w_screen,36,37,16,16,10);
			else
				draw_bar(w_screen,36,37,16,(life<<4)/MAX_FOOD,10);
			c3=((USHORT)peep->population)<<4;
			if((c1=c3/life) > 16)
				c1=16;
			if(c1 < 0)
				c1=0;
			draw_bar(w_screen,37,37,16,c1,12);
		}
		else
		{
			if(peep->flags & (I_AM_WAITING | WAIT_FOR_ME))
			{
				if(peep->head_for)
					c1=KNIGHT_WAITS;
				else
					c1=FIRST_WAIT_SPRITE;
				c1+=peep->player*2+toggle;
			}
			else if(peep->flags & IN_WATER)
			{
				if(peep->head_for)
					c1=FIRST_KNIGHT_WATER;
				else
					c1=FIRST_WATER_SPRITE;
				c1+=peep->player*4+(game_turn & 3);
			}
			else 
			{
				for(c1=0;c1 < 9 && dir_sprite[c1] != peep->direction;c1++);
				if(c1 > 7)
					c1=4;
				c1=c1*2+toggle;
				if(peep->head_for)
					c1+=32;
				c1+=peep->player*16;
			}
			draw_sprite(w_screen,272,22,c1);
			c1=peep->population;
			if(c1 > 4096)
			{
				draw_bar(w_screen,36,37,16,c1/1024,10);
				draw_bar(w_screen,37,37,16,c1%1024,9);
			}
			else
			{
				draw_bar(w_screen,36,37,16,c1/256,9);
				draw_bar(w_screen,37,37,16,(c1%256)/16,9);
			}
		}
	}
	if(magnet[GOD].population)
		draw_bar(w_screen,32,31,32,(SHORT)((magnet[GOD].population*31)/50000+1),15);
	else
		draw_bar(w_screen,32,31,32,0,15);
	if(magnet[DEVIL].population)
		draw_bar(w_screen,39,31,32,(SHORT)((magnet[DEVIL].population*31)/50000+1),8);
	else
		draw_bar(w_screen,39,31,32,0,8);
}

void CPopulous::set_temp_view(SHORT pos)
{
	if(view_timer == 0)
		old_view_who=view_who;
	view_timer=10;
	view_who=pos;
}

void CPopulous::move_mana()
{
	register SHORT	 c1;
	register SHORT	 c2;
	for(c1=0;magnet[player].mana > mana_values[c1];c1++);
	if(c1 <= 9)
	{
		// are: max(value, 0) added to stop mana bar going off bottom of scale.
		if (c1 == 0)
		{
			c2=((magnet[player].mana-mana_values[0])*8L)/
				(mana_values[c1]-mana_values[c1-1]);
		}
		else
		{
			c2=((max(magnet[player].mana-mana_values[c1-1], 0))*8L)/
				(mana_values[c1]-mana_values[c1-1]);
		}
		draw_sprite(w_screen,48+128-16+(c1-1)*16+c2*2,8+(c1-1)*8+c2,MANA_SPRITE);
	}
	else
		draw_sprite(w_screen,319-8,87,MANA_SPRITE);
}


