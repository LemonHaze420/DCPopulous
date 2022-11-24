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


#include <stdio.h>
#include <string.h>
//#include "bbc.h"
#include	"Computer.h"
#include	"God.h"
#include	"Extern.h"

#define MID_POS(l,w,s) ((l)+(w)/2-(strlen((char*)s)*8)/2)
#define MAP_ID  (LONG)(('M'<<12)+('A'<<8)+('P'<<4)+'S')
#define GAME_ID (LONG)(('G'<<12)+('A'<<8)+('M'<<4)+'E')

int CPopulous::save_game(UBYTE *name)
{
	FILE    *fp;
	LONG    id=GAME_ID;
//	SHORT   c1;
	SHORT   error=1;
	USHORT  check;
	UBYTE   fname[MAX_PATH];//="adfs::0.$.";
	strcpy((char*)fname,(char*)name);//strcat(fname,name);
	if(fp=fopen((char*)fname,"wb"))
	{
        check=checksum((unsigned short*)&alt[0],(LONG)(END_WIDTH*END_HEIGHT));
        check+=checksum((unsigned short*)&map_alt[0],(LONG)(MAP_WIDTH*MAP_HEIGHT/2));
        check+=checksum((unsigned short*)&map_blk[0],(LONG)(MAP_WIDTH*MAP_HEIGHT/2));
        check+=checksum((unsigned short*)&map_bk2[0],(LONG)(MAP_WIDTH*MAP_HEIGHT/2));
        check+=checksum((unsigned short*)&map_who[0],(LONG)(MAP_WIDTH*MAP_HEIGHT/2));
        check+=magnet[0].population+magnet[1].population;
        land_checksum=check;
        if(fwrite(&id,1,4L,fp) == 4L)
			if(fwrite(alt,1,(LONG)(END_WIDTH*END_HEIGHT*2),fp) == (LONG)(END_WIDTH*END_HEIGHT*2))
				if(fwrite(map_alt,1,(LONG)(MAP_WIDTH*MAP_HEIGHT),fp) == (LONG)(MAP_WIDTH*MAP_HEIGHT))
					if(fwrite(map_blk,1,(LONG)(MAP_WIDTH*MAP_HEIGHT),fp) == (LONG)(MAP_WIDTH*MAP_HEIGHT))
						if(fwrite(map_bk2,1,(LONG)(MAP_WIDTH*MAP_HEIGHT),fp) == (LONG)(MAP_WIDTH*MAP_HEIGHT))
							if(fwrite(map_who,1,(LONG)(MAP_WIDTH*MAP_HEIGHT),fp) == (LONG)(MAP_WIDTH*MAP_HEIGHT))
								if(fwrite(magnet,1,(LONG)(sizeof(struct m_magnet)*2),fp) == (LONG)(sizeof(struct m_magnet)*2))
									if(fwrite(peeps,1,(LONG)(sizeof(struct p_peeps)*END_OF_FUNNYS),fp) == (LONG)(sizeof(struct p_peeps)*END_OF_FUNNYS))
										if(fwrite(&stats[0],1,(LONG)sizeof(struct s_stats),fp) == (LONG)sizeof(struct s_stats))
											if(fwrite(&stats[1],1,(LONG)sizeof(struct s_stats),fp) == (LONG)sizeof(struct s_stats))
												if(fwrite(&mode,1,2L,fp) == 2L)
													if(fwrite(&seed,1,2L,fp) == 2L)
														if(fwrite(&start_seed,1,2L,fp) == 2L)
															if(fwrite(&player,1,2L,fp) == 2L)
																if(fwrite(&ground_in,1,2L,fp) == 2L)
																	if(fwrite(&war,1,2L,fp) == 2L)
																		if(fwrite(&game_turn,1,2L,fp) == 2L)
																			if(fwrite(&conquest,1,(LONG)sizeof(struct c_conquest),fp) == (LONG)sizeof(struct c_conquest))
																				if(fwrite(&in_conquest,1,2L,fp) == 2L)
																					if(fwrite(&paint_map,1,2L,fp) == 2L)
																						if(fwrite(battle_won,1,4L,fp) == 4L)
																							if(fwrite(&game_mode,1,2L,fp) == 2L)
																								if(fwrite(&level_number,1,2L,fp) == 2L)
																									if(fwrite(&score,1,4L,fp) == 4L)
																										if(fwrite(&check,1,2L,fp) == 2L)
																											error=0;
																										fclose(fp);
																										bbc_cls();
	}
	return(error);
}

