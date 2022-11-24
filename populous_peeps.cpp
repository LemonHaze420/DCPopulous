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

// Original code
#include	"Computer.h"
#include	"God.h"
#include	"Extern.h"

//#define NULL 0L

void CPopulous::move_peeps()
{
	SHORT           c1;
	SHORT           c2;
	UBYTE           *i_am_here;
	SHORT           stage;
	SHORT           life;
	struct p_peeps  *peep;
	struct p_peeps  *peep2;
	SHORT           big_town[2];
	SHORT           old_town[2];
	SHORT           last_town[2];
	SHORT           my_town[2];
	SHORT           no_castles[2];
//	SHORT           c_gods_old_town=0;
//	SHORT           c_devils_big_town=0;
	SHORT           old_frame;
//	UBYTE           tmp[100];
	game_turn++;
	for(c1=0;c1 < 2;c1++)
    {
		if(stats[c1].computer_done == 0)
			set_devil_magnet(c1);
		if(stats[c1].computer_done == 0)
			devil_effect(c1);
    }
	for(c1=0;c1<2;c1++)
    {
		last_town[c1]=stats[c1].no_towns+stats[c1].no_castles*3;
		magnet[c1].population=magnet[c1].no_towns=0;
		if(toggle)
			magnet[c1].mana++;
		stats[c1].best1=stats[c1].best2=stats[c1].mybest=NULL;
		big_town[c1]=old_town[c1]=no_castles[c1]=0;
		my_town[c1]=19999;
    }
	/* reduce the size of peeps structure if there is no populations at the tail */
	for(;peeps[no_peeps-1].population ==0 && no_peeps > 0;no_peeps--);
	
	/* if either paple magnet carrier is a knight make them drop the pm*/
	if(magnet[player].carried)
		if(peeps[magnet[player].carried-1].head_for)
        {
			set_magnet_to(player,peeps[magnet[player].carried-1].at_pos);
			magnet[player].carried=0;
        }
		if(magnet[not_player].carried)
			if(peeps[magnet[not_player].carried-1].head_for)
			{
				set_magnet_to(not_player,peeps[magnet[not_player].carried-1].at_pos);
				magnet[not_player].carried=0;
			}
			if(war)
			{
				c1=MAP_WIDTH/2+(MAP_HEIGHT/2)*MAP_WIDTH;
				magnet[0].go_to=magnet[1].go_to=c1;
				god_magnet=c1;
				devil_magnet=c1;
			}
			/* start the main loop */
			if(sound_effect & IN_BATTLE)
				PlaySound(TUNE_SWORD1+toggle, 3);
			if(sound_effect & IN_RUIN)
			{
				if(last_fire == 0)
					PlaySound(TUNE_FIRE, 0);
				last_fire=3;
			}
			else if(last_fire)
				last_fire--;
			for(c1=0,peep=&peeps[0];;peep++,c1++)
			{
				if(c1 >= no_peeps)
					break;
				if(peep->population <= 0)
					continue;
				magnet[peep->player].population+=peep->population;    
				i_am_here=&map_blk[peep->at_pos];
				if(peep->flags & IN_WATER)
				{
					if(game_mode & G_WATER_FATAL)
					{
						zero_population(peep,c1);
						continue;
					}
					if((stats[peep->player].type  == COMPUTER_PLAYER && 
						(game_mode & G_NO_BUILD) == 0) || war)
					{
						stats[peep->player].send[S_EFFECT]=S_RAISE;
						stats[peep->player].send[S_X]=GET_X(peep->at_pos);
						stats[peep->player].send[S_Y]=GET_Y(peep->at_pos);
						stats[peep->player].computer_done=1;
					}
					peep->population-=(walk_death<<1);
					if(*i_am_here != WATER_BLOCK)
					{
						peep->flags&=~IN_WATER;
						set_frame(peep);
					}
					else
						goto end_of_loop;
				}        
				else if(*i_am_here == WATER_BLOCK)
				{
					if(peep->flags & IN_TOWN)
						set_town(peep,1);
					peep->flags=ON_MOVE|IN_WATER;
					if(peep->frame >= 8)
						peep->frame=0;
				}
				if(peep->flags & IN_EFFECT)
				{
					if(set_frame(peep))
					{
						peep->flags&=~IN_EFFECT;
						set_frame(peep);
					}
				}
				else if(peep->flags == IN_TOWN)
				{
					/* first check to see if land can still support this population */        
					if((life=check_life(peep->player,peep->at_pos)) <= 0 || peep->head_for || war)
					{
						/* the land is now barren so move the people on but first check there situation */            
						peep->flags&=~IN_TOWN;
						peep->flags|=ON_MOVE;
						peep->frame=0;
						peep->direction=0;
						set_town(peep,1);
						continue;
					}
					old_frame=peep->frame;
					if(life >= CITY_FOOD)
					{
						no_castles[peep->player]++;
						peep->frame=LAST_TOWN;
					}
					else
					{
						peep->frame = FIRST_TOWN+(life*10)/MAX_FOOD;
						magnet[peep->player].no_towns++;
					}
					stage=peep->frame-FIRST_TOWN;
					
					/* set the map_who if it is zero */
					if(map_who[peep->at_pos]==0)
						map_who[peep->at_pos]=c1+1;
					
					if(big_town[peep->player] < life)
					{
						big_town[peep->player]=life;
						stats[!peep->player].best1=peep;
					}
					if(old_town[peep->player] <= game_turn-peep->battle_population)
					{
						old_town[peep->player]=game_turn-peep->battle_population;
						stats[!peep->player].best2=peep;
					}
					if(my_town[peep->player] > game_turn-peep->battle_population)
					{
						my_town[peep->player]=game_turn-peep->battle_population;
						stats[peep->player].mybest=peep;
					}
					/* old place of shove out */            
					if(stats[peep->player].computer_done == 0)
					{
						if(map_alt[peep->at_pos] == 0)
						{
							if(peep->status == 0 || old_frame != peep->frame || a_flat_block)
								peep->status=make_level(peep->at_pos,peep->player);
						}
						else if(last_town[peep->player] < 3)
						{
							if(game_turn > 250)
								make_level(peep->at_pos,peep->player);
						}
					}
					if((game_turn & 7) == 0)
					{
						if(peep->frame == LAST_TOWN)
							if(stats[peep->player].type==COMPUTER_PLAYER)
								if(peep->population > MAX_FOOD && stats[peep->player].computer_done == 0)
								{                    
									life=MAX_FOOD;
									stats[peep->player].computer_done=1;
								}
								if(cheat)
									if(peep->player == cheat-1)
										life=50;
									magnet[peep->player].mana+=mana_add[stage];
									peep->weapons=weapons_add[stage];
									/* check to see if the village hase exceeded its population limit */            
									if(peep->population > life)
										/* the population has been exceeded so create some walkers */
										/* look for a free item in the peeps structure */
										for(c2=0,peep2=&peeps[0];c2 < MAX_NO_PEEPS;c2++,peep2++)
											if(peep2->population <= 0)
											{
												if(c2 >= no_peeps)
													no_peeps=c2+1;
												peep2->population=peep->population-(life>>1);
												peep->population=life>>1;
												if(c1 == magnet[peep->player].carried-1)
													magnet[peep->player].carried=c2+1;
												if(c1 == view_who-1)
													view_who=c2+1;
												peep2->iq=peep->iq;
												peep2->at_pos=peep->at_pos;
												peep2->flags=ON_MOVE;
												peep2->player=peep->player;
												peep2->direction=0;
												peep2->frame=0;
												peep2->weapons=peep->weapons;
												peep2->head_for=NULL;
												peep2->in_out=peep->at_pos;
												peep2->status=0;
												
												// Check that the data gets copied correctly.
												DEBUG_CHECKPEEPS;

												if( c1 == map_who[peep2->at_pos]-1 || map_who[peep2->at_pos]==0)
													map_who[peep2->at_pos]=c2+1;
												if(peep->iq < MAX_MENSA)
													peep->iq++;
												break;
											}
											if(c2 == MAX_NO_PEEPS)
												if(funny_done == 0)
												{
													//do_place_funny(1);
													funny_done=1;
												}
												peep->population+=population_add[stage];
					}
					if(!toggle && (peep->player == player || serial_off))
						a_putpixel(peep->at_pos,stats[peep->player].house_color);
					if(old_frame != peep->frame || a_flat_block || (peep->frame == LAST_TOWN && all_of_city < 9))
						set_town(peep,0);
        }
		else if(peep->flags == ON_MOVE)
        {
			if(peep->population > 0)
            {
				if(set_frame(peep))
                {
					if(*i_am_here == SWAMP_BLOCK)
                    {
						zero_population(peep,c1);
						PlaySound(TUNE_SWAMP, 0);
						if((game_mode & G_SWAMP_REMAIN)==0)
							*i_am_here=FLAT_BLOCK;
						continue;
                    }
					if(stats[!peep->player].best2  == NULL)
                        stats[!peep->player].best2=peep;
					if(stats[peep->player].computer_done == 0)
						one_block_flat(peep->at_pos,peep->player);
					move_explorer(peep,c1);
					if(peep->in_out)
						if(peep->in_out != peep->at_pos-peep->direction)
							peep->in_out=0;
						peep->population-=walk_death;
                }
				if(peep->population > 0)
                {
					/* check to see if any one else is here if so tell them to wait*/
					c2=map_who[peep->at_pos]-1;
					if(c2 != c1)
						if(c2 != -1)
							if(peeps[c2].frame == 0 && c2 < MAX_NO_PEEPS)
                            {
								if((peeps[c2].flags & IN_WATER) == 0)
                                {
									peeps[c2].flags|=WAIT_FOR_ME;
									peeps[c2].frame=FIRST_WAIT_SPRITE;
									peeps[c2].battle_population=0;
                                }
                            }
							if(toggle && (peep->player == player || serial_off))
								a_putpixel(peep->at_pos,8+(peep->player == GOD)*7);
                }
            }
        }
		else if(peep->flags & (WAIT_FOR_ME|I_AM_WAITING))
        {
			/* time out the wait ie. 14 rounds is more than enough for a person to catch up */        
			set_frame(peep);
			if(toggle && (peep->player == player || serial_off))
				a_putpixel(peep->at_pos,8+(peep->player == GOD)*7);
			peep->population-=walk_death;
			if(peep->battle_population++ > 14)
            {
				peep->flags&=~(WAIT_FOR_ME|I_AM_WAITING);
				set_frame(peep);
				if(peep->flags==ON_MOVE)
					move_explorer(peep,c1);
            }
        }
		/* if the in battle flag is set then the peep who statarted the battle will
		always be on the move and so his flag will be set to just IN_BATTLE */
		else if(peep->flags & IN_BATTLE)
        {
			/* if the flag == IN_BATTLE then process the fight */
			if(peep->flags == IN_BATTLE) /*<<<<<<<< remeber it could be in the water */
            {
				do_battle(peep,c1);
				continue;
            }
        }
		else if(peep->flags & IN_RUIN)
        {
			if(map_who[peep->at_pos] ==0)
				map_who[peep->at_pos]=c1+1;
			if(peep->battle_population-- <=0)
				peep->population=0;
        }
end_of_loop:;
			/* for some reason these people have died so go through the death */
			if(peep->population <= 0)
				zero_population(peep,c1);
    }
	if(magnet[player].population == 0 || player == surender)
		end_game(1);
	else if(magnet[not_player].population == 0 || not_player == surender)
		end_game(0);
	for(c1=0;c1 < 2;c1++)
    {
		stats[c1].no_towns=magnet[c1].no_towns;
		stats[c1].no_castles=no_castles[c1];
    }
	if((game_turn & 7) == 0)
    {
		if(cheat)
			text(w_screen,0,0,(UBYTE*)"CHEAT");
		cheat=0;
    }
}

