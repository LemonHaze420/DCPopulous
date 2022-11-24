#ifndef	_LOCALIZATION_H_
#define	_LOCALIZATION_H_

#include	"Object.h"
#include	"SafeProgramming.h"
#include	"Misc.h"
#include	"String.h"
#include	"Platform.h"
#include	"Preferences.h"


// ----
#define	MAX_LOCALIZED_STRINGS	128
class CLocalization : public CObject
{
public:
	// ----
	CLocalization(	CPlatform		*_PlatformRef, 
					CPreferences	*_PreferencesRef);

	~CLocalization();

	// ----
	// Load localization strings - the language will be
	// read from the preferences object (defaulting to 
	// English if this fails).
	bool	LoadLocalizationStrings();

	// ----
	// Free memory
	void	FreeLocalizationStrings();

	// ----
	// Obtain a pointer to a string resource.
	// The return value will be NULL if the requested 
	// resource does not exists or is out of bounds.
	CString*	GetString(int	_Index);

	
protected:
	CPlatform		*PlatformRef;
	CPreferences	*PreferencesRef;
	CString			*StringLocalizationArray[MAX_LOCALIZED_STRINGS];
};
#endif