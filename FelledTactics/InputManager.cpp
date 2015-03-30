#include "StdAfx.h"
#include "InputManager.h"

#define DIRECTINPUT_VERSION 0x0800

#pragma region Static Declarations
IDirectInput8* InputManager::diObject = 0;
IDirectInputDevice8* InputManager::diKeyboardDevice = 0;
IDirectInputDevice8* InputManager::diMouseDevice = 0;
char InputManager::keyboardState[] = {'0'};
char InputManager::prevKeyboardState[] = {'0'};
int  InputManager::keyPressState[] = {0};
DIMOUSESTATE InputManager::mouseState;
BYTE InputManager::mouseButtons[] = {0};
BYTE InputManager::previousMouseButtons[] = {0};
long InputManager::mouseX = 0;
long InputManager::mouseY = 0;
#pragma endregion

InputManager::InputManager(void){}
InputManager::~InputManager(void){}

bool InputManager::Initialize(HWND hWnd)
{
	// Set the variables
	diObject = 0;
	diKeyboardDevice = 0;
	diMouseDevice = 0;
	mouseX = 0;
	mouseY = 0;

	// Set all the devices/listeners
	HRESULT hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&diObject, NULL);
	hr = diObject->CreateDevice(GUID_SysKeyboard, &diKeyboardDevice, NULL);
	hr = diKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	hr = diKeyboardDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	hr = diKeyboardDevice->Acquire();

	hr = diObject->CreateDevice(GUID_SysMouse, &diMouseDevice, NULL);
	hr = diMouseDevice->SetDataFormat(&c_dfDIMouse);
	hr = diMouseDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	hr = diMouseDevice->Acquire();

	POINT cursorPos;
	GetCursorPos(&cursorPos);
	mouseX = (int)cursorPos.x;
	mouseY = (int)cursorPos.y;

	return true;
}

void InputManager::ShutDown()
{
	if(diMouseDevice)
	{
		diMouseDevice->Unacquire();
		diMouseDevice->Release();
		diMouseDevice = 0;
	}

	if(diKeyboardDevice)
	{
		diKeyboardDevice->Unacquire();
		diKeyboardDevice->Release();
		diKeyboardDevice = 0;
	}

	if(diObject)
	{
		diObject->Release();
		diObject = 0;
	}
}

bool InputManager::Update()
{
	bool result;
	
	result = ReadKeyboard();
	if(!result)
		return false;

	result = ReadMouse();
	if(!result)
		return false;

	// Update location of the mouse based on changes since last call
	mouseX += mouseState.lX;
	mouseY += mouseState.lY;

	// Check if the mouse goes off the screen
	if(mouseX < 0)
		mouseX = 0;
	else if(mouseX > screenWidth)
		mouseX = screenWidth;

	if(mouseY < 0)
		mouseY = 0;
	else if(mouseY > screenHeight)
		mouseY = screenHeight;

	Analyze();
	return true;
}

// Update Keyboard Information
bool InputManager::ReadKeyboard()
{
	HRESULT hr;

	// Set Previous Keyboard State
	for(int i = 0; i < sizeof(keyboardState)/sizeof(bool); i++)
		prevKeyboardState[i] = keyboardState[i];

	hr = diKeyboardDevice->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	// Check to see if the keyboard has been lost, either by being unacquired or minimizing the window
	if(FAILED(hr))
	{
		if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			diKeyboardDevice->Acquire();
		else
			return false;
	}

	return true;
}

// Update Mouse Information
bool InputManager::ReadMouse()
{
	HRESULT hr;

	hr = diMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseState);

	// Check to see if the mouse has been lost, either by being unacquired or by minimizing the window
	if(FAILED(hr))
	{
		if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			diMouseDevice->Acquire();
		else
			return false;
	}

	mouseX += mouseState.lX;
	mouseY += mouseState.lY;
	for(int i = 0; i < 4; i++)
	{
		previousMouseButtons[i] = mouseButtons[i];
		mouseButtons[i] = mouseState.rgbButtons[i];
	}

	return true;
}

bool InputManager::IsLeftMouseDown()
{
	if(mouseButtons[0] & 0x80)
		return true;

	return false;
}

bool InputManager::IsRightMouseDown()
{
	if(mouseButtons[1] & 0x80)
		return true;

	return false;
}