void CPopulous::move_explorer(p_peeps *peep, SHORT peep_pos)
{
	struct  p_peeps *found_peep;
	SHORT           c1;
	SHORT           go_to = -999;
	int	dbroute = 0;

	if(magnet[peep->player].flags == MAGNET_MODE || peep->head_for || war)
	{
		go_to=move_magnet_peeps(peep,peep_pos);
		dbroute = 1;
	}
	else
	{
		go_to=where_do_i_go(peep,peep_pos);
		dbroute = 2;
	}

	/* check for an invalid return */
	if(go_to == 999)
    {
		peep->flags|=I_AM_WAITING;
		peep->battle_population=7;
		return;
    }
	else
		peep->flags&=~I_AM_WAITING;
	if(peep_pos == map_who[peep->at_pos-peep->direction]-1)
		map_who[peep->at_pos-peep->direction]=0; 
	
	/* check to see if there is any one on the block we are about to move onto */
	if(stats[peep->player].type == COMPUTER_PLAYER)
		if(map_blk[peep->at_pos] == BAD_LAND)
			if(stats[peep->player].computer_done == 0)
				if((game_mode & (G_NO_BUILD|G_ONLY_RAISE)) ==0)
				{
					stats[peep->player].send[S_EFFECT]=S_LOWER;
					stats[peep->player].send[S_X]=GET_X(peep->at_pos);
					stats[peep->player].send[S_Y]=GET_Y(peep->at_pos);
				}
				DebugCheckPeeps();
				if (	(peep->at_pos < 0)
					||	(peep->at_pos > 4095))
				{
					ODSN("Invalid");
				}

				c1=(unsigned char)map_who[peep->at_pos]-1;
				if(c1 != -1 && c1 != peep_pos && c1 < MAX_NO_PEEPS)
				{
					found_peep=&peeps[c1];
					if((found_peep->flags & IN_RUIN)==0)
					{
						if(found_peep->flags & IN_BATTLE)
							join_battle(peep_pos,c1);
						else if(found_peep->player == peep->player)
							join_forces(peep_pos,c1);
						else 
						{
							set_battle(peep_pos,c1);
							return;
						}
					}
				}        
				if(peep->population > 0)
				{
					/* if this population is still we must be on a flat piece of land that can be populated */    
					if(go_to == 0 && peep->head_for == 0)
					{
						if(map_who[peep->at_pos] == 0)
							map_who[peep->at_pos]=peep_pos+1;
						peep->flags=IN_TOWN;
						peep->battle_population=game_turn;
						set_frame(peep);
						set_town(peep,0);
					}
					else
					{
						map_steps[peep->at_pos]++;
						if(map_who[peep->at_pos] == 0)
							map_who[peep->at_pos]=peep_pos+1;
						peep->at_pos+=go_to;
						peep->direction=go_to;

						DEBUG_CHECKPEEPS;
					}
				}
}

