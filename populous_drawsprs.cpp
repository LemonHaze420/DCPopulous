#include	"Populous.h"
#include	"Object.h"
#include	"SafeProgramming.h"
#include	<windows.h>

#include	"god.h"
#include	"GraphicsClasses.h"
#include	"SoundSystem.h"
#include	"Controls.h"
#include	"Platform.h"
#include	"Preferences.h"
#include	"Localization.h"
#include	"extern.h"
//#include	"populous.h"

/*
		GET	  h.equates

		EXPORT	scale_alts
		EXPORT	start_scroll
		EXPORT	the_message
		EXPORT	do_text
		EXPORT	draw_it
		EXPORT	draw_sprite
		EXPORT	draw_s32
		EXPORT	ts1
		EXPORT	ts2
		EXPORT	draw_building
		
		  IMPORT	title_map_blk
		  IMPORT	s_size
		  IMPORT	w_screen
		  IMPORT	blk_data
		  IMPORT	extras
		  IMPORT	big_font
		  IMPORT	sprite_data
		  IMPORT	colortablef
		  IMPORT	font
		  
			AREA	code,CODE
*/
//int r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12 = 0;
//scale_alts
void CPopulous::scale_alts()
{
	long r0, r1, r2; 
	//	ROUT
	//	STMFD	 sp!,{r0-r12,lr}
	
	//	LDR	 r0,%0; 	  pointer to title_map_blk
	r0 = (long)title_map_blk;
	
	//	  ADD	  r0,r0,#200
	//	  MOV	  r1,#100
	r0 += 200;
	r1 = 100;
	
scale_loop:
	//		LDRB	  r2,[r0]
	r2 = *(UBYTE*)r0;
	
	//		MOV	  r2,r2,LSL#3
	r2 = r2 << 3;
	
	//	  STRB	  r2,[r0],#1
	*(UBYTE*)r0 = r2 & 0xFF;
	r0++;
	
	//	  SUBS	  r1,r1,#1
	r1--;
	
	//	BNE 	scale_loop
	if (r1 != 0)
		goto scale_loop;
	
	//	  LDMFD   sp!,{r0-r12,pc}
	//0
	// 	  DCD	  title_map_blk
}

long msg_ptr = 0;
//start_scroll
void CPopulous::start_scroll()
{
	//STMFD   sp!,{r0,lr}
	
	//ADR	  r0,the_message
	//r0 = *(the_message[0]);
	//STR	  r0,msg_ptr
	msg_ptr = (long)the_message;//r0;
	//LDMFD   sp!,{r0,pc}
}


