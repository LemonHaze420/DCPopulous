#include	"GameWindow.h"

#include	<windows.h>
#include	<Aygshell.h>
#include	"SafeProgramming.h"
#include	"Object.h"
#include	"GameDefines.h"

CGameWindow::CGameWindow()
{
	WindowHandle		= NULL;
	HInstance			= NULL;
	Active				= false;
	JustActivated		= false;
	LogicalPalette		= NULL;
	TheGraphicsObject	= NULL;
	TheController		= NULL;
	SystemPalette		= NULL;
}

// ----
CGameWindow::~CGameWindow()
{
	SAFELY_DELETE(TheGraphicsObject);
	SAFELY_DELETE(TheController);

	char*	charstar = (char*)LogicalPalette;
	SAFELY_DELETE_ARRAY(charstar);
	LogicalPalette = NULL;

	SAFELY_DELETE_ARRAY(SystemPalette);
}

// ----
// Opens a window.
bool	CGameWindow::Init(	HINSTANCE		_hInstance,
							LPTSTR			_lpCmdLine,
							int				_nCmdShow,
							CPlatform*		_PlatformRef,
							CPreferences*	_PreferencesRef)
{
#ifdef TEST_IN_EMULATION
	SAFELY_DELETE(TheGraphicsObject);
	NEW(TheGraphicsObject, CGraphics());
	TheGraphicsObject->Init(0, Palette, _PreferencesRef, _PlatformRef);

	NEW(TheController, CControls());
	SAFELY_CALL(TheController, Init());
	return true;
#endif

	SAFELY_DELETE(TheGraphicsObject);
	NEW(TheGraphicsObject, CGraphics());

	HWND		hWnd;
	WNDCLASS	wc;

	// Store instance handle.
	HInstance = _hInstance;

	// Attempt to locate any existing application instance.
	if (hWnd = FindWindow(GAME_WINDOW_CLASS_NAME, PROJECT_NAME))
	{
		// Store the handle of the currently active window.
		HWND	oldhWnd = GetActiveWindow();

		// Deactivate the currently active window, then 
		// activate the other instance of this program. 
		// And finally quit.
		PostMessage(oldhWnd, WM_ACTIVATE, WA_INACTIVE, (long)hWnd);
		PostMessage(hWnd, WM_ACTIVATE, WA_ACTIVE, (long)oldhWnd);
		return true;
	}

	// Either no previous instance, or calls failed. 
	// Go ahead with creating new window.
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)WindowProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= HInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= 0;
	wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= GAME_WINDOW_CLASS_NAME;

	if (RegisterClass(&wc))
	{

#ifdef _WIN32_WCE
		hWnd = CreateWindow(GAME_WINDOW_CLASS_NAME, 
							PROJECT_NAME, 
							WS_VISIBLE,
							0,								// Full screen display.
							0, 
							GetSystemMetrics(SM_CXSCREEN), 
							GetSystemMetrics(SM_CYSCREEN), 
							NULL, 
							NULL, 
							HInstance, 
							NULL);
#else
		hWnd = CreateWindow(GAME_WINDOW_CLASS_NAME, 
							PROJECT_NAME, 
							WS_VISIBLE,
							0,								// Emulate a 240x320 display.
							0, 
							240, 
							320,
							NULL, 
							NULL, 
							HInstance, 
							NULL);

#endif

		// Quit if failed.
		if (!hWnd)
		{	
			return false;
		}
		
		// Show window
		ShowWindow(hWnd, _nCmdShow);
		UpdateWindow(hWnd);

		// ----
		// Palette
		// 1. Determine if we need to generate a palette or not.
		if (GetDeviceCaps(GetDC(hWnd), RASTERCAPS) & RC_PALETTE)
		{
			PaletteSize = GetDeviceCaps(GetDC(hWnd), SIZEPALETTE);
			if (PaletteSize < 256)
				return false;	// Palette too small 

			SAFELY_DELETE_ARRAY(SystemPalette);
			SystemPalette = (PALETTEENTRY*)new char[sizeof(PALETTEENTRY) * PaletteSize];
			GetSystemPaletteEntries(	GetDC(hWnd),
										0, 
										PaletteSize,
										SystemPalette);

			//if (_PlatformRef->GetMachineType() == EM_HP_Jornada545)
			//{
			//	MessageBox(hWnd, TEXT("This device reports as a Jornada545, in the 8bit display mode.\n\n\Populous will run in this mode, however, you may enjoy a better gaming experience if you change the device to use 16bit colour. The option can be access via the 'HP settings' program, usually located on the 'Start' menu."), PROJECT_NAME, MB_OK);
			//}

			/*
			PaletteSize = GetDeviceCaps(GetDC(hWnd), SIZEPALETTE);
			if (PaletteSize < 256)
				return false;	// Palette too small 

			LogicalPalette = (LOGPALETTE*)new char[sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * PaletteSize];
			if (LogicalPalette)
			{
				// 2. Set up a palette then.
				LogicalPalette->palNumEntries	= (WORD)PaletteSize;	// Hopefully 256.
				LogicalPalette->palVersion		= (WORD)0x0300;

				for (int i = 0; i < PaletteSize; i++)
				{
					LogicalPalette->palPalEntry[i].peRed	= (i & 0xE0);
					LogicalPalette->palPalEntry[i].peGreen	= ((i & 0x1C) << 3);
					LogicalPalette->palPalEntry[i].peBlue	= ((i & 0x03) << 6);
					LogicalPalette->palPalEntry[i].peFlags	= 0;	// Flags are ignored on CE, and must be 0.
				}

				// 3. Create a palette
				Palette = CreatePalette(LogicalPalette);	// Palette is a class member now.
				if (Palette)
				{
					//if (!SetPaletteEntries(Palette, 0, 256, PaletteArray))
					//{
					//	MessageBox(hWnd, TEXT("Can't set palette!"), PROJECT_NAME, MB_OK);
					//}

					//wsprintf(buffer, TEXT("There are %d colours in the logical palette."), GetPaletteEntries(Palette, 0, 256, PaletteArray));
					//MessageBox(hWnd, buffer, PROJECT_NAME, MB_OK);

					if (SelectPalette(	GetDC(hWnd),
										Palette,
										false))		// Force background is ignored by CE.
					{
						if (RealizePalette(GetDC(hWnd)) == GDI_ERROR)
						{
						//	delete[] LogicalPalette;
							return false;
						}

						// Apparently may need to call it a second time before it takes effect on the display.
						if (RealizePalette(GetDC(hWnd)) == GDI_ERROR)
						{
						//	delete[] LogicalPalette;
							return false;
						}
					}
					else
					{
						//delete[] LogicalPalette;
						//MessageBox(hWnd, TEXT("Failed to select palette"), PROJECT_NAME, MB_OK);
					}
				}
				else
				{
					//delete[] LogicalPalette;
					return false;	// Failed to create palette.
				}
			}
			else
				return false; // Failed to allocate memory for palette. 
			*/
		}

		//ShowWindow(hWnd, _nCmdShow);
		UpdateWindow(hWnd);

		SAFELY_CONTROL(if, TheGraphicsObject, Init(hWnd, Palette, _PreferencesRef, _PlatformRef), ==, false)
		{
			return false;
		}
		else
		{
			TheGraphicsObject->AssignSysPalette(SystemPalette, PaletteSize);
		}

		NEW(TheController, CControls());
		SAFELY_CALL(TheController, Init());

		return true;
	}
	return false;
}

