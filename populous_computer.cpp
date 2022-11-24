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

// Original files
#include	"Computer.h"
#include	"God.h"
#include	"Extern.h"

/* the computer player will try to create flat blocks of land where posible */


int		CPopulous::make_level(SHORT pos, SHORT who)
{
	SHORT   x;
	SHORT   y;
	register SHORT   xx;
	register SHORT   yy;
	register SHORT   c1;
	SHORT   c2;
	SHORT   c3;
	SHORT   this_alt;
	struct  s_stats *stat;
	stat=&stats[who];
	if((stat->computer_mode & C_LAND)==0 || (game_mode & G_NO_BUILD))
		return (0);
	x=GET_X(pos);
	y=GET_Y(pos);
	this_alt=alt[x+y*(MAP_WIDTH+1)];
	for(c1=0;c1<162;c1+=2)
    {
		xx=x+a_flat[c1];
		yy=y+a_flat[c1+1];
		if(xx>=0 && xx<=MAP_WIDTH && yy>=0 && yy<=MAP_HEIGHT)
        {
			c3=xx+yy*MAP_WIDTH;
			if(map_blk[c3] == ROCK_BLOCK && (game_mode & G_ONLY_RAISE) == 0)
            {
				map_blk[c3]++;
				stat->send[S_EFFECT]=S_LOWER;
				stat->send[S_X]=xx;
				stat->send[S_Y]=yy;
				stat->computer_done=1;
				return(0);
            }
			else
            {
				c2=this_alt-alt[xx+yy*(MAP_WIDTH+1)];
				if(c2 > 0)
                {
					stat->send[S_EFFECT]=S_RAISE;
					stat->send[S_X]=xx;
					stat->send[S_Y]=yy;
					stat->computer_done=1;
					return(0);
                }
				else if((c2 < 0 || map_blk[c3] == BAD_LAND || 
					map_blk[c3] == SWAMP_BLOCK) &&
					(game_mode & G_ONLY_RAISE) == 0)
                {
					stat->send[S_EFFECT]=S_LOWER;
					stat->send[S_X]=xx;
					stat->send[S_Y]=yy;
					stat->computer_done=1;
					return(0);
                }
            }
        }
    }
	return(1);
}

void	CPopulous::one_block_flat(SHORT pos, SHORT who)
{
	SHORT       x;
	SHORT       y;
	register SHORT       xx;
	register SHORT       yy;
	SHORT       mod;
	SHORT       ave;
	struct  s_stats *stat;
	
	stat=&stats[who];
	
	if(stat->computer_done || 
		magnet[who].mana < 20 || 
		magnet[who].no_towns > 50 || 
		(stat->computer_mode & C_LAND)==0 ||
		(game_mode & G_NO_BUILD))
		return;
	x=GET_X(pos);
	y=GET_Y(pos);
	
	ave=alt[x+y*END_WIDTH]+alt[x+1+y*END_WIDTH]+alt[x+1+y*END_WIDTH+END_WIDTH]+
		alt[x+y*END_WIDTH+END_WIDTH];
	if(ave == 1)
		return;
	mod=ave%4;
	ave/=4;
	for(xx=x;xx<=x+1;xx++)
		for(yy=y;yy<=y+1;yy++)
			if(mod == 3)
            {
				if(alt[xx+yy*END_WIDTH] == ave)
                {
					stat->send[S_EFFECT]=S_RAISE;
					stat->send[S_X]=xx;
					stat->send[S_Y]=yy;
					stat->computer_done=1;
					return;
                }
            }
			else if(mod == 1)
            {
				if(alt[xx+yy*END_WIDTH] > ave && (game_mode & G_ONLY_RAISE) == 0)
                {
					stat->send[S_EFFECT]=S_LOWER;
					stat->send[S_X]=xx;
					stat->send[S_Y]=yy;
					stat->computer_done=1;
					return;
                }
            }
}            