UBYTE* the_message[] = 
{	
		(UBYTE*)"POPULOUS",(UBYTE*)"\0",
		(UBYTE*)"(C) 1989 BULLFROG PRODUCTIONS",(UBYTE*)"\0",
		(UBYTE*)(UBYTE*)"\0",
		(UBYTE*)"ARCHIMEDES CONVERSION BY",(UBYTE*)"\0",
		(UBYTE*)"RICHARD WALKER (C) 1992",(UBYTE*)"\0",
		(UBYTE*)(UBYTE*)"\0" ,
		(UBYTE*)"PUBLISHED BY",(UBYTE*)"\0",
		(UBYTE*)"KRISALIS SOFTWARE LTD",(UBYTE*)"\0" ,
		(UBYTE*)"AND ELECTRONIC ARTS",(UBYTE*)"\0" ,
		(UBYTE*)(UBYTE*)"\0" ,
		(UBYTE*)"ORIGINAL PROGRAMMING AND DESIGN",(UBYTE*)"\0" ,
		(UBYTE*)"GLENN CORPES",(UBYTE*)"\0" ,
		(UBYTE*)"AND PETER MOLYNEUX",(UBYTE*)"\0" ,
		(UBYTE*)"WITH TECHNICAL HELP FROM",(UBYTE*)"\0" ,
		(UBYTE*)"KEVIN DONKIN",(UBYTE*)"\0" ,
		(UBYTE*)(UBYTE*)"\0" ,
		(UBYTE*)"ORIGINAL GRAPHICS",(UBYTE*)"\0" ,
		(UBYTE*)"GLENN CORPES",(UBYTE*)"\0" ,
		(UBYTE*)"DAN WHEELER",(UBYTE*)"\0" ,
		(UBYTE*)"SEAN COOPER",(UBYTE*)"\0" ,
		(UBYTE*)"ANDY JONES",(UBYTE*)"\0" ,
		(UBYTE*)"AND PETER MOLYNEUX",(UBYTE*)"\0", 
		(UBYTE*)(UBYTE*)"\0" ,
		(UBYTE*)"ORIGINAL GAME SOUND AND MUSIC",(UBYTE*)"\0", 
		(UBYTE*)"DAVE HANLON",(UBYTE*)"\0" ,
		(UBYTE*)(UBYTE*)"\0" ,
		(UBYTE*)"ORIGINAL MUSIC IDEA",(UBYTE*)"\0", 
		(UBYTE*)"LES EDGAR",(UBYTE*)"\0" ,
		(UBYTE*)"\0" ,
		(UBYTE*)"ORIGINAL TITLE MUSIC",(UBYTE*)"\0", 
		(UBYTE*)"ROB HUBBARD",(UBYTE*)"\0" ,
		(UBYTE*)"\0"
};
//int msg_ptr = 0;
//		 r0 	 pointer to text buffer

void CPopulous::do_text(long* x)
{
	/*
	CString theCredits[50];

	theCredits[0] = "POPULOUS";
	theCredits[1] = "FOR THE";
	theCredits[2] = "POCKET PC";
	theCredits[3] = "";
	theCredits[4] = "BROADSWORD INTERACTIVE";
	theCredits[5] = "";
	theCredits[6] = "LEAD PROGRAMMERS";
	theCredits[7] = "";
	theCredits[8] = "ANDREW EVANS";
	theCredits[9] = "ANTHONY DUNN";
	theCredits[10] = "";
	theCredits[11] = "ORIGINAL PROGRAMMING"; 
	theCredits[12] = "AND DESIGN";
	theCredits[13] = "";
	theCredits[14] = "GLENN CORPES";
	theCredits[15] = "AND PETER MOLYNEUX";
	theCredits[16] = "WITH TECHNICAL";
	theCredits[17] = "HELP FROM";
	theCredits[18] = "KEVIN DONKIN";
	theCredits[19] = "";
	theCredits[20] = "ORIGINAL GRAPHICS";
	theCredits[21] = "";
	theCredits[22] = "GLENN CORPES";
	theCredits[23] = "DAN WHEELER";
	theCredits[24] = "SEAN COOPER";
	theCredits[25] = "ANDY JONES";
	theCredits[26] = "AND PETER MOLYNEUX";
	theCredits[27] = "";
	theCredits[28] = "ORIGINAL GAME";
	theCredits[29] = "SOUND AND MUSIC";
	theCredits[30] = "DAVE HANLON";
	theCredits[31] = "";
	theCredits[32] = "ORIGINAL MUSIC IDEA";
	theCredits[33] = "LES EDGAR";
	theCredits[34] = "";
	theCredits[35] = "ORIGINAL TITLE MUSIC";
	theCredits[36] = "ROB HUBBARD";
	theCredits[37] = "";
	theCredits[38] = "COPYRIGHT 1989";
	theCredits[39] = "BULLFROG PRODUCTIONS";
	theCredits[40] = "";
	theCredits[41] = "POCKET PC VERSION";
	theCredits[42] = "COPYRIGHT 2001";
	theCredits[43] = "BROADSWORD INTERACTIVE";
	theCredits[44] = "";
	theCredits[45] = "";
	theCredits[46] = "";
	theCredits[47] = "";
	theCredits[48] = "";
	theCredits[49] = "";

	for (int i = 0; i < 50; i++)
	{
		long yPos = x + (i * 20);
		if (yPos > 40)
			continue;

		GraphicsRef->DrawString(StandardFont, &(theCredits[i]), 160, yPos, TEXT_CENTRED);
	}
	*/
}


