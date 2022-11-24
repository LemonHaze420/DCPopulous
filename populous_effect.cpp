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

/// Original files.
#include	"Computer.h"
#include	"God.h"
#include	"Extern.h"
#include	"Score.h"


void	CPopulous::do_magnet(SHORT who, SHORT x, SHORT y)
{
	if(magnet[who].mana >= mana_values[MANA_MAGNET] && magnet[who].carried && war == 0)
    {
		PlaySound(TUNE_MAGNET, 0);
		magnet[who].mana-=mana_values[MANA_MAGNET];
		if(x >= MAP_WIDTH)
			x=MAP_WIDTH-1;
		if(x < 0)
			x=0;
		if(y >= MAP_HEIGHT)
			y=MAP_HEIGHT-1;
		if(y < 0)
			y=0;
		set_magnet_to(who,x+y*MAP_WIDTH);
    }
}

void CPopulous::rotate_all_map()
{
	register SHORT   c1;
	register SHORT   c2=END_WIDTH*END_HEIGHT-1;
//	SHORT   c3; // are: Not used
	for(c1=0;c1 < (c2>>1);c1++)
    {
		if(alt[c1] < alt[c2-c1])
			alt[c1]=alt[c2-c1];
		alt[c2-c1]=alt[c1];
    }
	c2=MAP_WIDTH*MAP_HEIGHT-1;
	for(c1=0;c1 < (c2>>1);c1++)
    {
		if(map_bk2[c1] >= TREE_BLOCK && map_bk2[c1] <= TREE_BLOCK+5)
			map_bk2[c2-c1]=map_bk2[c1];
		else if(map_bk2[c2-c1] >= TREE_BLOCK && map_bk2[c2-c1] <= TREE_BLOCK+5)
			map_bk2[c1]=map_bk2[c2-c1];
    }
	make_map(0,0,63,63);
	draw_map(0,0,63,63);
}

void CPopulous::clear_all_map()
{
	register SHORT   c1;
	
	for(c1=0;c1 < END_WIDTH*END_HEIGHT;c1++)
    {
		alt[c1]=0;
		if(c1 < MAP_WIDTH*MAP_HEIGHT)
        {
			map_who[c1]=0;
			map_bk2[c1]=0;
			map_blk[c1]=0;
        }
    }
	for(c1=0;c1<no_peeps;c1++)
		zero_population(&peeps[c1],c1);
	no_peeps=0;
	make_map(0,0,63,63);
	draw_map(0,0,63,63);
}

void CPopulous::place_people(SHORT who, SHORT x, SHORT y, SHORT leader)
{
	SHORT   c1;
	if(no_peeps >= MAX_NO_PEEPS)
		return;
	if(leader && magnet[who].carried)
    {
		c1=magnet[who].carried-1;
		zero_population(&peeps[c1],c1);
    }
	else
    {
		c1=no_peeps++;
    }
	peeps[c1].battle_population=0;
	peeps[c1].population=START_FOOD-5;
	peeps[c1].at_pos=GET_POS(x,y);
	map_who[peeps[c1].at_pos]=c1+1;
	peeps[c1].player=who;
	peeps[c1].flags=ON_MOVE;
	peeps[c1].frame=DUMMY_FRAME;
	peeps[c1].iq=1;
	peeps[c1].direction=0;
	peeps[c1].weapons=1;
	peeps[c1].head_for=NULL;
	if(leader)
		magnet[who].carried=c1+1;
	set_frame(&peeps[c1]);

	DEBUG_CHECKPEEPS;
}

void CPopulous::do_lower_point(SHORT who, SHORT x, SHORT y)
{
	xmax = xmin = x;
	ymax = ymin = y;
	if(magnet[who].mana < mana_values[MANA_POINT] && !paint_map)
		return;
	build_count=0;
	lower_point(x,y);
	if(paint_map==0 && war == 0)
		magnet[who].mana -= build_count*4+10;
	if(xmin<1)
		xmin = 1;
	if(xmax>63)
		xmax = 63;
	if(ymin<1)
		ymin = 1;
	if(ymax>63)
		ymax = 63;
	make_map(xmin-1,ymin-1,xmax,ymax);
	Setscreen(back_scr,-1L,-1); /* logical screen = background */
	draw_map(xmin-1,ymin-1,xmax,ymax);
	Setscreen(w_screen,-1L,-1); /* logical screen = work_screen */
}

