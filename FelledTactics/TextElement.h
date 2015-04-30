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
protected:
	TextElement(int layer, int width, int height, Position p, const char* t, D3DXCOLOR color);

public:
	static SmartPointer<TextElement> Create(int layer, int width, int height, Position p, const char* t, D3DXCOLOR color);
	virtual ~TextElement(void);

	int		Update(float dt);
	bool	Draw();

protected:
	const char*		text;
	wchar_t			lpcwText[20];
	D3DXCOLOR		fontColor;
};

inline SmartPointer<TextElement> TextElement::Create(int layer, int width, int height, Position p, const char* t, D3DXCOLOR color)
{
	return new TextElement(layer, width, height, p, t, color);
}
typedef SmartPointer<TextElement> TextElementPtr;
#endif