void CPopulous::draw_it()
{
	long	Stack[1024];
	long	Sp = 0;
	
	//	long	sr0, sr1, sr6, sr7, sr8, sr9, sr10, sr11, sr12;
	long	r0, r1, r2, r3, r6, r7, r8, r9, r10;
	UBYTE	*r11;
	SHORT	*r12;
	
	//ROUT
	//STMFD   sp!,{r0-r12,lr}
	
	//MOV	  r7,#0 				  ; 	  outer loop counter
	r7 = 0;
	
	//LDR	  r8,%0 				  ; 	  pointer to s_size variable
	//LDR	  r8,[r8]
	r8 = s_size;
	
	//LDR	  r9,%1 				  ; 	  pointer to w_screen variable
	//LDR	  r9,[r9]
	r9 = w_screen;
	
	//LDR	  r10,%2				  ; 	  pointer to blk_data variable
	//LDR	  r10,[r10]
	r10 = blk_data;
	
	//LDR	  r11,%3				  ; 	  pointer to title_map_blk
	r11 = title_map_blk;
	
	//LDR	  r12,%4				  ; 	  pointer to extras
	r12 = extras;
	
out_loop:
	//MOV	  r6,#0 				  ; 	  inner loop counter
	r6 = 0;
	
in_loop:
	//SUB	  r0,r6,r7				  ; 	  x = inner - outer
	r0 = r6 - r7;
	
	//MOV	  r0,r0,ASL#4			  ; 	  x * 16
	r0 = r0 << 4;
	
	//ADD	  r0,r0,#160-16 		  ; 	  add start offset
	r0 += (160 - 16);
	
	//ADD	  r1,r6,r7				  ; 	  y = inner + outer
	r1 = r6 + r7;
	
	//MOV	  r1,r1,ASL#3			  ; 	  y * 8
	r1 = r1 << 3;
	
	//LDRB	  r2,[r11,#200]
	r2 = *(UBYTE*)(r11 + 200);
	
	//SUB	  r1,r1,r2					;		y - height
	r1 = r1 - r2 * 8;	// are: *8 to correct.
	
	//ADD	  r1,r1,#200-160-8		  ; 	  add start screen
	r1 += (200 - 160 - 8);
	
	//LDRB	  r2,[r12,#0]
	//LDRB	  r3,[r12,#1]
	//ADD	  r12,r12,#2
	//ORR	  r2,r2,r3,ASL#8
	r2 = *(WORD*)r12;
	r12++;
	
	//MOV	  r2,r2,ASL#16
	//r2 = r2 << 16;
	if (r2 >> 15)
	{
		r2 = 0xFFFF0000 | r2;
	}

	//SUB	  r1,r1,r2,ASR#16		  ; 	  y - bounce pos
	//r1 = r1 - (r2 >> 16);
	r1 -= r2;
	
	//STMFD   sp!,{r0,r1,r6-r12}
	Stack[Sp++] = r0;
	Stack[Sp++] = r1;	
	Stack[Sp++] = r6;	
	Stack[Sp++] = r7;	
	Stack[Sp++] = r8;	
	Stack[Sp++] = r9;
	Stack[Sp++] = r10;
	Stack[Sp++] = (long)r11;
	Stack[Sp++] = (long)r12;
	
	//MUL	  r0,r8,r0
	r0 = r8 * r0;
	
	//MUL	  r1,r8,r1
	r1 = r8 * r1;
	
	//MOV	  r0,r0,ASR#7
	r0 = r0 >> 7;
	
	//MOV	  r1,r1,ASR#7
	r1 = r1 >> 7;
	
	//LDRB	  r2,[r11]				  ; 	  block number
	r2 = *(UBYTE*)r11;
	
	//MOV	  r3,#40/2*24
	//r3 = (40/2*24);
	
	//MLA	  r2,r3,r2,r10			  ; 	  address of block
	//r2 = r3 * r2 + r10; Don't need to go to all that trouble here.
	
	//MOV	  r3,r9 				  ; 	  screen base
	r3 = r9;
	
	//BL	  draw_map_block
	draw_map_block(r0, r1, r2, r3);
	
	//LDMFD   sp,{r0,r1,r6-r12}
	//this just loads back in the variables.....
	r12 = (SHORT*)Stack[Sp - 1];
	r11 = (UBYTE*)Stack[Sp - 2];	
	r10 = Stack[Sp - 9];//3];	
	r9 = Stack[Sp - 8];//4]; 
	r8 = Stack[Sp - 7];//5]; 
	r7 = Stack[Sp - 6];//6];
	r6 = Stack[Sp - 5];//7];
	r1 = Stack[Sp - 4];//8];
	r0 = Stack[Sp - 3];//9];
	
	//SUB	  r1,r1,#8
	r1 -= 8;

	//MUL	  r0,r8,r0
	r0 = r8 * r0;

	//MUL	  r1,r8,r1
	r1 = r8 * r1;

	//MOV	  r0,r0,ASR#7
	r0 = r0 >> 7;

	//MOV	  r1,r1,ASR#7
	r1 = r1 >> 7;

	//LDRB	  r2,[r11,#100] 		  ; 	  block number
	r2 = *(UBYTE*)(r11 + 100);
	
	//TEQ	  r2,#0
	//BEQ	  no_sec
	if(r2 == 0)
		goto no_sec;
	
	//MOV	  r3,#32/2*24
	//r3 = (32/2*24);
	
	//MLA	  r2,r3,r2,r10			  ; 	  address of block
	//r2 = r3 * r2 + r10;
	
	//MOV	  r3,r9 				  ; 	  screen base
	r3 = r9;
	
	//BL	  draw_map_block
	draw_map_block(r0, r1, r2, r3);
	
no_sec:
	//LDMFD   sp!,{r0,r1,r6-r12}
	r12 = (SHORT*)Stack[--Sp];
	r11 = (UBYTE*)Stack[--Sp];	
	r10 = Stack[--Sp];	
	r9 = Stack[--Sp];	
	r8 = Stack[--Sp];	
	r7 = Stack[--Sp];
	r6 = Stack[--Sp];
	r1 = Stack[--Sp];
	r0 = Stack[--Sp];
	
	//ADD	  r11,r11,#1
	r11++;

	//ADD	  r6,r6,#1
	r6++;

	//CMP	  r6,#10
	//BLT	  in_loop
	if(r6 < 10)
		goto in_loop;
	
	//ADD	  r7,r7,#1
	r7++;

	//CMP	  r7,#10
	//BLT	  out_loop
	if(r7 < 10)
		goto out_loop;
	
	//LDMFD   sp!,{r0-r12,pc}
	
	//0
	// 	  DCD	  s_size
	//1
	// 	  DCD	  w_screen
	//2
	//		  DCD	  blk_data
	//3
	//		  DCD	  title_map_blk
	//4
	//		  DCD	  extras
}

