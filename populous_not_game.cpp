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


#include	"Computer.h"
#include	<stdio.h>
#include	<string.h>	// ?
//#include	"bbc.h"     // no such file...... Havw no idea what it defines either.... shit.....
#include	"God.h"
#include	"Extern.h"
#include	"Score.h"


extern	UBYTE						dirs_buffer[];


#define TOP_LEFT        8+4
#define TOP             9+4
#define TOP_RIGHT       10+4
#define BOTTOM_LEFT     11+4
#define BOTTOM          12+4
#define BOTTOM_RIGHT    13+4
#define RIGHT           15+4
#define LEFT            14+4
#define BLANK           16+4
#define BOX_LEFT        17+4
#define BOX_MIDDLE      18+4
#define BOX_RIGHT       19+4

#define ICON1           1
#define ICON2           2
#define MID_POS(l,w,s) ((l)+(w)/2-(strlen((char*)s)*8)/2)

void CPopulous::requester(LONG screen,SHORT x,SHORT y,SHORT width,SHORT height,t_text *ender1,t_text *ender2)
{
	register SHORT   c1;
	register SHORT   c2;
	SHORT   c3,c4;
	SHORT   t_len;
	/* do top line */
	tempo=0;
	x>>=4;
	width>>=4;
	t_len=strlen((char*)ender1->text)>>1;
	ms_off();
	/*******Top line*******/
	draw_icon(screen,x,y,TOP_LEFT);
	for(c1=1;c1<width-1;c1++)
		draw_icon(screen,x+c1,y,TOP);
	draw_icon(screen,x+c1,y,TOP_RIGHT);
	/*******Main Body*******/
	for(c1=y+16;c1 < y+height-16;c1+=16)
    {
		draw_icon(screen,x,c1,LEFT);
		for(c2=1;c2<width-1;c2++)
			draw_icon(screen,x+c2,c1,BLANK);
		draw_icon(screen,x+c2,c1,RIGHT);
    }
	/*******bottom line*******/
	draw_icon(screen,x,c1,BOTTOM_LEFT);
	c2=1;
	ender1->y=c1;
	if(ender2)
    {
		ender1->x=(x+c2)<<4;
		ender2->y=ender1->y;
		draw_icon(screen,x+c2++,c1,BOX_LEFT);
		for(c4=0;c4 < t_len;c4++,c2++)
			draw_icon(screen,x+c2,c1,BOX_MIDDLE);
		draw_icon(screen,x+c2++,c1,BOX_RIGHT);
		text(screen,ender1->x+16,c1+3,ender1->text);
		for(;(int)c2 < (int)(width-(strlen((char*)ender2->text)>>1)-2-1); c2++)
			draw_icon(screen,x+c2,c1,BOTTOM);
		ender2->x=(x+c2)<<4;
		draw_icon(screen,x+c2++,c1,BOX_LEFT);
		for(c4=0; (int)c4 < (int)(strlen((char*)ender2->text)>>1);c4++,c2++)
			draw_icon(screen,x+c2,c1,BOX_MIDDLE);
		draw_icon(screen,x+c2++,c1,BOX_RIGHT);
		ender2->width=ender2->x+((strlen((char*)ender2->text)+4)<<3);
		text(screen,ender2->x+16,c1+3,ender2->text);
    }    
	else
    {
		for(c2=1;c2<(width>>1)-(t_len>>1)-1;c2++)
			draw_icon(screen,x+c2,c1,BOTTOM);
		ender1->x=(x+c2)<<4;
		draw_icon(screen,x+c2++,c1,BOX_LEFT);
		c3=c2;
		for(c4=0;c4 < t_len;c4++,c2++)
			draw_icon(screen,x+c2,c1,BOX_MIDDLE);
		draw_icon(screen,x+c2++,c1,BOX_RIGHT);
		for(;c2<width-1;c2++)
			draw_icon(screen,x+c2,c1,BOTTOM);
    }
	ender1->width=ender1->x+(( (strlen((char*)ender1->text)+4) )<<3);
	draw_icon(screen,x+c2,c1,BOTTOM_RIGHT);
	text(screen,ender1->x+16,c1+3,ender1->text);
	ms_on();
}

void	CPopulous::toggle_button(SHORT x,SHORT y,t_text *toggle)
{
	toggle->text[0]=(toggle->text[0] == 128) ? 129 : 128;
	text(d_screen,x+toggle->x,y+toggle->y,toggle->text);
}

// Moved here from about 100 lines down
#define R_LEFT  72
#define R_TOP   16

void	CPopulous::get_name(LONG screen,UBYTE *str,SHORT x,SHORT y,SHORT width,t_text *ender1,t_text *ender2)
{
	UBYTE       blank[40];
	SHORT       c1;
	SHORT       c2;
	SHORT       cur_pos=strlen((char*)str);
	
	c2 = 0;	// c2 is a latch, locked by str[n] == 0. Once set all following chars are set to 130.
	for(c1=0;c1 != width;c1++)
    {
		if(str[c1] == 0)
			c2=1;
		if(str[c1] <= 32 || c2)
			str[c1]=130;
		blank[c1]=130;
    }
	blank[c1]=str[c1]=0;
	left_button=2;
	/*toggle=1;*/
	text(screen,x,y,blank);
	c2=0;
	while(1)
    {
		UpdatePopulousData();

		if((c2++) % (width+20) == 0)
        {
			str[cur_pos]=130+(str[cur_pos] == 130);
			c2 = 1; // Prevent a possible overflow?
        }
		text(screen,x,y,str);
	
		if (	(mousex >= R_LEFT+16 && mousex <= R_LEFT+16+128)
			&&	(mousey >= R_TOP+18 && mousey <= R_TOP+18+12*8))	// Taken from line 285, of this file.
		{
			// User is trying to click on list 
			// at the top of this page.
			break;
		}

		if(keyboard())
        {
			if (	(asckey >= 0x20)
				&&	(asckey <= 0x7a)
				&&	(cur_pos < (width-1))
				)
            {
				str[cur_pos]=130;
				text(screen,x,y,str);
				if (	(asckey >= 0x61)
					&&	(asckey <= 0x7a))
					asckey-=0x20;
				str[cur_pos]=asckey;
				cur_pos++;
            }
			else if(	(	(asckey == 0x08)
						||	(asckey == 0x7f))
					&&	(cur_pos)
					)  /* delete character */
            {
				str[cur_pos]=130;
				cur_pos--;
            }
			else if(asckey == 0x0d)
            {
				break;
            }
        }
		if(c1=check_cancel(ender1,ender2))
        {
			if(c1 == ICON2)
            {
				str[0]=0;
				return;
            }
			else 
				break;
        }
		if(!left_button)
			left_button=2;
    }
	str[cur_pos]=130;
	text(screen,x,y,str);
	for(c1=width-1;	(c1 >= 0) && (((unsigned char)str[c1]) > 128) ;c1--) 
	{
		str[c1]=0;
	}

	TempString = (char*)str;
	ODCSN((&TempString));
}

// are: Moved toward the start of the file as I've used the values elsewhere.
//#define R_LEFT  72
//#define R_TOP   16

