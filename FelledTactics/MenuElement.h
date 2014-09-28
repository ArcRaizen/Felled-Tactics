#pragma once
#ifndef MENUELEMENT_H
#define MENUELEMENT_H

#ifndef LEVEL_H
//#include "Level.h"
#endif
#ifndef VISAUL_H
#include "VisualElement.h"
#endif;
#ifndef CORE_H
#include "Core.h"
#endif

class Level;
class MenuElement : public VisualElement
{
public:
	MenuElement(WCHAR* filename, int layer, int width, int height, int posX, int posY, Level* l, void (Level::*func)(), const char* t="");
	MenuElement(WCHAR* filename, int layer, int width, int height, int posX, int posY, Level* l, void (Level::*func)(int), int val, const char* t="");
	MenuElement(void);
	virtual ~MenuElement(void);

	bool	UpdateBuffer(float lOffset, float rOffset, float tOffset, float bOffset);
	void	MouseDown();
	void	MouseUp();
	void	MouseOver();
	void	MouseOut();
	void	Activate();
	bool	Draw();

private:
	bool	InitializeBuffer();

protected:
	const char*		text;
	wchar_t			lpcwText[20];
	D3DXCOLOR		fontColor;
	Level*			level;
	int				parameter;			// parameter to pass to activateFunctionParameter
	void (Level::*activateFunction)();
	void (Level::*activateFunctionParameter)(int);

private:
	ID3D10Buffer*	vertexBuffer;
	bool			isDrawn;
};
#endif

