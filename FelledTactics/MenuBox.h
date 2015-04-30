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

//class Level;	// Forward declared in included file "MenuElement.h"
class MenuBox : public VisualElement
{
protected:
	MenuBox(SmartPointer<Level> l, WCHAR* filename, int layer, int width, int height, int posX, int posY, int ew, int eh);

public:
	static SmartPointer<MenuBox> Create(SmartPointer<Level> l, WCHAR* filename, int layer, int width, int height, int posX, int posY, int ew, int eh);
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

	vector<SmartPointer<MenuElement>>	elements;
	D3DXVECTOR3							mousePosition;
	SmartPointer<Level>					level;
};

inline SmartPointer<MenuBox> MenuBox::Create(SmartPointer<Level> l, WCHAR* filename, int layer, int width, int height, int posX, int posY, int ew, int eh)
{
	return new MenuBox(l, filename, layer, width, height, posX, posY, ew, eh);
}
typedef SmartPointer<MenuBox> MenuBoxPtr;
#endif

