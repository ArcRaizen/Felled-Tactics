#pragma once 
#ifndef GAMEMASTER_H
#define GAMEMASTER_H

#include "InputManager.h"
#include "Camera.h"
#ifndef VISUAL_H
#include "VisualElement.h"
#endif

#include <vector>

// Layers for each Visual Element
#define TILE_LAYER				1
#define TILE_EFFECT_LAYER		2
#define UNIT_LAYER				3
#define ACTION_MENU_LAYER		4
#define SECONDARY_MENU_LAYER	5

class GameMaster
{
public:
	GameMaster(void);
	~GameMaster(void);

	virtual int		Update(float dt, HWND hWnd);
	virtual void	Draw() = 0;

protected:
	void			UpdateMouseEvents(HWND hWnd);
	void			UpdateKeyboardEvents();
	void			DeleteElements();
	void			SortVisualElements();
	void			SortVisualElementsInLayer(int layer);
	void			AddActiveLayer(int l);
	void			RemoveActiveLayer(int l);

	vector<VisualElement*>	VisualElements;
	int						activeLayers; // Only VisualElements on the ActiveLayers can be interacted with
	bool					deletionRequired;
};
#endif