int CPopulous::checksum(USHORT *data,LONG length)
{
	USHORT  check;
	LONG    c1;
	for(check=0,c1=0;c1 != length;c1++)
		check+=data[c1];
	return(check);
}


void CPopulous::load_game(UBYTE *name)
{
	FILE    *fp;
	LONG    id=GAME_ID;
	SHORT   c1;
//	SHORT   error=1;
//	USHORT  check;
	UBYTE   fname[MAX_PATH];//="adfs::0.$.";
	strcpy((char*)fname,(char*)name);	//strcat(fname,name);
	if(fp=fopen((char*)fname,"rb"))
	{
		fread(&id,1,4L,fp);
		if(id == GAME_ID)
		{
			set_tend_icons(-1,-1);
			set_mode_icons(-1,-1);
			fread(alt,1,(LONG)(END_WIDTH*END_HEIGHT*2),fp);
			fread(map_alt,1,(LONG)(MAP_WIDTH*MAP_HEIGHT),fp);
			fread(map_blk,1,(LONG)(MAP_WIDTH*MAP_HEIGHT),fp);
			fread(map_bk2,1,(LONG)(MAP_WIDTH*MAP_HEIGHT),fp);
			fread(map_who,1,(LONG)(MAP_WIDTH*MAP_HEIGHT),fp);
			fread(magnet,1,(LONG)(sizeof(struct m_magnet)*2),fp);
			fread(peeps,1,(LONG)(sizeof(struct p_peeps)*END_OF_FUNNYS),fp);
			fread(&stats[0],1,(LONG)sizeof(struct s_stats),fp);
			fread(&stats[1],1,(LONG)sizeof(struct s_stats),fp);
			fread(&mode,1,2L,fp);
			fread(&seed,1,2L,fp);
			fread(&start_seed,1,2L,fp);
			fread(&player,1,2L,fp);
			fread(&ground_in,1,2L,fp);
			fread(&war,1,2L,fp);
			fread(&game_turn,1,2L,fp);
			fread(&conquest,1,(LONG)(sizeof(struct c_conquest)),fp);
			fread(&in_conquest,1,2L,fp);
			fread(&paint_map,1,2L,fp);
			fread(battle_won,1,4L,fp);
			fread(&game_mode,1,2L,fp);
			fread(&level_number,1,2L,fp);
			fread(&score,1,4L,fp);
			fread(&land_checksum,1,2L,fp);
			fclose(fp);
			bbc_cls();
			load_ground(ground_in,0);
			not_player=!player;
			set_tend_icons(-1,-1);
			set_mode_icons(-1,-1);
			god_magnet=magnet[0].go_to;
			devil_magnet=magnet[1].go_to;
			Setscreen(back_scr,back_scr,-1);
			draw_map(0,0,63,63);
			Setscreen(w_screen,-1L,-1);
			for(c1=MAX_NO_PEEPS-1;c1 >= 0 && peeps[c1].population == 0;c1--);
			no_peeps=c1+1;
			for(c1=0;c1<S_RAND;c1++)
				stats[0].send[c1]=stats[1].send[c1]=0;
			for(c1=0;c1 < no_peeps;c1++)
				if(peeps[c1].head_for)
					get_heading(&peeps[c1]);
				if(mode == SCULPT)
					make_arc_mouse(pointer=(player == DEVIL)*3+1);
		}
		else
		{
			fclose(fp);
			bbc_cls();
		}
	}
}

#define IO_ABORT        13
#define IO_CHECK        14
#define IO_LAND         15