// ----
// Static callback method used to process windows messages.
LRESULT CALLBACK CGameWindow::WindowProc(	HWND	_hWnd,	
											UINT	_uMsg, 
											WPARAM	_wParam, 
											LPARAM	_lParam)
{
	switch (_uMsg)
    {
		// - Life control ---------------------------------
		// Basically, it is very easy for an application 
		// to lose focus, and some PocketPCs make it 
		// difficult for the user to return to a running 
		// application. Therefore, the application may 
		// remain running for days without the user being 
		// aware of the fact. With the systems limited 
		// resources we need to handle Windows messages 
		// correctly, especially WM_CLOSE and WM_HIBERNATE.

		// ----------------------------
		// Creation / Destruction
		// ----------------------------

		case WM_CREATE:
			// This code will be executed before the 
			// Windows create call returns. 
			break;

		case WM_CLOSE:
			// CE wants us to quit, and will kill this task 
			// in 8 seconds if we do not stop gracefully.

			// Don't need to save data, etc, yet.
			DestroyWindow(_hWnd);
			PostQuitMessage(0);
			break;

		case WM_DESTROY:
			PostMessage(_hWnd, WM_MAIN_THREAD_DESTROY, _wParam, _lParam);
			break;

		// ----------------------------
		// Hibernation / Activation
		// ----------------------------

		case WM_ACTIVATE:
			// Should get this message when coming out of 
			// hibernation and when the application is 
			// reactivated.
	
			PostMessage(_hWnd, WM_MAIN_THREAD_ACTIVATE, _wParam, _lParam);

			// Check to see that this is an activation 
			// message (and not a deactivation message). 
			if ((LOWORD(_wParam) == WA_ACTIVE) || 
				(LOWORD(_wParam) == WA_CLICKACTIVE))
			{
				// Attempt to, and probably fail to, set 
				// this as the active window.
				if (!SetForegroundWindow(_hWnd))
				{
					ODSN("Activation message!");
					ShowWindow(_hWnd, SW_SHOW);
					UpdateWindow(_hWnd);
				}
				PostMessage(_hWnd, WM_PAINT, 0, 0);

				return false;
			}
			return DefWindowProc(_hWnd, _uMsg, _wParam, _lParam);
		
		case WM_ENABLE:
			// The window has been enabled or disabled.
			// The state (i.e. whether it is enabled or 
			// disabled) can be obtained using the 
			// following expression -- (BOOL)wParam;
			//return 0;
			break;
		
#ifdef _WIN32_WCE
		case WM_HIBERNATE:
			// Windows wants us to return as many 
			// resources as possible. Probably best if we 
			// release all resources when we lose focus 
			// anyway. 
			break;
#endif


		case WM_KILLFOCUS:
			// Once GAPI is working we MUST call GXSuspend() 
			// here or otherwise the user is not going to 
			// be able to control the device. -  we would 
			// still have control of the screen and the 
			// function keys. 
			GXSuspend();
			break;


		case WM_SETFOCUS:
			// And always call GXResume() here.
			if (!GXResume())
			{
				PostQuitMessage(0);
			}
			else
			{
				// Okay, GAPI restored the display. 
				// Send a message WM_PAINT message to self.
				PostMessage(_hWnd, WM_PAINT, 0, 0);
			}
			break;

		case WM_COMMAND:
			// Process menu input - although there isn't one at the moment.
			break;

		case WM_LBUTTONDOWN:
			//PostQuitMessage(0); // For safety for now.
			break;

		case WM_LBUTTONUP:
			break;
		
		case WM_RBUTTONUP:
			break;

		case WM_MOUSEMOVE:
 			break;

		case WM_CHAR:
			PostMessage(_hWnd, WM_MAIN_THREAD_CHAR, _wParam, _lParam);
			break;

		default:
			return DefWindowProc(_hWnd, _uMsg, _wParam, _lParam);
	}

	return false;
}


