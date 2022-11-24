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

int	CPopulous::test_channel(SHORT Num)
{
	//return 0; 
	int ret = (int)TheSoundSystem->CheckChannel(Num);
	return(ret);
}


// ----
void	CPopulous::play_sound(USHORT number, USHORT channel)
{
	if(effect_off)
	{
		TheSoundSystem->PlaySample(	SampleArray[number].SampleRef,
									channel,
									SampleArray[number].Frequency,
									1, 
									0.0f);
	}
}


void	CPopulous::mod_play_sound(USHORT number, USHORT channel)
{
	if (	music_off 
		&&	number>=0 
		&&	number<=113 
		&&	channel<=4)
	{
		TheSoundSystem->PlaySample(	SampleArray[number].SampleRef,
									channel,
									SampleArray[number].Frequency,
									1, 
									0.0f);
	}
}