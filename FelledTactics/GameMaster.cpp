#include "stdafx.h"
#include "GameMaster.h"

GameMaster::GameMaster(lua_State* luaState) : nextActionTime(0), activeLayers(0), sortingRequired(false), deletionRequired(false) 
{
	L = luaState;
}
GameMaster::GameMaster(void){}

GameMaster::~GameMaster(void)
{
	for(int i = 0; i < VisualElements.size(); i++)
	{
		delete VisualElements[i];
		VisualElements[i] = NULL;
	}

	VisualElements.clear();
}

int GameMaster::Update(float dt, HWND hWnd)
{
	if(sortingRequired)
		SortVisualElements();

	// Do not allow user input
	if(GameTimer::GetGameTime() < nextActionTime || nextActionTime < 0)
	{
		UpdateNoInput();
		return 2;
	}

	UpdateMouseEvents(hWnd);
	UpdateKeyboardEvents();

	return 1;
}

// Perform all update actions that do not correspond with player input
void GameMaster::UpdateNoInput()
{
	return;
	for(int i = 0; i < VisualElements.size(); i++)
	{
	}
}

// Track the mouse
void GameMaster::UpdateMouseEvents(HWND hWnd)
{
	// Update and save mouse button states and mouse position
	InputManager::Update();
	bool mouseDown = InputManager::IsMouseButtonDown(0);		// Is mouse currently down?
	D3DXVECTOR3 mousePosition = InputManager::GetMouseWorldCoords(hWnd, Camera::GetPosition());

	for(int i = 0; i < VisualElements.size(); i++)
	{	
		// Only allow interactions with elements on the active layers
		if(!(activeLayers & (1 << VisualElements[i]->Layer)))
			continue;

		// Mouse Down and Mouse Up
		if(mouseDown && VisualElements[i]->IsPointContained(mousePosition))
			VisualElements[i]->MouseDown();
		else if(!mouseDown && VisualElements[i]->IsPointContained(mousePosition))
			VisualElements[i]->MouseUp();

		// Remove VisualElements from list once they've been deleted (some Mouse Up/Down events cause a deletion immediately)
		if(VisualElements[i]->deleted)
		{
			VisualElements[i] = NULL;
			VisualElements.erase(VisualElements.begin() + i--);
			continue;
		}

		// MouseOver and MouseOut - Logic handled on Visual Element side, just tell them where the mouse is
		VisualElements[i]->SetCurrentMousePosition(mousePosition);
	}
}

void GameMaster::UpdateKeyboardEvents(){}

// Do not allow user to do anything
// Used to allow certain actions or animations to proceed without being interfered with
void GameMaster::PauseUserInput(float t)
{
	nextActionTime = GameTimer::GetGameTime() + t;
}

// Prevent user from doing anything until RestoreUserInput is called
void GameMaster::PauseUserInputIndefinite()
{
	nextActionTime = -1;
}

// Allow user to give input to game again
void GameMaster::RestoreUserInput()
{
	nextActionTime = GameTimer::GetGameTime();
}

// Use a simple Insertion Sort to order the game's current Visual Elements for drawing
// Once sorted by Layer, sorts all elements in each layer by vertical position on screen
void GameMaster::SortVisualElements()
{
	int j, layers = 0;
	for(int i = 1; i < VisualElements.size(); i++)
	{
		VisualElement* temp = VisualElements[i];
		j = i;
		layers |= (1 << temp->Layer);

		while(j > 0 && VisualElements[j-1]->Layer > temp->Layer)
		{
			VisualElements[j] = VisualElements[j-1];
			j--;
		}

		VisualElements[j] = temp;
	}

	// For each layer that has a VisualElement, sort it on vertical position
	for(j = 0; j < 10; j++)
	{
		if(layers & (1<<j))
			SortVisualElementsInLayer(j);
	}

	sortingRequired = false;
}

// A new VisualElement has been created, add it to the master list
void GameMaster::AddVisualElement(VisualElement* const &ve)
{
	VisualElements.push_back(ve);
	sortingRequired = true;
}

// Use Simple Insertion Sort to order a single layer's VisualElements for drawing
// Lower VisualElements are drawn first to give an isometric look
void GameMaster::SortVisualElementsInLayer(int layer)
{
	int j;
	for(int i = 1; i < VisualElements.size(); i++)
	{
		VisualElement* temp = VisualElements[i];
		j = i;

		if(temp->Layer == layer)
		{
			while(j > 0 && VisualElements[j-1]->GetCorner().y < temp->GetCorner().y &&
				VisualElements[j-1]->Layer == temp->Layer)
			{
				VisualElements[j] = VisualElements[j-1];
				j--;
			}

			VisualElements[j] = temp;
		}
	}
}

// Add new Layer of VisualElements into the ActiveLayer
void GameMaster::AddActiveLayer(int layer)
{
	activeLayers |= (1<<layer);
}

// Remove Layer of VisualElements from the ActiveLayer
void GameMaster::RemoveActiveLayer(int layer)
{
	activeLayers &= ~(1<<layer);
}

void GameMaster::Draw()
{
	for(int i = 0; i < VisualElements.size(); i++)
	{
		// Remove VisualElements from list once they've been deleted
		if(VisualElements[i]->deleted)
		{
			VisualElements[i] = NULL;
			VisualElements.erase(VisualElements.begin() + i--);
			continue;
		}
		
		VisualElements[i]->Draw();
	}
}