#pragma once
#ifndef COMBATTEXT_H
#define COMBATTEXT_H

#ifndef TEXTELEMENT_H
#include "TextElement.h"
#endif
#ifndef VISUAL_H
#include "VisualElement.h"
#endif

class CombatText : public TextElement
{
public:
	CombatText(int layer, Position p, D3DXVECTOR3 move, const char* t, int damageType);
	~CombatText(void);

	static void			SetTileSize(float tSize);
	static Position		CalcCombatTextPosition(Position target, Position source);
	static D3DXVECTOR3	CalcCombatTextMovement(Position target, Position source);

	int		Update(float dt);

private:
	D3DXVECTOR3 textMove;
	float		lifeCounter;

	static float			tileSize;
	static const float		ALPHA_CHANGE;
	static const float		LIFE_TIME;
	static const D3DXCOLOR	TextColors[5];
};
#endif

