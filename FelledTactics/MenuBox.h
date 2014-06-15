#pragma once
#ifndef MENU_H
#define MENU_H

#ifndef MENUELEMENT_H
#include "MenuElement.h"
#endif
#ifndef VISUAL_H
#include "VisualElement.h"
#endif

#include <vector>

//class Level;
class MenuBox : public VisualElement
{
public:
	MenuBox(Level* l, WCHAR* filename, int layer, int width, int height, int posX, int posY);
	MenuBox(void);
	~MenuBox(void);

	void	CreateElement(void (Level::*func)(), WCHAR* filename, int width, int height, int offsetX, int offsetY, const char* t);
	
	// MenuBox manages all Mouse Events for its Elements/components, but has no MouseEvents of its own
	void	SetCurrentMousePosition(D3DXVECTOR3 mousePos);
	void	MouseDown();
	void	MouseUp();

	//int		Update(float dt);
	bool	Draw();
	void	DisableDraw();
	void	EnableDraw();

private:
	vector<MenuElement*>	elements;
	D3DXVECTOR3				mousePosition;
	Level* level;
};
#endif