//; 	  r0	  xpos
//; 	  r1	  ypos
//; 	  r2	  pointer to block data
//; 	  r3	  pointer to screen base

void CPopulous::draw_map_block(long a, long b, long c, long d)
{	
	GraphicsRef->SetActiveScreen(d);//_screen);
	Region.left 	= 0;
	Region.top		= c * 24;
	Region.right	= 32;
	Region.bottom	= Region.top + 24;
	
	GraphicsRef->DrawSprite(a, b, BlockGraphics, Region);
	
	/*
	//ROUT
	//STMFD   sp!,{r0-r12,lr}
	//TEQ	  r1,#0
	//LDMMIFD sp!,{r0-r12,pc}
	//CMP	  r1,#200
	//LDMGEFD sp!,{r0-r12,pc}		  ; 	  leave if completely off screen
	r0 = a;
	r1 = b;
	r2 = c;
	r3 = d;
	if( 	(r1 > 200)
	||	(r1 < 0) )
	return;
	//ADD	  r0,r3,r0,ASR#1
	r0 = r3 + (r0>>1);
	//BIC	  r0,r0,#3
	r0 = r0 & !(0x4);
	//MOV	  r3,#24				  ; 	  height of block
	r3 = 24;
	//CMP	  r1,#200-24			  ; 	  test for bottom clip
	//RSBGT   r3,r1,#200			  ; 	  adjust height
	if(r1 > (200 - 24))
	r3 = 200 - r1;
	
	  //MOV	  r4,#320/2
	  r4 = (320 / 2);
	  //MLA	  r0,r1,r4,r0			  ; 	  screen address
	  r0 = r1 * r4 + r0;
	  //LDR	  r1,%0 				  ; 	  pointer to colortablef
	  r1 = colortablef;
	  dmb_loop:
	  //LDMIA   r0,{r4-r7}
	  //LDRB	  r8,[r2],#1
	  r8 = r2;
	  r2++;
	  //LDR	  r8,[r1,r8,ASL#2]
	  r8 = r1 + r8 << 2;
	  //MVN	  r8,r8
	  r8 = -r8;
	  //BIC	  r4,r4,r8
	  r4 = r4 & !r8;
	  //LDRB	  r8,[r2],#1
	  r8 = r2;
	  r2++;
	  //LDR	  r8,[r1,r8,ASL#2]
	  r8 = r1 + r8 << 2;
	  //MVN	  r8,r8
	  r8 = -r8;
	  //BIC	  r5,r5,r8
	  r5 = r5 & !r8;
	  //LDRB	  r8,[r2],#1
	  r8 = r2;
	  r2++;
	  //LDR	  r8,[r1,r8,ASL#2]
	  r8 = r1 + r8 << 2;
	  //MVN	  r8,r8
	  r8 = -r8;
	  // BIC	  r6,r6,r8
	  r6 = r6 & !r8;
	  //LDRB	  r8,[r2],#1
	  r8 = r2;
	  r2++;
	  //LDR	  r8,[r1,r8,ASL#2]
	  r8 = r1 + r8 << 2;
	  //MVN	  r8,r8
	  r8 = -r8;
	  //BIC	  r7,r7,r8
	  r7 = r7 & !r8;
	  //LDMIA   r2!,{r8-r11}
	  //no idea
	  
		//ORR	  r4,r4,r8
		r4 = r4 | r8;
		//ORR	  r5,r5,r9
		r5 = r5 | r9;
		//ORR	  r6,r6,r10
		r6 = r6 | r10;
		//ORR	  r7,r7,r11
		r7 = r7 | r11;
		//STMIA   r0,{r4-r7}
		//huh????
		
		  //ADD	  r0,r0,#320/2
		  r0 = r0 + (320 / 2);
		  //SUBS	  r3,r3,#1
		  r3--;
		  //BNE	  dmb_loop
		  if(r3 > 0)
		  goto dmb_loop;
		  //LDMFD   sp!,{r0-r12,pc}
		  //0
		  // 	  DCD	  colortablef
	*/
}


