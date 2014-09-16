#include "StdAfx.h"
#include "TextElement.h"


TextElement::TextElement(int layer, int width, int height, Position p, const char* t, D3DXCOLOR color, float life, D3DXVECTOR3 move, float alpha) : 
	VisualElement(L"", layer, width, height, p.x, p.y), text(t), fontColor(color), lifeCounter(0), textMove(move), lifeTime(life), alphaChange(alpha)
{
	mbstowcs(lpcwText, t, 20);
	trans.x = trans.y = trans.z = 0.0f;
}


TextElement::~TextElement(void)
{
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

int TextElement::Update(float dt)
{
	Translate(textMove * (dt / lifeTime));
	fontColor.a += alphaChange * (-dt / lifeTime); 
	lifeCounter += dt;

	if(lifeCounter >= lifeTime)
		return 2;

	return 0;
}

bool TextElement::Draw()
{
	Direct3D::GetFont()->DrawText(NULL, lpcwText, -1, &rect, DT_CENTER | DT_VCENTER, fontColor);
	Direct3D::DrawTextReset();
	return true;
}
