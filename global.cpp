#include "computer.h"
#include "god.h"
#include "frontend.h"

// All of this data is globally accessible. 

#define SERIAL_NO			0xC1DB014DL
SHORT						big_mousex;
SHORT						big_mousey;
SHORT						mouse_limit;
LONG						sprite_list[8];
LONG						copper_list[3];
LONG						a_sprite[8];
SHORT						mmousex;
SHORT						mmousey;
LONG						spare_cl;
LONG						screen_origin=(8*8*40+20+2)*4;
LONG						*p_addr=(LONG *)(66L*4L);
SHORT						pal;
//LONG						sprite_data_32;
SHORT						sprites_in=0;
struct	s_serial			serial;
SHORT						cheat;
SHORT						bring_up_two;
USHORT						land_checksum;
SHORT						funny_done;
LONG						score;
USHORT						use_modem;
USHORT						quick_io;
//LONG						sprite_data;
//LONG						blk_data;		// This item has been replaced by new graphics code.
UBYTE						sound_effect=0;
UBYTE						login[20];
SHORT						saved;
UBYTE						filename[20];
UBYTE						drawer[MAX_PATH];
SHORT						dump_screen_number;
SHORT						all_of_city;
SHORT						war;
SHORT						tcmap[32];
SHORT						level_number;
LONG						screen_pointer[3];
SHORT						tempo=50;
SHORT						tempo_now=0;
SHORT						music_now=0;
SHORT						music=10;
SHORT						music_off;//=1;
SHORT						effect_off;//=1;
SHORT						effect=0;
SHORT						beat_two=0;
SHORT						battle_won[2];
SHORT						start_of_game;
SHORT						sound_in;
SHORT						old_view_who;
SHORT						view_timer;
UBYTE						message[40];
SHORT						a_flat_block;
LONG						back_scr;
LONG						w_screen;
LONG						d_screen;
/*volatile*/ SHORT				shift;
/*volatile*/ UBYTE				inkey;
/*volatile*/ UBYTE				asckey;
USHORT						game_turn;
volatile SHORT						 left_button;
//***************************************
struct	m_magnet			magnet[2]=
							{
								{0,0,0,0,0,0},
								{0,0,0,0,0,0}
							};
//*******************************
SHORT						alt[(MAP_WIDTH+1)*(MAP_HEIGHT+1)];
UBYTE						map_who[MAP_WIDTH*MAP_HEIGHT];
UBYTE						map_alt[MAP_WIDTH*MAP_HEIGHT];
UBYTE						map_bk2[MAP_WIDTH*MAP_HEIGHT];
UBYTE						map_blk[MAP_WIDTH*MAP_HEIGHT];

UBYTE						title_map_blk[300];	// Title screen had a smaller map, don't ya know?

USHORT						map_steps[MAP_WIDTH*MAP_HEIGHT];
struct	p_peeps 			peeps[END_OF_FUNNYS+1];
volatile SHORT						 mousex;
volatile SHORT						 mousey;
SHORT						no_peeps;
SHORT						player;
volatile SHORT						 right_button;
USHORT						seed;
USHORT						start_seed;
SHORT						serial_off=1;
SHORT						toggle;
LONG						work_screen;
SHORT						xmax;
SHORT						xmin;
SHORT						xoff;
SHORT						ymax;
SHORT						ymin;
SHORT						yoff;
SHORT						mode=SCULPT;
SHORT						view_who;
SHORT						no_sprites;
struct	s_sprite			sprite[64];
SHORT						view_fight;
SHORT						view_people;
SHORT						ok_to_build;
SHORT						build_count;
SHORT						quake_count;
SHORT						god_magnet;
SHORT						devil_magnet;
SHORT						pause;
SHORT						paint_map;
SHORT						surender=-1;
SHORT						cur_x;
SHORT						cur_y;
SHORT						not_player=1;
SHORT						new_map;
SHORT						last_fire;
SHORT						last_tree=TREE_BLOCK;
SHORT						last_rock=ROCK_BLOCK;
SHORT						p_done;
SHORT						b_rates[]={19200,9600,4800,3600,2400,2000,1800,1200,600,300,200,150,134,110,75,50,300};
SHORT						tend_x[]={3,4,5,4};
SHORT						tend_y[]={3,3,3,4};
struct	f_funny 			funny[3]=
							{
							{-64,0,4,0,-1,1},
							{-65,5,8,0,-64,-1},
							{65,9,12,0,64,1}
							};
