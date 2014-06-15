#pragma once
#ifndef TILE_H
#define TILE_H

#ifndef LEVEL_H
#include "Level.h"
#endif
#ifndef VISUAL_H
#include "VisualElement.h"
#endif

class Level;	// forward declaration
class Tile : public VisualElement
{
public:
	Tile(WCHAR* filename, int layer, int width, int height, int posX, int posY, Level* l, Position gp);
	~Tile(void);

	enum	Status  { Empty, AllyUnit, EnemyUnit, AllyFelled };						// What is on the tile
	enum	Mark	{ None, AllyMove, AllyMovePath, AllyMovePathFail, AllySkillRange, AllySkillAoE, EnemyMove, Attack };	// What markings the tile has / how is the tile being drawn

	bool	IsObstructed();			// is this spot passable by all units?
	bool	IsObstructedPlayer();	// is this spot passable by a player-owned unit?
	bool	IsObstructedEnemy();	// is this spot passable by an enemy-owned unit?

	void	MouseUp();
	void	MouseOver();
	void	MouseOut();

	__declspec(property(put=SetStatus, get=GetStatus)) Status TileStatus;				void SetStatus(Status s);		Status GetStatus();
	__declspec(property(put=SetMark, get=GetMark))	Mark TileMark;						void SetMark(Mark m);			Mark GetMark();
	__declspec(property(put=SetPrevMark, get=GetPrevMark)) Mark PrevTileMark;			void SetPrevMark(Mark m);		Mark GetPrevMark();

private:
	Position	gridPosition;
	Status		status;
	Mark		mark;
	Mark		prevMark;	// Used for simple resetting when marking/unmarking tiles
	Level*		level;

	// Highlight Color Effects (for each Mark)
	static D3DXVECTOR4 highlightNone;
	static D3DXVECTOR4 highlightAllyMove;
	static D3DXVECTOR4 highlightAllyMoveAlly;
	static D3DXVECTOR4 highlightAllySkillRange;
	static D3DXVECTOR4 highlightAllySkillAoE;
	static D3DXVECTOR4 highlightEnemyMove;
	static D3DXVECTOR4 highlightAttack;
};
#endif