void	CPopulous::devil_effect(SHORT who)
{
	register struct  s_stats *stat;
	SHORT   c1,c2;
	USHORT   mode;
	
	stat=&stats[who];
	if(stat->computer_done)
		return;
	mode=stat->computer_mode;
	if(mode & C_WAR)
		if(magnet[who].mana > mana_values[MANA_WAR]+999)
			if(magnet[who].population > magnet[!who].population)
            {
				stat->send[S_EFFECT]=S_ACTION;
				stat->send[S_Y]=SA_MANA_WAR;
				stat->computer_done=1;
				return;
            }
			if(mode & C_FLOOD)
				if(magnet[who].mana > mana_values[MANA_FLOOD]+1999)
				{
					stat->send[S_EFFECT]=S_ACTION;
					stat->send[S_Y]=SA_MANA_FLOOD;
					stat->computer_done=1;
					return;
				}
				if(mode & C_KNIGHT)
					if(magnet[who].carried)
						if(peeps[magnet[who].carried-1].population > DEVIL_MAKES_KNIGHT)
						{
							if(magnet[who].mana > mana_values[MANA_KNIGHT]+500)
							{
								stat->send[S_EFFECT]=S_ACTION;
								stat->send[S_Y]=SA_MANA_KNIGHT;
								stat->computer_done=1;
							}
							return;
						}
						if(stat->best2)
						{
							if(magnet[who].mana > mana_values[MANA_VOLCANO]+500)
							{
								if(mode & C_VOLCANO)
								{
									stat->send[S_EFFECT]=MANA_VOLCANO;
									do_computer_effect(who,stat);
									stat->computer_done=1;
									stat->quake_count=0;
									return;
								}
							}
							if(magnet[who].mana > mana_values[MANA_SWAMP]+500)
							{
								if(mode & C_SWAMP)
									if(stat->quake_count >= stat->no_quakes || (mode & C_QUAKE) == 0)
										if(stat->quake_count <= stat->no_swamps || (mode & C_VOLCANO) == 0)
										{
											c1=magnet[!who].carried-1;
											c2=magnet[who].carried-1;
											if(c1 != -1)
											{
												if(c2 == -1 || peeps[c1].population > peeps[c2].population)
												{
													stat->quake_count++;
													stat->send[S_EFFECT]=MANA_SWAMP;
													stat->send[S_X]=GET_X(peeps[c1].at_pos);
													stat->send[S_Y]=GET_Y(peeps[c1].at_pos);
													return;
												}
											}
										}
							}
							if(magnet[who].mana > mana_values[MANA_QUAKE]+500 && 
								stat->best2->flags == IN_TOWN)
							{
								if(mode & C_QUAKE)
									if(stat->quake_count < stat->no_quakes || 
										(mode & (C_SWAMP|C_VOLCANO)) == 0)
									{
										stat->send[S_EFFECT]=MANA_QUAKE;
										do_computer_effect(who,stat);
										stat->computer_done=1;
										stat->quake_count++;
									}
							}
						}
}

void	CPopulous::do_computer_effect(SHORT who, s_stats *stat)
{
	SHORT   x,y;
	if(magnet[who].carried == 0)
    {
		x=map_who[magnet[who].go_to];
		if(x)
        {
			if(peeps[x-1].player != who)
            {
				stat->send[S_X]=GET_X(magnet[who].go_to);
				stat->send[S_Y]=GET_Y(magnet[who].go_to);
				return;
            }
        }
    }
	x=GET_X(stat->best2->at_pos)-3;    
	y=GET_Y(stat->best2->at_pos)-3;
	if(x<0) x=0;
	if(y<0) y=0;
	stat->send[S_X]=x;
	stat->send[S_Y]=y;
}

