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

void	CPopulous::do_battle(p_peeps *peep, SHORT peep_pos)
{
//	SHORT   c1;
	LONG    b1;
	LONG    p1;
//	SHORT   life;
	struct  p_peeps *battle;
	
	
	/* find out who we are at battle with */
	
	battle=&peeps[peep->battle_population];
	
	/* show the battle in progress on the small map */
	
	a_putpixel(peep->at_pos,8+toggle*7);            
	
	/* find the larger of two population this will then be used to calculate the subtractions*/
	b1=(LONG)battle->population*(LONG)(newrand()%3+1);
	p1=(LONG)peep->population*(LONG)(newrand()%3+1);
	if(b1 > p1)
    {
		battle->population-=(p1/100)*peep->weapons+10;
		peep->population-=(p1/100)*battle->weapons+10;
    }
	else
    {
		peep->population-=(b1/100)*battle->weapons+10;
		battle->population-=(b1/100)*peep->weapons+10;
    }
	set_frame(peep);
	set_frame(battle);
	if(peep->population <= 0 && battle->population <= 0)
    {
		zero_population(battle,peep->battle_population);
		zero_population(peep,peep_pos);
    }
	else if(peep->population <= 0)
    {
		battle_over(peep->battle_population,peep_pos);
    }
	else if(battle->population <= 0)
    {
		battle_over(peep_pos,peep->battle_population);
    }
}

void	CPopulous::battle_over(SHORT won_pos, SHORT lost_pos)
{
	struct  p_peeps *won;
	struct  p_peeps *lost;
	SHORT   c1=0;
	SHORT   c2;
	SHORT   c3;
	won=&peeps[won_pos];
	lost=&peeps[lost_pos];
	battle_won[won->player]++;
	if(lost->flags & IN_TOWN)
    {
		lost->flags=IN_TOWN;
		set_frame(lost);
		c1=lost->frame-FIRST_TOWN;
		if(c1>=0 && c1<=10)
			c1=battle_add1[c1];
		else 
			c1=100;
		if(won->head_for && won->population)
        {
			lost->flags=IN_RUIN;
			lost->population=1;
			lost->battle_population=40;
			won->flags=ON_MOVE;
			if(map_bk2[lost->at_pos] == CITY_CENTRE)
				for(c2=0;c2<25;c2++)
                {
					c3=lost->at_pos+offset_vector[c2];
					if(c2<9 && c2 && map_bk2[c3] >= CITY_TOWER && map_bk2[c3] <= CITY_WALL2)
						map_bk2[c3]+=FIRST_RUIN_TOWN-FIRST_TOWN-1;
					if(map_blk[c3] == FARM_BLOCK+lost->player)
						map_blk[c3]=BAD_LAND;
                }
				else
				{
					for(c2=0;c2<17;c2++)
						if(valid_move(lost->at_pos,offset_vector[c2])==0)
						{
							c3=lost->at_pos+offset_vector[c2];
							if(map_blk[c3] == FARM_BLOCK+lost->player)
								map_blk[c3]=BAD_LAND;
						}
				}
				c3=lost->at_pos;
				if(map_bk2[c3] >= FIRST_TOWN && map_bk2[c3] <= CITY_CENTRE)
					map_bk2[c3]+=FIRST_RUIN_TOWN-FIRST_TOWN-1;
				if(lost_pos == map_who[c3]-1)
					map_who[c3]=0;
        }
		else
        {
			set_town(lost,1);
			if(check_life(won->player,won->at_pos) > 0)
            {
				lost->flags=IN_BATTLE;
				won->flags=IN_TOWN;
				won->battle_population=game_turn;
				set_frame(won);
				set_town(won,0);
            }
			else
            {
				won->flags=(won->flags & IN_TOWN) ? IN_TOWN : ON_MOVE;
            }
        }
    }
	else
    {
		won->flags=(won->flags & IN_TOWN) ? IN_TOWN : ON_MOVE;
		if(lost->head_for)
			c1=battle_add2[1];
		else if(lost_pos == magnet[lost->player].carried-1)
			c1=battle_add2[2];
		else
			c1=battle_add2[0];
    }
	if(lost_pos == map_who[won->at_pos]-1 || map_who[won->at_pos] == 0)
		map_who[won->at_pos]=won_pos+1;
	if(won->status)
    {
		stats[won->player].last_battle=won->at_pos;
		stats[won->player].arrived=2;
		won->status--;
    }
	if(lost->status)
    {
		stats[lost->player].arrived=0;
    }
	won->direction=0;
	won->flags|=IN_EFFECT;
	won->frame=VICTORY_SPRITE;
	magnet[won->player].mana+=c1;
	magnet[lost->player].mana-=c1;
	if(magnet[lost->player].mana < mana_values[0])
		magnet[lost->player].mana=mana_values[0];
	if(lost_pos == view_who-1)
		view_who=won_pos+1;
	if(lost->population <= 0)
		zero_population(lost,lost_pos);
}

void	CPopulous::set_battle(SHORT pos1, SHORT pos2)
{
	struct  p_peeps *peep1;
	struct  p_peeps *peep2;
	
	if (	(pos1 < 0) 
		||	(pos1 > 211)
		||	(pos2 < 0) 
		||	(pos2 > 211))
	{
		ODSN("Invalid");
	}
		
	peep1=&peeps[pos1];
	peep2=&peeps[pos2];
	peep1->flags=IN_BATTLE;
	peep2->flags=IN_BATTLE|(peep2->flags & 7);
	peep1->battle_population=pos2;
	peep2->battle_population=pos1;
	set_frame(peep1);
	set_frame(peep2);
	if(pos2 == view_who-1)
		view_who=pos2+1;
	if(pos2 == map_who[peep2->at_pos-peep2->direction]-1)
		map_who[peep2->at_pos-peep2->direction]=0;
	if(pos2 == map_who[peep2->at_pos]-1)
		map_who[peep2->at_pos]=0;
	if(peep2->flags & IN_TOWN)
		map_who[peep2->at_pos]=pos1+1;
	else
    {
		peep2->at_pos=peep1->at_pos;
		map_who[peep1->at_pos]=pos1+1;
    }
}

void	CPopulous::join_battle(SHORT pos1, SHORT pos2)
{
	struct  p_peeps *peep1;
	struct  p_peeps *peep2;

	if (	(pos1 < 0)
		||	(pos1 > 211)
		||	(pos2 < 0)
		||	(pos2 > 211))
	{
		ODSN("Invalid");
	}

	peep1=&peeps[pos1];
	peep2=&peeps[pos2];
	
	if(peep1->player != peep2->player)
    {
		pos2=peep2->battle_population;
		peep2=&peeps[pos2];
    }
	if((LONG)peep1->population + (LONG)peep2->population > 32000L)
		peep2->population=32000;
	else
		peep2->population+=peep1->population;
	if(pos1 == magnet[peep1->player].carried-1)
		magnet[peep1->player].carried=pos2+1;
	if(pos1 == view_who-1)
		view_who=pos2+1;
	if(pos2 > pos1)
		magnet[peep1->player].population-=peep1->population;
	if(peep1->head_for)
		peep2->head_for=peep1->head_for;
	if(peep1->weapons > peep2->weapons)
		peep2->weapons=peep1->weapons;
	peep1->population=0;
}

