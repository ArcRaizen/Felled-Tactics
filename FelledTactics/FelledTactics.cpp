// FelledTactics.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "FelledTactics.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;						// current instance
HWND hWnd;
TCHAR szTitle[MAX_LOADSTRING];			// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];	// the main window class name
double deltaTime;
DWORD desiredFramesPerSecond = 60;
LONGLONG curTime;			// current time
DWORD timeCount = 1000 / 60;	// Milliseconds(ms) per frame, default if no performance counter
LONGLONG perfCnt;			// performance timer frequency
BOOL perfFlag = false;		// flag determining which timer to use
LONGLONG nextTime = 0;		// time to render next frame
BOOL moveFlag = true;		// flag noting if we have updated yet

// Time variables
#define MAX_SAMPLES 100
float FRAME_RATE;
float frameRateAverage = 0;
int	  frameCount = 0;
int	  frameRateIndex = 0;
float frameRateSum = 0;
float frameRateList[MAX_SAMPLES];


// Window stuff
bool vsync = false;
bool fullscreen = false;

#pragma region Forward declarations of functions
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
BOOL				DXInit(HWND hWnd);
BOOL				CreateVertexBuffer();
BOOL				CreateIndexBuffer();
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
wstring				ToWString(string s);

void				InitFonts();
void				InitBlendStates();

void				GameInitialize();
void				Update(float dt);
void				Draw();
void				DrawFrameRate();
#pragma endregion

#pragma region Game Variables
Level*					currentLevel;
int						GameState = 0;
#pragma endregion

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_FELLEDTACTICS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
		return FALSE;

	srand((unsigned) time(NULL));
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FELLEDTACTICS));

	// Game Initialization
	GameInitialize();

	if(QueryPerformanceFrequency((LARGE_INTEGER*) &perfCnt))
	{
		perfFlag = true;
		timeCount = (DWORD)perfCnt / 60;	// calculate time per frame based on frequency
		QueryPerformanceCounter((LARGE_INTEGER*) &nextTime);
	}
	else
		nextTime = timeGetTime();

	// Main message loop: http://www.mvps.org/directx/articles/writing_the_game_loop.htm
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Keep frame rate at desired constant if possible
			#ifdef LOCK_FRAMERATE
				// Run Update
				if(moveFlag)
				{
					GameTimer::Tick();
					FRAME_RATE = 1.0f / GameTimer::GetDeltaTime();
					Update(GameTimer::GetDeltaTime());
					moveFlag = false;
				}

				// Use appropriate message to get time
				if(perfFlag)
					QueryPerformanceCounter((LARGE_INTEGER*) &curTime);
				else
					curTime = timeGetTime();

				// Draw
				if(curTime > nextTime)
				{
					Draw(GameTimer::GetDeltaTime());
					nextTime += timeCount;

					// If we get more than a frame ahead, allow us to drop one
					// Otherwise we will never catch up if we let the error accumulate
					//	and message handling will suffer
					if(nextTime < curTime)
						nextTime = curTime + timeCount;

					// Return to update now that draw is finished
					moveFlag = true;
				}
			#else
				// Let system run as fast as possible
				GameTimer::Tick();
				deltaTime = GameTimer::GetDeltaTime();
				FRAME_RATE = 1.0f / deltaTime;
				Update(deltaTime);
				Draw();
			#endif

				// Average frame rate
				frameRateSum -= frameRateList[frameRateIndex];
				frameRateSum += FRAME_RATE;
				frameRateList[frameRateIndex++] = FRAME_RATE;
				if(frameRateIndex == MAX_SAMPLES)
					frameRateIndex = 0;

				frameRateAverage = frameRateSum/MAX_SAMPLES;
		}
	}

	// SHUT DOWN GAME HERE

	return (int) msg.wParam;
}

void GameInitialize()
{
	srand(time(NULL));

	currentLevel = new Level(26, 16, 50);
	currentLevel->GenerateLevel();

	Skill* s = new Skill("bob");
}

void Update(float dt)
{
	Camera::Update();
	currentLevel->Update(dt, hWnd);
}

void Draw()
{
	// Reset back buffer for next frame and set Shader Matrices
	Direct3D::StartFrame(Camera::GetViewMatrix());

	currentLevel->Draw();

	DrawFrameRate();

	// Swap to back buffer
	Direct3D::EndFrame();
}


void DrawFrameRate(void)
{
	stringstream ss(stringstream::in | stringstream::out);
	ss << (int)FRAME_RATE;
	wstring r = ToWString(ss.str());
	LPCWSTR txt = r.c_str();
	//create debug text
	// Create a rectangle to indicate where on the screen it should be drawn
	RECT rect;
	rect.left = 100;
	rect.right = 1200;
	rect.top = 0;
	rect.bottom = rect.top + 20;

	// Draw frame rate
	Direct3D::GetFont()->DrawText(NULL, txt, -1, &rect, 10, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
	
	// Change for average frame rate
	stringstream ss2(stringstream::in | stringstream::out);
	ss2 << frameRateAverage;
	r = ToWString(ss2.str());
	txt = r.c_str();
	rect.top += 20;		// move drawing location
	rect.bottom += 20;

	// Draw average frame rate
	Direct3D::GetFont()->DrawText(NULL, txt, -1, &rect, 10, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));

	// Reset
	Direct3D::DrawTextReset();
}

#pragma region  DirectX Initialization
BOOL DXInit(HWND hWnd)
{
	hr = S_OK;	// Used for debugging

	Direct3D::InitializeDirect3D(vsync, hWnd, fullscreen, 1000.0f, 0.1f);
	Camera::Initialize();
	Camera::SetPosition(0.0f, 0.0f, -10.0f);

	return true;
}
#pragma endregion

#pragma region WindowsFunctions
//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FELLEDTACTICS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_FELLEDTACTICS);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   //HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, screenWidth, screenHeight, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   if(!DXInit(hWnd))
   {
	   return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		// Static Class Initialization
		GameTimer::Init();
		InputManager::Initialize(hWnd);
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
#pragma endregion

#pragma region Utility Funcions
wstring ToWString(string s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	wstring r(buf);
	delete[] buf;
	return r;
}
#pragma endregion