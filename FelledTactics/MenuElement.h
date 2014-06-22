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
	MenuElement(void);
	~MenuElement(void);

	void	MouseDown();
	void	MouseUp();
	void	MouseOver();
	void	MouseOut();
	void	Activate();

protected:
	const char*		text;
	Level*			level;
	void (Level::*activateFunction)();
};
#endif

