#include	"Object.h"
#include	<windows.h>
#include	"SoundSystem.h"

#define	HIGH_ACCURACY_TIMER		0
#define	LOWER_ACCURACY_TIMER	1

class CTimer	: public CObject
{
public:
	CTimer(int _milliseconds);
	~CTimer();

	// Note: Calling any of these methods resets the 
	// timer state.

	// ----
	// Allows for the processing of sound whilst we are blocking.
	void	AssignSoundSystem(CSoundSystem	*_SoundSystemRef) {SoundSystemRef = _SoundSystemRef;}

	// ----
	// Returns the number of time slices that have 
	// passed since the last time it was called.
	int		GetNumberOfEvents();

	// ----
	void	ClearNumberOfEvents();

	// ----
	// Sits in a loop until at least one event has passed -
	// will drop out immediately if one, or more, have 
	// already passed since GetNumberOfEvents was last 
	// called.
	void	BlockUntilClock();

protected:
	int				Method;
	int				MilliSeconds;
	LARGE_INTEGER	LastSample; 
	LARGE_INTEGER	SampleFrequency;
	DWORD			LastTickCount;	// Back up, if there is no high resolution timer.

	CSoundSystem	*SoundSystemRef;
};