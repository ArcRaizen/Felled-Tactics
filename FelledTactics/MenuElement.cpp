#include "StdAfx.h"
#include "MenuElement.h"


MenuElement::MenuElement(WCHAR* filename, int layer, int width, int height, int posX, int posY, const char* t, Level* l, void (Level::*func)()) :
	VisualElement(filename, layer, width, height, posX, posY)
{
	level = l;
	text = t;
	activateFunction = func;
}

MenuElement::MenuElement(void)
{
}


MenuElement::~MenuElement(void)
{
}

void MenuElement::MouseDown(){ mouseDown = true; }
void MenuElement::MouseUp(){ if(mouseDown) { mouseDown = false; Activate(); } }
void MenuElement::MouseOver(){ mouseEntered = true; }
void MenuElement::MouseOut(){ mouseEntered = false; }

void MenuElement::Activate()
{
	((level)->*(activateFunction))();
}
