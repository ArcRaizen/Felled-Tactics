#pragma once 
#ifndef GAMEMASTER_H
#define GAMEMASTER_H

#include "InputManager.h"
#include "Camera.h"
#ifndef VISUAL_H
#include "VisualElement.h"
#endif

#include <vector>

class GameMaster
{
public:
	GameMaster(void);
	~GameMaster(void);

	virtual void	Draw() = 0;

protected:
	void			UpdateMouseEvents(HWND hWnd);
	void			UpdateKeyboardEvents();
	void			SortVisualElements();
	void			SortVisualElementsInLayer(int layer);
	void			AddActiveLayer(int l);
	void			RemoveActiveLayer(int l);

	vector<VisualElement*>	VisualElements;
	int						activeLayers; // Only VisualElements on the ActiveLayers can be interacted with
};
#endif