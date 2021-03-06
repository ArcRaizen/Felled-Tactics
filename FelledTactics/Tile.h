#pragma once
#ifndef TILE_H
#define TILE_H

#ifndef LEVEL_H
#include "Level.h"
#endif
#ifndef VISUAL_H
#include "VisualElement.h"
#endif
#ifndef LEVELENTITY_H
#include "LevelEntity.h"
#endif

class Level;	// forward declaration
class Tile : public VisualElement, public LevelEntity
{
protected:
	Tile(WCHAR* filename, int layer, int width, int height, int posX, int posY, WeakPointer<Level> l, Position gp);

public:
	static SmartPointer<Tile> Create(WCHAR* filename, int layer, int width, int height, int posX, int posY, WeakPointer<Level> l, Position gp);
	~Tile(void);

	enum class	Status  { Empty, AllyUnit, EnemyUnit, AllyFelled };						// What is on the tile
	enum class	Mark	{ Blank, AllyMove, AllyMovePath, AllyMovePathFail, AllyAbilityRange, AllyAbilityAoE, EnemyMove, Attack };	// What markings the tile has / how is the tile being drawn
	enum class	Effect  { None, Fire, Vector_Plate_Left, Vector_Plate_Right, Vector_Plate_Up, Vector_Plate_Down };

	void	NewTurn(lua_State* L);	// Update Tile for new turn
	void	Clear();				// Tile reset to default state

	void	ActivateMovementEffect(lua_State* L);
	void	ActivateNewTurnEffect(lua_State* L);
	void	ReenableEffect();

	bool	IsObstructed();			// is this spot passable by all units?
	bool	IsObstructedPlayer();	// is this spot passable by a player-owned unit?
	bool	IsObstructedEnemy();	// is this spot passable by an enemy-owned unit?

	void	MouseUp();
	void	MouseOver();
	void	MouseOut();

	void	SetMark(Mark m);
	void	SaveMark();
	void	ResetMark();

	__declspec(property(put=SetStatus, get=GetStatus)) Status TileStatus;	void SetStatus(Status s);		Status GetStatus();
	__declspec(property(get=GetMark))	Mark TileMark;														Mark GetMark();
	__declspec(property(get=GetEffect)) Effect TileEffect;													Effect GetEffect();
	
	void	SetTileEffect(int e, int effectLength);
	bool	HasEffect();

private:
	Position				gridPosition;	// Position of the tile in the levels Tile grid
	Status					status;			// Current occupation status of the tile
	Mark					mark;			// Current mark drawn on the tile
	Mark					prevMark;		// Used for simple resetting when marking/unmarking tiles
	WeakPointer<Level>		level;			// Pointer to the level in which this Tile resides

	Effect		effect;					// Current effect on the tile
	int			effectTimer;			// Timer for how many turns the tile's current Effect lasts
	bool		effectEnabled;			// Is the effect on this tile currently enabled?
	std::string	effectMovementScript;	// Script file to run for this tile's effect (if it has one) when a unit moves over it
	std::string effectNewTurnScript;	// Script file to run for this tile's effect (if it has one) when a unit begins a new turn on it
	
	std::wstring defaultTexturePath;	// File path for the tile's base texture

	// Highlight Color Marks (for each Mark)
	static D3DXVECTOR4 highlightAllyMove;
	static D3DXVECTOR4 highlightAllyMoveAlly;
	static D3DXVECTOR4 highlightAllySkillRange;
	static D3DXVECTOR4 highlightAllySkillAoE;
	static D3DXVECTOR4 highlightEnemyMove;
	static D3DXVECTOR4 highlightAttack;
};

inline SmartPointer<Tile> Tile::Create(WCHAR* filename, int layer, int width, int height, int posX, int posY, WeakPointer<Level> l, Position gp)
{
	return new Tile(filename, layer, width, height, posX, posY, l, gp);
}
typedef SmartPointer<Tile> TilePtr;
typedef WeakPointer<Tile> TilePtrW;
#endif

