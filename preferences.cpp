#include	"Preferences.h"
#include	"SafeProgramming.h"
#include	"Object.h"
#include	"RegistryFunctions.h"
#include	"String.h"
#include	"Misc.h"

// ----
CPreferences::CPreferences()
{
	// These values are assigned to show that the class has
	// not been 'init'd yet.
//	SlowVRAMWorkAround		= false;
//	DoubleBufferingEnabled	= false;

	Volume					= DEFAULT_VOLUME_LEVEL;
	MusicEnabled			= DEFAULT_MUSIC_STATE;
	EffectsEnabled			= DEFAULT_EFFECTS_STATE;
	PreferredOrientation	= EO_Landscape_Normal;
	HardwareDoubleBuffering	= false;
//	MouseRepeat				= DEFAULT_MOUSE_REPEAT;
}

// ----
CPreferences::~CPreferences()
{
	// Nothing
}

// ----
bool	CPreferences::LoadPreferences(CPlatform*	_PlatformRef)
{
	DWORD	DataType	= 0;
	char*	Value		= NULL;
	DWORD	DataLength	= 0;
	LONG	Ret;
	LONG	OverallRet	= true;
	CString Temp;

	AssumeDefaults(_PlatformRef);

#if BOXEDSALE
	Registered = true;	//  Always registered
#else
	// Determine if we are registered
	CString TempName;
		
	// Load double buffering settings.
	Ret = LoadFromRegistry(	HKEY_LOCAL_MACHINE,				// Existing key or default root.
							&CString(REG_PREFERENCE_PATH),	// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_ACTIVATION_CODE),	// Name of value to retrieve.
							&DataType,						// A pointer to a DWORD filled-in by the function.
							&Value,							// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(Ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		RegistrationCode = DecryptCode(*(unsigned long*)Value); 
	}
	else
	{
		RegistrationCode = 0;
	}
	SAFELY_DELETE_ARRAY(Value);

	// See if the activation code is valid
	if (GetRegistrationString(&TempName))
	{
		Registered = ValidateRegistrationDetails(&TempName, RegistrationCode, PRODUCT_ACTIVATION_RPN_STRING); 
	}
	else
	{
		Registered = false;
	}
#endif

	// ----
	// Load Volume from registry if we can.
	Ret = LoadFromRegistry(	HKEY_CURRENT_USER,				// Existing key or default root.
							&CString(REG_PREFERENCE_PATH),	// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_VOLUME_SETTING),	// Name of value to retrieve.
							&DataType,						// A pointer to a DWORD filled-in by the function.
							&Value,							// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);
	OverallRet	&= Ret;
	if (	(SUCCEEDED(Ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		Volume = (int)*Value; 
		if (	(Volume < 0)
			||	(Volume > 10))
			Volume	= DEFAULT_VOLUME_LEVEL;
	}
	SAFELY_DELETE_ARRAY(Value);


	// Load Music setting from registry if we can.
	Ret = LoadFromRegistry(	HKEY_CURRENT_USER,				// Existing key or default root.
							&CString(REG_PREFERENCE_PATH),	// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_MUSIC_SETTING),	// Name of value to retrieve.
							&DataType,						// A pointer to a DWORD filled-in by the function.
							&Value,							// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);
	OverallRet	&= Ret;
	if (	(SUCCEEDED(Ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		MusicEnabled = (bool)*Value;
	}
	SAFELY_DELETE_ARRAY(Value);

	// Load Effects setting from registry if we can.
	Ret = LoadFromRegistry(	HKEY_CURRENT_USER,				// Existing key or default root.
							&CString(REG_PREFERENCE_PATH),	// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_EFFECTS_SETTING),	// Name of value to retrieve.
							&DataType,						// A pointer to a DWORD filled-in by the function.
							&Value,							// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);
	OverallRet	&= Ret;
	if (	(SUCCEEDED(Ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		EffectsEnabled = (bool)*Value;
	}
	SAFELY_DELETE_ARRAY(Value);

#if 0
	// Load Mouse Repeat setting from registry if we can.
	Ret = LoadFromRegistry(	HKEY_CURRENT_USER,				// Existing key or default root.
							&CString(REG_PREFERENCE_PATH),	// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_MOUSE_REPEAT),		// Name of value to retrieve.
							&DataType,						// A pointer to a DWORD filled-in by the function.
							&Value,							// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);
	OverallRet	&= Ret;
	if (	(SUCCEEDED(Ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		if (Value > 0)
		{
			MouseRepeat = *Value;
		}
	}
	SAFELY_DELETE_ARRAY(Value);
#endif
#if 0	
	// Load double buffering settings.
	Ret = LoadFromRegistry(	HKEY_CURRENT_USER,				// Existing key or default root.
							&CString(REG_PREFERENCE_PATH),	// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_ENABLE_DB_SETTING),// Name of value to retrieve.
							&DataType,						// A pointer to a DWORD filled-in by the function.
							&Value,							// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);
	OverallRet	&= Ret;
	if (	(SUCCEEDED(Ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		DoubleBufferingEnabled = (bool)*Value; 
	}
	SAFELY_DELETE_ARRAY(Value);


	// Load double buffering settings.
	Ret = LoadFromRegistry(	HKEY_CURRENT_USER,				// Existing key or default root.
							&CString(REG_PREFERENCE_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_SLOW_VRAM_SETTING),// Name of value to retrieve.
							&DataType,						// A pointer to a DWORD filled-in by the function.
							&Value,							// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);
	OverallRet	&= Ret;
	if (	(SUCCEEDED(Ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		SlowVRAMWorkAround = (bool)*Value; 
	}
	SAFELY_DELETE_ARRAY(Value);
	

	// Load and validate stylus control setting
	Ret = LoadFromRegistry(	HKEY_CURRENT_USER,				// Existing key or default root.
							&CString(REG_PREFERENCE_PATH),	// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_STYLUS_SETTING),	// Name of value to retrieve.
							&DataType,						// A pointer to a DWORD filled-in by the function.
							&Value,							// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);
	OverallRet	&= Ret;
	if (	(SUCCEEDED(Ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		StylusControl = (int)*Value; 
		if (	(StylusControl < 0)
			/*||	(StylusControl > 1)*/)	// This will be refined later
			StylusControl = 0;
	}
	SAFELY_DELETE_ARRAY(Value);
#endif

	// Load and validate orientation setting
	Ret = LoadFromRegistry(	HKEY_CURRENT_USER,					// Existing key or default root.
							&CString(REG_PREFERENCE_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_ORIENTATION_SETTING),	// Name of value to retrieve.
							&DataType,							// A pointer to a DWORD filled-in by the function.
							&Value,								// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);
	OverallRet	&= Ret;
	if (	(SUCCEEDED(Ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		//if ((int)*Value == EO_Portrait_Normal)
		//	PreferredOrientation = EO_Portrait_Normal;
		if ((int)*Value == EO_Landscape_Normal)
			PreferredOrientation = EO_Landscape_Normal;
		if ((int)*Value == EO_Landscape_Flipped)
			PreferredOrientation = EO_Landscape_Flipped;
	}
	SAFELY_DELETE_ARRAY(Value);


	// Load and validate hardware double buffering work around setting
	Ret = LoadFromRegistry(	HKEY_CURRENT_USER,					// Existing key or default root.
							&CString(REG_PREFERENCE_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_HARDWARE_DB_SETTING),	// Name of value to retrieve.
							&DataType,							// A pointer to a DWORD filled-in by the function.
							&Value,								// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);
	OverallRet	&= Ret;
	if (	(SUCCEEDED(Ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		HardwareDoubleBuffering = (bool)*Value;
	}
	SAFELY_DELETE_ARRAY(Value);

	return true; // This implementation can not fail.
}

// ----
bool	CPreferences::SavePreferences()
{
	LONG	OverallRet = true;

	// Save out volume setting
	OverallRet &= SaveToRegistry(	HKEY_CURRENT_USER,					// Existing key or default root.
									&CString(REG_PREFERENCE_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
									&CString(REG_VOLUME_SETTING),		// Name of the value to store.
									REG_BINARY,							// Specifies the format for storing the data.
									(char*)&Volume,						// A pointer to the data to store.
									sizeof(Volume));

	// Save out music setting
	OverallRet &= SaveToRegistry(	HKEY_CURRENT_USER,					// Existing key or default root.
									&CString(REG_PREFERENCE_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
									&CString(REG_MUSIC_SETTING),		// Name of the value to store.
									REG_BINARY,							// Specifies the format for storing the data.
									(char*)&MusicEnabled,				// A pointer to the data to store.
									sizeof(MusicEnabled));

	// Save out effects setting
	OverallRet &= SaveToRegistry(	HKEY_CURRENT_USER,					// Existing key or default root.
									&CString(REG_PREFERENCE_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
									&CString(REG_EFFECTS_SETTING),		// Name of the value to store.
									REG_BINARY,							// Specifies the format for storing the data.
									(char*)&EffectsEnabled,				// A pointer to the data to store.
									sizeof(EffectsEnabled));

#if 0
	// Save double buffer setting
	OverallRet &= SaveToRegistry(	HKEY_CURRENT_USER,					// Existing key or default root.
									&CString(REG_PREFERENCE_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
									&CString(REG_ENABLE_DB_SETTING),	// Name of the value to store.
									REG_BINARY,							// Specifies the format for storing the data.
									(char*)&DoubleBufferingEnabled,		// A pointer to the data to store.
									sizeof(DoubleBufferingEnabled));

	// Save slow vram setting - only used if double buffering is enabled.
	OverallRet &= SaveToRegistry(	HKEY_CURRENT_USER,					// Existing key or default root.
									&CString(REG_PREFERENCE_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
									&CString(REG_SLOW_VRAM_SETTING),	// Name of the value to store.
									REG_BINARY,							// Specifies the format for storing the data.
									(char*)&SlowVRAMWorkAround,			// A pointer to the data to store.
									sizeof(SlowVRAMWorkAround));

	// Save stylus settings.
	OverallRet &= SaveToRegistry(	HKEY_CURRENT_USER,					// Existing key or default root.
									&CString(REG_PREFERENCE_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
									&CString(REG_STYLUS_SETTING),		// Name of the value to store.
									REG_BINARY,							// Specifies the format for storing the data.
									(char*)&StylusControl,				// A pointer to the data to store.
									sizeof(StylusControl));
#endif
	// Orientation.
	OverallRet &= SaveToRegistry(	HKEY_CURRENT_USER,					// Existing key or default root.
									&CString(REG_PREFERENCE_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
									&CString(REG_ORIENTATION_SETTING),	// Name of the value to store.
									REG_BINARY,							// Specifies the format for storing the data.
									(char*)&PreferredOrientation,		// A pointer to the data to store.
									sizeof(PreferredOrientation));
#if 0
	// Mouse Repeat.
	OverallRet &= SaveToRegistry(	HKEY_CURRENT_USER,					// Existing key or default root.
									&CString(REG_PREFERENCE_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
									&CString(REG_MOUSE_REPEAT),			// Name of the value to store.
									REG_BINARY,							// Specifies the format for storing the data.
									(char*)&MouseRepeat,				// A pointer to the data to store.
									sizeof(MouseRepeat));
#endif

	return OverallRet;
}

// ----
bool	CPreferences::ValidatePreferences()
{
	if ((Volume < 0) || (Volume > 10))
		return false;

	// The following items are boolean
	//	MusicEnabled
	//	DoubleBufferingEnabled
	//	SlowVRAMWorkAround

	//if (StylusControl	< 0)
	//	return false;
	
	// The following can take any value.
	//	ShowDemoTimeOut
	return true;
}

// ----
void	CPreferences::AssumeDefaults(CPlatform*	_PlatformRef)
{
	PlatformRef = _PlatformRef;

	// Assume some sensible defaults
	Volume				= DEFAULT_VOLUME_LEVEL;
	MusicEnabled		= DEFAULT_MUSIC_STATE;
	EffectsEnabled		= DEFAULT_EFFECTS_STATE;
	PreferredOrientation= EO_Landscape_Normal;
	HardwareDoubleBuffering = false;
//	MouseRepeat			= DEFAULT_MOUSE_REPEAT;

	if (PlatformRef)
	{
		switch (PlatformRef->GetMachineType())
		{
		case EM_Casio_Cassiopeia125:
			HardwareDoubleBuffering = true;
		}
	}

	RegistrationCode = 0;
	Registered = true;	// Play safe!

#if 0	// These features are not supported.
	if (PlatformRef)
	{
		switch (PlatformRef->GetMachineFamily())
		{
		case EM_Casio_Cassiopeia_Generic:
			DoubleBufferingEnabled	= false;
			break;

		case EM_Compaq_IPaq_Generic:
			DoubleBufferingEnabled	= true;
			SlowVRAMWorkAround		= false;
			break;

		default:
			DoubleBufferingEnabled	= true;
			SlowVRAMWorkAround		= true;
			break;
		}
	}
	else
	{
		DoubleBufferingEnabled		= false;
		SlowVRAMWorkAround			= false;
	}

	// Hmmm, work around
	DoubleBufferingEnabled	= false;
	SlowVRAMWorkAround		= false;

	// Other, more game specific preferences.
	StylusControl		= DEFAULT_STYLUS_CONTROL;
	ShowDemoTimeOut		= DEFAULT_DEMO_TIME_OUT;
#endif
}


// ----
// Return registration code
unsigned long	CPreferences::GetRegistrationCode()
{
	return RegistrationCode;
}

// ----
void			CPreferences::SetRegistrationCode(unsigned long code)
{
	RegistrationCode = code;
}