int		CPopulous::save_load(SHORT is_save)
{
	UBYTE           save_load[10];
	UBYTE           tmp[70];
	UBYTE           *buffer=(UBYTE *)dirs_buffer;
	struct  t_text  ok;
	SHORT           no_dirs;
	SHORT           top_dir;
	SHORT           c1,c2,c3,c4;
	SHORT           status=0xf;

	is_save_load = true;
	//LONG            fpz;
	if(is_save)
    {
		strcpy((char*)ok.text,"SAVE");
		strcpy((char*)save_load,"SAVE GAME");
    }
	else
    {
		strcpy((char*)ok.text,"LOAD");
		strcpy((char*)save_load,"LOAD GAME");
    }
	requester(d_screen,R_LEFT,R_TOP,176,176,&ok,&end_cancel);
	text(d_screen,R_LEFT+16,R_TOP+8,save_load);
	text(d_screen,R_LEFT+144,R_TOP+18,(UBYTE*)"\210");
	text(d_screen,R_LEFT+144,R_TOP+106,(UBYTE*)"\211");
	text(d_screen,R_LEFT+16,R_TOP+135,(UBYTE*)"FILE NAME");
	text(d_screen,R_LEFT+16,R_TOP+144,blank_box+3);
	
	do{
		if(status & 0x01)
        {
			for(c1=0;c1<12;c1++)
				text(d_screen,R_LEFT+16,R_TOP+18+c1*8,blank_box+3);
			if(no_dirs=get_a_dir(drawer,buffer))
				sort(no_dirs,buffer);
			top_dir=0;
        }
		if(status & 0x02)
        {
			for(c1=0;c1<12;c1++)
				text(d_screen,R_LEFT+16,R_TOP+18+c1*8,blank_box+3);
			for(c1=0,c2=top_dir;c1 < 12;c1++,c2++)
            {
				c4=(c2 <no_dirs) ? strlen((char*)&buffer[(c2<<5)]) : 0;
				for(c3=0;c3<16;c3++)
					if(c3 < c4)
						tmp[c3]=buffer[(c2<<5)+c3];
					else
						tmp[c3]=130;
					tmp[c3]=0;
					text(d_screen,R_LEFT+16,R_TOP+18+c1*8,tmp);
            }
        }
		if(status & 0x08)
        {
			c4=strlen((char*)filename);
			for(c3=0;c3<16;c3++)
				if(c3 < c4)
					tmp[c3]=filename[c3];
				else
					tmp[c3]=130;
				tmp[c3]=0;
				text(d_screen,R_LEFT+16,R_TOP+144,blank_box+3);
				text(d_screen,R_LEFT+16,R_TOP+144,tmp);
        }
		status=0;
		left_button=2;
		while(left_button)
		{
			UpdatePopulousData();
			if (Redraw)
			{
				copy_screen(w_screen,d_screen);
				if(is_save)
				{
					strcpy((char*)ok.text,"SAVE");
					strcpy((char*)save_load,"SAVE GAME");
				}
				else
				{
					strcpy((char*)ok.text,"LOAD");
					strcpy((char*)save_load,"LOAD GAME");
				}
				requester(d_screen,R_LEFT,R_TOP,176,176,&ok,&end_cancel);
				text(d_screen,R_LEFT+16,R_TOP+8,save_load);
				text(d_screen,R_LEFT+144,R_TOP+18,(UBYTE*)"\210");
				text(d_screen,R_LEFT+144,R_TOP+106,(UBYTE*)"\211");
				text(d_screen,R_LEFT+16,R_TOP+135,(UBYTE*)"FILE NAME");
				text(d_screen,R_LEFT+16,R_TOP+144,blank_box+3);
				Redraw = false;
			}

			if(mousex >= R_LEFT+16 && mousex <= R_LEFT+16+128)
			{
				if(mousey >= R_TOP+18 && mousey <= R_TOP+18+12*8)
				{
					c1=((mousey-(R_TOP+18))>>3)+top_dir;
					if(c1 < no_dirs)
					{
						strcpy((char*)filename,(char*)&buffer[(c1<<5)]);
						status=0x08;
					}
				}
				else if(mousey >= R_TOP+144 && mousey <= R_TOP+144+8)
				{
					get_name(d_screen,filename,R_LEFT+16,R_TOP+144,16,&ok,&end_cancel);
				}
			}
			else if(mousex >= R_LEFT+144 && mousex <=R_LEFT+144+8)
			{
				if(mousey >= R_TOP+18 && mousey <= R_TOP+18+8 && top_dir)
				{
					top_dir--;
					status=0x02;
				}
				else if(mousey >= R_TOP+18+11*8 && mousey <= R_TOP+18+11*8+8 && top_dir+5 < no_dirs)
				{
					top_dir++;
					status=0x02;
				}
			}
		}
		if(c1=check_cancel(&ok,&end_cancel))
        {
			left_button=2;
			if(c1 == 1 && filename[0])
            {
				strcpy((char*)tmp,(char*)drawer);
				c2=strlen((char*)tmp)-1;
				if(c2 > 0)
                {
					if (tmp[c2] != '\\') // && tmp[c2] != '/')
						tmp[++c2]='\\';
					tmp[++c2]=0;
                }      
				strcat((char*)tmp,(char*)filename);
				strcat((char*)tmp, ".sav");
				if(is_save)
                {
					if(save_game(tmp))
                    {
						text(d_screen,R_LEFT+16,R_TOP+135,&blank[11]);
						text(d_screen,R_LEFT+16,R_TOP+135,(UBYTE*)"ERROR IN SAVING");
						left_button=2;
						right_button=2;
						while(left_button && right_button) UpdatePopulousData();
						text(d_screen,R_LEFT+16,R_TOP+135,&blank[11]);
						text(d_screen,R_LEFT+16,R_TOP+135,(UBYTE*)"FILE NAME");
						continue;
                        status=0x3;
                    }
					else
                    {
						saved=1;
                    }
                }
				else
					load_game(tmp);

				is_save_load = false;
				return(1);
            }
			is_save_load = false;
			return(0);
        }
    }while(1);
	is_save_load = false;
}        

void	CPopulous::sort(SHORT n,UBYTE *buffer)
{
	SHORT   m;
	register SHORT   h,i,j,k;
	UBYTE   tmp[40];
	m=n;
	while(m>>=1)
    {
		k=n-m;
		j=1;
		do{
			i=j;
			do{
				h=i+m;
				if(strcmp((char*)&buffer[(i-1)<<5],(char*)&buffer[(h-1)<<5]) > 0)
                {
					strcpy((char*)tmp,(char*)&buffer[(i-1)<<5]);
					strcpy((char*)&buffer[(i-1)<<5],(char*)&buffer[(h-1)<<5]);
					strcpy((char*)&buffer[(h-1)<<5],(char*)tmp);
					i-=m;
                }
				else
					break;
            }while(i >= 1);
			j++;
        }while(j <= k);
    }
}

SHORT		CPopulous::do_message(UBYTE *tmp,SHORT all)
{
	struct  t_text  *end_c;
	SHORT   ret;
	if(all)
		end_c=&end_cancel;
	else
		end_c=NULL;
	requester(d_screen,40,60,256,48,&end_ok,end_c);
	text(d_screen,40+16,60+12,tmp);
	do{
		left_button=2;
		while(left_button) UpdatePopulousData();
    }while((ret=check_cancel(&end_ok,end_c))==0);
	left_button=2;
	return(ret);
}

void	CPopulous::get_a_message(UBYTE *tmp)
{
	requester(d_screen,40,60,256,80,&end_ok,&end_cancel);
	text(d_screen,MID_POS(40,256,g_text[0]),76,g_text[0]);
	tmp[0]=0;
	get_name(d_screen,tmp,48,100,28,&end_ok,&end_cancel);
	left_button=2;
}

