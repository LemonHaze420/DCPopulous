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

#include	"Extern.h"

// ----
void		CPopulous::DebugCheckPeeps()
{
	for (int iii = 0 ; iii < no_peeps; iii++)
	{
		if ((peeps[iii].at_pos < 0) || (peeps[iii].at_pos > 4095))
		{
			ODSN("Error: Peep at_pos invalid!!!");
		//	RaiseException (0xEFFFFFFF, 0, 0, NULL);
		}

		if ((peeps[iii].battle_population < 0) || (peeps[iii].battle_population > 212))
		{
		//	ODSN("Error: Peep battle_population invalid!!!");
		//	RaiseException (0xEFFFFFFF, 0, 0, NULL);
		}

		if ((peeps[iii].frame < 0) || (peeps[iii].frame > 255))
		{
			ODSN("Error: Peep frame (possibly) invalid.");
		}
		/*
		UBYTE       flags;
		UBYTE       player;
		UBYTE       iq;
		UBYTE       weapons;
		SHORT       population;
		USHORT      battle_population;
		SHORT       at_pos;
		SHORT       direction;
		SHORT       frame;
		struct p_peeps  *head_for;
		SHORT       in_out;
		BYTE        status;
		BYTE        magnet_last_move;
		*/
	}
}