void CPopulous::do_raise_point(SHORT who, SHORT x, SHORT y)
{
	if(magnet[who].mana < mana_values[MANA_POINT] && !paint_map && war == 0)
		return;
	xmax = xmin = x;
	ymax = ymin = y;
	build_count=0;
	raise_point(x,y);
	if(paint_map == 0 && war == 0)
		magnet[who].mana -= build_count*4+10;
	if(xmin<1)
		xmin = 1;
	if(xmax>63)
		xmax = 63;
	if(ymin<1)
		ymin = 1;
	if(ymax>63)
		ymax = 63;
	make_map(xmin-1,ymin-1,xmax,ymax);
	Setscreen(back_scr,-1L,-1); /* logical screen = background */
	draw_map(xmin-1,ymin-1,xmax,ymax);
	Setscreen(w_screen,-1L,-1); /* logical screen = work_screen */
}

int CPopulous::sculpt()
{
	SHORT   x, y;
	register SHORT   mx, my;
	SHORT   to_check;
	SHORT   maybes[9];
	SHORT   x_org, y_org;
	SHORT   alt_ptr;
	register SHORT   i;
	SHORT   screen_y;
	SHORT   old_screen_y;
	SHORT   which_maybe = -1;
//	SHORT   a;
	if(pause)
		return 1; // are: Inserted 1
	mx = mousex;
	my = mousey;
	x=my+(mx>>1)-32;
	y=my-(mx>>1)+32;
	
	/* check for active hexagon & a sprite is on screen */
	if( war || x < 62 || x>266 || y < -64 || y > 136 || mx < 64  || (ok_to_build == 0 && mode == SCULPT && paint_map == 0) ) 
		return(0);
	mx-= (64-8);       /* must try making this (64-8) ! */
	mx>>=4;
	
    
	x_org = xoff;
	y_org = yoff;
	
	alt_ptr = x_org+y_org*END_WIDTH;
	
	to_check = 9;   /* if mx == 8 */
	screen_y = 72;
	
	if(mx>8)        /* top right edge   */
    {
		to_check = 17-mx;
		alt_ptr += (mx - 8);
		screen_y += 8*(mx-8);
    }
	if(mx<8)        /* top left edge    */
    {
		to_check = mx+1;
		alt_ptr += (MAP_WIDTH+1)*(8-mx);
		screen_y += 8*(8-mx);
    }
	
	for(i = 0;i < to_check;i++)
    {
		maybes[i] = screen_y -8*alt[alt_ptr+66*i];
		screen_y +=16;
		if(maybes[i] <= my+4)
        {
			which_maybe = i;
			old_screen_y=maybes[i]-3;
        }
    }
	
	if(which_maybe<0)
		return(0);
	
	alt_ptr += 66*which_maybe;
	
	cur_y = y = alt_ptr/65;
	cur_x = x = alt_ptr-65*y;
	if(paint_map == 0 && (mode & (PUT_MAGNET|PUT_SWAMP)) == 0)
    {
		if(game_mode & G_NO_BUILD)
			return(0);
		if(alt[alt_ptr] > 0 && (game_mode & G_RAISE_TOWN)
			&& (ok_to_build & IN_TOWN) == 0)
			return(0);
    }
	if(old_screen_y < 192)
    {
		ms_off(); 
		draw_sprite(d_screen,mx*16+61,old_screen_y,CROSSHAIR_SPRITE);
		ms_on();
    }
	if(!left_button)
    {
		if(mode & PUT_SWAMP)
        {
			set_mode_icons(6,(mode&3)-1);
			stats[player].send[S_EFFECT]=MANA_SWAMP;
			stats[player].send[S_X]=x-1;
			stats[player].send[S_Y]=y-1;
			mode&=~PUT_SWAMP;
			make_arc_mouse(pointer=mode-1);
			if(pointer == 1 && player == DEVIL)
                make_arc_mouse(pointer=4);
        }
		else if(mode & PUT_MAGNET)
        {
			set_mode_icons(6,(mode&3)-1);
			stats[player].send[S_EFFECT]=S_MAGNET;
			stats[player].send[S_X]=x-1;
			stats[player].send[S_Y]=y-1;
			mode&=~PUT_MAGNET;
			make_arc_mouse(pointer=mode-1);
			if(pointer == 1 && player == DEVIL)
                make_arc_mouse(pointer=4);
        }
		else
        {
			stats[player].send[S_EFFECT]=S_RAISE;
			stats[player].send[S_X]=x;
			stats[player].send[S_Y]=y;
        }
    }
	else if(!right_button && mode == SCULPT && (game_mode & G_ONLY_RAISE)==0)
    {
		right_button=2;
		stats[player].send[S_EFFECT]=S_LOWER;
		stats[player].send[S_X]=x;
		stats[player].send[S_Y]=y;
    }


	return 1; // are: Inserted return value.
}