void	CPopulous::serial_message(UBYTE *tmp)
{
	USHORT  cancel;
	SHORT   c1;
	requester(d_screen,40,60,256,80,&end_ok,NULL);
	text(d_screen,40+16,60+16,tmp);
	text(d_screen,40+16,60+16+10,g_text[3]);
	if(quick_io)
    {
		message[0]=S_CHECK;
		cancel=compress(message);
		for(c1=0;c1<10;c1++)
			write_serial(&cancel,S_CHECKSUM+1,NULL);
    }
	do{
		left_button=2;
		while(left_button) UpdatePopulousData();
    }while(check_cancel(&end_ok,NULL)==0);
	clear_serial();
	left_button=2;
	serial_off=1;
	pause=!pause;
	toggle=0;
	toggle_icon(back_scr,1,3,BANK2);
	clear_send();
	stats[not_player].type=COMPUTER_PLAYER;
}

#define TWO_LOGIN       1
#define TWO_BAUD        2
#define TWO_MODEM       3
#define TWO_FAST        4
#define TWO_WHO         5
#define TWO_SAVED       6
#define TWO_CONNECT     7
#define TWO_LINES       8
#define TWO_TRYGOOD     12
#define TWO_LAND        14
#define TWO_ERROR       16
#define TWO_GAP         5
#define TWO_QUICK       17
#define TWO_LAND_ERROR  19

int		CPopulous::two_players(SHORT in_serial)
{
	SHORT   c1,c2;
	UBYTE   baud_text[33];
	SHORT   setup=999;
	SHORT   you_are;
	SHORT   use_random=1;
	struct  t_text  *the_end;
	struct  t_text  cont;
	if(bring_up_two)
    {
		strcpy((char*)cont.text,"CONTINUE");
		the_end=&cont;
		clear_serial();
    }
	else
		the_end=&end_cancel;
	you_are=player;
	while(1)
    {
		if(setup)
        {
			requester(d_screen,32,46,256,128,the_end,NULL);
			if(setup == 2)
				text(d_screen,32+serial_text[TWO_TRYGOOD+you_are].x,
				46+serial_text[TWO_TRYGOOD+you_are].y,serial_text[TWO_TRYGOOD+you_are].text);
			else if(setup == 3)
				text(d_screen,32+serial_text[TWO_LAND+use_random].x,
				46+serial_text[TWO_LAND+use_random].y,serial_text[TWO_LAND+use_random].text);
			else if(setup == 4)
				text(d_screen,32+serial_text[TWO_ERROR].x,46+serial_text[TWO_ERROR].y,serial_text[TWO_ERROR].text);
			else if(setup == 5)
				text(d_screen,32+serial_text[TWO_QUICK+quick_io].x,46+serial_text[TWO_QUICK+quick_io].y,serial_text[TWO_QUICK+quick_io].text);
			else if(setup == 6)
            {
				text(d_screen,32+serial_text[TWO_LAND_ERROR].x,46+serial_text[TWO_LAND_ERROR].y,serial_text[TWO_LAND_ERROR].text);
				use_random=1;
				sprintf((char*)baud_text,"%d",land_checksum);
				text(d_screen,0,0,baud_text);
            }
			c2=serial_text[TWO_LOGIN].x+32+11*8;
			sprintf((char*)baud_text,"%d",baud_rate);
			if(use_modem)
            {
				text(d_screen,c2,46+serial_text[TWO_LOGIN].y,&blank_box[strlen((char*)blank_box)-17]);
				text(d_screen,c2,46+serial_text[TWO_LOGIN].y,login);
            }
			text(d_screen,c2,46+serial_text[TWO_BAUD].y,&blank_box[strlen((char*)blank_box)-6]);
			text(d_screen,c2,46+serial_text[TWO_BAUD].y,baud_text);
        }
		else
			text(d_screen,32+serial_text[TWO_ERROR].x,46+serial_text[TWO_ERROR].y,
			blank);
		for(c1=0;c1<TWO_LINES;c1++)
        {
			if(use_modem == 0)
				if(c1 == TWO_LOGIN || c1 == TWO_FAST)
					continue;
				if(c1 == TWO_MODEM)
					c2=c1+use_modem*TWO_GAP;
				else if(c1 == TWO_FAST)
					c2=c1+quick_io*TWO_GAP;
				else if(c1 == TWO_WHO)
					c2=c1+you_are*TWO_GAP;
				else if(c1 == TWO_SAVED)
					c2=c1+(use_random == 0)*TWO_GAP;
				else 
					c2=c1;
				text(d_screen,32+serial_text[c2].x,46+serial_text[c2].y,
					serial_text[c2].text);
        }
		
		setup=0;
		left_button=2;    
		while(left_button) 
		{
			UpdatePopulousData();	// This could be wrong
			waittof();
		}
		for(c1=1;c1 < TWO_LINES;c1++)
			if(mousex >= 32+serial_text[c1].x && mousex <= 32+serial_text[c1].x+serial_text[c1].width)
				if(mousey >= 46+serial_text[c1].y && mousey <= 46+serial_text[c1].y+8)
					break;
				c2=serial_text[TWO_LOGIN].x+32+11*8;
				switch(c1)
				{
				case    TWO_LOGIN:
					if(use_modem)
					{
						get_name(d_screen,login,c2,46+serial_text[c1].y,17,the_end,NULL);
						if(login[0])
						{
							for(c2=0;login[c2];c2++)
							{
								waittof();
								write_serial(&login[c2],1,NULL);
								waittof();
							}
							waitfor(75);
							write_serial("\015",1,NULL);
						}
					}
					break;
				case    TWO_BAUD:
					get_name(d_screen,baud_text,c2,46+serial_text[c1].y,6,the_end,NULL);
					if(baud_text[0])
					{
						baud_rate=atoi((char*)baud_text);
						for(c2=0;c2<16 && baud_rate < b_rates[c2];c2++);
						baud_rate=b_rates[c2];
						set_baud_rate();
					}
					break;
				case    TWO_MODEM:
					use_modem=!use_modem;
					setup=1;
					break;
				case    TWO_FAST:
					if(use_modem)
						quick_io=!quick_io;
					break;
				case    TWO_WHO:
					you_are=!you_are;
					break;
				case    TWO_SAVED:
					if(use_random)
					{
						if(save_load(0))
						{
							c2=player;
							player=you_are;
							use_random=0;
							Setscreen(w_screen,d_screen,-1);
							player=c2;
						}
						setup=1;
					}
					else
						use_random=1;
					break;
				case    TWO_CONNECT:
try_it:;        
	   if((setup=try_serial(you_are,use_random)) == 1)
	   {
		   if(use_random == 0)
		   {
			   set_tend_icons(-1,-1);
			   set_mode_icons(-1,-1);
		   }
		   player=you_are;
		   not_player=!player;
		   if(in_serial == 0 || use_random)
		   {
			   seed=start_seed;
			   game_turn=0;
		   }
		   serial_off=0;
		   if(pause)
			   toggle_icon(back_scr,1,3,BANK2);
		   pause=0;
		   toggle=0;
		   stats[0].type=stats[1].type=HUMAN_PLAYER;
		   if(use_random && in_serial == 0)
			   setup_display(0,-2);
		   else if(use_random == 0)
		   {
			   set_tend_icons(-1,-1);
			   set_mode_icons(-1,-1);
		   }
		   return(1);
	   }
	   break;
				default:
					break;
				}
				if(c1 >= TWO_MODEM && c1 <= TWO_SAVED)
					text(d_screen,32+serial_text[c1].x,46+serial_text[c1].y,
					blank);
				if(c1=check_cancel(the_end,NULL))
				{
					if(c1 == 1 && in_serial)
						goto try_it;
					break;
				}
    }
	return(0);
}