//SHORT
void CPopulous::get_message(SHORT wait)
{
	UBYTE       *send;
	UBYTE       *recieve;
	register SHORT   c1;
	SHORT       c2;
	SHORT       effect;
	SHORT       x,y;
//	UBYTE       rnd;
	UBYTE       *aptr;
	USHORT      send_word;
	USHORT      read_word;
	SHORT       r_check;
	if(serial_off == 0)
    {
		send=stats[player].send;
		recieve=stats[not_player].send;
		if(send[S_X] > 63)
			send[S_X] = 63;
		if(send[S_Y] > 63)
			send[S_Y]=63;
		if(quick_io)
        {
			send_word=compress(send);
			r_check=seed;
			if(player == GOD)
            {
				if((game_turn & 0xf) == 0)
                {
					write_serial(&seed,2,NULL);
					if(read_serial(&r_check,2,(t_text*)(LONG)ESCAPE))
                    {
						serial_message(g_text[IO_ABORT]);
						goto an_error;
                    }
                }
				write_serial(&send_word,2,NULL);
				if(read_serial(&read_word,2,(t_text*)(LONG)ESCAPE))
                {
					serial_message(g_text[IO_ABORT]);
					goto an_error;
                }
				decompress(recieve,read_word);
            }
			else
            {
				if((game_turn & 0xf) == 0)
                {
					if(read_serial(&r_check,2,(t_text*)(LONG)ESCAPE)) // are: Changed to force prototype.
                    {
						serial_message(g_text[IO_ABORT]);
						goto an_error;
                    }
					write_serial(&seed,2,NULL);
                }
				if(read_serial(&read_word,2,(t_text*)(LONG)ESCAPE))
                {
					serial_message(g_text[IO_ABORT]);
					goto an_error;
                }
				write_serial(&send_word,2,NULL);
				decompress(recieve,read_word);
            }            
			if(r_check != seed)
				serial_message(g_text[IO_LAND]);
        }
		else
        {
			send[S_RAND]=seed;
			for(c1=0,send[S_CHECKSUM]=0;c1<=S_RAND;c1++)
				send[S_CHECKSUM]+=send[c1];
			if(player == GOD)
            {
				if(c1=write_full(send))
                {
					serial_message(g_text[IO_ABORT+c1-1]);
					goto an_error;
                }
				if(c1=read_full(recieve,send))
                {
					serial_message(g_text[IO_ABORT+c1-1]);
					goto an_error;
                }
            }
			else
            {
				if(c1=read_full(recieve,send))
                {
					serial_message(g_text[IO_ABORT+c1-1]);
					goto an_error;
                }
				if(c1=write_full(send))
                {
					serial_message(g_text[IO_ABORT+c1-1]);
					goto an_error;
                }
            }
			if(send[S_RAND] != recieve[S_RAND])
				serial_message(g_text[IO_LAND]);        
        }
    }
an_error:;
		 for(c1=0;c1<2;c1++)
		 {
			 if(stats[c1].type == COMPUTER_PLAYER)
				 if(game_turn % stats[c1].computer_speed == 0)
					 stats[c1].computer_done=0;
				 if((effect=stats[c1].send[S_EFFECT]) == 0)
					 continue;
				 x=stats[c1].send[S_X];
				 y=stats[c1].send[S_Y];
				 switch(effect)
				 {
				 case    S_MAGNET:
					 do_magnet(c1,x,y);
					 break;
				 case    S_RAISE:
					 do_raise_point(c1,x,y);
					 break;
				 case    S_LOWER:
					 do_lower_point(c1,x,y);
					 break;
				 case    MANA_QUAKE:
					 do_quake(c1,x,y);
					 break;
				 case    MANA_SWAMP:
					 do_swamp(c1,x,y);
					 break;
				 case    MANA_VOLCANO:
					 do_volcano(c1,x,y);
					 break;
				 case    S_PLACE_GODS:
					 place_people(GOD,x,y,0);
					 break;
				 case    S_PLACE_DEVILS:
					 place_people(DEVIL,x,y,0);
					 break;
				 case    S_PLACE_GODS_LEADER:
					 place_people(GOD,x,y,1);
					 break;
				 case    S_PLACE_DEVILS_LEADER:
					 place_people(DEVIL,x,y,1);
					 break;
				 case    S_PLACE_TREES:
					 if(map_alt[x+y*MAP_WIDTH] < 7)
					 {
						 c2=map_bk2[x+y*MAP_WIDTH];
						 if(c2 < TREE_BLOCK || c2 > LAST_TREE_BLOCK)
							 c2=last_tree;
						 else if(c2 == LAST_TREE_BLOCK)
							 c2=TREE_BLOCK;
						 else
							 c2++;
						 map_bk2[x+y*MAP_WIDTH]=last_tree=c2;
					 }
					 break;
				 case    S_PLACE_ROCKS:
					 if(map_alt[x+y*MAP_WIDTH] < 7)
					 {
						 c2=map_blk[x+y*MAP_WIDTH];
						 if(c2 < ROCK_BLOCK || c2 > LAST_ROCK_BLOCK)
							 c2=last_rock;
						 else if(c2 == LAST_ROCK_BLOCK)
							 c2=ROCK_BLOCK;
						 else
							 c2++;
						 map_blk[x+y*MAP_WIDTH]=last_rock=c2;
					 }
					 break;
				 case    S_CLEAR_WHO:
					 c2=x+y*MAP_WIDTH;
					 if(map_blk[c2] >= ROCK_BLOCK && map_blk[c2] <= ROCK_BLOCK+2)
					 {
						 map_blk[c2]=FLAT_BLOCK;
						 make_map(x,y,x+1,y+1);
					 }
					 map_bk2[c2]=0;
					 aptr=&map_who[c2];
					 if(*aptr)
					 {
						 zero_population(&peeps[*aptr-1],(SHORT)(*aptr-1));
						 map_bk2[x+y*MAP_WIDTH]=0;
						 *aptr=0;
					 }
					 break;
				 case    S_ACTION:
					 if(do_action(c1,x,y))
					 {
						 clear_send();
						 return;
					 }
					 break;
				 default:
					 break;
				 }
		 }
		 clear_send();
}

