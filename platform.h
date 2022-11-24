#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include	"Object.h"
#include	"String.h"
#include	"GameDefines.h"


#define	GENERIC_MANUFACTURER_BLOCK	10000
#define	GENERIC_MACHINE_BLOCK		100
// Machine type.
enum EMachine
{
	EM_Generic							= 0,	// Generic, generic.

		// HP.
		EM_HP_Generic					= (1 * GENERIC_MANUFACTURER_BLOCK),	// HP Generic
			// Jornada family
			EM_HP_Jornada_Generic		= (EM_HP_Generic + 1 * GENERIC_MACHINE_BLOCK),	// Generic Jornada
				// Specific members
				EM_HP_Jornada540,	
				EM_HP_Jornada545,

		// Casio
		EM_Casio_Generic				= (2 * GENERIC_MANUFACTURER_BLOCK),	// Casio generic
			// Cassiopeia family	
			EM_Casio_Cassiopeia_Generic	= (EM_Casio_Generic + 1 * GENERIC_MACHINE_BLOCK),	// Cassiopeia generic
				// Specific members
				EM_Casio_Cassiopeia125,

		// Compaq
		EM_Compaq_Generic				= (3 * GENERIC_MANUFACTURER_BLOCK),	// Compaq generic
			// IPaq family
			EM_Compaq_IPaq_Generic		= (EM_Compaq_Generic + 1 * GENERIC_MACHINE_BLOCK),
				// Specific members
				EM_Compaq_IPaq_3100,
				EM_Compaq_IPaq_3600,

	EM_FailedToQuery					= -1
};

// ----
// Enumerated list of known paths that the game can use.
enum EPath
{
	EP_CWD						= 0,		// Current working directory
	EP_Install,								// Install path
	EP_Data,								// General data
	EP_AudioData,							// Audio data
	EP_ImageData,							// Image data
	EP_Fonts,								// Font data
	EP_Text,								// Text files
	EP_Saves,								// Save game path

	// Debug / development
	EP_Snapshot
};
#define MAX_PATHS ((int)EP_Snapshot + 1)


// ----
// Stores game data. 
class CPlatform : public CObject
{
public:
	CPlatform();
	~CPlatform();

	// ----
	bool		Init();

	// ----
	EMachine	GetMachineType();
	EMachine	GetMachineFamily();
	EMachine	GetMachineManufacturer();

#ifdef USE_DEPRICATED
	// ----
	ELanguage	GetLangauge();
#endif

	// ----
	int			GetDeviceLanguage();

	// ----
	// Get various paths
	CString*	GetPath(EPath _Locale);

	// Returns the time, in seconds, that the device will 
	// remain on without user activity.
	int			GetIdleTimeOut();

protected:
	EMachine	MachineType;
#ifdef USE_DEPRICATED
	ELanguage	Language;
#endif
	CString		PathArray[MAX_PATHS];
};

#endif