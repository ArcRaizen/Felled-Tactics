#pragma once
#ifndef MENUELEMENT_H
#define MENUELEMENT_H

#ifndef VISAUL_H
#include "VisualElement.h"
#endif;
#ifndef CORE_H
#include "Core.h"
#endif

class Level;
class MenuElement : public VisualElement
{
protected:
	MenuElement(WCHAR* filename, int layer, int width, int height, int posX, int posY, WeakPointer<Level> l, void (Level::*func)(), const char* t="");
	MenuElement(WCHAR* filename, int layer, int width, int height, int posX, int posY, WeakPointer<Level> l, void (Level::*func)(int), int val, const char* t="");

public:
	static SmartPointer<MenuElement> Create(WCHAR* filename, int layer, int width, int height, int posX, int posY, WeakPointer<Level> l, void (Level::*func)(), const char* t="");
	static SmartPointer<MenuElement> Create(WCHAR* filename, int layer, int width, int height, int posX, int posY, WeakPointer<Level> l, void (Level::*func)(int), int val, const char* t="");
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
	const char*			text;
	wchar_t				lpcwText[20];
	D3DXCOLOR			fontColor;
	WeakPointer<Level>	level;
	int					parameter;			// parameter to pass to activateFunctionParameter
	void (Level::*activateFunction)();
	void (Level::*activateFunctionParameter)(int);

private:
	ID3D10Buffer*	vertexBuffer;
	bool			isDrawn;
};

inline SmartPointer<MenuElement> MenuElement::Create(WCHAR* filename, int layer, int width, int height, int posX, int posY, WeakPointer<Level> l, void (Level::*func)(), const char* t)
{
	return new MenuElement(filename, layer, width, height, posX, posY, l, func, t);
}
inline SmartPointer<MenuElement> MenuElement::Create(WCHAR* filename, int layer, int width, int height, int posX, int posY, WeakPointer<Level> l, void (Level::*func)(int), int val, const char* t)
{
	return new MenuElement(filename, layer, width, height, posX, posY, l, func, val, t);
}
typedef SmartPointer<MenuElement> MenuElementPtr;
typedef WeakPointer<MenuElement> MenuElementPtrW;
#endif