void CPopulous::do_flood(SHORT who)
{
	SHORT   c1;
	if(paint_map == 0)
    {
		if(magnet[who].mana < mana_values[MANA_FLOOD] || war || pause || (stats[who].computer_mode & C_FLOOD) == 0) 
			return;
		magnet[who].mana-=mana_values[MANA_FLOOD];
    }
	if(who == player)
		score+=SCORE_FLOOD;
	PlaySound(TUNE_FLOOD, 0);
	for(c1=0;c1<(MAP_WIDTH+1)*(MAP_HEIGHT+1);c1++)
		if(alt[c1] > 0)
			alt[c1]--;
		make_map(0,0,63,63);
		PlaySound(TUNE_FLOOD, 1);
		/*mod_map(0,0,63,63);*/
		PlaySound(TUNE_FLOOD, 2);
		Setscreen(back_scr,-1L,-1);
		draw_map(0,0,63,63);
		Setscreen(w_screen,-1L,-1);
		PlaySound(TUNE_FLOOD, 3);
}

void CPopulous::place_first_people()
{
	SHORT   c1;
	SHORT   count;
	if(in_conquest != -1 || start_of_game == 3)
		count=conquest.my_pop;
	else if(serial_off == 0)
		count=1;
	else    
		count=1+(stats[0].computer_skill>4)*2;
	if(player == GOD)
		score+=count*SCORE_PEOPLE;
	for(c1=MAP_WIDTH*2;c1<MAP_WIDTH*MAP_HEIGHT;c1++)
		if(map_blk[c1] == FLAT_BLOCK)
        {
			count--;
			place_people(0,GET_X(c1),GET_Y(c1),(count <= 0));
			if(!count)
				break;
        }
		if(count)
		{
			for(c1=0;c1<MAP_WIDTH*MAP_HEIGHT;c1++)
				if(map_blk[c1] && map_who[c1] == 0)
				{
					count--;
					place_people(0,GET_X(c1),GET_Y(c1),(count <= 0));
					if(!count)
						break;
				}
		}
		if(in_conquest != -1 || start_of_game == 3)
			count=conquest.his_pop;
		else if(serial_off == 0)
			count=1;
		else
			count=1+(stats[1].computer_skill>4)*2;
		if(player == DEVIL)
			score+=count*SCORE_PEOPLE;
		for(c1=(MAP_WIDTH-1)*(MAP_HEIGHT-1)-1;c1 >= MAP_WIDTH*2;c1--)
			if(map_blk[c1] == FLAT_BLOCK)
			{
				count--;
				place_people(DEVIL,GET_X(c1),GET_Y(c1),(count <= 0));
				if(!count)
					break;
			}
			if(count)
			{
				for(c1=MAP_WIDTH*MAP_HEIGHT;c1;c1--)
					if(map_blk[c1] && map_who[c1] == 0)
					{
						place_people(DEVIL,GET_X(c1),GET_Y(c1),(count <= 0));
						count--;
						if(!count)
							break;
					}
			}
}

