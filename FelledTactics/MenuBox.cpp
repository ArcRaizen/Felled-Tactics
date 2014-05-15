#include "StdAfx.h"
#include "MenuBox.h"


MenuBox::MenuBox(void)
{
	mousePosition.x = mousePosition.y = -1;
}


MenuBox::~MenuBox(void)
{
}


void MenuBox::CreateElement(void (Level::*func)(), WCHAR* filename, int width, int height, int offsetX, int offsetY, const char* t)
{
	elements.push_back(new MenuElement(filename, layer, width, height, 
		leftCorner.x + offsetX, leftCorner.y + offsetY, t, func));
}

// Save the mouse position for MouseDown/MouseUp checks on its Elements, check MouseOver/MouseOut events
void MenuBox::SetCurrentMousePosition(D3DXVECTOR3 mousePos)
{
	mousePosition = mousePos;		// save current mouse position
	if(IsPointContained(mousePos))
	{
		for(int i = 0; i < elements.size(); i++)
			elements[i]->SetCurrentMousePosition(mousePos);
	}
}

// Do MouseDown event only on Element that the mouse is over
void MenuBox::MouseDown()
{
	mouseDown = true;
	for(int i = 0; i < elements.size(); i++)
	{
		if(elements[i]->IsPointContained(mousePosition))
			elements[i]->MouseDown();
	}
}

// Do MouseUp event only on Element that mouse is over
void MenuBox::MouseUp()
{
	if(!mouseDown)
		return;

	mouseDown = false;
	for(int i = 0; i < elements.size(); i++)
	{
		if(elements[i]->IsPointContained(mousePosition))
			elements[i]->MouseUp();
	}
}


bool MenuBox::Draw()
{
	VisualElement::Draw();

	for(int i = 0; i < elements.size(); i++)
		elements[i]->Draw();

	return true;
}