bool InputManager::IsMouseButtonDown(int button)
{
	if(mouseButtons[button] & 0x80)
		return true;

	return false;
}

// Was the Left Mouse Button pressed this frame
bool InputManager::MouseDownEvent()
{
	if(mouseButtons[0] & 0x80 && !(previousMouseButtons[0] & 0x80))
		return true;
	
	return false;
}

// Was the Left MouseButton released this frame
bool InputManager::MouseUpEvent()
{
	if(!(mouseButtons[0] & 0x80) && previousMouseButtons[0] & 0x80)
		return true;
	
	return false;
}

// Was the Right Mouse Button pressed this frame
bool InputManager::RightMouseDownEvent()
{
	if(mouseButtons[1] & 0x80 && !(previousMouseButtons[1] & 0x80))
		return true;

	return false;
}

// Was the Right MouseButton released this frame
bool InputManager::RightMouseUpEvent()
{
	if(!(mouseButtons[1] & 0x80) && previousMouseButtons[1] & 0x80)
		return true;

	return false;
}

D3DXVECTOR3 InputManager::GetMouseWorldCoords(HWND hWnd, D3DXVECTOR3 cameraPosition)
{
	// Get client(window) width and height
	RECT clientRect;
	GetClientRect(hWnd, (LPRECT)&clientRect);
	float clientWidth = clientRect.right;
	float clientHeight = clientRect.bottom;

	// Get cursor position on the screen, convert to client coordinates
	LPPOINT cursorPos = new POINT;
	GetCursorPos(cursorPos);
	ScreenToClient(hWnd, cursorPos);

	// Convert cursor coords to world coordinates using camera position
	float mouseWorldX, mouseWorldY;
	//mouseWorldX = cursorPos->x - (screenWidth / 2) + cameraPosition.x;
	//mouseWorldY = (screenHeight / 2) - cursorPos->y + cameraPosition.y;
	mouseWorldX = (((cursorPos->x / clientWidth) * (int)screenWidth) - (int)screenWidth/2) + cameraPosition.x;
	mouseWorldY = (-((cursorPos->y / clientHeight) * (int)screenHeight) + (int)screenHeight/2) + cameraPosition.y;

	return D3DXVECTOR3(mouseWorldX, mouseWorldY, 0.0f);
}

D3DXVECTOR2 InputManager::GetMouseUV(HWND hWnd)
{
	RECT clientRect;
	GetClientRect(hWnd, (LPRECT)&clientRect);
	float clientWidth = clientRect.right;
	float clientHeight = clientRect.bottom;

	// Get cursor position on the screen, convert to client coordinates
	LPPOINT cursorPos = new POINT;
	GetCursorPos(cursorPos);
	ScreenToClient(hWnd, cursorPos);

	// Convert cursor coords to [-1,1]
	float mouseU, mouseV;
	mouseU = ((2.0f * cursorPos->x) / clientWidth) - 1.0f;
	mouseV = (((2.0f * cursorPos->y) / clientHeight) -1.0f) * -1.0f;

	return D3DXVECTOR2(mouseU, mouseV);
}



D3DXVECTOR3 InputManager::GetMouseClient(HWND hWnd)
{
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(hWnd, &cursorPos);
	return D3DXVECTOR3(cursorPos.x, cursorPos.y, 0.0f);
}

// Is any specific key down
bool InputManager::IsKeyDown(DWORD key)
{
	if(keyboardState[key] & 0x80)
		return true;

	return false;
}

// Is any specific key up
bool InputManager::IsKeyUp(DWORD key)
{
	if(keyboardState[key] & 0x80)
		return false;

	return true;
}

bool InputManager::IsKeyPressed(DWORD key)
{
	// check for keydown
	if(IsKeyDown(key))
		keyPressState[key] = 1;

	if(keyPressState[key] == 1)
	{
		if(IsKeyUp(key))
			keyPressState[key] = 2;
	}

	if(keyPressState[key] == 2)
	{
		keyPressState[key] = 0;
		return true;
	}

	return false;
}

void InputManager::Analyze()
{
	/*for(int i = 0; i < ARRAY_SIZE; i++)
	{
		if(keyboardState[i] != 0 && prevKeyboardState[i] == 0)		// Key was pressed this frame
			state[i] = true;
		else if(keyboardState[i] = 0 && prevKeyboardState[i] != 0)	// Key was released this frame
			state[i] = false;
	}*/
}