int		CPopulous::try_serial(SHORT you_are,SHORT use_random)
{
	USHORT  i_am[15];
	USHORT  he_is[15];	// are: Not initialised
	UBYTE   rep[12];
	UBYTE   send = 0; // are: Wasn't initialised
	USHORT  counter=1;
	//	SHORT   c1;
	i_am[0]=you_are;
	i_am[1]=start_seed;
	i_am[2]=game_mode;
	i_am[3]=stats[you_are].computer_skill;
	i_am[4]=stats[you_are].computer_mode;
	i_am[5]=stats[you_are].computer_speed;
	i_am[6]=use_random;
	i_am[7]=ground_in;
	i_am[8]=paint_map;
	i_am[9]=quick_io;
	i_am[10]=land_checksum;
	requester(d_screen,48,70,228,64,&end_cancel,NULL);
	ms_on();
	text(d_screen,64,88,(UBYTE*)"TRY NUMBER ");
	left_button=2;
	while(1)
    {
		sprintf((char*)rep,"%d",counter++);
		if(counter > 30000)
			counter=1;
		text(d_screen,64+11*8,88,&blank[strlen((char*)blank)-12]);
		text(d_screen,64+11*8,88,rep);
		if(write_serial("r",1,&end_cancel))
			goto the_end;
		if(check_serial(1))
        {   
			if(read_serial(&send,1,&end_cancel))
				goto the_end;
			if(send == 'r')
            {
				if(write_serial("z",1,&end_cancel))
					goto the_end;
				do  {
					if(read_serial(&send,1,&end_cancel))
						goto the_end;
                }while(send != 'z');
				break;
            }
			else if(send == 'z')
				break;
        }
    }
	if(write_serial(i_am,22,&end_cancel))
		goto the_end;
	if(read_serial(he_is,22,&end_cancel))
		goto the_end;
	clear_serial();
	text(d_screen,64,96,(UBYTE*)"CONNECTION MADE");
	if(he_is[0] == i_am[0])
		return(2);
	if(he_is[6] != i_am[6])
		return(3);
	if(he_is[9] != i_am[9])
		return(5);
	if(he_is[10] != i_am[10] && use_random == 0)
		return(6);
	if(he_is[0] == DEVIL)
    {
		start_seed=he_is[1];
		game_mode=he_is[2];
		stats[DEVIL].computer_skill=he_is[3];
		stats[DEVIL].computer_mode=he_is[4];
		stats[DEVIL].computer_speed=he_is[5];
		ground_in=he_is[7];
		paint_map=he_is[8];
		quick_io=he_is[9];
		return(1);
    }
	else if(he_is[0] == GOD)
    {
		stats[GOD].computer_skill=he_is[3];
		stats[GOD].computer_mode=he_is[4];
		stats[GOD].computer_speed=he_is[5];
		return(1);
    }
the_end:;
		clear_serial();
		left_button=2;
		return(99);
}

int		CPopulous::check_cancel(t_text *ender1,t_text *ender2)
{
	//UpdatePopulousData();
	if(ControllerRef->IsButtonPressedDB(MouseSelect))
    {
		if(ControllerRef->GetMousePosition()->y < 200)
		{
			if(ender1)
			{
				if(mousex >= ender1->x && mousex <= ender1->width && 
					mousey >= ender1->y && mousey <= ender1->y+16)
					return(1);
			}
			if(ender2)
			{
				if(mousex >= ender2->x && mousex <= ender2->width && 
					mousey >= ender2->y && mousey <= ender2->y+16)
					return(2);
			}
		}
    }
	return(0);
}

void	CPopulous::put_in_string(UBYTE *into,UBYTE *from,SHORT width)
{
	while(width) 
    {
		*into++;
		width--;
    }
	while(*from) *into++=*from++;
	*into=0;
}

int		CPopulous::show_world()
{
	UBYTE   tmp2[10];
	UBYTE   *tmp;
	//	SHORT   land;
	SHORT   rand;
	SHORT   c1;
	SHORT   do_name=0;
	FILE    *fp;
	struct  t_text  start;
	struct  t_text  newname;
	strcpy((char*)start.text,"START GAME");
	strcpy((char*)newname.text,"NEW GAME");

	if (ThePreferences->Registered)
	{
		if(message[0] == 0)
			strcpy((char*)message,"GENESIS");

	new_name:
		requester(d_screen,16,24,288,160,&start,&newname);
		text(d_screen,16+c_text[0].x,24+c_text[0].y,c_text[0].text);
		do
		{
			text(d_screen,16+16,24+20,blank_box+3);
			text(d_screen,16+16,24+20,message);
			if(do_name)
			{
				get_name(d_screen,message,16+16,24+20,16,&start,&newname);
				text(d_screen,16+c_text[1].x,24+c_text[1].y,blank);
			}
		
			UpdatePopulousData();	
			if(ControllerRef->IsButtonPressed(MouseSelect))
			{
				if(ControllerRef->GetMousePosition()->y < 200)
				{
					if(mousex >= start.x && mousex <= start.width && 
						mousey >= start.y && mousey <= start.y+16)
					{
						if(strcmp((char*)message,"") == 0)
						{
							strcpy((char*)message,"GENESIS");
						}
					}
				
				}
			 }

			 if(atoi((char*)message))
			 {
				 seed=start_seed=atoi((char*)message);
				 message[0]=0;
				 return(-1);
			 }

			 if(message[0] == 0)
				 continue;

			 if(strcmp((char*)message,"GENESIS") == 0)
				 rand=0;
			 else    
				 rand=decode(message);
			 if(rand < 0)
				 text(d_screen,16+c_text[1].x,24+c_text[1].y,(UBYTE*)"NO SUCH WORLD");
#ifdef	CASSIO
			GraphicsRef->EndDraw();
			GraphicsRef->StartDraw();
#endif
		}while(rand < 0);
	}
	else
		rand = 0;

	// Now load the level
	do_name=1;
	c1=rand/25;
	
	int t1;
	int t2;
	do
	{
		//fp=fopen("<Populous$Dir>.dat.level","rb");
		TempString = ThePlatform->GetPath(EP_Data);
		TempString += "level";
		fp=FOPEN(TempString.GetString(),"rb");
		if(!fp) wait_for_disc();
	}
	while(!fp);
	fseek(fp,(LONG)(c1*10),SEEK_SET);
	fread(&conquest,10,1,fp);
	fclose(fp);
	bbc_cls();
	t1 = (conquest.seed << 8) & 0xff00;
	t2 = (conquest.seed >> 8) & 0x00ff;
	conquest.seed = t1 | t2;        
	
	if (!ThePreferences->Registered)
		goto end;

	do
	{
		left_button=2;
		for(c1=1;c1<15;c1++)
		{
			switch(c1)
			{
			case    1:
				sprintf((char*)tmp2,"%d",rand/5);
				put_in_string(c_text[c1].text,tmp2,c_text[c1].width);
				break;
			case    2:
				put_in_string(c_text[c1].text,con_text[conquest.land],c_text[c1].width);
				break;
			case    3:
				put_in_string(c_text[c1].text,con_text[4+(10-conquest.speed)/2],c_text[c1].width);
				break;
			case    4:
				put_in_string(c_text[c1].text,con_text[9+(10-conquest.skill)/2],c_text[c1].width);
				break;
			case    5:
				if(conquest.game_mode & G_NO_BUILD)
					tmp=con_text[14];
				else if(conquest.game_mode & G_RAISE_TOWN)
					tmp=con_text[15];
				else if(conquest.game_mode & G_ONLY_RAISE)
					tmp=con_text[16];
				else
					tmp=con_text[17];
				put_in_string(c_text[c1].text,tmp,c_text[c1].width);
				break;
			case    6:
				if(conquest.game_mode & G_SWAMP_REMAIN)
					tmp=con_text[19];
				else    
					tmp=con_text[18];
				put_in_string(c_text[c1].text,tmp,c_text[c1].width);
				break;
			case    7:
				if(conquest.game_mode & G_WATER_FATAL)
					tmp=con_text[21];
				else    
					tmp=con_text[20];
				put_in_string(c_text[c1].text,tmp,c_text[c1].width);
				break;
			case    8:
				break;
			case    9:
				sprintf((char*)tmp2,"%d",(SHORT)conquest.my_pop);
				text(d_screen,16+c_text[c1].x+c_text[c1].width,
					24+c_text[c1].y,tmp2);
				sprintf((char*)tmp2,"%d",(SHORT)conquest.his_pop);
				text(d_screen,16+c_text[c1].x+c_text[c1].width+7*8,
					24+c_text[c1].y,tmp2);
				break;
			default:
				if((1<<(c1-10)) & conquest.my_mode)
					tmp=con_text[22];
				else
					tmp=con_text[23];
				text(d_screen,16+c_text[c1].x+c_text[c1].width,
					24+c_text[c1].y,tmp);
				if((1<<(c1-10)) & conquest.his_mode)
					tmp=con_text[22];
				else
					tmp=con_text[23];
				text(d_screen,16+c_text[c1].x+c_text[c1].width+7*8,
					24+c_text[c1].y,tmp);
				break;
			}
			text(d_screen,16+c_text[c1].x,24+c_text[c1].y,c_text[c1].text);

#ifdef	CASSIO
			GraphicsRef->EndDraw();
			GraphicsRef->StartDraw();
#endif
		 }
		 while(left_button) 
		 {
			 UpdatePopulousData();	// This could be wrong
			 waittof();
		 }

		if(ControllerRef->GetMousePosition()->y  < 200)
		{
			if(c1=check_cancel(&start,&newname))
			{
				if(c1 == 2)
				{
					message[0]=0;
#ifdef	CASSIO
					GraphicsRef->EndDraw();
					GraphicsRef->StartDraw();
#endif
					goto new_name;
				}
			}
		}
	}
	while(c1 != 1);
end:
	player=GOD;
	not_player=DEVIL;
	stats[1].type=COMPUTER_PLAYER;
	stats[0].type=HUMAN_PLAYER;
	stats[0].computer_done=1;
	stats[1].computer_done=0;
	serial_off=1;
	paint_map=0;
	war=0;
	level_number=rand;

	return(rand);
}