int		CPopulous::where_do_i_go(p_peeps     *peep, SHORT peep_pos)
{
	register SHORT           c2;
	register SHORT           c1;
	register SHORT           c3;
	SHORT                    c4;
	struct  p_peeps         *found_peeps;
	SHORT           life;
	SHORT           offset;
	SHORT           footsteps=9999;
	SHORT           the_best[MOVE_TO_EMPTY+1];
	SHORT           move_offset[MOVE_TO_EMPTY+1];
	
	/*
	First look in all 8 directions for a flat piece of land that the people can
	populate
	*/
	for(c1=0;c1!=MOVE_TO_EMPTY+1;c1++)
		the_best[c1]=MAX_MENSA+1;
	for(c1=-1,c4=0;c4!=9;c4++)
    {
		if(c1 == 0 && c4 == 1) c1=(newrand()&7)+1;
		else c1++;
		if(c1 == 9) c1 = 0;
		offset=peep->at_pos;
		for(c2=0;c2!=peep->iq;c2++)
        {
			/* if this offset points to a waterblock then its no point continuing the MENSA loop */
			c3=valid_move(offset,offset_vector[c1]);
			if(c3)
				break;
			/* a flat block is found but is there people on, around the flat block already */
			offset+=offset_vector[c1];

			if(map_blk[offset] == FLAT_BLOCK)
				if(c2 < the_best[MOVE_TO_FLAT])
				{
					/* this means we are on a flat block but is it liverbull on */
					if(c1 == 0)
					{
						if(life=check_life(peep->player,offset))
						{
							the_best[MOVE_TO_FLAT]=c2;
							move_offset[MOVE_TO_FLAT]=0;
							continue;
						}
					}
					else
					{
						for(c3=9;c3<17;c3++)
							if(!valid_move(offset,offset_vector[c3]))
							{
								life=map_bk2[offset+offset_vector[c3]];
								if(life > FIRST_TOWN)
									if(life <= CITY_WALL2)
										break;
							}
							if(c3 == 17)
							{
								the_best[MOVE_TO_FLAT]=c2;
								move_offset[MOVE_TO_FLAT]=offset_vector[c1];
								continue;
							}
					}
				}
				/* look for somewhere where there is sombody */            
				if(c1)
				{
					if(map_who[offset])
					{
						if(peep_pos != map_who[offset]-1)
						{
							found_peeps=&peeps[map_who[offset]-1];
							/* first we check for the nearest battle */            
							if(found_peeps->flags & IN_BATTLE)
								if(c2 < the_best[MOVE_TO_BATTLE])
								{
									the_best[MOVE_TO_BATTLE]=c2;
									move_offset[MOVE_TO_BATTLE]=offset_vector[c1];
									continue;
								}
								/* now check for nearest enemy */                
								if(found_peeps->player != peep->player)
									if(c2 < the_best[MOVE_TO_ENEMY])
									{
										the_best[MOVE_TO_ENEMY]=c2;
										move_offset[MOVE_TO_ENEMY]=offset_vector[c1];
										continue;
									}
									/* then check for the nearest tribe on the move */                
									if(found_peeps->flags & ON_MOVE)
										if(c2 < the_best[MOVE_TO_FRIEND])
										{
											the_best[MOVE_TO_FRIEND]=c2;
											move_offset[MOVE_TO_FRIEND]=offset_vector[c1];
											continue;
										}
						}
					}
					/* look for the least explored reigeion */
					if(offset_vector[c1] != peep->direction)
					{
						if(map_steps[offset] < footsteps ||
							(map_steps[offset] == footsteps && 
							c2 < the_best[MOVE_TO_EMPTY]))
						{
							footsteps=map_steps[offset];
							the_best[MOVE_TO_EMPTY]=c2;
							move_offset[MOVE_TO_EMPTY]=offset_vector[c1];
						}
					}
				}
        }
    }
	if(magnet[peep->player].flags == FIGHT_MODE)
		if(the_best[MOVE_TO_ENEMY] != MAX_MENSA+1)
		{
			return(move_offset[MOVE_TO_ENEMY]);
		}
		if(magnet[peep->player].flags == JOIN_MODE)
			if(the_best[MOVE_TO_FRIEND] != MAX_MENSA+1)
			{
				return(move_offset[MOVE_TO_FRIEND]);
			}
			for(c1=0;c1!=MOVE_TO_EMPTY+1;c1++)
				if(the_best[c1] != MAX_MENSA+1)
				{
					return(move_offset[c1]);
				}
				return(999);
}