void CPopulous::clear_send()
{
	SHORT   c1;
	for(c1=0;c1<2;c1++)
    {
		stats[c1].send[S_EFFECT]=0;
		stats[c1].send[S_X]=0;
		stats[c1].send[S_Y]=0;
    }
}

int CPopulous::write_full(UBYTE *send)
{
	UBYTE   error_check = 0;	// are: this wasn't initialised. Hope this is correct.
//	LONG    retry;
	UBYTE   go_again='g';
	while(1)
    {                                                
		write_serial(send,S_CHECKSUM+1,NULL);
		do{
			if(read_serial(&error_check,1,(t_text*)(LONG)ESCAPE))
				return(1);
			if(error_check == (UBYTE)'|')
				return(0);
			clear_serial();
        }while(error_check != 'b');
		write_serial(&go_again,1,NULL);
    }
	return(2);
}

int CPopulous::read_full(UBYTE *recieve,UBYTE *send)
{
//	LONG    retry;
	UBYTE   good='|';
	UBYTE   bad_check='b';
	SHORT   c1;
	UBYTE   check;
	
	while(1)
    {
		if(read_serial(recieve,S_CHECKSUM+1,(t_text*)(LONG)ESCAPE))
			return(1);
		for(c1=check=0;c1 <= S_RAND;c1++)
			check+=recieve[c1];
		if(recieve[S_CHECKSUM] != check)
        {
			clear_serial();
			check=0;
			do{
				if(write_serial(&bad_check,1,(t_text*)(LONG)ESCAPE))
					return(1);
				if(check_serial(1))
					read_serial(&check,1,NULL);
            }while(check != 'g');
        }
		else
        {
			write_serial(&good,1,NULL);
			return(0);
        }
    }
	return(2);
}

