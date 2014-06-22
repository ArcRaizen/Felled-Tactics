#include "StdAfx.h"
#include "MenuBox.h"

MenuBox::MenuBox(Level* l, WCHAR* filename, int layer, int width, int height, int posX, int posY) :
	VisualElement(filename, layer, width, height, posX, posY)
{
	level = l;
	mousePosition.x = mousePosition.y = -1;
}


MenuBox::MenuBox(void)
{
	mousePosition.x = mousePosition.y = -1;
}


MenuBox::~MenuBox(void)
{
}


void MenuBox::CreateElement(void (Level::*func)(), WCHAR* filename, int width, int height, int offsetX, int offsetY, const char* t/*=""*/)
{
	elements.push_back(new MenuElement(filename, layer, width, height, 
		leftCorner.x + offsetX, leftCorner.y + offsetY, level, func, t));
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

// Turn off this element drawing by making it invisible
void MenuBox::DisableDraw()
{
	VisualElement::DisableDraw();
	
	for(int i = 0; i < elements.size(); i++)
		elements[i]->DisableDraw();
}

// Restore original visibility to the element
void MenuBox::EnableDraw()
{
	VisualElement::EnableDraw();

	for(int i = 0; i < elements.size(); i++)
		elements[i]->EnableDraw();
}