int CPopulous::move_magnet_peeps(p_peeps     *peep, SHORT peep_pos)
{
	SHORT               pos;
	register SHORT               c1;
	SHORT               c2;
	SHORT               x;
	SHORT               y;
	SHORT               dx;
	SHORT               dy;
	SHORT               who;
	who=peep->player;
	if ((who < 0) || (who > 1))
		ODSN("Bug");

	if(peep->head_for)
    {
		if(peep->at_pos == peep->head_for->at_pos 
			|| peep->head_for->population <= 0 
			|| peep->head_for->player == peep->player 
			|| (peep->head_for->flags & IN_RUIN))
			get_heading(peep);
		dx=GET_X(peep->head_for->at_pos)-GET_X(peep->at_pos);
		dy=GET_Y(peep->head_for->at_pos)-GET_Y(peep->at_pos);
		x=sign(dx);
		y=sign(dy);
    }
	else if(magnet[who].carried == 0)
    {
		if(magnet[who].go_to == peep->at_pos)
        {
			magnet[who].carried=peep_pos+1;
			if(view_who == 0)
				view_who=peep_pos+1;
			peep->player=who;
        }
        
		x=sign(GET_X(magnet[who].go_to)-GET_X(peep->at_pos));
		y=sign(GET_Y(magnet[who].go_to)-GET_Y(peep->at_pos));
    }    
	else if(peep_pos == magnet[who].carried-1)
    {
		x=sign(GET_X(magnet[who].go_to)-GET_X(peep->at_pos));
		y=sign(GET_Y(magnet[who].go_to)-GET_Y(peep->at_pos));
    }
	else
    {
		if (	((magnet[who].carried-1) < 0)
			||	((magnet[who].carried-1) >= no_peeps))
			ODSN("Bug");

		dx=GET_X(peeps[magnet[who].carried-1].at_pos)-GET_X(peep->at_pos);
		dy=GET_Y(peeps[magnet[who].carried-1].at_pos)-GET_Y(peep->at_pos);
		x=sign(dx);
		y=sign(dy);
    }

	if (	(abs(x) > 1)
		||	(abs(y) > 1))
		ODSN("Bug");

	pos=to_delta[(x+1)*3+y+1];
	if((c2=valid_move(peep->at_pos,to_offset[pos])) == 0)
	{
		if(peep->head_for==NULL || map_blk[peep->at_pos+to_offset[pos]] != SWAMP_BLOCK)
		{
			ODSN("mmp: 1\nat_pos, offset");
			ODI(peep->at_pos);
			ODIN(to_offset[pos]);

			if (	((peep->at_pos + to_offset[pos]) < 0)
				||	((peep->at_pos + to_offset[pos]) > 4095))
				ODSN("Placed person illegally");

			peep->magnet_last_move = to_offset[pos];
			return(to_offset[pos]);
		}
	}
		
	if(c2 == 2 && war)
	{
		ODSN("mmp: 2");

		if (	((peep->at_pos + to_offset[pos]) < 0)
			||	((peep->at_pos + to_offset[pos]) > 4095))
			ODSN("Placed person illegally");
		
		return(peep->magnet_last_move=to_offset[pos]);
	}

	if(stats[peep->player].type==COMPUTER_PLAYER || war)
	{
		if((game_mode & G_NO_BUILD) == 0 || war)
		{
			if(c2 == 3)
			{
				stats[peep->player].send[S_EFFECT]=S_RAISE;
				stats[peep->player].send[S_X]=GET_X(peep->at_pos);
				stats[peep->player].send[S_Y]=GET_Y(peep->at_pos);
				stats[peep->player].computer_done=1;
			}
			else if(map_blk[peep->at_pos+to_offset[pos]] == SWAMP_BLOCK && (game_mode & G_ONLY_RAISE) == 0 && war ==0)
			{
				stats[peep->player].send[S_EFFECT]=S_RAISE;
				stats[peep->player].send[S_X]=GET_X(peep->at_pos+to_offset[pos]);
				stats[peep->player].send[S_Y]=GET_Y(peep->at_pos+to_offset[pos]);
				stats[peep->player].computer_done=1;
			}

			//map_blk[peep->at_pos+to_offset[pos]]
		}
	}
	
	for(c1=pos-1,c2=0;c2 < 8;c1++,c2++)
	{
		if(c1 < 0)
			c1=7;
		if(c1 > 7)
			c1=0;
		if(valid_move(peep->at_pos,to_offset[c1]) == 0 && 
               to_offset[c1] != peep->magnet_last_move)
		{
			if( peep->head_for == NULL ||
				map_blk[peep->at_pos+to_offset[c1]] != SWAMP_BLOCK )
			{
				peep->magnet_last_move=to_offset[opposite[c1]];
					
				ODSN("mmp: 3");

				if (	((peep->at_pos + to_offset[c1]) < 0)
					||	((peep->at_pos + to_offset[c1]) > 4095))
					ODSN("Placed person illegally");
					
				return(to_offset[c1]);
			}
		}
	}
	
	if(c2 == 8)
	{
		ODSN("mmp: 4");

		peep->magnet_last_move=to_offset[opposite[c1]];
		return(999);
	}
		
	ODSN("I've not returned a value.");
	return 999;
}

