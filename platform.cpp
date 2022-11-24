#include	"Platform.h"
#include	"SafeProgramming.h"
#include	"Object.h"
#include	"GameDefines.h"
#include	"String.h"
#include	"RegistryFunctions.h"

// ----
CPlatform::CPlatform()
{
}


CPlatform::~CPlatform()
{
}

	// ----
bool	CPlatform::Init()
{
	// Lets work out the directories

	//DWORD	DataType	= 0;
	//char*	Value		= NULL;
	//DWORD	DataLength	= 0;
	//LONG	ret;
	CString Temp;

	// ----
	PathArray[(int)EP_CWD].Clear();	// Special case. Done on the fly.

	TCHAR szBuff[MAX_PATH];
	if (GetModuleFileName(NULL, szBuff, MAX_PATH))
	{
#ifdef UNICODE
		TCHAR* end = wcsrchr(szBuff, '\\');
#else
		TCHAR* end = strrchr(szBuff, '\\');
#endif
		if (end > 0) 
			*(end + 1) = 0;

		PathArray[(int)EP_CWD] = szBuff;
	}

	PathArray[(int)EP_Install]		= PathArray[(int)EP_CWD];

	PathArray[(int)EP_Data]			= PathArray[(int)EP_CWD];
	PathArray[(int)EP_Data]			+= "Data\\";

	PathArray[(int)EP_AudioData]	= PathArray[(int)EP_Data];
	PathArray[(int)EP_AudioData]	+= "Audio\\";

	PathArray[(int)EP_ImageData]	= PathArray[(int)EP_Data];
	PathArray[(int)EP_ImageData]	+= "Images\\";

	PathArray[(int)EP_Fonts]		= PathArray[(int)EP_Data];
	PathArray[(int)EP_Fonts]		+= "Images\\";

	PathArray[(int)EP_Text]			= PathArray[(int)EP_Data];
	PathArray[(int)EP_Text]			+= "Text\\";

	PathArray[(int)EP_Saves]		= PathArray[(int)EP_Install];
//	PathArray[(int)EP_Saves]		+= "Saves\\";

	PathArray[(int)EP_Snapshot]		= PathArray[(int)EP_Install];
		
	/*
	// ----
	// Install path
	ret = LoadFromRegistry(	HKEY_CURRENT_USER,			// Existing key or default root.
							&CString(REGISTRY_PATH),	// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_INSTALL_PATH),	// Name of value to retrieve.
							&DataType,					// A pointer to a DWORD filled-in by the function.
							&Value,						// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		PathArray[(int)EP_Install] = (TCHAR*)Value; // Cast it to a wide string.
	}
	SAFELY_DELETE_ARRAY(Value);


	// ----
	// Data path
	ret = LoadFromRegistry(	HKEY_CURRENT_USER,			// Existing key or default root.
							&CString(REGISTRY_PATH),	// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_DATA_PATH),	// Name of value to retrieve.
							&DataType,					// A pointer to a DWORD filled-in by the function.
							&Value,						// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		Temp					=	(TCHAR*)Value;
		PathArray[(int)EP_Data]	=	PathArray[(int)EP_Install];
		PathArray[(int)EP_Data]	+=	Temp;
	}
	SAFELY_DELETE_ARRAY(Value);


	// ----
	// Audio data path
	ret = LoadFromRegistry(	HKEY_CURRENT_USER,				// Existing key or default root.
							&CString(REGISTRY_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_AUDIO_DATA_PATH),	// Name of value to retrieve.
							&DataType,						// A pointer to a DWORD filled-in by the function.
							&Value,							// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		Temp							=	(TCHAR*)Value;
		PathArray[(int)EP_AudioData]	=	PathArray[(int)EP_Data];
		PathArray[(int)EP_AudioData]	+=	Temp;
	}
	SAFELY_DELETE_ARRAY(Value);


	// ----
	// Image data path
	ret = LoadFromRegistry(	HKEY_CURRENT_USER,				// Existing key or default root.
							&CString(REGISTRY_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_IMAGE_DATA_PATH),	// Name of value to retrieve.
							&DataType,						// A pointer to a DWORD filled-in by the function.
							&Value,							// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		Temp							=	(TCHAR*)Value;
		PathArray[(int)EP_ImageData]	=	PathArray[(int)EP_Data];
		PathArray[(int)EP_ImageData]	+=	Temp;
	}
	SAFELY_DELETE_ARRAY(Value);


	// ----
	// Font data path
	ret = LoadFromRegistry(	HKEY_CURRENT_USER,			// Existing key or default root.
							&CString(REGISTRY_PATH),	// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_FONTS_PATH),	// Name of value to retrieve.
							&DataType,					// A pointer to a DWORD filled-in by the function.
							&Value,						// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		Temp						=	(TCHAR*)Value;
		PathArray[(int)EP_Fonts]	=	PathArray[(int)EP_Data];
		PathArray[(int)EP_Fonts]	+=	Temp;
	}
	SAFELY_DELETE_ARRAY(Value);


	// ----
	// Save data path
	ret = LoadFromRegistry(	HKEY_CURRENT_USER,			// Existing key or default root.
							&CString(REGISTRY_PATH),	// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_SAVE_PATH),	// Name of value to retrieve.
							&DataType,					// A pointer to a DWORD filled-in by the function.
							&Value,						// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		Temp						=	(TCHAR*)Value;
		PathArray[(int)EP_Saves]	=	PathArray[(int)EP_Install];
		PathArray[(int)EP_Saves]	+=	Temp;
	}
	SAFELY_DELETE_ARRAY(Value);

#ifdef NOT_FINAL_BUILD
	// ----
	// Save out path
	ret = LoadFromRegistry(	HKEY_CURRENT_USER,			// Existing key or default root.
							&CString(REGISTRY_PATH),	// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_SNAPSHOT_PATH),// Name of value to retrieve.
							&DataType,					// A pointer to a DWORD filled-in by the function.
							&Value,						// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		Temp						=	(TCHAR*)Value;
		PathArray[(int)EP_Snapshot]	=	PathArray[(int)EP_Install];
		PathArray[(int)EP_Snapshot]	+=	Temp;
	}
	SAFELY_DELETE_ARRAY(Value);
#endif
	*/
	
#ifdef _UNICODE
	TCHAR wszMachineName[128];
	SystemParametersInfo(SPI_GETOEMINFO, sizeof(wszMachineName), &wszMachineName, 0);
	if (wcsicmp(wszMachineName, TEXT("HP,Jornada 540")) == 0) 
	{	
		MachineType = EM_HP_Jornada540;		
	}
	else if (wcsicmp(wszMachineName, TEXT("HP,Jornada 540,,9357,USEnglish,SH3")) == 0) 
	{
		MachineType = EM_HP_Jornada545;
	}
	else if (wcsicmp(wszMachineName, TEXT("Pocket PC J670")) == 0) 
	{
		MachineType = EM_Casio_Cassiopeia125;
	}
	else if (wcsicmp(wszMachineName, TEXT("Compaq iPAQ H3600")) == 0) 
	{
		MachineType = EM_Compaq_IPaq_3600;
	}
	else
		MachineType = EM_Generic;

	//EM_Casio_Generic

#else
	MachineType = EM_FailedToQuery;
#endif
	return true;
}


