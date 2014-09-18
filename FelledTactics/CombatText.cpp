#include "StdAfx.h"
#include "CombatText.h"

float CombatText::tileSize = 0.0f;
const float	CombatText::ALPHA_CHANGE = 0.5f;
const float	CombatText::LIFE_TIME = 1.0f;
const D3DXCOLOR CombatText::TextColors[5] = {
	D3DXCOLOR(1,0,0,1),	// Fire
	D3DXCOLOR(0,1,0,1), // Lightning
	D3DXCOLOR(0,0,1,1), // Ice
	D3DXCOLOR(1,1,1,1), // Physical / Non-elemental
	D3DXCOLOR(0,0,0,1)  // fuck if I know
};

CombatText::CombatText(int layer, Position p, D3DXVECTOR3 move, const char* t, int damageType) : 
	TextElement(layer, tileSize, tileSize, p, t, TextColors[damageType]), textMove(move)
{

}

CombatText::~CombatText(void)
{}

int CombatText::Update(float dt)
{
	Translate(tileSize/4 * textMove * (dt / LIFE_TIME));
	fontColor.a += ALPHA_CHANGE * (-dt / LIFE_TIME);
	lifeCounter += dt;

	if(lifeCounter >= LIFE_TIME)
		return 2;

	return 0;
}

void CombatText::SetTileSize(float tSize)
{
	tileSize = tSize;
}

Position CombatText::CalcCombatTextPosition(Position target, Position source)
{
	if(target.x < source.x)
		return Position(target.x*tileSize - tileSize/2, target.y*tileSize + tileSize/2);
	return Position(target.x*tileSize + tileSize/2, target.y*tileSize + tileSize/2);
}

D3DXVECTOR3 CombatText::CalcCombatTextMovement(Position target, Position source)
{
	if(target.x < source.x)
		return D3DXVECTOR3(-1, -1, 0);

	return D3DXVECTOR3(1, -1, 0);
}
