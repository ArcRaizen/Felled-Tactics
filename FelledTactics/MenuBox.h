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
	MenuBox(Level* l, WCHAR* filename, int layer, int width, int height, int posX, int posY, int ew, int eh);
	MenuBox(void);
	virtual ~MenuBox(void);

	void	CreateElement(void (Level::*func)(), WCHAR* filename, const char* t="");
	void	CreateElement(void (Level::*func)(int), int val, WCHAR* filename, const char* t="");
	
	// MenuBox manages all Mouse Events for its Elements/components, but has no MouseEvents of its own
	void	SetCurrentMousePosition(D3DXVECTOR3 mousePos);
	void	MouseDown();
	void	MouseUp();

	int		Update(float dt);
	bool	Draw();
	void	DisableDraw();
	void	EnableDraw();

private:
	int		numColumns;					// number of elements that can be shown horizontally
	int		numRows;					// number of elements than can be shown vertically

	float	offsetX, offsetY;			// offset from border for entry position
	float	entryWidth, entryHeight;	// dimensions of each entry based on row and col

	vector<MenuElement*>	elements;
	D3DXVECTOR3				mousePosition;
	Level* level;
};
#endif