void	CPopulous::game_options()
{
	SHORT   c1,setup=1;
	SHORT   old_serial_off=serial_off;
	SHORT   old_paint_map=paint_map;
	SHORT   old_player=player;
	SHORT   old_type=stats[player].type;
	SHORT   old_in_conquest=in_conquest;
	SHORT   c2;
	
	bring_up_two=0;
	/*
	game_text[1].text[0]=128+serial_off;
	game_text[2].text[0]=128+!serial_off;
	*/
	game_text[3].text[0]=128+!paint_map;
	game_text[4].text[0]=128+paint_map;
	game_text[5].text[0]=128+!player;
	game_text[6].text[0]=128+player;
	game_text[7].text[0]=128+!stats[player].type;
	game_text[8].text[0]=128+stats[player].type;
	game_text[9].text[0]=128+(in_conquest != -1);
	game_text[10].text[0]=128+(in_conquest == -1);
	saved=0;
	while(1)
    {
		if(setup)
        {
			requester(d_screen,16,16,288,176,&end_ok,&end_cancel);
			setup=0;
        }
		for(c1=0;c1<17;c1++)
			text(d_screen,16+game_text[c1].x,16+game_text[c1].y,game_text[c1].text);

#ifdef	CASSIO
			GraphicsRef->EndDraw();
			GraphicsRef->StartDraw();
#endif
		left_button=2;
		while(left_button) 
		{	
			UpdatePopulousData();	// This could be wrong
			if (Redraw)
			{
				copy_screen(w_screen,d_screen);
				requester(d_screen,16,16,288,176,&end_ok,&end_cancel);
				for(c1=0;c1<17;c1++)
					text(d_screen,16+game_text[c1].x,16+game_text[c1].y,game_text[c1].text);
#ifdef	CASSIO
				GraphicsRef->EndDraw();
				GraphicsRef->StartDraw();
#endif
				Redraw = false;
			}

			waittof();
		}
		for(c1=0;c1<17;c1++)
			if(mousex >= 16+game_text[c1].x)
				if(mousex <= 16+game_text[c1].x+game_text[c1].width)
					if(mousey >=16+game_text[c1].y)
						if(mousey <= 16+game_text[c1].y+8)
							break;
						if(c1 < 9 && in_conquest != -1)
							c1=19;
						switch(c1)
						{
						case    1:                  /* one players */
													/*
													game_text[c1].text[0]=129;
													game_text[c1+1].text[0]=128;
													if(serial_off == 0)
													{
													stats[player].type=HUMAN_PLAYER;
													stats[player].type=COMPUTER_PLAYER;            
													game_text[7].text[0]=129;
													game_text[8].text[0]=128;
													serial_off=1;
													}
							*/
							break;
						case    2:                 /* two players */
												   /*
												   if(serial_off)
												   {
												   game_text[c1].text[0]=129;
												   game_text[c1-1].text[0]=128;
												   if(two_players(0))
												   goto the_end;
												   setup=1;
												   }
												   else
												   {
												   bring_up_two=1;
												   return;
												   }
							*/
							break;
						case    3:
							game_text[c1].text[0]=129;
							game_text[c1+1].text[0]=128;
							paint_map=0;
							break;
						case    4:                  /* paint_map */
							game_text[c1].text[0]=129;
							game_text[c1-1].text[0]=128;
							paint_map=1;
							break;
						case    5:
							game_text[c1].text[0]=129;
							game_text[c1+1].text[0]=128;
							set_tend_icons(-1,-1);
							set_mode_icons(-1,-1);
							c2=stats[not_player].type;
							stats[GOD].type=stats[player].type;
							stats[DEVIL].type=c2;
							player=GOD;
							if(mode == SCULPT)
								make_arc_mouse(pointer=1);
							else if(mode & PUT_MAGNET)
								make_arc_mouse(pointer=player+2);
							not_player=!player;
							set_tend_icons(-1,-1);
							set_mode_icons(-1,-1);
							break;
						case    6:
							game_text[c1].text[0]=129;
							game_text[c1-1].text[0]=128;
							set_tend_icons(-1,-1);
							set_mode_icons(-1,-1);
							c2=stats[not_player].type;
							stats[DEVIL].type=stats[player].type;            
							stats[GOD].type=c2;
							player=DEVIL;
							if(mode == SCULPT)
								make_arc_mouse(pointer=4);
							else if(mode & PUT_MAGNET)
								make_arc_mouse(pointer=player+2);
							not_player=!player;
							set_tend_icons(-1,-1);
							set_mode_icons(-1,-1);
							break;
						case    7:
							game_text[c1].text[0]=129;
							game_text[c1+1].text[0]=128;
							stats[player].type=HUMAN_PLAYER;
							if(serial_off)
								stats[!player].type=COMPUTER_PLAYER;
							break;
						case    8:
							game_text[c1].text[0]=129;
							game_text[c1-1].text[0]=128;
							stats[player].type=COMPUTER_PLAYER;
							if(serial_off)
								stats[!player].type=COMPUTER_PLAYER;
							break;
						case    9:
							if(serial_off)
							{
								game_text[c1].text[0]=129;
								game_text[c1+1].text[0]=128;
								message[0]=0;
								in_conquest=show_world();
								new_map=1;
								goto the_end;
							}
							break;
						case    10:
							game_text[c1].text[0]=129;
							game_text[c1-1].text[0]=128;
							in_conquest=-1;
							break;
						case    11:
							set_options();
							setup=1;
							break;
						case    12:
							if(save_load(1))
								goto the_end;
							setup=1;
							break;
						case    13:
							if(serial_off)
							{
								if(save_load(0))
									goto the_end;
								setup=1;
							}
							break;
						case    14:             /*next map*/
							new_map=1;
							goto the_end;
							break;            
						case    15:
							new_map=ground_in+2;
							goto the_end;
							break;
						case    16:
							surender=player;
							goto the_end;
						default:
							break;
							
        }
		if(c1=check_cancel(&end_ok,&end_cancel))
        {
			if(c1 == 2)
            {
				serial_off=old_serial_off;
				paint_map=old_paint_map;
				if(player != old_player)
                {
					set_tend_icons(-1,-1);
					set_mode_icons(-1,-1);
					player=old_player;
                }
				not_player=!player;
				stats[player].type=old_type;
				in_conquest=old_in_conquest;
            }
			break;
        }
    }
the_end:;
#ifdef	CASSIO
				GraphicsRef->EndDraw();
				GraphicsRef->StartDraw();
#endif
		return_to_game();
}

