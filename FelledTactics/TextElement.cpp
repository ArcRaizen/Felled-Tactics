#include "StdAfx.h"
#include "TextElement.h"


TextElement::TextElement(int layer, int width, int height, Position p, const char* t, D3DXCOLOR color) : 
	VisualElement(L"", layer, width, height, p.x, p.y)
{
	text = t;
	mbstowcs(lpcwText, t, 20);
	fontColor = color;
	trans.x = trans.y = trans.z = 0.0f;
}


TextElement::~TextElement(void)
{
}

void TextElement::ChangeAlpha(float alphaChange)
{
	fontColor.a += alphaChange;
}

void TextElement::Translate(D3DXVECTOR3 t)
{
	trans += t;
	D3DXMATRIX translate;
	D3DXMatrixTranslation(&translate, t.x, t.y, t.z);
	world *= translate;

	rect.left = leftCorner.x + trans.x;
	rect.right = rect.left + width;
	rect.bottom = screenHeight - leftCorner.y + trans.y;
	rect.top = rect.bottom - height;
}

bool TextElement::NoAlpha()
{
	return fontColor.a <= 0.0f;
}

bool TextElement::Draw()
{
	Direct3D::GetFont()->DrawText(NULL, lpcwText, -1, &rect, DT_CENTER | DT_VCENTER, fontColor);
	return true;
}