struct	s_stats 			stats[2]=
							{
							{"abc",HUMAN_PLAYER   ,1,0,1,0xffff,1,0,0,0,0,0,0,0,5,NULL,NULL,NULL},
							{"abc",COMPUTER_PLAYER,1,0,5,0xffff,3,0,0,0,0,0,0,0,1,NULL,NULL,NULL}
							};
struct	s_stats 			tutorial[2]=
							{
							{"abc",HUMAN_PLAYER   ,1,0,10,0x3f,10,0,0,0,0,0,0,0,5,NULL,NULL,NULL},
							{"abc",COMPUTER_PLAYER,1,0,10,0x3,10,0,0,0,0,0,0,0,1,NULL,NULL,NULL},
							};
USHORT						tutorial_game_mode=0x0001;
UBYTE						map_colour[]=
							{
							14,12,11,11,12,12,11,11,
							13,13,12,12,13,13,12,12,
							14,12,11,11,12,12,11,11,
							13,13,12,12,13,13,12,12,
							25, 25, 25, 25, 25, 25, 25, 25,
							25, 25, 25, 25, 25, 25, 25, 2,
							2, 2, 25, 25, 25, 25, 25, 25,
							25, 25, 25, 25, 25, 25, 25, 25,
							25, 25, 25, 25, 25, 25, 25, 25
							};
SHORT						baud_rate=9600;
SHORT						opposite[]={4,5,6,7,0,1,2,3};
USHORT						palette[]=
							{
							0x000,0x444,0x666,0x888,0xaaa,0xccc,0x620,0x840,
							0xa00,0xa60,0xaa0,0x4a0,0x280,0x262,0x248,0x26c,
							0x000,0x444,0x666,0x888,0xaaa,0xccc,0x620,0x840,
							0xa00,0xa60,0xaa0,0x4a0,0x280,0x262,0x248,0x26c
							};
USHORT						lolpalette[]=
							{
							0x000,0x002,0x023,0x112,0x011,0x032,0x044,0x555,
							0x277,0x777,0x999,0xaaa,0xbb9,0xcda,0x999,0xc77,
							0x000,0x002,0x023,0x112,0x011,0x032,0x044,0x555,
							0x277,0x777,0x999,0xaaa,0xbb9,0xcda,0x999,0xc77,
							};
SHORT						ground_in;
SHORT						to_delta[]={7,6,5,0,0,4,1,2,3};
SHORT						to_offset[]={-64,-63,1,65,64,63,-1,-65};
SHORT						offset_vector[]={0,-64,1,64,-1,-63,65,63,-65,-64*2,1*2,64*2,-1*2,-63*2,65*2,63*2,-65*2,-64*2+1,-64+2,64+2,64*2+1,64*2-1,64-2,-64-2,-64*2-1};
SHORT						dir_sprite[]={-65,-64,-63,1,65,64,63,-1,0};
SHORT						big_city[]={CITY_CENTRE,CITY_WALL2,CITY_WALL1,CITY_WALL2,CITY_WALL1,CITY_TOWER,CITY_TOWER,CITY_TOWER,CITY_TOWER};
LONG						mana_values[]={-250,10,200,2500,5000,7500,10000,40000,80000,160000,1999999};
SHORT						mana_add[11];
SHORT						weapons_add[11];
SHORT						weapons_order[11];
SHORT						population_add[11];
SHORT						battle_add1[11];
SHORT						battle_add2[11];
SHORT						walk_death;
SHORT						pointer=1;
USHORT						game_mode=0x0010;
SHORT						in_conquest=-1;
struct	c_conquest			conquest={1,1,0,0x3f,3,0,15,3,0};
struct	t_text				end_ok={0,0,0,"OK"};
struct	t_text				end_cancel={0,0,0,"CANCEL"};
struct	t_text				option_text[]=
							{
							{36,16,128,"OPTIONS FOR EVIL"},
							{32,32,8,"\200 CAN MODIFY LAND"},
							{32,41,8,"\200 CAN ATTACK TOWNS"},
							{32,50,8,"\200 CAN ATTACK LEADER"},
							{32,59,8,"\200 CAN USE EARTHQUAKES"},
							{32,67,8,"\200 CAN USE SWAMP"},
							{32,76,8,"\200 CAN USE KNIGHT"},
							{32,85,8,"\200 CAN USE VOLCANO"},
							{32,94,8,"\200 CAN USE FLOOD"},
							{32,103,8,"\200 CAN USE ARMAGEDDON"},
							{100,122,32,"AGGRESSION"},
							{72,131,104,"LOW\204\205\205\205\205\205\205\205\205\206HIGH"},
							{120,141,32,"RATE"},
							{64,150,112,"SLOW\204\205\205\205\205\205\205\205\205\206FAST"}
							};