void	CPopulous::set_options()
{
	SHORT   c1;
	SHORT   c2;
	//	UBYTE   tmp[40];
	USHORT  old_game_mode=game_mode;

	
	requester(d_screen,32,24,256,112,&end_ok,&end_cancel);
	text(d_screen,32+set_text[0].x,24+set_text[0].y,set_text[0].text);
	for(c1=0;c1<5;c1++)
    {
		c2=1+((game_mode & (1<<c1)) != 0);
		text(d_screen,32+set_text[c1*2+c2].x,24+set_text[c1*2+c2].y,blank);
		text(d_screen,32+set_text[c1*2+c2].x,24+set_text[c1*2+c2].y,set_text[c1*2+c2].text);
    }
	while(1)
    {
		left_button=2;
		while(left_button) 
		{	
			UpdatePopulousData();	// This could be wrong
			if (Redraw)
			{
				copy_screen(w_screen,d_screen);
				requester(d_screen,32,24,256,112,&end_ok,&end_cancel);
				text(d_screen,32+set_text[0].x,24+set_text[0].y,set_text[0].text);
				for(c1=0;c1<5;c1++)
				{
					c2=1+((game_mode & (1<<c1)) != 0);
					text(d_screen,32+set_text[c1*2+c2].x,24+set_text[c1*2+c2].y,blank);
					text(d_screen,32+set_text[c1*2+c2].x,24+set_text[c1*2+c2].y,set_text[c1*2+c2].text);
				}

#ifdef	CASSIO
							GraphicsRef->EndDraw();
							GraphicsRef->StartDraw();
#endif
		
				Redraw = false;
			}
			waittof();
		}
		for(c1=1;c1<10;c1+=2)
			if(mousex >= 32+set_text[c1].x && in_conquest == -1)
				if(mousex <= 32+set_text[c1].x+set_text[c1].width)
					if(mousey >=24+set_text[c1].y)
						if(mousey <= 24+set_text[c1].y+8)
                        {
							c2=1<<(c1>>1);        
							game_mode^=c2;
							if(game_mode & c2)
								c2=c1+1;
							else
								c2=c1;
							text(d_screen,32+set_text[c2].x,24+set_text[c2].y,blank);
							text(d_screen,32+set_text[c2].x,24+set_text[c2].y,set_text[c2].text);

					break;
                        }
						if(c1=check_cancel(&end_ok,&end_cancel))
						{
							if(c1 == ICON2)
								game_mode=old_game_mode;
							break;
						}
    }
	return_to_game();
}

void	CPopulous::options(SHORT who)
{
	SHORT       c1;
	SHORT       c2;
	UBYTE       c_tmp[2];
	SHORT       width=288;
	SHORT       left=16;
	struct      s_stats old_stats;
	old_stats=stats[who];
	c_tmp[0]=135;
	c_tmp[1]=0;
	requester(d_screen,left,16,width,166,&end_ok,&end_cancel);
	stats[who].computer_skill=10-stats[who].computer_skill;
	stats[who].computer_speed=10-stats[who].computer_speed;
	do
    {
		left_button=2;
		for(c1=0,c2=1;c1<14;c1++)
        {
			if(c1==0)
            {
				if(who == 0)
					strcpy((char*)&option_text[c1].text[12],"GOOD");
				else
					strcpy((char*)&option_text[c1].text[12],"EVIL");
            }
			if(c1>=1 && c1 < 10)
            {
				option_text[c1].text[0]=128+((stats[who].computer_mode&c2)!=0);
				c2<<=1;
            }
			text(d_screen,option_text[c1].x+16,option_text[c1].y+16,option_text[c1].text);
			if(c1 == 11)
            {
				text(d_screen,option_text[c1].x+16+24+stats[who].computer_skill*8,option_text[c1].y+16,c_tmp);
            }
			else if(c1 == 13)
            {
				text(d_screen,option_text[c1].x+16+32+stats[who].computer_speed*8,option_text[c1].y+16,c_tmp);
            }

        };
		while(left_button) 
		{	
			UpdatePopulousData();	// This could be wrong
			if (Redraw)
			{
				copy_screen(w_screen,d_screen);
				c_tmp[0]=135;
				c_tmp[1]=0;
				requester(d_screen,left,16,width,166,&end_ok,&end_cancel);
				stats[who].computer_skill=10-stats[who].computer_skill;
				stats[who].computer_speed=10-stats[who].computer_speed;
				for(c1=0,c2=1;c1<14;c1++)
				{
					if(c1==0)
					{
						if(who == 0)
							strcpy((char*)&option_text[c1].text[12],"GOOD");
						else
							strcpy((char*)&option_text[c1].text[12],"EVIL");
					}
					if(c1>=1 && c1 < 10)
					{
						option_text[c1].text[0]=128+((stats[who].computer_mode&c2)!=0);
						c2<<=1;
					}
					text(d_screen,option_text[c1].x+16,option_text[c1].y+16,option_text[c1].text);
					if(c1 == 11)
					{
						text(d_screen,option_text[c1].x+16+24+stats[who].computer_skill*8,option_text[c1].y+16,c_tmp);
					}
					else if(c1 == 13)
					{
						text(d_screen,option_text[c1].x+16+32+stats[who].computer_speed*8,option_text[c1].y+16,c_tmp);
					}
				};

				Redraw = false;
			}

			waittof();
		}
		for(c1=1;c1<14;c1++)
			if(mousex >= 16+option_text[c1].x)
				if(mousex <= 16+option_text[c1].x+option_text[c1].width)
					if(mousey >=16+option_text[c1].y)
						if(mousey <= 16+8+option_text[c1].y)
                        {
							break;
                        }
						if(in_conquest == -1)
						{
							if(c1 < 10)
							{
								c1=1<<(c1-1);
								stats[who].computer_mode^=c1;
							}
							else if(c1 == 11)
							{
								c2=(mousex-(option_text[c1].x+40))/8;
								if(c2 >=0 && c2<=9)
									stats[who].computer_skill=c2;
							}
							else if(c1 == 13)
							{
								c2=(mousex-(option_text[c1].x+48))/8;
								if(c2 >=0 && c2<=9)
									stats[who].computer_speed=c2;
							}
						}
						if(c1=check_cancel(&end_ok,&end_cancel))
						{
							if(c1 == ICON2)
							{
								stats[who]=old_stats;
							}
							else
							{
								stats[who].computer_skill=10-stats[who].computer_skill;
								stats[who].computer_speed=10-stats[who].computer_speed;
							}
							break;
						}
    }while(c1 == 0);
	return_to_game();
}