// ----
EMachine	CPlatform::GetMachineType()
{
	return MachineType;
}


// ----
EMachine	CPlatform::GetMachineFamily()
{
	return (EMachine)(MachineType - (MachineType % GENERIC_MACHINE_BLOCK));
}

// ----
EMachine	CPlatform::GetMachineManufacturer()
{
	return (EMachine)(MachineType - (MachineType % GENERIC_MANUFACTURER_BLOCK));
}

#ifdef USE_DEPRICATED
// ----
ELanguage	CPlatform::GetLangauge()
{
	return Language;
}
#endif

// ----
int			CPlatform::GetDeviceLanguage()
{
	return GetUserDefaultLangID();
	/*
	switch (GetUserDefaultLangID())
	{
	case 0x0407:	// German (Standard) 
	case 0x0807:	// German (Swiss) 
	case 0x0c07:	// German (Austrian) 
	case 0x1007:	// German (Luxembourg) 
	case 0x1407:	// German (Liechtenstein) 
		break;
	
	case 0x0409:	// English (United States) 
	case 0x0809:	// English (United Kingdom) 
	case 0x0c09:	// English (Australian) 
	case 0x1009:	// English (Canadian) 
	case 0x1409:	// English (New Zealand) 
	case 0x1809:	// English (Ireland) 
	case 0x1c09:	// English (South Africa) 
	case 0x2009:	// English (Jamaica) 
	case 0x2409:	// English (Caribbean) 
	case 0x2809:	// English (Belize) 
	case 0x2c09:	// English (Trinidad) 
		break;
	
	case 0x040a:	// Spanish (Traditional Sort) 
	case 0x080a:	// Spanish (Mexican) 
	case 0x0c0a:	// Spanish (Modern Sort) 
	case 0x100a:	// Spanish (Guatemala) 
	case 0x140a:	// Spanish (Costa Rica) 
	case 0x180a:	// Spanish (Panama) 
	case 0x1c0a:	// Spanish (Dominican Republic) 
	case 0x200a:	// Spanish (Venezuela) 
	case 0x240a:	// Spanish (Colombia) 
	case 0x280a:	// Spanish (Peru) 
	case 0x2c0a:	// Spanish (Argentina) 
	case 0x300a:	// Spanish (Ecuador) 
	case 0x340a:	// Spanish (Chile) 
	case 0x380a:	// Spanish (Uruguay) 
	case 0x3c0a:	// Spanish (Paraguay) 
	case 0x400a:	// Spanish (Bolivia) 
	case 0x440a:	// Spanish (El Salvador) 
	case 0x480a:	// Spanish (Honduras) 
	case 0x4c0a:	// Spanish (Nicaragua) 
	case 0x500a:	// Spanish (Puerto Rico) 
		break;
	
	case 0x040c:	// French (Standard) 
	case 0x080c:	// French (Belgian) 
	case 0x0c0c:	// French (Canadian) 
	case 0x100c:	// French (Swiss) 
	case 0x140c:	// French (Luxembourg) 
		break;
	
	case 0x0410:	// Italian (Standard) 
	case 0x0810:	// Italian (Swiss) 
		break;
		
	default:
		break;
	}
	*/
}


// ----
// Get various paths
CString*	CPlatform::GetPath(EPath _Locale)
{
	//if (_Locale == EP_CWD)
	//{
	//	PathArray[((int)_Locale)] = ;
	//}

	return &PathArray[((int)_Locale)];
}
