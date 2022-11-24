#ifndef _COMPILER_ENVIRONMENT_H_
#define _COMPILER_ENVIRONMENT_H_

	// - Comment ------------------------------------------
	// This block of precompiler directives are intended 
	// to ensure that the compiler environment is 
	// 'correctly' defined. 

	// This program must be compiled using a C++ compiler.
	#ifndef __cplusplus
		#error A C++ compiler is required to compile this project.
	#endif

	// Define the lowest version of CE this project is intended for.
	#define TARGET_VERSION 300

	// Ensure that the compiler is compiling for 
	// CE and then the correct version of CE.
#ifndef _WINDOWS
	#if (!defined(_WIN32_WCE) || (_WIN32_WCE < TARGET_VERSION))
		#error This program was not intended for use on your chosen platform, and will not compile
	#endif
#endif

	// Exclude unrequired Windows functionality.
	#ifndef STRICT
		#define STRICT
	#endif

	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	// Disable some warnings.
	#pragma warning (disable:4100 4201 4238 4244 4514 4800)

#ifndef NOT_FINAL_BUILD
	#pragma warning (error:4101 4189 4245)
#endif

	// Define the project we are building.
	#define	SPEEDBALL	001
	#define	PROJECTX	002
	#define	POPULOUS	003

	#define	PROJECT		POPULOUS
	
	// - Observations -------------------------------------
	// 1. Use CompareString() for case-insensitive string 
	// comparisons and wcsncmp() for case-sensitive ones.
	// 2. Don't use relative paths.

#endif