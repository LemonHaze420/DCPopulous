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

// ----
// Returns true if all the files required by the class
// can be located. Otherwise false.
bool		CPopulous::ValidateFiles()
{
	bool	ret = true;

	// The content of "Data"
	TempString = ThePlatform->GetPath(EP_Data); TempString += "land0";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_Data); TempString += "land1";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_Data); TempString += "land2";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_Data); TempString += "land3";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_Data); TempString += "level";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_Data); TempString += "intro";
	ret &= TestReadFile(&TempString);

	// Audio data.
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "being.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "complongdr.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "compthunde.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "compvolcan.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "compwindy2.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "crackfire.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "deity.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "deva.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "eternal.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "god.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "greater.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "heart4.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "immortal.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "knighting.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "laugh.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "measln.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "measures.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "mortal.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "music.umd";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "rumble.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "seqlen.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "sequn.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "swamped.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "sword.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "voicesaah.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_AudioData); TempString += "well_done.dat";
	ret &= TestReadFile(&TempString);

	// The content of "Images"
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "bigfont";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "demo";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "exit.cbd";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "font";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "heads.cbd";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "helpicons.cbd";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "helpsidebar.cbd";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "icon";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "iconmask";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "keyboard.cbd";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "keys.cbd";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "load.cbd";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "lord";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "menucorner.cbd";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "mouths";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "options.cbd";
	ret &= TestReadFile(&TempString);
#ifndef BOXEDSALE
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "registration.cbd";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "regfont.cbd";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "regfont.dat";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "regkeyboard.cbd";
	ret &= TestReadFile(&TempString);
#endif
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "qaz";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "sprites0";
	ret &= TestReadFile(&TempString);
	TempString = ThePlatform->GetPath(EP_ImageData); TempString += "spr_320";
	ret &= TestReadFile(&TempString);

	// Return overall value.
	return ret;
}

