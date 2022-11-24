#ifndef _GAMEWINDOW_H_
#define _GAMEWINDOW_H_

#include	"Object.h"
#include	<windows.h>
#include	<gx.h>
#include	"GraphicsClasses.h"
#include	"Controls.h"
#include	"Preferences.h"


class CGameWindow : public CObject
{
public:
	CGameWindow();
	~CGameWindow();

	// ----
	// Opens a window.
	bool	Init(	HINSTANCE		_hInstance,
					LPTSTR			_lpCmdLine,
					int				_nCmdShow,
					CPlatform*		_PlatformRef,
					CPreferences*	_PreferencesRef);

	// ----
	// Return a handle to the GAPI wrapper object.
	CGraphics*	GetGraphicsObject();

	// ----
	// Return a handle to the GAPI wrapper object.
	CControls*	GetControlObject();
	
	// ----
	// Process Windows(R) messages. 
	// Returns true to indicate all is okay. 
	// Returns false if the program should exit.
	// This method will block execution if the app is minimised / lost focus.
	bool	Process();

#if (defined(PROJECT) && (PROJECT == SPEEDBALL))
	// ----
	// Returns the location of the mouse pointer.
	// -1, -1 means there is no mouse.
	POINT*	GetMousePoint();
#endif

	// ----
	void	InvalidateMouse();

	// ----
	// Returns true if the program has just woken up.
	bool	GetJustActivated();

	// ----
	HWND	GetWindowHandle() {return WindowHandle;}

	// ----
	// Static callback method used to process windows messages.
	static	LRESULT CALLBACK WindowProc(HWND	_hWnd,	
										UINT	_uMsg, 
										WPARAM	_wParam, 
										LPARAM	_lParam);
protected:
	HWND			WindowHandle;
	HINSTANCE		HInstance;
	bool			Active;
	bool			JustActivated;
	LPLOGPALETTE 	LogicalPalette;
	int				PaletteSize;
	HPALETTE		Palette;
	PALETTEENTRY	*SystemPalette;

	POINT			MousePointer;

	CGraphics*		TheGraphicsObject;
	CControls*		TheController;
};

#endif