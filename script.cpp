#include	"god.h"

/*
#define WALKER_BUILDS       100
#define WALKER_EFFECT       130
#define WALKER_WAITS        200
#define WALKER_LOOPS        220
#define BD0                 WALKER_BUILDS+0 //build house
#define BW0                 BD0+12          //build wreck
#define BD1                 BD0+1
#define BW1                 BW0+1           //build wreck
#define BD2                 BD0+2
#define BW2                 BW0+2           //build wreck
#define BD3                 BD0+3
#define BW3                 BW0+3           //build wreck
#define BD4                 BD0+4
#define BW4                 BW0+4           //build wreck
#define BD5                 BD0+5
#define BW5                 BW0+5           //build wreck
#define BD6                 BD0+6
#define BW6                 BW0+6           //build wreck
#define BD7                 BD0+7
#define BW7                 BW0+7           //build wreck
#define BD8                 BD0+8
#define BW8                 BW0+8           //build wreck
#define BD9                 BD0+9
#define BW9                 BW0+9           //build wreck
#define BDa                 BD0+10
#define BWa                 BW0+10
#define BDb                 BD0+11
#define BWb                 BW0+11
#define WB0                 WALKER_EFFECT+27 //blue player waits
#define WB1                 WB0+1
#define WR0                 WB0+2           //red player waits
#define WR1                 WB0+3
#define BS0                 WALKER_EFFECT+19
#define BS1                 BS0+1
#define BS2                 BS0+2
#define BS3                 BS0+3
#define DK0                 WALKER_EFFECT+67
#define DK1                 DK0+1
#define DK2                 DK0+2
#define KW0                 WALKER_EFFECT+47
#define KW1                 KW0+1
#define KW2                 KW0+2
#define KW3                 KW0+3
#define FR0                 WALKER_EFFECT+31
#define FR1                 FR0+1
#define FR2                 FR0+2
#define FR3                 FR0+3
#define RE0                 253             //res on 0
#define RE1                 254             //res on 1
#define HLD                 252             //hold till reson
#define WA0                 251             //cheer a battle
#define WF0                 250             //plant a flag
#define FF0                 249             //fight of kights
#define FF1                 248
#define SET                 247
#define END                 246
#define STP                 255             //stop using character
*/

#define WALKER_BUILDS	(100)
#define WALKER_EFFECT	(130)
#define WALKER_WAITS	(200)
#define WALKER_LOOPS	(220)
#define BD0 			(WALKER_BUILDS+0)
#define BW0 			(BD0+12)
#define BD1 			(BD0+1)
#define BW1 			(BW0+1)
#define BD2 			(BD0+2)
#define BW2 			(BW0+2)
#define BD3 			(BD0+3)
#define BW3 			(BW0+3)
#define BD4 			(BD0+4)
#define BW4 			(BW0+4)
#define BD5 			(BD0+5)
#define BW5 			(BW0+5)
#define BD6 			(BD0+6)
#define BW6 			(BW0+6)
#define BD7 			(BD0+7)
#define BW7 			(BW0+7)
#define BD8 			(BD0+8)
#define BW8 			(BW0+8)
#define BD9 			(BD0+9)
#define BW9 			(BW0+9)
#define BDa 			(BD0+10)
#define BWa 			(BW0+10)
#define BDb 			(BD0+11)
#define BWb 			(BW0+11)
#define WB0 			(WALKER_EFFECT+27)
#define WB1 			(WB0+1)
#define WR0 			(WB0+2)
#define WR1 			(WB0+3)
#define BS0 			(WALKER_EFFECT+19)
#define BS1 			(BS0+1)
#define BS2 			(BS0+2)
#define BS3 			(BS0+3)
#define DK0 			(WALKER_EFFECT+67)
#define DK1 			(DK0+1)
#define DK2 			(DK0+2)
#define KW0 			(WALKER_EFFECT+47)
#define KW1 			(KW0+1)
#define KW2 			(KW0+2)
#define KW3 			(KW0+3)
#define FR0 			(WALKER_EFFECT+31)
#define FR1 			(FR0+1)
#define FR2 			(FR0+2)
#define FR3 			(FR0+3)
#define RE0 			(253)
#define RE1 			(254)
#define HLD 			(252)
#define WA0 			(251)
#define WF0 			(250)
#define FF0 			(249)
#define FF1 			(248)
#define SET 			(247)
#define END 			(246)
#define STP 			(255)