void	CPopulous::return_to_game()
{
	//	LONG    old_scr;
	ms_off();
	copy_screen(w_screen,d_screen);
	ms_on();
	left_button=2;
}


void	CPopulous::start_game()
{
	//	SHORT   c1;
	load_sound((UBYTE*)"");
	read_back_scr();
	
	// Now update the music and effects values.
	if (ThePreferences->EffectsEnabled)
	{
		effect_off = 1;
	}
	else
	{
		effect_off = 0;
		toggle_icon(back_scr,0,4,BANK2);
	}

	if (ThePreferences->MusicEnabled)
	{
		music_off = 1;
	}
	else
	{
		music_off = 0;
		toggle_icon(back_scr,0,3,BANK2);
	}
	
	create_mouse();
	if(start_of_game == 3)
	{
		stats[0]=tutorial[0];
		stats[1]=tutorial[1];
		pause=1;
		toggle_icon(back_scr,1,3,BANK2);
		start_seed=seed=27068;                
		game_mode=tutorial_game_mode;
	}        
	make_arc_mouse(pointer=1);
	left_button=2;
	Setscreen(back_scr,back_scr,-1);
}


void	CPopulous::end_game(SHORT lost)
{
	
	USHORT  c1,c2;
	//	SHORT   c3;
	ULONG   him,you;
	struct  t_text  ender;
	UBYTE   tmp[10];
	UBYTE   tmp2[10];
	for(c1=0;c1 < 7;c1++)
    {
		switch(c1)
        {
        case    0:
            if(lost == 1)
                strcpy((char*)&end_text[c1].text[5],"LOST");
            else
                strcpy((char*)&end_text[c1].text[5],"WON");
            break;
        case    2:
            sprintf((char*)tmp,"%d",battle_won[player]);
            sprintf((char*)tmp2,"%d",battle_won[not_player]);
            break;
        case    3:
            for(c2=you=him=0;c2<no_peeps;c2++)
			{
                if(peeps[c2].head_for && peeps[c2].population)
				{
                    if(peeps[c2].player == player)
                        you++;
                    else
                        him++;
				}
			}
            sprintf((char*)tmp,"%d",(SHORT)you);
            sprintf((char*)tmp2,"%d",(SHORT)him);
            break;
        case    4:
            for(c2=you=him=0;c2<no_peeps;c2++)
			{
                if(peeps[c2].flags == IN_TOWN && peeps[c2].population && peeps[c2].frame != LAST_TOWN)
				{
                    if(peeps[c2].player == player)
                        you++;
                    else
                        him++;
				}
			}
            sprintf((char*)tmp,"%d",(SHORT)you);
            sprintf((char*)tmp2,"%d",(SHORT)him);                    
            break;
        case    5:
            for(c2=you=him=0;c2<no_peeps;c2++)
			{
                if(peeps[c2].flags == IN_TOWN && peeps[c2].population && peeps[c2].frame == LAST_TOWN)
				{
                    if(peeps[c2].player == player)
                        you++;
                    else
                        him++;
				}
			}                    
            sprintf((char*)tmp,"%d",(SHORT)you);
            sprintf((char*)tmp2,"%d",(SHORT)him);
            him=you;
            break;
        case    6:
            if(battle_won[player] > battle_won[not_player])
                score+=SCORE_BATTLE;
            for(c2=C_QUAKE;c2<=C_WAR;c2<<=1)
			{
                if((stats[player].computer_mode & c2) == 0)
                    score+=SCORE_YOUR_OPTIONS;
                if(stats[not_player].computer_mode & c2)
                    score+=SCORE_HIS_OPTIONS;
			}
            if(serial_off)
                score+=(10-stats[not_player].computer_speed)*SCORE_SPEED;
            if(!lost)
                score*=SCORE_WON;
            if(score < 500)
                score=500;
            if(score > 555555)
                score = 515090;
            sprintf((char*)&end_text[c1].text[19],"%d",(LONG)score);
            break;
        default:
            break;
        }
		if(c1 >=2 && c1 <= 5)
        {
			for(c2=strlen((char*)tmp);c2<5;c2++)
				tmp[c2]=32;
			tmp[c2]=0;
			strcpy((char*)&end_text[c1].text[19],(char*)tmp);
			strcat((char*)&end_text[c1].text[19],(char*)tmp2);
        }
    }            
	if(lost && in_conquest != -1)
		strcpy((char*)ender.text,"TRY IT AGAIN");
	else
		strcpy((char*)ender.text,"NEW GAME");
	requester(d_screen,16,16,288,166,&ender,NULL);
	for(c1=0;c1 < 7;c1++)
		text(d_screen,end_text[c1].x+16,end_text[c1].y+16,end_text[c1].text);
	
	//	GraphicsRef->EndDraw();
#ifndef TEST_IN_EMULATION
	while(1)
    {
		left_button=2;
		while(left_button) UpdatePopulousData();
		if(check_cancel(&ender,NULL))
			break;
    }
#endif
	if(lost == 0 && in_conquest != -1)
		won_conquest(score);
	stats[player].type=HUMAN_PLAYER;
	ms_off();
	copy_screen(w_screen,d_screen);
	setup_display(0,-1);
	ms_on();
	left_button=2;
}

