#include "StdAfx.h"
#include "TextElement.h"


TextElement::TextElement(int layer, int width, int height, Position p, const char* t, D3DXCOLOR color) : 
	VisualElement(L"", layer, width, height, p.x, p.y), text(t), fontColor(color)
{
	mbstowcs(lpcwText, t, 20);
}


TextElement::~TextElement(void) {}

int TextElement::Update(float dt)
{
	return 1;
}

bool TextElement::Draw()
{
	Direct3D::GetFont()->DrawText(NULL, lpcwText, -1, &rect, DT_CENTER | DT_VCENTER, fontColor);
	Direct3D::DrawTextReset();
	return true;
}