// ----
// Return a handle to the GAPI wrapper object.
CGraphics*	CGameWindow::GetGraphicsObject()
{
	return TheGraphicsObject;
}


// ----
// Return a handle to the GAPI wrapper object.
CControls*	CGameWindow::GetControlObject()
{
	return TheController;
}



// ----
// Process Windows(R) messages. 
// Returns true to indicate all is okay. 
// Returns false if the program should exit.
// This method will block execution if the app is minimised / lost focus.
bool	CGameWindow::Process()
{
	BOOL		Continue;
	MSG			Msg;
//	SHRGINFO	shrg;
//	DWORD		Gesture;

	// Obviously the program is running and hence not just been activated. 
	JustActivated = false;

	// Update controls.
	SAFELY_CALL(TheController, Process());

	// Start by invalidating mouse pointer location. 
	// (Unless the new feature to disable this is set).
	// If the stylus is on the screen then it will be updated.
	if (!TheController->IsButtonPressed(MouseSelect))
	{
	//	MousePointer.x = -1;
	//	MousePointer.y = -1;
	}

	// This loop has two states. One for when the 
	// application is active and another for when the app 
	// has been deactivated. The first basically peeks for 
	// messages and dispatches them to the WinProc function
	// for processing. The second state involves using the 
	// GetMessage function which blocks until the function 
	// can return a message. The loop can not be exited
	// when it is in the second state, unless a WM_QUIT 
	// message is received. The second state is a wait 
	// state, for when the program has been deactivated. 
	// The loop changes between states when it processes a 
	// WM_MAIN_THREAD_ACTIVATE message. This is 
	// effectively a custom name for the WM_ACTIVATE 
	// message typically sent directly to the WinProc 
	// function. It would be safe to process the message 
	// using the DispatchMessage function, however, there 
	// is no point doing so.
	Continue = PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE);
	while (Continue)
	{
		// Activate and deactivate the application - should stop 
		// the program running when it is deactivated.
		switch (Msg.message)
		{
		case WM_MAIN_THREAD_ACTIVATE:
		case WM_ACTIVATE:
			
			// Force the display to refresh.
			SAFELY_CALL(TheGraphicsObject, ForceRedraw());

			Active =	(	(LOWORD(Msg.wParam) != WA_INACTIVE) );
					//	&& 	(!HIWORD(Msg.wParam))	// Minimised!
					//	); 

			// Will be set to true if the program has just been activated. 
			JustActivated = Active;
			
			// Just been reactiveated - update controls.
			SAFELY_CALL(TheController, Process());
			if (!TheController->IsButtonPressed(MouseSelect))
			{
				MousePointer.x = -1;
				MousePointer.y = -1;
			}
			break;

		// Destroy the window.
		case WM_MAIN_THREAD_DESTROY:
		case WM_DESTROY:
			SAFELY_DELETE(TheGraphicsObject);
			SAFELY_DELETE(TheController);
			PostQuitMessage(0);
			break;

		case WM_MAIN_THREAD_CHAR:
		case WM_CHAR:
			TheController->QueueChar((TCHAR)Msg.wParam, Msg.lParam);
			break;

		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
			//fwKeys = wParam;  
			MousePointer.x = LOWORD(Msg.lParam);
			MousePointer.y = HIWORD(Msg.lParam);

			break;

		case WM_LBUTTONUP:
			//fwKeys = wParam;  
			MousePointer.x = -1;//LOWORD(Msg.lParam);
			MousePointer.y = -1;//HIWORD(Msg.lParam);
			break;

		default:
			// Translate and dispatch the message
			//if (!(m_pMainWindow && TranslateAccelerator(m_pMainWindow->GetHwnd(), m_hAccel, &msg)))
			{
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
			break;
		}

		// Get next message. 
		if (Active)
		{
			Continue = PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE);
			if (Msg.message == WM_QUIT)
			{
				return false;
			}
		}
		else
		{
			Continue = GetMessage(&Msg, NULL, 0, 0);
			if (!Continue) // i.e. the message was WM_QUIT.
			{
				return false;
			}
		}
    }

	// Pass mouse pointer info to the controller class.
	if (TheController)
		TheController->SetMouse(&MousePointer);

	return true;
}

#if (defined(PROJECT) && (PROJECT == SPEEDBALL))
// ----
// Returns the location of the mouse pointer.
// -1, -1 means there is no mouse.
POINT*	CGameWindow::GetMousePoint()
{
	return &MousePointer;
}
#endif

// ----
void	CGameWindow::InvalidateMouse()
{
	MousePointer.x = -1;
	MousePointer.y = -1;
}

// ----
// Returns true if the program has just woken up.
bool	CGameWindow::GetJustActivated()
{
	return JustActivated;
}