/*)
UBYTE blue1[] =	{   38,26,BW0),WB0,WB1,BD0,WB0,WB1,WB0,WB1,WB0,WB1,04,BW0,WB0,WB1,
				    BD0,WB0,WB1,WB0,WB1,WB0,WB1,BW0,BW1,WB0,WB1,BD1,26,BW0,BW1,WB0,
				    WB1,BD1,04,BW1,BW2,WB0,WB1,BD2,26,BW1,BW2,WB0,WB1,BD2,26,BW2,
				    BW3,WB0,WB1,BD3,65,50,WB0,WB1,WB0,WB1,WB0,WB1,WB0,WB1,WB0,WB1,
				    4,BW2,BW3,WB0,WB1,BD3,WB0,WB1,WB0,WB1,30,WB0,WB1,WB0,WB1,WB0,
				    4,BW3,BW5,WB0,WB1,BD5,26,BW2,BW5,WB0,WB1,BD5,04,BW5,BW7,WB0,
				    WB1,WB0,WB1,BD7,26,BW5,BW7,BW0,BW1,BD7,04,BW7,BW8,WB0,WB1,BDm,
				    26,BW7,BW8,WB0,WB1,BDm,86,WB0,WB1,WB0,WB1,WB0,WB1,WB0,WB1,WB0,
				    14,BWa,WB0,WB1,BDa,16,BWa,WB0,WB1,BDa,15,WB0,WB1,WB0,WB1,WB0,RE0,
				    HLD,44,WA0,50,BW8,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,15,
					STP  };


UBYTE	blue2[] =   {  44,24,BW0,WB0,WB1,BD0,26,06,BW0,WB0,WB1,BD0,24,BW0,BW1,WB0,
				    WB1,BD1,06,BW1,BW2,WB0,WB1,BD2,24,BW1,BW2,WB0,WB1,BD2,06,BW2,
				    BW3,WB0,WB1,BD3,24,BW2,BW3,WB0,WB1,BD3,06,BW3,BW4,WB0,WB1,BD4,
				    24,BW3,BW4,WB0,WB1,BD4,06,BW4,BW5,WB0,WB1,BD5,24,BW4,BW5,WB0,
				    WB1,BD5,06,BW5,BW6,WB0,WB1,BD6,24,BW5,BW6,WB0,WB1,BD6,06,BW6,
				    BW8,WB0,WB1,BDm,24,BW6,BW8,WB0,WB1,BDm,53,68,85,87,86,96,BS0,BS1,
				    BS2,BS3,BS0,BS1,BS2,BS3,BS0,BS1,BS2,BS3,BS0,BS1,BS2,BS3,BS0,BS1,
				    BS2,BS3,BS0,BS1,BS2,BS3,BS0,BS1,BS2,BS3,BS0,BS1,BS2,BS3,BS0,BS1,BS2,BS3,BS0,BS1,
				    5,BWb,WB0,WB1,BDb,25,BWb,WB0,WB1,BDb,15,BW9,BD9,
				    HLD,45,WA0,72,BW8,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,15,
					STP  };


UBYTE blue3[] =  { HLD,46,WA0,62,BWa,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,15,STP };   


UBYTE blue4[] =  { HLD,47,WA0,52,BWa,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,15,STP };     


UBYTE blue5[] =  { HLD,48,WA0,42,BW8,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,15,STP };


UBYTE blue6[] =  { HLD,59,WA0,42,15,STP };


UBYTE blue7[] =  {  HLD,56,65,FF0,DK0,DK0,DK1,DK1,DK2,DK2,DK2,DK2,DK2,DK2,DK2,
				   DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,
				   DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,
				   DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,
				   DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,
				   DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,
				   DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,
				   STP };


UBYTE red1[] = {  83,72,BW0,WR0,WR1,BD0,42,BW0,WR0,WR1,BD0,
				 72,BW0,BW1,WR0,WR1,BD1,42,BW0,BW1,WR0,WR1,BD1,
				 72,BW1,BW2,WR0,WR1,BD2,42,BW1,BW2,WR0,WR1,BD2,
				 72,BW2,BW3,WR0,WR1,BD3,42,BW2,BW3,WR0,WR1,BD3,
				 72,BW3,BW4,WR0,WR1,BD4,42,BW3,BW4,WR0,WR1,BD4,
				 72,BW4,BW5,WR0,WR1,BD5,42,BW4,BW5,WR0,WR1,BD5,
				 72,BW5,BW6,WR0,WR1,BD6,42,BW5,BW6,WR0,WR1,BD6, 
				 72,BW6,BW7,WR0,WR1,BD7,42,BW6,BW7,WR0,WR1,BD7,
				 72,BW7,BW8,WR0,WR1,BDm,42,BW7,BW8,WR0,WR1,BDm,
				 52,BWa,WR0,WR1,BDa,WR0,WR1,WR0,WR1,WR0,WR1,WR0,50,RE1,
				 HLD,94,WA0,24,BW8,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,50,
				 STP };

UBYTE red2[] =  {   80,50,31,WR0,WR1,WR0,WR1,WR0,WR1,50,
				   BW0,WR0,WR1,WR0,WR1,BD0,70,
				   50,BW0,BW1,WR0,WR1,WR0,WR1,BD1,71,
				   50,BW1,BW2,WR0,WR1,WR0,WR1,BD2,61,
				   50,BW2,BW3,WR0,WR1,WR0,WR1,BD3,51,
				   50,BW3,BW4,WR0,WR1,WR0,WR1,BD4,41,
				   50,BW4,BW5,WR0,WR1,WR0,WR1,BD5,31,
				   50,BW5,BW6,WR0,WR1,WR0,WR1,BD6,71,
				   50,BW6,BW7,WR0,WR1,WR0,WR1,BD7,20,
				   50,BW7,BW8,WR0,WR1,WR0,WR1,BDm,
				   66,18,WR0,WR1,WR0,WR1,WR0,WR1,WR0,WR1,WR0,WR1,
				   62,BWa,WR0,WR1,BDa,50,
				   HLD,83,WA0,04,BW8,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,50,
				   STP };

UBYTE red3[] = { HLD,73,WA0,26,BW8,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,50,STP };

UBYTE red4[] = { HLD,63,WA0,06,BW8,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,50,STP };

UBYTE red5[] = { HLD,53,WA0,14,BWa,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,50,STP };

UBYTE red6[] = { HLD,43,WA0,05,BWb,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,50,STP };

UBYTE red7[] = { HLD,74,65,FF0,KW0,KW1,KW2,KW3,KW3,KW2,KW1,KW0,KW1,KW2,KW3,
				025,BWb,16,BWa,50,74,SET,15,65,END,
				STP };

UBYTE b_flag[] = { HLD,56,WF0,WF0,STP };

UBYTE r_flag[] = { HLD,74,WF0,WF0,15,WF0,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,STP };

UBYTE dests_array[] = {
						 blue1[162],	// 160
						 blue2[162],	// 162
						 blue3[162],	// 15
						 blue4[162],	// 15
						 blue5[162],	// 15
						 blue6[162],	// 6
						 blue7[162],	// 100
						 red1[162],		// 136
						 red2[162],		// 121
						 red3[162],		// 15
						 red4[162],		// 15
						 red5[162],		// 15
						 red6[162],		// 15
						 red7[162],		// 26
						 b_flag[162],	// 5
						 r_flag[162]	// 15
					};
*/