UBYTE						*g_text[]=
							{
							(UBYTE*)"SEND A MESSAGE",
							(UBYTE*)"MESSAGE FROM GOOD",
							(UBYTE*)"MESSAGE FROM EVIL",
							(UBYTE*)"ON TWO PLAYER SERIAL GAME\nSWITCHING BACK TO ONE\n PLAYER GAME IN PAUSE",
							(UBYTE*)"OTHER PLAYER HAS SAVED HIS\nGAME DO YOU WANT TO SAVE\nYOUR GAME NOW AS WELL",
							(UBYTE*)"OTHER PLAYER HAS CHANGED\nGAME OPTIONS",
							(UBYTE*)"OTHER PLAYER HAS SELECTED\nPAINT MODE",
							(UBYTE*)"OTHER PLAYER HAS CHANGED\nSIDES",
							(UBYTE*)"OTHER PLAYER HAS CHANGED\nHIS OPTIONS",
							(UBYTE*)"OTHER PLAYER HAS CHANGED\nYOUR OPTIONS",
							(UBYTE*)"OTHER PLAYER HAS SELECTED\nCOMPUTER ASSISTANCE",
							(UBYTE*)"OTHER PLAYER HAS TURNED OFF\nCOMPUTER ASSISTANCE",
							(UBYTE*)"OTHER PLAYER HAS GONE OFF\nLINE, NOW IN ONE PLAYER",
							(UBYTE*)"SERIAL IO ABORTED",
							(UBYTE*)"SERIAL CHECKSUM ERROR",
							(UBYTE*)"INCOMPATABLE LANDSCAPE ERROR",
							(UBYTE*)"INSERT THE ORIGINAL\nPOPULOUS DISK IN DRIVE 0"
							};
struct	t_text				game_text[]=
							{
							{112,16,72, "GAME SETUP"},
/*
							{16,36,8,	"\200 ONE PLAYER"},
*/
							{16,36,8,	""},
/*
							{152,36,8,	"\200 TWO PLAYERS"},
*/
							{152,36,8,	""},
							{16,46,8,	"\200 PLAY GAME"}, 
							{152,46,8,	"\200 PAINT MAP"},
							{16,56,8,	"\200 GOOD"}, 
							{152,56,8,	"\200 EVIL"},
							{16,66,8,	"\200 HUMAN VS PPC"},
							{152,66,8,	"\200 PPC VS PPC"},
							{16,76,8,	"\200 CONQUEST"}, 
							{152,76,8,	"\200 CUSTOM GAME"},
							{80,95,128, "\200 GAME OPTIONS"},
							{80,105,128, "\200 SAVE A GAME"},
							{80,115,128, "\200 LOAD A GAME"},
							{80,125,128,"\200 MOVE TO NEXT MAP"},
							{80,135,128,"\200 RESTART THIS MAP"},
							{80,145,128,"\200 SURRENDER THIS GAME"}
							};
struct	t_text				c_text[]=
							{
							{16,10,16,	"WORLD TO CONQUER"},
							{16,30,17,	"BATTLE NUMBER IS "},
							{16,38,20,	"WORLDS LANDSCAPE IS "},
							{16,46,18,	"HIS REACTIONS ARE "},
							{16,54,14,	"HIS RATING IS "},
							{16,62,5,	"LAND "},
							{16,70,15,	"THE SWAMPS ARE "},
							{16,78,9,	"WATER IS "},
							{16,88,72,	"               YOU    HIM"},
							{16,96,120,  "POPULATION"},
							{16,104,120, "EARTHQUAKES"},
							{16,112,120, "SWAMPS"},
							{16,120,120, "KNIGHTS"},
							{16,128,120, "VOLCANOS"},
							{16,136,120, "FLOODS"}
							};
