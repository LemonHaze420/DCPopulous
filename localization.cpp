#include	"Localization.h"
#include	"Object.h"
#include	"SafeProgramming.h"

#include	"Misc.h"
#include	"String.h"
#include	"Platform.h"
#include	"Preferences.h"

	// ----
CLocalization::CLocalization(	CPlatform		*_PlatformRef, 
								CPreferences	*_PreferencesRef)
{
	PlatformRef		= _PlatformRef;
	PreferencesRef	= _PreferencesRef;

	// Clear all pointers to NULL.
	for (int i = MAX_LOCALIZED_STRINGS; i--;)
	{
		StringLocalizationArray[i] = NULL;
	}
}

CLocalization::~CLocalization()
{
	FreeLocalizationStrings();
}

// ----
// Load localization strings - the language will be
// read from the preferences object (defaulting to 
// English if this fails).
bool	CLocalization::LoadLocalizationStrings()
{
	FILE		*FH;
	CString		Path;
	char		Buffer[256], Token[256];
	char		*Index;
	char		*EndToken;
	int			SupportedLangArray[32];
	int			i, NumOfTokens;
	int			LanguageToLoad;
	
	// Free any existing strings.
	FreeLocalizationStrings();

	// Generate file path
	if (!PlatformRef)
		return false;
	Path = PlatformRef->GetPath(EP_Text);
	Path += "lanarch.ctd";	// LANgauge ARCHive . Converted Text Data.

	FH = FOPEN(Path.GetString(), "rb");
	if (FH)
	{
		// Now, see what langauges the archive supports.
		fread(Buffer, 256, 1, FH);
		ChangeEncryptionState(Buffer, 256);
		Index = strchr(Buffer, '\n'); // Try to find the end of the info.

		if (Index)
		{
			*(Index - 1) = 0; 
			i = Index - Buffer;	Index++;	// Calculate the length of the header
			fseek(FH, i + 1, SEEK_SET);			// Seek to the end of the header + new line char

			NumOfTokens = 0;
			Index = Buffer;
			while (i > 0)
			{
				EndToken = strchr(Index, ',');		// Comma seperated list.
				if (!EndToken)
					EndToken = strchr(Index, '\0');// End of line.

				strncpy(Token, Index, (EndToken - Index));
				*(Token + (EndToken - Index))	= 0;
				SupportedLangArray[NumOfTokens] = atoi(Token);

				ODS("Langauge ");
				ODI(SupportedLangArray[NumOfTokens]);
				ODSN(" supported");

				// Prepare for next loop
				i -= ((EndToken - Index) + 1);
				NumOfTokens++;
				Index = EndToken + 1;
			}

			ODI(NumOfTokens);
			ODSN(" languages supported");
		}
		else
			return false; // The language list heading is too long.

		// If Preferences object exists.
		//	1. and the request language exists -> Load it.
		//	2. The langauge does exist -> Does English exist?
		//	 a. Yes -> Load it.
		//	 b. No -> Load first langauge we come across.
		// No Preferences object.
		//	1. Is English available.
		//	 a. Yes -> Load it.
		//	 b. No -> Load first langauge.

		LanguageToLoad = 0;	// First one.
		if (PreferencesRef)
		{
			for (i = NumOfTokens; i--;)
			{
				if (SupportedLangArray[i] == (int)PreferencesRef->Language)
				{
					LanguageToLoad = i;
					break;
				}
			}
		}
		else
		{
			// Hunt for English
			for (i = NumOfTokens; i--;)
			{
				if (SupportedLangArray[i] == (int)EL_English)
				{
					LanguageToLoad = i;
					break;
				}
			}
		}

		// Got our index - now load the data.
		// LanguageToLoad <- index of language we wish to load.

		// Need to locate the correct place in the file.
		while (LanguageToLoad)
		{
			*Buffer = 0;
			while (*Buffer != '~')
			{
				if (fread(Buffer, 1, 1, FH) != 1)
					return false;	// Should never run out of file.

				// Unencrypt the char.
				ChangeEncryptionState(Buffer, 1);
			}

			fread(Buffer, 2, 1, FH);	// Read new-line chars
			LanguageToLoad--;
		}

		int		StringIndex = 0;
		bool	Continue = true;
		bool	NotEndOfToken;
		int		TokenLength;
		int		StringLength;
		while (Continue)
		{
			// Hunt for (next) token
			NotEndOfToken	= true;
			TokenLength		= 0;
			while ((Continue) && (NotEndOfToken))
			{
				Continue &= (fread((Buffer + TokenLength), 1, 1, FH) == 1);
				if (Continue)
				{
					ChangeEncryptionState((Buffer + TokenLength), 1);	// Unencrypt the particular character

					if (*(Buffer + TokenLength) == '~')
						Continue = false;	// Found the end of this language section.

					// See if it is the end of line delimiter.
					if (*(Buffer + TokenLength) == '\n')
						NotEndOfToken = false;
					else
						TokenLength++;
				}
			}
			*(Buffer + TokenLength) = 0;

			// Safeguard last loop.
			if (Continue)
			{
				// Should now have valid tokens.
				Index = strchr(Buffer, ',');	// Find the comma between the two tokens.
				*Index = 0;	Index++;
				StringIndex = atoi(Buffer);
				StringLength = atoi(Index);

				SAFELY_DELETE_C2248(StringLocalizationArray[StringIndex]);
				NEW_C2248(StringLocalizationArray[StringIndex], CString());
				StringLocalizationArray[StringIndex]->LoadEncodedString(FH, StringLength);
			}

			fread(Buffer, 2, 1, FH); // Read the line feed.
		}

		fclose(FH);
		return true;
	}
	return true;	// Can't load archive - not flagging this as an error as user may not want to use localization. (If they do, then specify that the file is checked somewhere).
}

// ----
// Free memory
void	CLocalization::FreeLocalizationStrings()
{
	for (int i = MAX_LOCALIZED_STRINGS; i--;)
	{
		SAFELY_DELETE(StringLocalizationArray[i]);
	}
}

// ----
// Obtain a pointer to a string resource.
// The return value will be NULL if the requested 
// resource does not exists or is out of bounds.
CString*	CLocalization::GetString(int	_Index)
{
	if (	(_Index >= 0) 
		&&	(_Index < MAX_LOCALIZED_STRINGS))
	{
		return StringLocalizationArray[_Index];
	}

	return NULL;
}


