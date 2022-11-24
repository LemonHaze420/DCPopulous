#include	"Timer.h"
#include	"Misc.h"

// ----
CTimer::CTimer(int _milliseconds)
{
	SoundSystemRef	= NULL;
	Method			= HIGH_ACCURACY_TIMER;
	MilliSeconds	= _milliseconds;

	if (	(!QueryPerformanceFrequency(&SampleFrequency))
		||	(!QueryPerformanceCounter(&LastSample)))
	{
		// Time to resort to the back up plan...
		Method = LOWER_ACCURACY_TIMER;
		LastTickCount = GetTickCount();
	}
}

// ----
CTimer::~CTimer()
{
}

// ----
// Returns the number of time slices that have 
// passed since the last time it was called.
int		CTimer::GetNumberOfEvents()
{
#ifdef TEST_IN_EMULATION
	return 1;
#endif

	if (MilliSeconds <= 0)
		return 1;

	LARGE_INTEGER	NowSample;
	DWORD			NowTickCount;
	int				Ret;

	if (Method == HIGH_ACCURACY_TIMER)
	{
		QueryPerformanceCounter(&NowSample);
		float	temp = (float)(NowSample.QuadPart - LastSample.QuadPart) / (float)SampleFrequency.QuadPart;
		temp	*= 1000.0f;
		Ret		= temp / (float)MilliSeconds;
//		LastSample = NowSample;
	}
	else
	{
		NowTickCount = GetTickCount();	
		if (NowTickCount > LastTickCount)
		{
			Ret = (NowTickCount - LastTickCount) / MilliSeconds;
		}
		else
		{
			// Clock ticked over.
			Ret = (NowTickCount / MilliSeconds) + ((4294967295 - LastTickCount) / MilliSeconds);
		}
//		LastTickCount = GetTickCount();	
	}

	return Ret;
}

// ----
void	CTimer::ClearNumberOfEvents()
{
	if (Method == HIGH_ACCURACY_TIMER)
	{
		QueryPerformanceCounter(&LastSample);
	}
	else
	{
		LastTickCount = GetTickCount();	
	}
}

// ----
// Sits in a loop until at least one event has passed -
// will drop out immediately if one, or more, have 
// already passed since GetNumberOfEvents was last 
// called.
void	CTimer::BlockUntilClock()
{
#if (defined TEST_IN_EMULATION) || (defined DONT_LIMIT_FRAME_RATE)
	return;
#endif

	if (MilliSeconds <= 0)
		return;

	LARGE_INTEGER	NowSample;
	DWORD			NowTickCount;
	bool			Stay = true;

	if (Method == HIGH_ACCURACY_TIMER)
	{
		while (Stay)
		{
			if (SoundSystemRef)
				SoundSystemRef->Process();

			QueryPerformanceCounter(&NowSample);
			float	temp = (float)(NowSample.QuadPart - LastSample.QuadPart) / (float)SampleFrequency.QuadPart;
			temp	*= 1000.0f;
			Stay	= ((temp / (float)MilliSeconds) < 1.0f);
		}
	}
	else
	{
		while (Stay)
		{
			if (SoundSystemRef)
				SoundSystemRef->Process();

			NowTickCount = GetTickCount();	
			if (NowTickCount > LastTickCount)
			{
				Stay = (((NowTickCount - LastTickCount) / MilliSeconds) >= 1);
			}
			else
			{
				// Clock ticked over.
				Stay = (((NowTickCount / MilliSeconds) + ((4294967295 - LastTickCount) / MilliSeconds)) < 1);
			}
		}
	}
}