#define	PADDING_26		0,0,0,0,0,				\
						0,0,0,0,0,				\
						0,0,0,0,0,				\
						0,0,0,0,0,				\
						0,0,0,0,0,				\
						0

#define	PADDING_41		PADDING_26,				\
						0,0,0,0,0,				\
						0,0,0,0,0,				\
						0,0,0,0,0				

#define	PADDING_62		PADDING_26,				\
						PADDING_26,				\
						0,0,0,0,0,				\
						0,0,0,0,0

#define	PADDING_136		PADDING_62,				\
						PADDING_62,				\
						0,0,0,0,0,				\
						0,0,0,0,0,				\
						0,0

#define	PADDING_147		PADDING_136,			\
						0,0,0,0,0,				\
						0,0,0,0,0,				\
						0

#define	PADDING_156		PADDING_147,			\
						0,0,0,0,0,				\
						0,0,0,0

UBYTE	dest_array[] = {
						// Blue 1. (160)
						38,26,BW0,WB0,WB1,BD0,WB0,WB1,WB0,WB1,WB0,WB1,4,BW0,WB0,WB1,
						BD0,WB0,WB1,WB0,WB1,WB0,WB1,BW0,BW1,WB0,WB1,BD1,26,BW0,BW1,WB0,
						WB1,BD1,4,BW1,BW2,WB0,WB1,BD2,26,BW1,BW2,WB0,WB1,BD2,26,BW2,
						BW3,WB0,WB1,BD3,65,50,WB0,WB1,WB0,WB1,WB0,WB1,WB0,WB1,WB0,WB1,
						4,BW2,BW3,WB0,WB1,BD3,WB0,WB1,WB0,WB1,30,WB0,WB1,WB0,WB1,WB0,
						4,BW3,BW5,WB0,WB1,BD5,26,BW2,BW5,WB0,WB1,BD5,4,BW5,BW7,WB0,
						WB1,WB0,WB1,BD7,26,BW5,BW7,BW0,BW1,BD7,4,BW7,BW8,WB0,WB1,BD8,
						26,BW7,BW8,WB0,WB1,BD8,86,WB0,WB1,WB0,WB1,WB0,WB1,WB0,WB1,WB0,
						14,BWa,WB0,WB1,BDa,16,BWa,WB0,WB1,BDa,15,WB0,WB1,WB0,WB1,WB0,RE0,
						HLD,44,WA0,50,BW8,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,15,
						STP,
						0,0,	// Padding

						// Blue 2. (162)
						44,24,BW0,WB0,WB1,BD0,26,6,BW0,WB0,WB1,BD0,24,BW0,BW1,WB0,
						WB1,BD1,6,BW1,BW2,WB0,WB1,BD2,24,BW1,BW2,WB0,WB1,BD2,6,BW2,
						BW3,WB0,WB1,BD3,24,BW2,BW3,WB0,WB1,BD3,6,BW3,BW4,WB0,WB1,BD4,
						24,BW3,BW4,WB0,WB1,BD4,6,BW4,BW5,WB0,WB1,BD5,24,BW4,BW5,WB0,
						WB1,BD5,6,BW5,BW6,WB0,WB1,BD6,24,BW5,BW6,WB0,WB1,BD6,6,BW6,
						BW8,WB0,WB1,BD8,24,BW6,BW8,WB0,WB1,BD8,53,68,85,87,86,96,BS0,BS1,
						BS2,BS3,BS0,BS1,BS2,BS3,BS0,BS1,BS2,BS3,BS0,BS1,BS2,BS3,BS0,BS1,
						BS2,BS3,BS0,BS1,BS2,BS3,BS0,BS1,BS2,BS3,BS0,BS1,BS2,BS3,BS0,BS1,BS2,BS3,BS0,BS1,
						5,BWb,WB0,WB1,BDb,25,BWb,WB0,WB1,BDb,15,BW9,BD9,
						HLD,45,WA0,72,BW8,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,15,
						STP,

						// Blue 3. (15)
						HLD,46,WA0,62,BWa,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,15,STP,
						PADDING_147,

						// Blue 4. (15)
						HLD,47,WA0,52,BWa,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,15,STP,
						PADDING_147,

						// Blue 5. (15)
						HLD,48,WA0,42,BW8,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,15,STP,
						PADDING_147,

						// Blue 6. (6)
						HLD,59,WA0,42,15,STP,
						PADDING_156,

						// Blue 7. (100)
						HLD,56,65,FF0,DK0,DK0,DK1,DK1,DK2,DK2,DK2,DK2,DK2,DK2,DK2,
						DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,
						DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,
						DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,
						DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,
						DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,
						DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,DK2,
						STP,
						PADDING_62,

						// Red 1 (136)
						83,72,BW0,WR0,WR1,BD0,42,BW0,WR0,WR1,BD0,
						72,BW0,BW1,WR0,WR1,BD1,42,BW0,BW1,WR0,WR1,BD1,
						72,BW1,BW2,WR0,WR1,BD2,42,BW1,BW2,WR0,WR1,BD2,
						72,BW2,BW3,WR0,WR1,BD3,42,BW2,BW3,WR0,WR1,BD3,
						72,BW3,BW4,WR0,WR1,BD4,42,BW3,BW4,WR0,WR1,BD4,
						72,BW4,BW5,WR0,WR1,BD5,42,BW4,BW5,WR0,WR1,BD5, 
						72,BW5,BW6,WR0,WR1,BD6,42,BW5,BW6,WR0,WR1,BD6,
						72,BW6,BW7,WR0,WR1,BD7,42,BW6,BW7,WR0,WR1,BD7,
						72,BW7,BW8,WR0,WR1,BD8,42,BW7,BW8,WR0,WR1,BD8,
						52,BWa,WR0,WR1,BDa,WR0,WR1,WR0,WR1,WR0,WR1,WR0,50,RE1,
						HLD,94,WA0,24,BW8,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,50,
						STP,
						PADDING_26,

						// Red 2. (121)
						80,50,31,WR0,WR1,WR0,WR1,WR0,WR1,50,
						BW0,WR0,WR1,WR0,WR1,BD0,70,
						50,BW0,BW1,WR0,WR1,WR0,WR1,BD1,71,
						50,BW1,BW2,WR0,WR1,WR0,WR1,BD2,61,
						50,BW2,BW3,WR0,WR1,WR0,WR1,BD3,51,
						50,BW3,BW4,WR0,WR1,WR0,WR1,BD4,41,
						50,BW4,BW5,WR0,WR1,WR0,WR1,BD5,31,
						50,BW5,BW6,WR0,WR1,WR0,WR1,BD6,71,
						50,BW6,BW7,WR0,WR1,WR0,WR1,BD7,20,
						50,BW7,BW8,WR0,WR1,WR0,WR1,BD8,
						66,18,WR0,WR1,WR0,WR1,WR0,WR1,WR0,WR1,WR0,WR1,
						62,BWa,WR0,WR1,BDa,50,
						HLD,83,WA0,4,BW8,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,50,
						STP,
						PADDING_41,

						// Red 3. (15)
						HLD,73,WA0,26,BW8,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,50,STP,
						PADDING_147,

						// Red 4. (15)
						HLD,63,WA0,6,BW8,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,50,STP,
						PADDING_147,

						// Red 5. (15)
						HLD,53,WA0,14,BWa,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,50,STP,
						PADDING_147,

						// Red 6. (15)
						HLD,43,WA0,5,BWb,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,50,STP,
						PADDING_147,

						// Red 7. (26)
						HLD,74,65,FF0,KW0,KW1,KW2,KW3,KW3,KW2,KW1,KW0,KW1,KW2,KW3,
						25,BWb,16,BWa,50,74,SET,15,65,END,STP,
						PADDING_136,

						// Blue flag (5)
						HLD,56,WF0,WF0,STP,
						PADDING_156, 
						0,

						// Red flag (15)
						HLD,74,WF0,WF0,15,WF0,FR0,FR1,FR2,FR3,FR0,FR1,FR2,FR3,STP,
						PADDING_147
};
