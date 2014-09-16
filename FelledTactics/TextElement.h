#pragma once
#ifndef TEXTELEMENT_H
#define TEXTELEMENT_H

#ifndef VISUAL_H
#include "VisualElement.h"
#endif
#ifndef CORE_H
#include "Core.h"
#endif

class TextElement : public VisualElement
{
public:
	TextElement(int layer, int width, int height, Position p, const char* t, D3DXCOLOR color, float life, D3DXVECTOR3 move, float alpha);
	~TextElement(void);

	void	Translate(D3DXVECTOR3 t);

	int		Update(float dt);
	bool	Draw();

private:
	const char*		text;
	wchar_t			lpcwText[20];
	D3DXCOLOR		fontColor;
	D3DXVECTOR3		trans;

	D3DXVECTOR3		textMove;
	float			alphaChange;
	float			lifeTime;
	float			lifeCounter;
};
#endif