void CPopulous::do_quake(SHORT who, SHORT x, SHORT y)
{
	int oldAScreen = GraphicsRef->GetActiveScreen();

	SHORT   x1;
	SHORT   y1;
	SHORT   c1;
//	SHORT   c2;
	if(paint_map == 0)
    {
		if(magnet[who].mana < mana_values[MANA_QUAKE] || war || pause || (stats[who].computer_mode & C_QUAKE) == 0)
			return;
		magnet[who].mana-=mana_values[MANA_QUAKE];
    }


	if (GraphicsRef->GetActiveScreen() > 1)
	{
		ODSN("Potential graphical glitch detected.");
		ODS("Trying to draw to screen: ");
		ODIN(GraphicsRef->GetActiveScreen());
		ODSN("Will attempt to side step.");
		GraphicsRef->SetActiveScreen(w_screen);	// Should hopefully rendered to the working screen, like a good little program.
	}

	xmax = xmin = x;
	ymax = ymin = y;
	screen_origin-=SCREEN_WIDTH;
	PlaySound(TUNE_QUAKE, 3);
	if(who == player)
		score+=SCORE_QUAKE;
	ms_off();
	for(c1=0;c1 != 20;c1++)
    {
		if(c1&1)
			screen_origin+=SCREEN_WIDTH*3;
		else
			screen_origin-=SCREEN_WIDTH*3;
		draw_it(xoff,yoff);    
		swap_screens();    
    }
	screen_origin+=SCREEN_WIDTH;
	GraphicsRef->SetActiveScreen(oldAScreen);

	for(c1=0;c1!=2;c1++)
		for(x1=x;x1<x+9;x1++)
			for(y1=y;y1<y+9;y1++)
				if(alt[x1+y1*(MAP_WIDTH+1)])
					switch(newrand()%5)
				{
                    case    1:
                        raise_point(x1,y1);
                        break;
                    case    2:
                    case    3:
                    case    4:
                        lower_point(x1,y1);
                        break;
                    default:
                        break;
				}
				if(xmin<1)
					xmin = 1;
				if(xmax>63)
					xmax = 63;
				if(ymin<1)
					ymin = 1;
				if(ymax>63)
					ymax = 63;
				ms_on();
				make_map(xmin-1,ymin-1,xmax,ymax);
				Setscreen(back_scr,-1L,-1);
				draw_map(xmin-1,ymin-1,xmax,ymax);
				Setscreen(w_screen,-1L,-1);
				kill_effect(2,4);

	
}

void CPopulous::do_volcano(SHORT who, SHORT x, SHORT y)
{
	SHORT   x1;
	SHORT   y1;
	SHORT   c1;
//	SHORT   c2;
	SHORT   xy;
	SHORT   car_0;
	SHORT   car_1;
	if(paint_map == 0)
    {
		if(magnet[who].mana < mana_values[MANA_VOLCANO] || war || pause || (stats[who].computer_mode & C_VOLCANO)==0)
			return;
		magnet[who].mana-=mana_values[MANA_VOLCANO];
    }
	if(who == player)
		score+=SCORE_VOLCANO;
	xmax = xmin = x;
	ymax = ymin = y;
	PlaySound(TUNE_VOLCANO, 1);
	for(c1=0;c1 <= 4;c1++)
		for(x1=c1;x1<9-c1;x1++)
			for(y1=c1;y1<9-c1;y1++)
				switch(newrand()%5)
			{
                case    1:
                case    2:
                case    4:
                    raise_point(x1+x,y1+y);
                    break;
                default:
                    break;
			}
			
			car_0=magnet[0].carried;
			if(car_0)
				car_0=peeps[car_0-1].at_pos;
			car_1=magnet[0].carried;
			if(car_1)
				car_1=peeps[car_1-1].at_pos;
			for(x1=x;x1<x+8;x1++)
				for(y1=y;y1<y+8;y1++)
					if(x1 >=0 && x1 < MAP_WIDTH && y1 >= 0 && y1 < MAP_HEIGHT)
					{
						xy=GET_POS(x1,y1);
						if(newrand()%5 == 0)
						{
							if(car_0 != xy && xy != car_1 && xy != magnet[0].go_to && xy !=magnet[1].go_to)
							{
								map_blk[x1+y1*MAP_WIDTH]=ROCK_BLOCK;
								map_bk2[x1+y1*MAP_WIDTH]=0;
							}
						}
					}
					if(xmin<1)
						xmin = 1;
					if(xmax>63)
						xmax = 63;
					if(ymin<1)
						ymin = 1;
					if(ymax>63)
						ymax = 63;
					make_map(xmin-1,ymin-1,xmax,ymax);
					Setscreen(back_scr,-1L,-1);
					draw_map(xmin-1,ymin-1,xmax,ymax);
					Setscreen(w_screen,-1L,-1);
					PlaySound(TUNE_VOLCANO, 2);
}

