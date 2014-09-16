#pragma once 
#ifndef GAMEMASTER_H
#define GAMEMASTER_H

#include "InputManager.h"
#include "Camera.h"
#ifndef VISUAL_H
#include "VisualElement.h"
#endif
#ifndef GAMETIMER_H
#include "GameTimer.h"
#endif

#include <vector>

// Layers for each Visual Element
#define TILE_LAYER				1
#define TILE_EFFECT_LAYER		2
#define UNIT_LAYER				3
#define ACTION_MENU_LAYER		4
#define SECONDARY_MENU_LAYER	5
#define COMBAT_TEXT_LAYER		6

class GameMaster
{
public:
	GameMaster(lua_State* luaState);
	GameMaster(void);
	~GameMaster(void);

	virtual int		Update(float dt, HWND hWnd);
	virtual void	Draw();

protected:
	void			UpdateNoInput();
	void			UpdateMouseEvents(HWND hWnd);
	void			UpdateKeyboardEvents();
	void			PauseUserInput(float t);
	void			PauseUserInputIndefinite();
	void			RestoreUserInput();
	void			AddVisualElement(VisualElement* const &ve);
	void			SortVisualElements();
	void			SortVisualElementsInLayer(int layer);
	void			AddActiveLayer(int l);
	void			RemoveActiveLayer(int l);

	float		nextActionTime;		// track when the user is allowed to perform an action

private:
	vector<VisualElement*>	VisualElements;		// List of all VisualElements currently in the game
	int						activeLayers;		// Only VisualElements on the ActiveLayers can be interacted with
	bool					sortingRequired;	// Has a VisualElement been added? A sort is needed afterwards
	bool					deletionRequired;	// Has a VisualElement been marked for deletion?

protected:
	lua_State* L;
};
#endif