void	CPopulous::won_conquest(LONG score)
{
	int timer = 0;
	UBYTE   tmp[100];
	//	LONG    fpz;
	SHORT   old_level;
	SHORT   c1,dir,/*count,*/got_to;
	SHORT   old_music_off,old_effect_off;
	SHORT   c2;
	SHORT   end_game=0;
	old_level=level_number;
	level_number+=score/5000+1;
	if(level_number%5)
		level_number+=5-(level_number%5);
	if(level_number > 2470)
    {
		if(old_level == 2470)
        {
			old_level=0;
			level_number=0;
			end_game=1;
        }
		else
			level_number=2470;
    }
	seed=level_number;
	code(message,newrand());
	old_music_off=music_off;
	music_off=0;
	old_effect_off=effect_off;
	effect_off=1;
	kill_effect(0,4);
	
	
	change_palette(1);	// Change palette before we load images
	if(read_lord())
    {
		//if (!ThePreferences->Registered)
			// quit

		read_mouth();
		seed=old_level;
		if(end_game)
        {
			text(d_screen,8,178,con_text[37]);
			strcpy((char*)tmp,(char*)con_text[38]);
        }
		else
        {
			strcpy((char*)tmp,(char*)con_text[34]);
			got_to=level_number/250;
			strcat((char*)tmp,(char*)con_text[24+got_to]);
			strcat((char*)tmp,(char*)con_text[35]);
			text(d_screen,8,178,tmp);
			if(seed == 0)
				strcpy((char*)tmp,"GENESIS");
			else
				code(tmp,newrand());
			strcat((char*)tmp,(char*)con_text[36]);
        }
		strcat((char*)tmp,(char*)message);
		text(d_screen,8,178+8,tmp);//text(w_screen,8,178+8,tmp);
		Setscreen(-1L,w_screen,-1);
		//change_palette(lolpalette,16);
		change_palette(1);
		left_button=2;
		dir=1;
		c1=0;
		c2=0;    
		while(left_button) 
		{	
			GraphicsRef->StartDraw();
			UpdatePopulousData();	// This could be wrong
			
			if(end_game)
            {
				if(c2 >= 10 && c2 <= 14)
                {
					if(check_effect(2))
                    {
						PlaySound(end_words[c2-10]+104,2);
						c2++;
                    }
                }
				else
                {
					Timer50ths->BlockUntilClock();
					timer += Timer50ths->GetNumberOfEvents();
					Timer50ths->ClearNumberOfEvents();

					if(timer > 300)//if(c2 > 250)
					{
						timer = 0;
						c2=0;
					}
					if(c2 == 16)  
						draw_mouth(d_screen,8*2,75,35,5);
					c2++;
					c1=0;
					dir=1;
                }
            }
			else 
            {
				if(c2 >= 10 && c2 <=12)
                {
					if(c2 == 10)
                    {
						PlaySound(0+104,2);
						c2++;
                    }
					else if(check_effect(2))
                    {
						if(words[got_to*2+(c2-11)] >= 0)
							PlaySound(words[got_to*2+(c2-11)]+104,2);
						c2++;
                    }
                }
				else
                {
					Timer50ths->BlockUntilClock();
					timer += Timer50ths->GetNumberOfEvents();
					Timer50ths->ClearNumberOfEvents();

					if(timer > 300)//if(c2 > 250)
					{
						timer = 0;
						c2=0;
					}
					c2++;
					c1=0;
					dir=1;
                }
            }
			waittof();
			waittof();
			waittof();
			waittof();
			draw_mouth(d_screen,9*2,135,35,c1);
			if(check_effect(2) == 0)
            {
				c1+=dir;
				if(c1 >= 4 || c1 <= 0)
                {
					dir=-dir;
                }
            }
			GraphicsRef->EndDraw();
        }
		GraphicsRef->StartDraw();
		left_button=2;
		kill_effect(0,4);
		clr_wsc();
		Setscreen(d_screen,d_screen,-1);
		//change_palette(palette,16);
		change_palette(0);
    }
	else
    {
		level_number=old_level;
		message[0]=0;
    }
	music_off=old_music_off;
	effect_off=old_effect_off;
	while((in_conquest=show_world()) == -1);
	return_to_game();
	swap_screens();
}    


void	CPopulous::code(UBYTE *str,SHORT rnd)
{
	register SHORT   c1,c2;
	for(c1=0;start[rnd & 0x01f][c1];c1++)
		str[c1]=start[rnd & 0x01f][c1];
	for(c2=0;mid[(rnd>>5) & 0x01f][c2];c2++,c1++)
		str[c1]=mid[(rnd>>5) & 0x01f][c2];
	for(c2=0;end[(rnd>>10) & 0x01f][c2];c2++,c1++)
		str[c1]=end[(rnd>>10) & 0x01f][c2];
	str[c1]=0;
}

int		CPopulous::decode(UBYTE *str)
{
	SHORT   rnd;
	register USHORT   c1;
	register USHORT   c2;
	USHORT   c3=0;
	
	
	for(c1=0;c1 < 32;c1++)
    {
		for(c2=0;start[c1][c2] && str[c3+c2] == start[c1][c2];c2++);
		if(start[c1][c2] == 0)
        {
			c3+=c2;
			break;
        }
    }
	if(c1 == 32)
		return(-1);
	rnd=c1;
	for(c1=0;c1 < 32;c1++)
    {
		for(c2=0;mid[c1][c2] && str[c3+c2] == mid[c1][c2];c2++);
		if(mid[c1][c2] == 0)
        {
			c3+=c2;
			break;
        }
    }
	if(c1 == 32)
		return(-2);
	rnd|=c1<<5;
	for(c1=0;c1 < 32;c1++)
    {
		for(c2=0;end[c1][c2] && str[c3+c2] == end[c1][c2];c2++);
		if(end[c1][c2] == 0)
        {
			c3+=c2;
			break;
        }
    }
	if(c1 == 32)
		return(-3);
	if(strlen((char*)str) != c3)
		return(-4);
	
	rnd|=c1<<10;
	for(c1=0;c1<=5000;c1+=5)
    {
		seed=c1;
		c2=newrand();
		if(c2 == rnd)
			return(c1);
    }
	return(-5);
}

void	CPopulous::paint_the_map(UBYTE *send)
{
	if(paint_map)
    {
		send[S_X]=cur_x;
		send[S_Y]=cur_y;
		switch(inkey)
        {
        case    113:                         /* F1 */
            send[S_EFFECT]=(shift) ? S_PLACE_GODS_LEADER : S_PLACE_GODS;
            break;
        case    114:                   /* F2 */
            send[S_EFFECT]=(shift) ? S_PLACE_DEVILS_LEADER : S_PLACE_DEVILS;
            break;
        case    115:                     /* F3 */
            send[S_EFFECT]=S_PLACE_TREES;
            break;
        case    20:                     /* F4 */
            send[S_EFFECT]=S_PLACE_ROCKS;
            break;
        case    116:                      /* F5 */
            send[S_EFFECT]=S_CLEAR_WHO;
            break;
        case    117:                      /* F6 */
            send[S_X]=shift;
            send[S_EFFECT]=S_ACTION;
            send[S_Y]=SA_GODS_MANA;
            break;
        case    22:                 /* F7 */
            send[S_X]=shift;
            send[S_EFFECT]=S_ACTION;
            send[S_Y]=SA_DEVILS_MANA;
            break;
        case    118:                     /* F8 */
            send[S_EFFECT]=S_ACTION;
            send[S_Y]=SA_ROTATE_MAP;
            break;
        case    89:                      /* DEL */
            send[S_EFFECT]=S_ACTION;
            send[S_Y]=SA_CLEAR_MAP;
            break;
        default:
            if(inkey == 48 || inkey == 49 || inkey == 17 || inkey == 18)
			{
                if(inkey == 48)
                    send[S_X]=0;
                if(inkey == 49)
                    send[S_X]=1;
                if(inkey == 17)
                    send[S_X]=2;
                if(inkey == 18)
                    send[S_X]=3;
                send[S_EFFECT]=S_ACTION;
                send[S_Y]=SA_LOAD_LAND;
			}
            break;
        }
    }
}

void	CPopulous::waitfor(SHORT num)
{
	// vbi_timer is not declared anywhere!!! - Anyway, can't be arsed to wait for a vertical blank.
	//LONG    wait;
	//for(wait=vbi_timer+(LONG)num;wait > vbi_timer;);
	return;
}