void CPopulous::set_magnet_to(SHORT who, SHORT where)
{
	if(pause)
		return;
	magnet[who].go_to=where;
	if(who == GOD)
		god_magnet=where;
	else
		devil_magnet=where;
}

void CPopulous::do_swamp(SHORT who, SHORT start_x, SHORT start_y)
{
	SHORT       x;
	SHORT       y;
	SHORT       c1;
	SHORT       c2;
	if(paint_map == 0)
    {
		if(magnet[who].mana < mana_values[MANA_SWAMP] || war || pause || (stats[who].computer_mode & C_SWAMP) == 0)
			return;
		magnet[who].mana-=mana_values[MANA_SWAMP];
    }
	if(who == player)
		score+=SCORE_SWAMP;
	for(c1=0;c1!=NO_TREES;c1++)
    {
		x=start_x+newrand()%7-3;
		y=start_y+newrand()%7-3;
		c2=map_blk[x+y*MAP_WIDTH];
		if(x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)
			if(c2 == FLAT_BLOCK || c2 == FARM_BLOCK || c2 == FARM_BLOCK+1 || c2 == BAD_LAND)
				if(map_who[x+y*MAP_WIDTH] == 0)
					map_blk[x+y*MAP_WIDTH]=SWAMP_BLOCK;
    }
}

void CPopulous::do_knight(SHORT who)
{
	if(magnet[who].carried == 0)
		return;
	if(paint_map == 0)
    {
		if(magnet[who].mana < mana_values[MANA_KNIGHT] || war || pause || (stats[who].computer_mode & C_KNIGHT) == 0)
			return;
		magnet[who].mana-=mana_values[MANA_KNIGHT];
    }
	if(who == player)
		score+=SCORE_KNIGHT;
	peeps[magnet[who].carried-1].status=1;
	get_heading(&peeps[magnet[who].carried-1]);
	set_magnet_to(who,peeps[magnet[who].carried-1].at_pos);
	magnet[who].carried=0;
	PlaySound(TUNE_KNIGHTED, 1);
}

void CPopulous::do_war(SHORT who)
{
	SHORT       c1;
	if(paint_map == 0)
    {
		if(magnet[who].mana < mana_values[MANA_WAR] || pause || 
			(stats[who].computer_mode & C_WAR) == 0)
			return;
		magnet[who].mana-=mana_values[MANA_WAR];
		if(who == player)
			score+=SCORE_WAR;
    }
	for(c1=0;c1<no_peeps;c1++)
    {
		peeps[c1].head_for=NULL;
    }
	PlaySound(TUNE_WAR, 2);
	war=1;
}

void CPopulous::make_woods_rocks()
{
	SHORT   x,y;
	SHORT   start_x,start_y;
	SHORT   c1,c2;
	SHORT   type;
	
	for(c1=0;c1!=NO_WOODS;c1++)
    {
		type=TREE_BLOCK-(c1<NO_WOODS/3)*3;
		start_x=newrand()%(MAP_WIDTH-5);
		start_y=newrand()%(MAP_HEIGHT-5);
		for(c2=0;c2!=NO_TREES;c2++)
        {
			x=start_x+newrand()%9;
			y=start_y+newrand()%9;
			if(x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)
				if(map_blk[x+y*MAP_WIDTH] != WATER_BLOCK && 
					map_blk[x+y*MAP_WIDTH] != ROCK_BLOCK)
                {
					if(type == ROCK_BLOCK)
						map_blk[x+y*MAP_WIDTH]=type+newrand()%3;
					else
						map_bk2[x+y*MAP_WIDTH]=type+newrand()%3;
                }
        }
    }
}

