                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            #pragma once
#ifndef LEVEL_H
#define LEVEL_H

#pragma region Includes
#ifndef GAMEMASTER_H
#include "GameMaster.h"
#endif
#ifndef TILE_H
#include "Tile.h"
#endif
#ifndef UNIT_H
#include "Unit.h"
#endif
#ifndef MENU_H
#include "MenuBox.h"
#endif
#ifndef COMBATCAL_H
#include "CombatCalculator.h"
#endif
#ifndef TRAVELNODE_H
#include "TravelNode.h"
#endif
#ifndef GAMETIMER_H
#include "GameTimer.h"
#endif

#include <list>
#include <queue>
#pragma endregion

#define TEST_OBSTRUCTION_ALL_UNITS		(1<<1)
#define TEST_OBSTRUCTION_ALLY_UNITS		(1<<2)
#define TEST_OBSTRUCTION_ENEMY_UNITS	(1<<3)

class Tile; // forward declaration
//class MenuBox;
class Level : public GameMaster
{
public:
	Level(int width, int height, int tSize);
	~Level(void);

#pragma region Enums
	enum WinCondition	{Rout, Capture, Defeat, Defend, Collect};	// Possible Conditions for winning a specific level
	enum LoseCondition	{Death, Turn, Surrender};					// Possible Conditions for losing a specific level
	enum Phase			{SelectUnit, SelectMove, ExecuteMove, SelectPrimaryAction, SelectSecondaryAction, SelectTarget, SelectSkillTarget, ExecuteAction, EnemyTurn};
#pragma endregion

	int		Update(float dt, HWND hWnd);
	void	Draw();
private:
	void	HandleRightClick();


#pragma region Level Functions
public:
	void	GenerateLevel();
private:
	bool	CheckWin();
	bool	CheckLoss();
#pragma endregion

#pragma region Map Functions
private:
	void	PlayerDrawPath();
	void	AutoDrawPath();
	void	CalcShortestPathAStar(Position start, Position end, int unitMove, list<Position> &path, int options);
	int		CalcPathHeuristic(Position p, Position target, int pathNum, int unitMove);
	int 	CheckListContainsTravelNode(vector<TravelNode*> &list, TravelNode* node);
	void	MarkTiles(bool undo, Position start, int range, int markType, vector<Position> skillRange = vector<Position>());
	bool	DoMovementEnd(Position start, Position end);
	void	CreateActionMenu();
	void	ActivateAttack();
	void	ActivateSkill();
	void	ActivateItem();
	void	ActivateEndTurn();
	void	CreateCombatUI();
	bool	IsObstructed(Position p);
	bool	IsObstructedPlayer(Position p);
	bool	IsObstructedEnemy(Position p);
public:
	void	SetSelectedTile(Position p);
	void	SetHoveredTile(Position p);
#pragma endregion 

#pragma region Properties
public:
	__declspec(property(get=GetMap)) Tile*** Map;											Tile*** GetMap();
	__declspec(property(get=GetUnits)) Unit*** Units;										Unit*** GetUnits();
	__declspec(property(put=SetWidth, get=GetWidth)) int Width;		void SetWidth(int w);	int GetWidth();
	__declspec(property(put=SetHeight, get=GetHeight)) int Height;	void SetHeight(int h);	int GetHeight();
#pragma endregion

private:
	// ~~~~ Level ~~~~
	vector<WinCondition>	winConditions;	// List of any/all conditions that must be met to win a level
	vector<LoseCondition>	loseConditions;	// List of any/all conditions that will result in losing a level
	Phase					currentPhase;	// What current action is to be taken (select a unit to move, select what action to take, etc/)
	int						turn;			// Current Turn of the level (Player Phase + Enemy Phase = 1 turn)

	Unit*					boss;
	int						numAllyDeaths;
	int						maximumDeaths;
	int						maximumTurns;
	CombatCalculator*		combatCalculator;

	// ~~~~ Map ~~~~
	Tile***					map;			// 2D-Array of all tiles that make up this level
	Unit***					unitMap;		// 2D-Array of all Units (mirrors map - empty tiles are NULL)
	vector<Unit*>			unitList;		// List of all units
	int						mapWidth;		// Width of map (in tiles)
	int						mapHeight;		// Height of map (in tiles)
	int						tileSize;		// Dimmension of tiles

	// Movement
	Position				movementBeginning;	// Location of unit selected for movement
	Position				selectedTile;		// Tile clicked this frame
	Position				hoveredTile;		// Tile the mouse is currently hovering over
	Position				lastSelectedTile, lastHoveredTile;
	list<Position>			currentMovementPath;// Path currently developed for selected unit to follow for movement
	list<Position>**		movementMap;		// Map of paths to surrounding tiles for unit selected for movement
	bool					pathDrawEnabled;	// Is the player drawing a path for their selected unit to follow?

	// Actions
	MenuBox*				actionMenu;		// Menu for selecting action for unit to take
	MenuBox*				secondaryMenu;	// Menu for selecting skill/item for unit to use
	Position				actionBeginning;// Location of unit selected to take an action
	Position				target;			// Target location of a skill to be cast

#pragma region Utility Functions
private:
	inline bool	IsValidPosition(Position p) { if(p.x < 0 || p.y < 0) { return false; } if(p.x > mapWidth || p.y > mapWidth) { return false;	} return true; }
#pragma endregion
};
#endif

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    