void CPopulous::get_heading(p_peeps *peep)
{
	register struct p_peeps *find;
	register SHORT  c1;
	SHORT           sx,sy;
	SHORT           dd;
	SHORT           closest=9999;
	SHORT           d1,d2;
	sx=GET_X(peep->at_pos);
	sy=GET_Y(peep->at_pos);
	peep->head_for=peep;
	for(c1=0,find=&peeps[0];c1 < no_peeps;find++,c1++)
		if(peep->player != find->player)
			if(find->population > 0)
				if( (find->flags & IN_RUIN) == 0)
                {
					d1=GET_X(find->at_pos)-sx;
					d2=GET_Y(find->at_pos)-sy;
					dd=abs(d1) + abs(d2);
					if(dd < closest)
                    {
						closest=dd;
						peep->head_for=find;
                    }
                }
}

void CPopulous::join_forces(SHORT pos1, SHORT pos2)
{
	struct  p_peeps *peep1;
	struct  p_peeps *peep2;
	SHORT   mag;
	peep1=&peeps[pos1];
	peep2=&peeps[pos2];
	mag=magnet[peep1->player].carried-1;
	
	if(peep1->head_for)
    {
		if(peep2->flags == IN_TOWN)
			return;
		peep2->head_for=peep1->head_for;
    }
	if((LONG)peep1->population + (LONG)peep2->population > 32000L)
		peep2->population=32000;
	else
		peep2->population+=peep1->population;
	if(pos1 == mag)
		magnet[peep1->player].carried=pos2+1;
	if(pos1 == view_who-1)
		view_who=pos2+1;
	if(pos2 > pos1)
		magnet[peep1->player].population-=peep1->population;
	if(peep1->weapons > peep2->weapons)
		peep2->weapons=peep1->weapons;
	peep1->population=0;
	peep2->flags&=~(WAIT_FOR_ME|I_AM_WAITING);
	peep2->frame=0;
}

