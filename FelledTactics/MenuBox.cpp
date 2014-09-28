#include "StdAfx.h"
#include "MenuBox.h"

MenuBox::MenuBox(Level* l, WCHAR* filename, int layer, int width, int height, int posX, int posY, int ew, int eh) :
	VisualElement(filename, layer, width, height, posX, posY), numColumns(ew), numRows(eh)
{
	level = l;
	mousePosition.x = mousePosition.y = -1;
	offsetX = offsetY = width * 0.1f;
	entryWidth = (width - (offsetX * 2)) / numColumns;
	entryHeight = (height - (offsetY * 2)) / numRows;
}


MenuBox::MenuBox(void)
{
	mousePosition.x = mousePosition.y = -1;
}


MenuBox::~MenuBox(void)
{
	level = NULL;
	for(int i = 0; i < elements.size(); i++)
	{
		delete elements[i];
		elements[i] = NULL;
	}

	elements.clear();
}


void MenuBox::CreateElement(void (Level::*func)(), WCHAR* filename, const char* t/*=""*/)
{
	elements.push_back(new MenuElement(filename, layer,											// texture path, VisualElement layer
		entryWidth, entryHeight,																// width, height
		leftCorner.x + offsetX + (elements.size() % numColumns * entryWidth),					// posX
		leftCorner.y + Height() - offsetY - (((elements.size() / numColumns) + 1) * entryHeight),	// posY (+1 because posY refers to bottom corner not top)
		level, func, t));																		// callback pointer, callback function pointer, text
}

void MenuBox::CreateElement(void (Level::*func)(int), int val, WCHAR* filename, const char* t/*=""*/)
{
	elements.push_back(new MenuElement(filename, layer,											// texture path, VisualElement layer
		entryWidth, entryHeight,																// width, height
		leftCorner.x + offsetX + (elements.size() % numColumns * entryWidth),					// posX
		leftCorner.y + Height() - offsetY - (((elements.size() / numColumns) + 1) * entryHeight), // posY (+1 because posY refers to bottom corner not top)
		level, func,																			// callback pointer, callback function pointer, 
		val, t));																				// callback function parameter, text
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

int MenuBox::Update(float dt)
{
	float left, right, top, bottom;
	RECT temp;
	for(int i = 0; i < elements.size(); i++)
	{
		temp = elements[i]->GetRect();
		if(RectCollide(temp))
		{
			left = rect.left > temp.left ? rect.left - temp.left : 0.0f;
			right = rect.right < temp.right ? temp.right - rect.right : 0.0f;
			top = rect.top > temp.top ? rect.top - temp.top : 0.0f;
			bottom = rect.bottom < temp.bottom ? temp.bottom - rect.bottom : 0.0f;
			elements[i]->UpdateBuffer(left, right, top, bottom);
		}
		else
			elements[i]->DisableDraw();
	}

	return 1;
}

bool MenuBox::Draw()
{
	VisualElement::Draw();

	for(int i = 0; i < elements.size(); i++)
	{
		if(RectCollide(elements[i]->GetRect()))
			elements[i]->Draw();
	}

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