UBYTE						*con_text[]=
							{
							(UBYTE*)"GRASS PLANES",
							(UBYTE*)"DESERT",
							(UBYTE*)"SNOW AND ICE",
							(UBYTE*)"ROCKY",
							(UBYTE*)"VERY SLOW",
							(UBYTE*)"SLOW",
							(UBYTE*)"MEDIUM",
							(UBYTE*)"FAST",
							(UBYTE*)"VERY FAST",
							(UBYTE*)"VERY POOR",
							(UBYTE*)"POOR",
							(UBYTE*)"AVERAGE",
							(UBYTE*)"GOOD",
							(UBYTE*)"VERY GOOD",
							(UBYTE*)"CANNOT BE BUILT",
							(UBYTE*)"BUILT JUST ON TOWNS",
							(UBYTE*)"ONLY BUILT UP",
							(UBYTE*)"BUILT ON PEOPLE",
							(UBYTE*)"SHALLOW",
							(UBYTE*)"BOTTOMLESS",
							(UBYTE*)"HARMFUL",
							(UBYTE*)"FATAL",
							(UBYTE*)"YES",
							(UBYTE*)"NO",
							(UBYTE*)"MORTAL",
							(UBYTE*)"IMMORTAL",
							(UBYTE*)"ETERNAL",
							(UBYTE*)"DEVA",
							(UBYTE*)"GREATER BEING",
							(UBYTE*)"DEITY",
							(UBYTE*)"GREATER DEITY",
							(UBYTE*)"MORTAL GOD",
							(UBYTE*)"GREATER GOD",
							(UBYTE*)"ETERNAL GOD",
							(UBYTE*)"WELL DONE ",
							(UBYTE*)" YOU CONQUERED ",
							(UBYTE*)" NOW BATTLE AT ",
							(UBYTE*)"WELL DONE YOU HAVE CONQUERED EVIL",
							(UBYTE*)"THE BATTLE IS OVER BUT TRY "
							};
SHORT						end_words[]={0,1,3,5,6};
SHORT						words[]=
							{
							9,-1, 5,-1, 3,-1, 7,-1, 1,8, 2,-1, 1,2, 9,6, 
							1,6, 3,6
							};
struct	t_text				end_text[]=
							{
							{112,10,72, "GAME LOST"},
							{16,20,24,	"                   YOU  HIM"},
							{16,35,24,	"BATTLES WON        0000 0000"},
							{16,45,24,	"NUMBER OF KNIGHTS  0000 0000"},
							{16,55,24,	"NUMBER OF TOWNS    0000 0000"},
							{16,65,24,	"NUMBER OF CASTLES  0000 0000"},
							{16,80,24,	"YOUR SCORE         000000000"}
							};
struct	t_text				set_text[]=  
							{
							{48,25,118, "SET GAME OPTIONS"},
							{16,40,8,	"\200 WATER IS HARMFUL"},
							{16,40,8,	"\201 WATER IS FATAL"},
							{16,50,8,	"\200 SWAMPS SHALLOW"},
							{16,50,8,	"\201 SWAMPS BOTTOMLESS"},
							{16,60,8,	"\200 CAN BUILD"},
							{16,60,8,	"\201 CANNOT BUILD"},
							{16,70,8,	"\200 BUILD UP AND DOWN"},
							{16,70,8,	"\201 ONLY BUILD UP"},
							{16,80,8,	"\200 BUILD NEAR PEOPLE"},
							{16,80,8,	"\201 BUILD NEAR TOWNS"},
							};