//; 	  r0	  pointer to screen base
//; 	  r1	  xpos
//; 	  r2	  ypos
//; 	  r3	  sprite number

//I think I might be able to get away with our one here.. maybe not though
/*
CPopulous::draw_sprite()
ROUT
STMFD	sp!,{r0-r12,lr}
TEQ 	r2,#0
LDMMIFD sp!,{r0-r12,pc}
CMP 	r2,#200
LDMGEFD sp!,{r0-r12,pc}
ADD 	r0,r0,r1,ASR#1
BIC 	r0,r0,#3
MOV 	r4,#320/2
MLA 	r0,r2,r4,r0 			;		screen address (word aligned)
CMP 	r2,#200-16
MOVLE	r2,#16
RSBGT	r2,r2,#200				;		height
MOV 	r4,#20/2*16
LDR 	r5,%0					;		pointer to sprite_data
MLA 	r3,r4,r3,r5 			;		graphic address
LDR 	r4,%1					;		pointer to colortablef
draw_sprite_loop
LDRB	r5,[r3],#1
LDR 	r5,[r4,r5,ASL#2]
MVN 	r5,r5
LDRB	r6,[r3],#1
LDR 	r6,[r4,r6,ASL#2]
MVN 	r6,r6					;		mask data
LDRB	r8,[r3],#1
LDRB	r9,[r3],#1
ORR 	r8,r8,r9,LSL#8
LDRB	r9,[r3],#1
ORR 	r8,r8,r9,LSL#16
LDRB	r9,[r3],#1
ORR 	r8,r8,r9,LSL#24
LDRB	r9,[r3],#1
LDRB	r10,[r3],#1
ORR 	r9,r9,r10,LSL#8
LDRB	r10,[r3],#1
ORR 	r9,r9,r10,LSL#16
LDRB	r10,[r3],#1
ORR 	r9,r9,r10,LSL#24		;		sprite data
AND 	r10,r1,#7				;		get pixel offset
ADD 	pc,pc,r10,ASL#2 		;		boing !
MOVNV	r0,r0
B		draw_sprite_00
B		draw_sprite_01
B		draw_sprite_02
B		draw_sprite_03
B		draw_sprite_04
B		draw_sprite_05
B		draw_sprite_06
B		draw_sprite_07
draw_sprite_00
MOV 	r7,#0					;		extra mask word
MOV 	r10,#0					;		extra sprite word
B		draw_sprite_08
draw_sprite_01
MOV 	r7,r6,LSR#28
MOV 	r6,r6,LSL#4
ORR 	r6,r6,r5,LSR#28
MOV 	r5,r5,LSL#4
MOV 	r10,r9,LSR#28
MOV 	r9,r9,LSL#4
ORR 	r9,r9,r8,LSR#28
MOV 	r8,r8,LSL#4
B		draw_sprite_08
draw_sprite_02
MOV 	r7,r6,LSR#24
MOV 	r6,r6,LSL#8
ORR 	r6,r6,r5,LSR#24
MOV 	r5,r5,LSL#8
MOV 	r10,r9,LSR#24
MOV 	r9,r9,LSL#8
ORR 	r9,r9,r8,LSR#24
MOV 	r8,r8,LSL#8
B		draw_sprite_08
draw_sprite_03
MOV 	r7,r6,LSR#20
MOV 	r6,r6,LSL#12
ORR 	r6,r6,r5,LSR#20
MOV 	r5,r5,LSL#12
MOV 	r10,r9,LSR#20
MOV 	r9,r9,LSL#12
ORR 	r9,r9,r8,LSR#20
MOV 	r8,r8,LSL#12
B		draw_sprite_08
draw_sprite_04
MOV 	r7,r6,LSR#16
MOV 	r6,r6,LSL#16
ORR 	r6,r6,r5,LSR#16
MOV 	r5,r5,LSL#16
MOV 	r10,r9,LSR#16
MOV 	r9,r9,LSL#16
ORR 	r9,r9,r8,LSR#16
MOV 	r8,r8,LSL#16
B		draw_sprite_08
draw_sprite_05
MOV 	r7,r6,LSR#12
MOV 	r6,r6,LSL#20
ORR 	r6,r6,r5,LSR#12
MOV 	r5,r5,LSL#20
MOV 	r10,r9,LSR#12
MOV 	r9,r9,LSL#20
ORR 	r9,r9,r8,LSR#12
MOV 	r8,r8,LSL#20
B		draw_sprite_08
draw_sprite_06
MOV 	r7,r6,LSR#8
MOV 	r6,r6,LSL#24
ORR 	r6,r6,r5,LSR#8
MOV 	r5,r5,LSL#24
MOV 	r10,r9,LSR#8
MOV 	r9,r9,LSL#24
ORR 	r9,r9,r8,LSR#8
MOV 	r8,r8,LSL#24
B		draw_sprite_08
draw_sprite_07
MOV 	r7,r6,LSR#4
MOV 	r6,r6,LSL#28
ORR 	r6,r6,r5,LSR#4
MOV 	r5,r5,LSL#28
MOV 	r10,r9,LSR#4
MOV 	r9,r9,LSL#28
ORR 	r9,r9,r8,LSR#4
MOV 	r8,r8,LSL#28
draw_sprite_08
LDR 	r11,[r0,#0]
BIC 	r5,r11,r5
ORR 	r5,r5,r8
LDR 	r11,[r0,#4]
BIC 	r6,r11,r6
ORR 	r6,r6,r9
LDR 	r11,[r0,#8]
BIC 	r7,r11,r7
ORR 	r7,r7,r10
CMP 	r1,#320-16
STMLTIA r0,{r5-r7}
BLT 	done_xclip
CMP 	r1,#320-8
STMLTIA r0,{r5-r6}
BLT 	done_xclip
CMP 	r1,#320
STMLTIA r0,{r5}
done_xclip
ADD 	r0,r0,#320/2
SUBS	r2,r2,#1
BNE 	draw_sprite_loop
LDMFD	sp!,{r0-r12,pc}
0
DCD 	sprite_data
1
DCD 	colortablef

  
	
	  ;		r0		pointer to screen base
	  ;		r1		xpos
	  ;		r2		ypos
	  ;		r3		sprite number
	  
		draw_s32
		ROUT
		STMFD	sp!,{r0-r12,lr}
		TEQ 	r2,#0
		LDMMIFD sp!,{r0-r12,pc}
		CMP 	r2,#200
		LDMGEFD sp!,{r0-r12,pc}
		ADD 	r0,r0,r1,ASR#1
		BIC 	r0,r0,#3
		MOV 	r4,#320/2
		MLA 	r0,r2,r4,r0 			;		screen address (word aligned)
		AND 	r1,r1,#7				;		get pixel offset
		CMP 	r2,#200-32
		MOVLE	r2,#32
		RSBGT	r2,r2,#200				;		height
		MOV 	r4,#20/2*32
		LDR 	r5,%0					;		pointer to big_font
		MLA 	r3,r4,r3,r5 			;		graphic address
		LDR 	r4,%1					;		pointer to colortablef
		B		draw_sprite_loop
		0
		DCD 	big_font
		1
		DCD 	colortablef
		
		  
			ts1
			%		8*40*5
			
			  
				ts2
				%		8*40*5
				ALIGN
				
				  
					;		r0		pointer to screen
					;		r1		position in map
					;		r2		sprite number
*/

