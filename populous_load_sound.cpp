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

#include	"God.h"

//extern	UBYTE			measures[];
//extern	USHORT			seqlen;
//extern	struct SeqItem	sequn[];
//extern	USHORT			measln[];
//UBYTE					measures[];
//USHORT					seqlen;
//struct SeqItem			sequn[];
//USHORT					measln[];
UBYTE					*curmeas;
USHORT					curseq;
USHORT					curbeat;
USHORT					curtime;


void	CPopulous::PlaySound(USHORT number, USHORT channel)
{
	if (number>=0 && number<=113 && channel<=4)
		play_sound(number, channel);
}


void	CPopulous::PlayMeas()
{
	curbeat++;
	if(curbeat == measln[sequn[curseq].Measure])
	{
		curtime++;
		curbeat=0;
		if(curtime >= sequn[curseq].Times)
		{
			curseq++;
			if(curseq == seqlen)
				curseq=0;
		}
		curmeas=measures+(sequn[curseq].Measure<<8);
	}
}


void	CPopulous::load_sound(UBYTE *fname)
{
	int 	i,t1,t2;
	for(i=0;i!=64;i++)
	{
		t1 = (measln[i] << 8) & 0xff00;
		t2 = (measln[i] >> 8) & 0x00ff;
		measln[i] = t1 | t2;
	}
	t1 = (seqlen << 8) & 0xff00;
	t2 = (seqlen >> 8) & 0x00ff;
	seqlen	= t1 | t2;
	for(i=0;i!=seqlen;i++)
	{
		t1 = (sequn[i].Measure << 8) & 0xff00;
		t2 = (sequn[i].Measure >> 8) & 0x00ff;
		sequn[i].Measure  = t1 | t2;
		
		t1 = (sequn[i].Times << 8) & 0xff00;
		t2 = (sequn[i].Times >> 8) & 0x00ff;
		sequn[i].Times	= t1 | t2;
	}		
	curmeas=measures+(sequn[0].Measure<<8);
	curbeat=curseq=curtime=0;
}


void	CPopulous::kill_effect(SHORT from, SHORT to)
{
/*
SHORT	c1;
for(c1=from;c1<to;c1++)
AbortIO(ioas[c1]);
	*/
}

int		CPopulous::check_effect(SHORT no)
{
	SHORT ret = ((SHORT)!test_channel(no));
	return(ret);
	//return 1;
}