int	 CPopulous::do_action(SHORT c1,SHORT x,SHORT y)
{
	SHORT   c2;
	
	switch(y)
    {
    case    SA_MODE:
        if(c1 == player)
            set_tend_icons(tend_x[x],tend_y[x]);
        magnet[c1].flags=x;
        break;
    case    SA_MESSAGE:
        if(serial_off || (stats[!c1].send[S_Y] == SA_MESSAGE && stats[!c1].send[S_EFFECT] == S_ACTION))
            break;
        if(c1 == player)
		{
            get_a_message(message);
            for(c2=0;message[c2];c2++)
                write_serial(&message[c2],1,NULL);
            message[c2]='e';
            write_serial(&message[c2],1,NULL);
            message[c2]=0;
		}
        else
		{
            requester(d_screen,40,60,256,80,&end_ok,NULL);
            text(d_screen,MID_POS(40,256,g_text[1+not_player]),76,g_text[1+not_player]);
            for(c2=0;c2<100;c2++)
			{
                if(read_serial(&message[c2],1,(t_text*)(LONG)ESCAPE))
				{
                    serial_message(g_text[IO_ABORT]);
                    return(1);
				}
                if(message[c2] == 'e')
                    break;
                message[c2+1]=0;
                text(d_screen,48+c2*8,100,&message[c2]);
			}
            while(c2)
			{
                left_button=2;
                while(left_button) UpdatePopulousData();
                if(check_cancel(&end_ok,NULL))
                    break;
                keyboard();
                if(inkey == 0x77 || inkey == 0x79)
                    break;
			}
            left_button=2;
            message[0]=0;
		}
        break;
    case    SA_MANA_WAR:
        do_war(c1);
        break;
    case    SA_MANA_FLOOD:
        do_flood(c1);
        break;
    case    SA_MANA_KNIGHT:
        do_knight(c1);
        break;
    case    SA_PAUSE:
        toggle_icon(back_scr,1,3,BANK2);
        pause=!pause;
        toggle=0;
        break;
    case    SA_COMPUTER_OPTIONS:
        if(stats[!c1].send[S_Y] == SA_COMPUTER_OPTIONS && stats[!c1].send[S_EFFECT] == S_ACTION)
            break;
        if(player == c1)
		{
            if(x)
			{
                options(player);
                c2=player;
			}
            else
			{
                options(not_player);
                c2=not_player;
			}
            if(serial_off == 0)
			{
                *((USHORT *)message)=stats[c2].computer_mode;
                message[2]=c2;
                message[3]=stats[c2].computer_skill;
                message[4]=stats[c2].computer_speed;
                write_serial(message,5,NULL);
			}
		}
        else if(serial_off == 0)
		{
            if(read_serial(message,5,(t_text*)(LONG)ESCAPE))
			{
                serial_message(g_text[IO_ABORT]);
                return(1);
			}
            c2=message[2];
            if(stats[c2].computer_mode != *((USHORT *)message))
                do_message(g_text[8+(c2==player)],0);
            stats[c2].computer_mode=*((USHORT *)message);
            stats[c2].computer_skill=message[3];
            stats[c2].computer_speed=message[4];
		}
        break;      
    case    SA_GAME_OPTIONS:
		if (ThePreferences->Registered)
		{
			if(c1 == player)
			{
				new_map=0;
				c2=serial_off;
				game_options();
				if(c2 == 0)
				{
					*((USHORT *)message)=game_mode;
					message[2]=serial_off;
					message[3]=paint_map;
					message[4]=player;
					message[5]=stats[0].type;
					message[6]=stats[1].type;
					message[7]=new_map;
					message[8]=surender;
					message[9]=saved;
					message[10]=serial_off;
					message[11]=bring_up_two;
					write_serial(message,12,NULL);
				}
			}
			else if(serial_off == 0)
			{
				if(read_serial(message,12,(t_text*)(LONG)ESCAPE))
				{
					serial_message(g_text[IO_ABORT]);
					return 0; // are: Return value added.
				}
				if(game_mode != *((USHORT *)message))
					do_message(g_text[5],0);
				game_mode=*((USHORT *)message);
				if(paint_map == 0 && message[3])
					do_message(g_text[6],0);
				if(serial_off != message[2])
				{
					do_message(g_text[12],0);
				}
				else 
				{
					if(stats[0].type != message[5])
					{
						if(message[5] == COMPUTER_PLAYER)
							do_message(g_text[10],0);
						else
							do_message(g_text[11],0);
					}
					if(stats[1].type != message[6])
					{
						if(message[6] == COMPUTER_PLAYER)
							do_message(g_text[10],0);
						else
							do_message(g_text[11],0);
					}
				}
				serial_off=message[2];
				paint_map=message[3];
				if(not_player != message[4])
				{
					do_message(g_text[7],0);
					if(mode == SCULPT)
						make_arc_mouse(pointer=(not_player)*3+1);
				}
				not_player=message[4];
				player=!not_player;
				stats[0].type=message[5];
				stats[1].type=message[6];
				new_map=message[7];
				surender=message[8];
				serial_off=message[10];
				bring_up_two=message[11];
				if(message[9])
				{
					requester(d_screen,32,60,256,80,&end_ok,&end_cancel);
					text(d_screen,32+16,60+16,g_text[4]);
					do{
						left_button=2;
						while(left_button) UpdatePopulousData();
					}while((c2=check_cancel(&end_ok,&end_cancel)) == 0);
					if(c2==1)
						save_load(1);
				}
			}
			if(new_map)
			{
				if(new_map != 1)
					seed=start_seed;
				setup_display(0,new_map-2);
				return(1);
			}
			if(bring_up_two)
			{
				two_players(1);
				return_to_game();
				return(1);
			}
		}
        break;

	case SA_QUIT:
		game_quit();
		break;

    case    SA_GODS_MANA:
        if(x)
            magnet[GOD].mana/=2;
        else if(magnet[GOD].mana < 100000)
            magnet[GOD].mana=magnet[GOD].mana*2+500;
        break;
    case    SA_DEVILS_MANA:
        if(x)
            magnet[DEVIL].mana/=2;
        else if(magnet[DEVIL].mana < 100000)
            magnet[DEVIL].mana=magnet[DEVIL].mana*2+500;
        break;
    case    SA_ROTATE_MAP:
        rotate_all_map();
        break;
    case    SA_CLEAR_MAP:
        clear_all_map();
        break;
    case    SA_LOAD_LAND:
        ground_in=x;
        if(load_ground(x,1))
            ground_in=x;
        draw_map(0,0,63,63);
        break;
    case    SA_CHEAT:
        cheat=x+1;
        break;
    default:
        break;
    }
	return(0);
}