void CPopulous::zero_population(p_peeps *peep, SHORT peep_pos)
{
	SHORT   c1;
	c1=peep->player;
	peep->population=0;
	if(peep->flags == IN_BATTLE)
		peeps[peep->battle_population].flags&=~IN_BATTLE;
	if(peep->flags & IN_TOWN)
		set_town(peep,1);
	if(peep_pos == map_who[peep->at_pos]-1)
		map_who[peep->at_pos]=0;
	if(peep_pos == map_who[peep->at_pos-peep->direction]-1)
		map_who[peep->at_pos-peep->direction]=0;
	if(peep_pos == magnet[peep->player].carried-1)
    {
		magnet[peep->player].carried=0;
		set_magnet_to(peep->player,peep->at_pos);
    }
	if(peep_pos == view_who-1)
		view_who=0;
}

int		CPopulous::set_frame(p_peeps *peep)
{
	register struct  p_peeps    *peep2;
	register SHORT           life;
	if(peep->population)
    {
		if(peep->flags == ON_MOVE)
        {
			peep->frame++;
			if(peep->frame >= 7)
            {
				peep->frame = 0;
				return(1);
            }
        }
		else if(peep->flags & IN_WATER)
        {
			peep->frame++;
			if(peep->frame > LAST_WATER_SPRITE || peep->frame < FIRST_WATER_SPRITE)
            {
				peep->frame=FIRST_WATER_SPRITE;
				return(1);
            }
        }        
		else if(peep->flags & IN_BATTLE)
        {
			peep2=&peeps[peep->battle_population];
			if(peep->head_for)
            {
				if(peep2->head_for)
					life=BLUE_VS_RED;
				else if(peep->player == GOD)
					life=BLUE_VS_PEEP;
				else 
					life=RED_VS_PEEP;
            }
			else if(peep2->head_for)
            {
				if(peep2->player == GOD)
					life=BLUE_VS_PEEP;
				else
					life=RED_VS_PEEP;
            }
			else
				life=BATTLE_FIRST_FRAME;
			peep->frame++;
			if(peep->frame >= life+3 || peep->frame < life)
            {
				peep->frame=life;
				return(1);
            }
        }
		else if(peep->flags == IN_TOWN)
        {
			life=check_life(peep->player,peep->at_pos);
			if(life >= CITY_FOOD)
				peep->frame=LAST_TOWN;
			else
				peep->frame = FIRST_TOWN+(life*10)/MAX_FOOD;
        }
		else if(peep->flags & IN_EFFECT)
        {
			peep->frame++;
			if(peep->frame >= LAST_VICTORY_SPRITE || peep->frame < VICTORY_SPRITE)
				return(1);
        }
		else if(peep->flags & (I_AM_WAITING | WAIT_FOR_ME))
        {
			peep->frame++;
			if(peep->frame > LAST_WAIT_SPRITE || peep->frame < FIRST_WAIT_SPRITE)
            {
				peep->frame=FIRST_WAIT_SPRITE;
				return(1);
            }
        }
        
    }
	return(0);
} 