void CPopulous::draw_building(long r0, long r1, long r2)
{
//;       r0      pointer to screen
//;       r1      position in map
//;       r2      sprite number

//draw_building
	//	ROUT
	//	STMFD	sp!,{r0-r12,lr}
	//	MOV 	r2,#0
	long Lscreen = r0;
	long Lsprite = r2;
	long r3, r4, r5, r6;

	r3 = r0;
	r4 = r1;
	r5 = r2;

	r2 = 0;

db_loop:
	//	SUBS	r1,r1,#10
	r1 -= 10;
	
	//	ADDPL	r2,r2,#1
	//	BPL 	db_loop
	if (r1 >= 0)
	{
		r2++;
		goto db_loop;
	}

	//	ADD 	r1,r1,#10
	r1 += 10;

	//	MOV 	r0,r1
	//	SUB 	r0,r0,r2
	//	ADD 	r1,r1,r2
	r0 = r1;
	r0 -= r2;
	r1 += r2;

	//	MOV 	r0,r0,ASL#4
	//	MOV 	r1,r1,ASL#3
	r0 = r0 << 4;
	r1 = r1 << 3;

	//	ADD 	r0,r0,#160-16
	//	ADD 	r1,r1,#200-160-8-8
	r0 += 160 - 16;
	r1 += 200 - 160 - 8 - 8;

	//	LDMFD	sp,{r3-r5}
	// Where the hell does this info come from? 
	// More importantly why? 
	// r3 is ignored
	// r4 is used to modify the screen pos.
	// r5 is the sprite number

	//	LDR 	r6,%0					;		pointer to map_blk
	r6 = (long)title_map_blk;

	//	ADD 	r6,r6,#200
	r6 += 200;

	//	LDRB	r6,[r6,r4]				;		get altitude of map block
	r6 = *(UBYTE*)(r6 + r4);

	//	MOV 	r6,r6,ASL#16
	r6 = r6 << 16;
	r6 *= 8;	// Fix.

	//	SUB 	r1,r1,r6,ASR#16 		;		modify screen pos
	r1 -= (r6 >> 16);

	//	MOV 	r6,#40/2*24
	r6 = 40/2*24;

	//	LDR 	r7,%1
	//	LDR 	r7,[r7]
	//r7 = blk_data;

	//	MLA 	r2,r5,r6,r7 			;		pointer to graphic
//	r2 = r5;// * r6 + r7;

	//	BL		draw_map_block
	draw_map_block(r0, r1, Lsprite, Lscreen); 
	//	LDMFD	sp!,{r0-r12,pc}
//0
//		DCD 	map_blk
//1
//		DCD 	blk_data

//		END


	/*

	long r3, r4, r5, r6, r7;
	
	//ROUT
	//STMFD   sp!,{r0-r12,lr}
	//MOV	  r2,#0
	r2 = 0;
db_loop:
	//SUBS	  r1,r1,#10
	r1 -= 10;
	//ADDPL   r2,r2,#1
	r2++;
	//BPL	  db_loop
	if(r2 >= r1)
		goto db_loop;
	//ADD	  r1,r1,#10
	r1 += 10;
	//MOV	  r0,r1
	r0 = r1;
	//SUB	  r0,r0,r2
	r0 -= r2;
	//ADD	  r1,r1,r2
	r1 += r2;
	//MOV	  r0,r0,ASL#4
	r0 = r0 << 4;
	//MOV	  r1,r1,ASL#3
	r1 = r1 << 3;  
	//ADD	  r0,r0,#160-16
	r0 += (160 - 16);
	//ADD	  r1,r1,#200-160-8-8
	r1 += (200-160-8-8);
	//LDMFD   sp,{r3-r5}
	//LDR	  r6,%0 				  ; 	  pointer to title_map_blk
	r6 = *title_map_blk;
	//ADD	  r6,r6,#200
	r6 += 200;
	//LDRB	  r6,[r6,r4]			  ; 	  get altitude of map block
	r6 += r4;
	//MOV	  r6,r6,ASL#16
	r6 = r6 << 16;
	//SUB	  r1,r1,r6,ASR#16		  ; 	  modify screen pos
	r1 -= (r6 >> 16);
	//MOV	  r6,#40/2*24
	r6 = 40/2*24;
	//LDR	  r7,%1
	//LDR	  r7,[r7]
	r7 = blk_data;
	//MLA	  r2,r5,r6,r7			  ; 	  pointer to graphic
	r2 = (r5 * r6) + r7;
	//BL	  draw_map_block
	draw_map_block(r0, r1, r2, r3);
	//LDMFD   sp!,{r0-r12,pc}
	//0
	//		  DCD	  title_map_blk
	//1
	// 	  DCD	  blk_data
	*/
}

//		 END