UBYTE						blank[]="^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
UBYTE						blank_box[]="\202\202\202\202\202\202\202\202\202\202\202\202\202\202\202\202\202\202\202";
struct	t_text				serial_text[]=
							{
							{16,16,0,	"STARTUP SERIAL"},
							{16,31,208, "SEND LOGIN"},
							{16,41,208, "BAUD RATE"},
							{16,51,80,	"\200 DATALINK"},
							{16,61,96,	"\200 FULL SERIAL CHECKING"},
							{16,71,88,	"\200 I AM GOOD"},
							{16,81,168, "\201 USE THIS LANDSCAPE"},
							{16,91,96,	"\200 CONNECT ME"},
							{16,51,56,	"\201 MODEM"},
							{16,61,88,	"\201 NO SERIAL CHECKING"},
							{16,71,88,	"\201 I AM EVIL"},
							{16,81,176, "\201 USE SAVED LANDSCAPE"},
							{16,101,11, "HE'S TRYING TO BE GOOD"},
							{16,101,11, "HE'S TRYING TO BE EVIL"},
							{16,101,11, "HE'S NOT USING A SAVED GAME"},
							{16,101,11, "HE'S USING A SAVED GAME"},
							{16,101,11, "SERIAL ERROR!"},
							{16,101,11, "HE IS USING NO CHECKING"},
							{16,101,11, "HE IS USING FULL CHECKING"},
							{16,101,11, "INCOMPATABLE SAVED GAMES"},
							};
UBYTE						*start[]=
							{
							(UBYTE*)"RING",(UBYTE*)"VERY",(UBYTE*)"KILL",(UBYTE*)"SHAD",(UBYTE*)"HURT",(UBYTE*)"WEAV",(UBYTE*)"MIN",(UBYTE*)"EOA",
							(UBYTE*)"COR",(UBYTE*)"JOS",(UBYTE*)"ALP",(UBYTE*)"HAM",(UBYTE*)"BUR",(UBYTE*)"BIN",(UBYTE*)"TIM",(UBYTE*)"BAD",
							(UBYTE*)"FUT",(UBYTE*)"MOR",(UBYTE*)"SAD",(UBYTE*)"CAL",(UBYTE*)"IMM",(UBYTE*)"SUZ",(UBYTE*)"NIM",(UBYTE*)"LOW",
							(UBYTE*)"SCO",(UBYTE*)"HOB",(UBYTE*)"DOU",(UBYTE*)"BIL",(UBYTE*)"QAZ",(UBYTE*)"SWA",(UBYTE*)"BUG",(UBYTE*)"SHI"
							};
UBYTE						*end[]=
							{
							(UBYTE*)"HILL",(UBYTE*)"TORY",(UBYTE*)"HOLE",(UBYTE*)"PERT",(UBYTE*)"MAR",(UBYTE*)"CON",(UBYTE*)"LOW",(UBYTE*)"DOR",
							(UBYTE*)"LIN",(UBYTE*)"ING",(UBYTE*)"HAM",(UBYTE*)"OLD",(UBYTE*)"PIL",(UBYTE*)"BAR",(UBYTE*)"MET",(UBYTE*)"END",
							(UBYTE*)"LAS",(UBYTE*)"OUT",(UBYTE*)"LUG",(UBYTE*)"ILL",(UBYTE*)"ICK",(UBYTE*)"PAL",(UBYTE*)"DON",(UBYTE*)"ORD",
							(UBYTE*)"OND",(UBYTE*)"BOY",(UBYTE*)"JOB",(UBYTE*)"ER",(UBYTE*)"ED",(UBYTE*)"ME",(UBYTE*)"AL",(UBYTE*)"T"
							};
UBYTE						*mid[]=
							{
							(UBYTE*)"OUT",(UBYTE*)"QAZ",(UBYTE*)"ING",(UBYTE*)"OGO",(UBYTE*)"QUE",(UBYTE*)"LOP",(UBYTE*)"SOD",(UBYTE*)"HIP",
							(UBYTE*)"KOP",(UBYTE*)"WIL",(UBYTE*)"IKE",(UBYTE*)"DIE",(UBYTE*)"IN" ,(UBYTE*)"AS" ,(UBYTE*)"MP" ,(UBYTE*)"DI" ,
							(UBYTE*)"OZ" ,(UBYTE*)"EA" ,(UBYTE*)"US" ,(UBYTE*)"GB" ,(UBYTE*)"CE" ,(UBYTE*)"ME" ,(UBYTE*)"DE" ,(UBYTE*)"PE" ,
							(UBYTE*)"OX" ,(UBYTE*)"A"  ,(UBYTE*)"E"  ,(UBYTE*)"I"  ,(UBYTE*)"O"  ,(UBYTE*)"U"  ,(UBYTE*)"T"  ,(UBYTE*)"Y"
							};
