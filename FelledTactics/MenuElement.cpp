#include "StdAfx.h"
#include "MenuElement.h"


MenuElement::MenuElement(WCHAR* filename, int layer, int width, int height, int posX, int posY, const char* t, void (Level::*func)()) :
	VisualElement(filename, layer, width, height, posX, posY)
{
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

/*
bool (Level::*ObstructedFunction)(Position) = &Level::IsObstructed;
if(options & TEST_OBSTRUCTION_ALLY_UNITS)
	ObstructedFunction = &Level::IsObstructedPlayer;
else if(options & TEST_OBSTRUCTION_ENEMY_UNITS)
	ObstructedFunction = &Level::IsObstructedEnemy;

(*this.*ObstructedFunction)(p)
*/