void CPopulous::set_town(p_peeps *peep, SHORT clear)
{
	register SHORT           c1;
	SHORT           at_pos;
	register SHORT           *vec;
	register SHORT           c2;
	at_pos=peep->at_pos;
	vec=offset_vector;
	if(clear)
    {
		if(map_bk2[at_pos] == CITY_CENTRE)
        {
			for(c1=0;c1 < 25;c1++,vec++)
            {
				if(valid_move(at_pos,*vec) != 1)
                {
					c2=at_pos+(*vec);
					map_bk2[c2]=0;
					if(map_blk[c2] == FARM_BLOCK+peep->player)
						map_blk[c2]=FLAT_BLOCK;
                }
            }
        }
		else
        {
			for(c1=0;c1 < 17;c1++,vec++)
            {
				if(valid_move(at_pos,*vec) != 1)
                {
					c2=at_pos+(*vec);
					if(c1 < 9)
						map_bk2[c2]=0;            
					if(map_blk[c2] == FARM_BLOCK+peep->player)
						map_blk[c2]=FLAT_BLOCK;
                }
            }
			map_bk2[at_pos]=0;            
        }
    }
	else
    {
		if(peep->frame == LAST_TOWN)
        {
			for(c1=0;c1 < 25;c1++,vec++)
            {
				if(valid_move(at_pos,*vec) != 1)
                {
					c2=at_pos+(*vec);
					if(c1 < 9)
						map_bk2[c2]=big_city[c1];
					if(map_blk[c2] == FLAT_BLOCK)
						map_blk[c2]=FARM_BLOCK+peep->player;
                }
            }   
        }
		else
        {
			if(map_bk2[at_pos] == CITY_CENTRE)
            {
				for(c1=0;c1 < 25;c1++,vec++)
                {
					if(valid_move(at_pos,*vec) != 1)
                    {
						c2=at_pos+(*vec);
						map_bk2[c2]=0;
						if(map_blk[c2] == FARM_BLOCK+peep->player)
							map_blk[c2]=FLAT_BLOCK;
                    }
                }
				vec=offset_vector;
            }
			for(c1=0;c1 < 17;c1++,vec++)
            {
				if(valid_move(at_pos,*vec) != 1)
                {
					c2=at_pos+(*vec);
					if(c1 < 9)
						map_bk2[c2]=0;
					if(map_blk[c2] == FLAT_BLOCK)
						map_blk[c2]=FARM_BLOCK+peep->player;
                }
            }   
			if(map_blk[at_pos]==FARM_BLOCK+peep->player)
				map_bk2[at_pos]=peep->frame;
        }
    }
}
