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
#ifndef COMBATMANAGER_H
#include "CombatManager.h"
#endif
#ifndef COMBATEXT_H
#include "CombatText.h"
#endif
#ifndef TRAVELNODE_H
#include "TravelNode.h"
#endif
#ifndef GAMETIMER_H
#include "GameTimer.h"
#endif

#include <list>
#include <queue>
#include <iostream>
#include <fstream>
#pragma endregion

// Movement Options
#define TEST_OBSTRUCTION_ALL_UNITS		(1<<1)
#define TEST_OBSTRUCTION_ALLY_UNITS		(1<<2)
#define TEST_OBSTRUCTION_ENEMY_UNITS	(1<<3)

// Tile Mark Types
#define TILE_MARK_TYPE_MOVEMENT					 (1<<4)
#define TILE_MARK_TYPE_ATTACK					 (1<<5)
#define TILE_MARK_TYPE_ALLY_ABILITY_RANGE		 (1<<6)
#define TILE_MARK_TYPE_ALLY_BATTLE_ABILITY_RANGE (1<<7)
#define TILE_MARK_TYPE_ALLY_ABILITY_AOE			 (1<<8)
#define TILE_MARK_TYPE_ENEMY_MOVEMENT			 (1<<9)

// Win Conditions
#define WIN_CONDITION_ELIMINATE  (1<<10)	// Kill all enemies on the map
#define WIN_CONDITION_ROUT		 (1<<11)	// Kill a specific amount of enemies
#define WIN_CONDITION_CAPTURE	 (1<<12)	// Capture a specific point on the map
#define WIN_CONDITION_DEFEAT	 (1<<13)	// Defeat a specific enemy on the map
#define WIN_CONDITION_DEFEND	 (1<<14)	// Defend a specific point for a certain number of turns
#define WIN_CONDITION_COLLECT	 (1<<15)	// Find, Collect, and Retrieve tokens somewhere on the map

// Lose Conditions
#define LOSE_CONDITION_DEATH	 (1<<16)	// Too many ally units were felled
#define LOSE_CONDITION_TURN		 (1<<17)	// Too many turns have passed
#define LOSE_CONDITION_SURRENDER (1<<18)	// Specific point on map was captured by enemy units
#define LOSE_CONDITION_SURVIVE   (1<<19)	// Specific ally unit was felled

class Tile; // forward declaration
//class MenuBox;
class Level : public GameMaster
{
public:
	Level(lua_State* luaState, int width, int height, int tSize);
	~Level(void);

#pragma region Enums
	enum Phase			{SelectUnit, SelectMove, ExecuteMove, SelectPrimaryAction, SelectSecondaryAction, SelectTarget, SelectAbilityTarget, ExecuteAttack, ExecuteAbility, EnemyTurn};
#pragma endregion

	int		Update(float dt, HWND hWnd);
	void	Draw();
private:
	void	HandleRightClick();


#pragma region Level Functions
public:
	void	GenerateLevel();
private:
	void	StartNewTurn();
	bool	CheckWin(int cond);
	bool	CheckLoss(int cond);
#pragma endregion

#pragma region Map Functions
private:
	void	PlayerDrawPath();
	void	AutoDrawPath();
	void	CalcShortestPathAStar(Position start, Position end, int unitMove, list<Position> &path, int options);
	int		CalcPathHeuristic(Position p, Position target, int pathNum, int unitMove);
	int 	CheckListContainsTravelNode(vector<TravelNode*> &list, TravelNode* node);
	void	MarkTiles(bool undo, Position start, int range, int markType, vector<Position> skillRange = vector<Position>());

private:
	void	CreateActionMenu();
	void	SelectAttack();
	void	SelectAbility();
	void	SelectItem();
	void	ActivateEndTurn();
	void	CreateCombatUI();
	void	ActivateAbility(int selectedAbility);

	bool	IsObstructed(Position p);
	bool	IsObstructedPlayer(Position p);
	bool	IsObstructedEnemy(Position p);
public:
	void	CreateCombatText(Position target, Position source, const char* t, int damageType);
	void	SetSelectedTile(Position p);
	void	SetHoveredTile(Position p);
#pragma endregion 

#pragma region Properties
public:
	__declspec(property(get=GetMap)) Tile*** Map;											Tile*** GetMap();
	__declspec(property(get=GetUnits)) Unit*** Units;										Unit*** GetUnits();
	__declspec(property(put=SetWidth, get=GetWidth)) int Width;		void SetWidth(int w);	int GetWidth();
	__declspec(property(put=SetHeight, get=GetHeight)) int Height;	void SetHeight(int h);	int GetHeight();

	Tile* GetTile(int x, int y);
	Unit* GetUnit(int x, int y); Unit* GetEnemyUnit(int x, int y); Unit* GetAllyUnit(int x, int y);
	bool  IsOccupied(int x, int y);
#pragma endregion

private:
	// ~~~~ Level ~~~~
	int						winConditions;		// List of any/all conditions that must be met to win a level
	int						loseConditions;		// List of any/all conditions that will result in losing a level
	int						winConditionCheck;	// 
	Phase					currentPhase;		// What current action is to be taken (select a unit to move, select what action to take, etc/)
	int						turn;				// Current Turn of the level (Player Phase + Enemy Phase = 1 turn)

	Unit*					boss;				// Enemy unit that must be killed to win level
	Unit*					survivor;			// Ally unit that must survive to not lose level
	Tile*					targetTile;			// Tile that must be capture by ally units to win level
	Tile*					defenseTile;		// Tile that must not be captured by enemy units to lose level
	int						targetEnemyDeaths;	// Number of enemy units that must be killed to win level 
	int						maximumDeaths;		// Maximum number of ally deaths allowed before losing level
	int						maximumTurns;		// Maximum number of turns allowed to pass before for this level
	CombatManager			combatManager;

	int						numEnemyDeaths;		// Total number of enemy deaths in this level
	int						numAllyDeaths;		// Total number of ally deaths in this level
	int						numAllies;			// Total number of allies in this level
	int						numEnemies;			// Total number of enemies in this level
	int						numUnitsMoved;		// Number of ally units who have moved/taken action this turn


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
	vector<Position>		activatedTiles;		// List of tiles whose effects were activated during a single movement phase

	// Actions
	MenuBox*				actionMenu;				// Menu for selecting action for unit to take
	MenuBox*				secondaryMenu;			// Menu for selecting skill/item for unit to use
	Position				currentUnitPosition;	// Location of unit selected to take an action
	Position				target;					// Target location of a skill to be cast

	// Combat Text
	vector<TextElement*>	combatText;				// list of text to print to screen to show combat results

#pragma region Utility Functions
private:
	inline bool	IsValidPosition(Position p) { if(p.x < 0 || p.y < 0) { return false; } if(p.x >= mapWidth || p.y >= mapWidth) { return false; } return true; }
	inline bool IsValidPosition(int x, int y) { if (x < 0 || y < 0) { return false; } if(y >= mapWidth || y >= mapWidth) { return false; } return true; }
	inline bool IsValidUnit(Position p) { if(!IsValidPosition(p) || unitMap[p.x][p.y] == NULL) { return false; } return true; }
	inline bool IsValidUnit(int x, int y) { if(!IsValidPosition(x,y) || unitMap[x][y] == NULL) { return false; } return true; }
#pragma endregion
};
#endif

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    