#include "stdafx.h"
#include "GameMaster.h"

GameMaster::GameMaster(void){ nextActionTime = 0; }
GameMaster::~GameMaster(void){}

int GameMaster::Update(float dt, HWND hWnd)
{
	// Do not allow user input
	if(GameTimer::GetGameTime() < nextActionTime || nextActionTime < 0)
		return 2;

	UpdateMouseEvents(hWnd);
	UpdateKeyboardEvents();

	return 1;
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
		// Remove VisualElements from list once they've been deleted
		if(VisualElements[i]->deleted)
		{
			delete VisualElements[i];
			VisualElements[i] = NULL;
			VisualElements.erase(VisualElements.begin() + i--);
			continue;
		}

		// Only allow interactions with elements on the active layers
		if(!(activeLayers & (1 << VisualElements[i]->Layer)))
			continue;

		// Mouse Down and Mouse Up
		if(mouseDown && VisualElements[i]->IsPointContained(mousePosition))
			VisualElements[i]->MouseDown();
		else if(!mouseDown && VisualElements[i]->IsPointContained(mousePosition))
			VisualElements[i]->MouseUp();

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