void	CPopulous::set_devil_magnet(SHORT who)
{
	register struct  s_stats *stat;
	register struct  p_peeps *imp=NULL;
	register struct  p_peeps *pope=NULL;
	stat=&stats[who];
	
	if(stat->no_towns+stat->no_castles < stat->computer_skill*2+15
		|| game_turn % 90 < 10+stat->computer_skill)
    {
		if(magnet[who].flags == MAGNET_MODE)
        {
			stat->send[S_EFFECT]=S_ACTION;
			stat->send[S_Y]=SA_MODE;
			stat->send[S_X]=SETTLE_MODE+newrand()%3;
			stat->computer_done=1;
        }
		return;
    }
	if(magnet[!who].carried)
		pope=&peeps[magnet[!who].carried-1];
	if(magnet[who].carried)
		imp=&peeps[magnet[who].carried-1];
	/*  devil to pick up his paple magnet */
	if(imp == NULL)
    {
		if(magnet[who].flags != MAGNET_MODE)
        {
			stat->send[S_EFFECT]=S_ACTION;
			stat->send[S_Y]=SA_MODE;
			stat->send[S_X]=MAGNET_MODE;
			stat->computer_done=1;
			stat->arrived=0; /*******/
        }
		return;
    }
	/* the devil has his magnet so we see if we can do anything */
	/* if we are weeker than him then attack towns */
	if(imp->at_pos == stat->last_battle && stat->arrived)
    {
		stat->arrived--;
    }
	if(imp->population < DEVIL_MAKES_KNIGHT*2)
    {
		if(stat->arrived)
        {
			if(magnet[who].go_to != stat->last_battle)
            {
				stat->send[S_EFFECT]=S_MAGNET;
				stat->send[S_X]=GET_X(stat->last_battle);
				stat->send[S_Y]=GET_Y(stat->last_battle);
				stat->computer_done=1;
				return;
            }
        }
		else if(magnet[who].go_to != stat->mybest->at_pos)
        {
			stat->send[S_EFFECT]=S_MAGNET;
			stat->send[S_X]=GET_X(stat->mybest->at_pos);
			stat->send[S_Y]=GET_Y(stat->mybest->at_pos);
			stat->last_battle=stat->mybest->at_pos;   /*******/
			stat->arrived=2;                               /*******/
			stat->computer_done=1;
			return;
        }
		if(magnet[who].flags != MAGNET_MODE)
        {
			stat->send[S_EFFECT]=S_ACTION;
			stat->send[S_Y]=SA_MODE;
			stat->send[S_X]=MAGNET_MODE;
			stat->computer_done=1;
        }
		return;
    }
	
	/* otherwise we must go and kill his paple magnet person */
	if(pope)
		if(imp->population >pope->population+500)
			if(magnet[!who].flags == MAGNET_MODE)  
            {
				if(stat->computer_mode & C_LEADER)
                {
					if(magnet[who].flags != MAGNET_MODE)
                    {
						stat->send[S_EFFECT]=S_ACTION;
						stat->send[S_Y]=SA_MODE;
						stat->send[S_X]=MAGNET_MODE;
						stat->computer_done=1;
                    }
					else if(magnet[who].go_to != magnet[!who].go_to)
                    {
						stat->send[S_EFFECT]=S_MAGNET;
						stat->send[S_X]=GET_X(magnet[!who].go_to);
						stat->send[S_Y]=GET_Y(magnet[!who].go_to);
						stat->computer_done=1;
                    }
                }
				return;
            }
			if(stat->computer_mode & C_TOWN)
			{
				if(stat->best1 != NULL)
				{
					/* we alternate between moving to his town and setting */
					if(stat->arrived == 0 && magnet[who].go_to != stat->best1->at_pos)
					{
						stat->send[S_EFFECT]=S_MAGNET;
						stat->send[S_X]=GET_X(stat->best1->at_pos);
						stat->send[S_Y]=GET_Y(stat->best1->at_pos);
						stat->last_battle=stat->best1->at_pos;   /*******/
						stat->arrived=2;
						stat->computer_done=1;
					}
					else if(magnet[who].flags != MAGNET_MODE)
					{
						stat->send[S_EFFECT]=S_ACTION;
						stat->send[S_Y]=SA_MODE;
						stat->send[S_X]=MAGNET_MODE;
						stat->computer_done=1;
					}
				}
			}
}