void CPopulous::do_funny()
{
	struct p_peeps  *fun=&peeps[FUNNY];
	SHORT   c1,c2,c3,c4;
	for(c1=FUNNY;c1<END_OF_FUNNYS;c1++,fun++)
    {
		if(fun->population)
        {
			fun->frame++;
			c2=fun->magnet_last_move;
			fun->battle_population+=c2;
			if(fun->battle_population >= (SHORT)fun->iq || 
				fun->battle_population <= (SHORT)fun->weapons)
            {
				c2=-c2;
				fun->magnet_last_move=c2;
            }
			if(fun->frame > 7)
            {
				fun->frame=0;
				if(c1 == map_who[fun->at_pos]-1)
					map_who[fun->at_pos]=0;
				if(fun->direction == 0)
					fun->direction=funny[fun->status].dir;
				c2=valid_move(fun->at_pos,fun->direction);
				if(c2 == 1)
                {
					fun->population=0;
					continue;
                }
				fun->at_pos+=fun->direction;

				for(c2=0;c2<3;c2++)
					if((c4=valid_move(fun->at_pos,funny[fun->status].kill[c2])) != 1)
                    {
						c3=fun->at_pos+funny[fun->status].kill[c2];
						if(fun->status == 0)
                        {
							if(map_blk[c3] != WATER_BLOCK && map_blk[c3] != WATER_BLOCK+16)
								map_bk2[c3]=TREE_BLOCK+c2;
                        }
						else if(fun->status == 1)
                        {
							if(map_blk[c3] == FLAT_BLOCK || map_blk[c3] == FARM_BLOCK || map_blk[c3] == FARM_BLOCK+1 || map_blk[c3] == BAD_LAND)
								map_blk[c3]=SWAMP_BLOCK;
                        }
						else if(map_blk[c3] != WATER_BLOCK && map_blk[c3] != WATER_BLOCK+16)
                        {
							map_blk[c3]=ROCK_BLOCK+c2;
							a_putpixel(c3,map_colour[map_blk[c3]]);
                        }
						if(map_who[c3])
							zero_population(&peeps[map_who[c3]-1],map_who[c3]-1);
                    }
					map_who[fun->at_pos]=c1+1;
            }
        }
		
    }
}

void CPopulous::do_place_funny(SHORT monster, SHORT pos)
{
/*
	// show me the funny...
	int tttt = GraphicsRef->GetActiveScreen();
	GraphicsRef->SetActiveScreen(0);
	
	GraphicsRef->ClearScreen(0,0,0);
	for (int i = 0; i < 13; i++)
	{
		Region.left		= 0;
		Region.top		= 32 * i;
		Region.right	= 32;
		Region.bottom	= Region.top + 32;
		GraphicsRef->DrawSprite((i % 10) * 32,
								(i / 10) * 32,
								Sprite32Graphics,
								Region);
	}
	GraphicsRef->SetActiveScreen(tttt);
*/

	SHORT   c1;
	SHORT   *c2;
	SHORT   *c3;
//	SHORT   *c4;
	struct  p_peeps *peep;
	if(monster > 2)
		return;
	c2=&stats[player].type;
	c3=&stats[!player].type;
	for(c1=FUNNY,peep=&peeps[c1];c1<END_OF_FUNNYS;c1++,peep++)
		if(peep->population == 0)
        {
			peep->population=1;
			peep->flags=ON_MOVE;
			if(pos == 0)
			{
				peep->at_pos=((newrand()%125)>>1)+63*MAP_WIDTH;
				DEBUG_CHECKPEEPS;
			}
			else if(pos == 1)
            {
				if(newrand() & 1)
				{
					peep->at_pos=MAP_WIDTH-1+(20+newrand()%43)*MAP_WIDTH;   
					DEBUG_CHECKPEEPS;
				}
				else
				{
					peep->at_pos=(20+newrand()%43)*MAP_WIDTH+(MAP_WIDTH-1)*MAP_WIDTH;   
					DEBUG_CHECKPEEPS;
				}
            }
			else if(pos == 2)
            {
				if(newrand() & 1)
				{
					peep->at_pos=(newrand()%43)*MAP_WIDTH;
					DEBUG_CHECKPEEPS;
				}
				else
				{
					peep->at_pos=newrand()%43;
					DEBUG_CHECKPEEPS;
				}
            }    
			map_who[peep->at_pos]=c1+1;
			peep->direction=funny[monster].dir;
			peep->iq=funny[monster].end;
			peep->weapons=funny[monster].start;
			peep->magnet_last_move=1;            
			peep->battle_population=peep->weapons;
			peep->status=monster;
			/*
			xoff = peep->at_pos % 64;
			yoff = peep->at_pos / 64;
			if (xoff > 56)
				xoff = 56;
			if (yoff > 56)
				yoff = 56;
			*/
			break;
        }
}



