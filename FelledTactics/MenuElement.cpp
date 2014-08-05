#include "StdAfx.h"
#include "MenuElement.h"


MenuElement::MenuElement(WCHAR* filename, int layer, int width, int height, int posX, int posY, Level* l, void (Level::*func)(), const char* t/*=""*/) :
	VisualElement(filename, layer, width, height, posX, posY)
{
	level = l;
	text = t;
	mbstowcs(lpcwText, t, 20);
	activateFunction = func;
	fontColor.r = fontColor.g = fontColor.b = 0.0f;
	fontColor.a = 1.0f;
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
	if(level != NULL)
		((level)->*(activateFunction))();
}

bool MenuElement::Draw()
{
	VisualElement::Draw();

	// If we have unique text to draw, do so
	if(strcmp(text, ""))
		Direct3D::GetFont()->DrawText(NULL, lpcwText, -1, &rect, DT_CENTER | DT_VCENTER, fontColor);

	return true;
}