SBYTE						 a_flat[]=
							{
							-4,-4,-4,-3,-4,-2,-4,-1,-4, 0,-4, 1,-4, 2,-4, 3,-4, 4 ,
							-3,-4,-3,-3,-3,-2,-3,-1,-3, 0,-3, 1,-3, 2,-3, 3,-3, 4 ,
							 4,-4, 4,-3, 4,-2, 4,-1, 4, 0, 4, 1, 4, 2, 4, 3, 4, 4 ,
							 3,-4, 3,-3, 3,-2, 3,-1, 3, 0, 3, 1, 3, 2, 3, 3, 3, 4 ,
							-2,-4,-2,-3,-2,-2,-2,-1,-2, 0,-2, 1,-2, 2,-2, 3,-2, 4 ,
							 2,-4, 2,-3, 2,-2, 2,-1, 2, 0, 2, 1, 2, 2, 2, 3, 2, 4 ,
							-1,-4,-1,-3,-1,-2,-1,-1,-1, 0,-1, 1,-1, 2,-1, 3,-1, 4 ,
							 1,-4, 1,-3, 1,-2, 1,-1, 1, 0, 1, 1, 1, 2, 1, 3, 1, 4 ,
							 0,-4, 0,-3, 0,-2, 0,-1, 0, 0, 0, 1, 0, 2, 0, 3, 0, 4 
							};
UBYTE						dirs_buffer[32000];

//SHORT                   big_mousex;
//SHORT                   big_mousey;
//SHORT                   mouse_limit;
//LONG                    sprite_list[8];
//LONG                    copper_list[4];
//LONG                    a_sprite[8];
//SHORT                   mmousex;
//SHORT                   mmousey;
//LONG                    spare_cl;
//LONG                    screen_origin=8*8*40+20+2;
//SHORT                   pal;
LONG                    sprite_data_32;
LONG                    current_copper;
LONG                    old_copper;

struct  b_building      building[20];
//LONG                    screen_pointer[4]={0,0,0,0};
LONG                    blk_data;
//LONG                    pointer=7;
LONG                    selected;
//SHORT                   sound_in;
//UBYTE                   sound_effect;
//SHORT                   tcmap[32];
//SHORT                   tempo=50;
//SHORT                   tempo_now;
///SHORT                   music_now;
//SHORT                   music;
//SHORT                   music_off=1;
//SHORT                   effect_off=1;
//SHORT                   effect;
//SHORT                   beat_two;
//LONG                    back_scr;
//LONG                    w_screen;
//LONG                    d_screen;
//SHORT                   shift;
//UBYTE                   inkey;
//UBYTE                   asckey;
//SHORT                   left_button;
//SHORT                   right_button;
//SHORT                   mousex;
//SHORT                   mousey;
SHORT                   extras[100];
SHORT                   e_vels[100];
UBYTE                   sprite_data[SPRITE_DATA];
LONG                    text_line[2];
UBYTE                   font[4400];
SHORT                   s_size;
USHORT                  game_name[] = {0,1,0,2,3,1,2,4};
LONG                    b_screen;
LONG                    o_screen;
SHORT                   oldrez;

//USHORT                  seed;
//SHORT                   toggle;
//LONG                    work_screen;
LONG                    ts1 = 40;
LONG                    ts2 = 40;  
UBYTE                   **dests;           

UBYTE res_data[] = 
					{
							0xFF,0xFF,
							0xFF,0xFF,
							0xFF,0xFF,
							0xFF,0xFD,
							0xEF,0xBF,
							0xBD,0xEF,
							0xEF,0x76,
							0x6E,0xB7,
							0xB5,0x6D,
							0xD5,0xAA,
							0x55,0x55,
							0x4A,0xA9,
							0x25,0x52,
							0x49,0x24,
							0x22,0x12,
							0x84,0x10,
							0x20,0x80,
							0x00,0x10,
							0x00,0x00,
							0x00,0x00,
							0x00,0x00,
							0x00,0x00,
							0x00,0x00,
							0x00,0x00,
							0x00,0x00,
							0x00,0x00,
							0x00,0x00,
							0x00,0x00,
							0x00,0x00
					};      

bool					is_save_load = false;        