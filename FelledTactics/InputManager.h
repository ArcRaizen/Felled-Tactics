#pragma once
#ifndef INPUT_H
#define INPUT_H
#include "Core.h"
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <dinput.h>
#include <D3DX10.h>
#include <Windows.h>
#include <WindowsX.h>
const int ARRAY_SIZE = 256;

static class InputManager
{
public:
	InputManager(void);
	~InputManager(void);

	static bool Initialize(HWND);
	static void ShutDown();
	static bool Update();
	static bool ReadKeyboard();
	static bool ReadMouse();
	static void GetMouseAngle(float& angle);
	static bool IsLeftMouseDown();
	static bool IsRightMouseDown();
	static bool IsMouseButtonDown(int button);
	static bool MouseDownEvent();
	static bool MouseUpEvent();
	static bool RightMouseDownEvent();
	static bool RightMouseUpEvent();
	static bool IsKeyDown(DWORD key);
	static bool IsKeyUp(DWORD key);
	static bool IsKeyPressed(DWORD key);
	static void Analyze();
	static D3DXVECTOR3 GetMouseWorldCoords(HWND hWnd, D3DXVECTOR3 cameraPosition);
	static D3DXVECTOR3 GetMouseClient(HWND hWnd);

	static IDirectInput8*		diObject;
	static IDirectInputDevice8*	diKeyboardDevice;
	static IDirectInputDevice8*	diMouseDevice;

private:
	static char			keyboardState[ARRAY_SIZE];
	static char			prevKeyboardState[ARRAY_SIZE];
	static int			keyPressState[ARRAY_SIZE];			// record key state for key press (down and up)
	static DIMOUSESTATE	mouseState;
	static BYTE			mouseButtons[4];
	static BYTE			previousMouseButtons[4];
	static long			mouseX;
	static long			mouseY;
};
#endif