void	CPopulous::ProcessAudio()
{
	long	r0, r1, r3;
	UBYTE*	r2;

	//	LDR 	r1,%5					;		pointer to music_off
	//	LDRB	r0,[r1,#0]
	//	LDRB	r1,[r1,#1]
	//	ORRS	r0,r0,r1,LSL#8
	//	BEQ 	music_is_off
	r0 = music_off;
	if (music_off == 0)
		goto music_is_off;

	//	LDR 	r1,%6					;		pointer to music_now
	//	LDRB	r0,[r1,#0]
	//	LDRB	r1,[r1,#1]
	//	ORR 	r0,r0,r1,LSL#8
	//	MOV 	r0,r0,ASL#16
	//	MOV 	r0,r0,ASR#16
	r0 = music_now;
	
	//	ADD 	r0,r0,#1
	r0++;

	//	LDR 	r2,%7					;		pointer to music
	//	LDRB	r1,[r2,#0]
	//	LDRB	r2,[r2,#1]
	//	ORR 	r1,r1,r2,LSL#8
	//	MOV 	r1,r1,ASL#16
	//	MOV 	r1,r1,ASR#16
	r1 = music;

	//	CMP 	r0,r1
	//	BNE 	no_music
	if (r0 != r1)
		goto no_music;

	//	MOV 	r1,#MUSIC_TEMPO
	r1 = MUSIC_TEMPO;

	//	LDR 	r2,%7					;		pointer to music
	//	STRB	r1,[r2,#0]
	//	MOV 	r1,r1,LSR#8
	//	STRB	r1,[r2,#1]
	music = r1;

	//	LDR 	r2,%8					;		pointer to curmeas
	//	LDR 	r2,[r2]
	r2 = curmeas;

	//	LDR 	r4,%9					;		are:curbeat
	//	LDRB	r3,[r4,#0]
	//	LDRB	r4,[r4,#1]
	//	ORR 	r3,r3,r4,LSL#8
	r3 = curbeat;

	//	ADD 	r2,r2,r3
	r2 += r3;

	//	LDRB	r0,[r2,#0]
	r0 = *r2;

	//	SUB 	r0,r0,#65
	r0 -= 65;

	//	MOV 	r1,#0
	r1 = 0;

	//	BL		PlaySound
//	PlaySound(r0, r1);
	mod_play_sound(r0, r1);

	//	LDRB	r0,[r2,#64]
	r0 = *(r2 + 64);

	//	SUB 	r0,r0,#65
	r0 -= 65;

	//	MOV 	r1,#1
	r1 = 1;

	//	BL		PlaySound
//	PlaySound(r0, r1);
	mod_play_sound(r0, r1);

	//	LDRB	r0,[r2,#128]
	r0 = *(r2 + 128);

	//	SUB 	r0,r0,#65
	r0 -= 65;

	//	MOV 	r1,#2
	r1 = 2;

	//	BL		PlaySound
//	PlaySound(r0, r1);
	mod_play_sound(r0, r1);

	//	LDRB	r0,[r2,#192]
	r0 = *(r2 + 192);

	//	SUB 	r0,r0,#65
	r0 -= 65;

	//	MOV 	r1,#3
	r1 = 3;

	//	BL		PlaySound
//	PlaySound(r0, r1);
	mod_play_sound(r0, r1);

	//	BL		PlayMeas
	PlayMeas();

	//	MOV 	r0,#0
	r0 = 0;

no_music:
	//	LDR 	r1,%6					;		pointer to music_now
	//	STRB	r0,[r1,#0]
	//	MOV 	r0,r0,LSR#8
	//	STRB	r0,[r1,#1]
	music_now = r0;

music_is_off:
	//	LDR 	r1,%10					;		pointer to effect_off
	//	LDRB	r0,[r1,#0]
	//	LDRB	r1,[r1,#1]
	//	ORRS	r0,r0,r1,LSL#8
	//	BEQ 	effect_is_off
	if (effect_off == 0)
		goto effect_is_off;

	//	LDR 	r1,%11					;		pointer to effect
	//	LDRB	r0,[r1,#0]
	//	LDRB	r1,[r1,#1]
	//	ORRS	r0,r0,r1,LSL#8
	//	BEQ 	do_heart
	if (effect == 0)
		goto do_heart;

	//	MOV 	r0,#MUSIC_TEMPO*4
	r0 = MUSIC_TEMPO*4;

	//	LDR 	r1,%7					;		pointer to music
	//	STRB	r0,[r1,#0]
	//	MOV 	r0,r0,LSR#8
	//	STRB	r0,[r1,#1]
	music = r0;

	//	LDR 	r1,%11					;		pointer to effect
	//	LDRB	r0,[r1,#0]
	//	LDRB	r1,[r1,#1]
	//	ORRS	r0,r0,r1,LSL#8
	//	MOV 	r0,r0,ASL#16
	//	MOV 	r0,r0,ASR#16
	//	MOV 	r1,#2
	r0 = effect;
	r1 = 2;

	//	BL		PlaySound
	PlaySound(r0, r1);

	//	LDR 	r1,%11					;		pointer to effect
	//	LDRB	r0,[r1,#0]
	//	LDRB	r1,[r1,#1]
	//	ORRS	r0,r0,r1,LSL#8
	//	MOV 	r0,r0,ASL#16
	//	MOV 	r0,r0,ASR#16
	//	MOV 	r1,#3
	//	BL		PlaySound
	PlaySound(effect, 3);

	//	MOV 	r0,#0
	//	LDR 	r1,%11					;		pointer to effect
	//	STRB	r0,[r1,#0]
	//	STRB	r0,[r1,#1]
	effect = 0;

	//	B		effect_is_off
	goto effect_is_off;


// HEART BEAT CODE REMOVED.
do_heart:
	//	LDR 	r1,%12					;		pointer to tempo
	//	LDRB	r0,[r1,#0]
	//	LDRB	r1,[r1,#1]
	//	ORRS	r0,r0,r1,LSL#8
	//	BEQ 	effect_is_off
	if (tempo == 0)
		goto effect_is_off;

	//	LDR 	r1,%13					;		pointer to tempo_now
	//	LDRB	r0,[r1,#0]
	//	LDRB	r1,[r1,#1]
	//	ORR 	r0,r0,r1,LSL#8
	//	MOV 	r0,r0,ASL#16
	//	MOV 	r0,r0,ASR#16
	r0 = tempo_now;

	//	ADD 	r0,r0,#1
	r0++;

	//	MOV 	r1,r0
	r1 = r0;

	//	LDR 	r2,%13					;		pointer to tempo_now
	//	STRB	r1,[r2,#0]
	//	MOV 	r1,r1,LSR#8
	//	STRB	r1,[r2,#1]
	tempo_now = r1;

	//	LDR 	r2,%12					;		pointer to tempo
	//	LDRB	r1,[r2,#0]
	//	LDRB	r2,[r2,#1]
	//	ORR 	r1,r1,r2,LSL#8
	//	MOV 	r1,r1,ASL#16
	//	MOV 	r1,r1,ASR#16
	r1 = tempo;

	//	CMP 	r0,r1
	//	BLT 	do_heart2
	if (r0 < r1)
		goto do_heart2;

	//	MOV 	r0,#TUNE_HEART1
	//	MOV 	r1,#0
	//	BL		PlaySound
	PlaySound(TUNE_HEART1, 0);

	//	MOV 	r0,#0
	//	LDR 	r1,%13					;		pointer to tempo_now
	//	STRB	r0,[r1,#0]
	//	STRB	r0,[r1,#1]
	tempo_now = 0;

	//	B		effect_is_off
	goto effect_is_off;

do_heart2:
	//	LDR 	r1,%12					;		pointer to tempo
	//	LDRB	r0,[r1,#0]
	//	LDRB	r1,[r1,#1]
	//	ORR 	r0,r0,r1,LSL#8
	//	MOV 	r0,r0,ASL#16
	//	MOV 	r0,r0,ASR#16
	r0 = tempo;

	//	LDR 	r2,%14					;		pointer to beat_two
	//	LDRB	r1,[r2,#0]
	//	LDRB	r2,[r2,#1]
	//	ORR 	r1,r1,r2,LSL#8
	//	MOV 	r1,r1,ASL#16
	//	MOV 	r1,r1,ASR#16
	r1 = beat_two;

	//	SUB 	r0,r0,r1
	r0 -= r1;

	//	LDR 	r2,%13					;		pointer to tempo_now
	//	LDRB	r1,[r2,#0]
	//	LDRB	r2,[r2,#1]
	//	ORR 	r1,r1,r2,LSL#8
	//	MOV 	r1,r1,ASL#16
	//	MOV 	r1,r1,ASR#16
	r1 = tempo_now;

	//	CMP 	r0,r1
	//	BNE 	effect_is_off
	if (r0 != r1)
		goto effect_is_off;

	//	MOV 	r0,#TUNE_HEART2
	//	MOV 	r1,#0
	//	BL		PlaySound
	PlaySound(TUNE_HEART2, 0);

effect_is_off:
	//	LDMFD	sp!,{r11,lr}
	//	TEQP	r11,#0
	//	MOVNV	r0,r0					;		restore processor mode
	//	LDMFD	sp!,{r0-r12,pc}
	return;
/*
0
        DCD     mousex
1
        DCD     mousey
2
        DCD     left_button
3
        DCD     right_button
4
        DCD     shift
5
        DCD     music_off
6
        DCD     music_now
7
        DCD     music
8
        DCD     curmeas
9
        DCD     curbeat
10
        DCD     effect_off
11
        DCD     effect
12
        DCD     tempo
13
        DCD     tempo_now
14
        DCD     beat_two
*/
}
