#pragma once
#include "VisualElement.h"

class TextElement : public VisualElement
{
public:
	TextElement(int layer, int width, int height, Position p, const char* t, D3DXCOLOR color);
	~TextElement(void);

	void	ChangeAlpha(float alphaChange);
	void	Translate(D3DXVECTOR3 t);
	bool	NoAlpha();

	bool	Draw();

private:
	const char*		text;
	wchar_t			lpcwText[20];
	D3DXCOLOR		fontColor;
	D3DXVECTOR3		trans;
};

