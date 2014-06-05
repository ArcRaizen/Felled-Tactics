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
#ifndef TRAVELNODE_H
#include "TravelNode.h"
#endif
#include "GameTimer.h"

#include <list>
#include <queue>
#pragma endregion

#define TEST_OBSTRUCTION_ALL_UNITS		(1<<1)
#define TEST_OBSTRUCTION_ALLY_UNITS		(1<<2)
#define TEST_OBSTRUCTION_ENEMY_UNITS	(1<<3)

class Tile; // forward declaration
class Level : public GameMaster
{
public:
	Level(int width, int height, int tSize);
	~Level(void);

#pragma region Enums
	enum WinCondition	{Rout, Capture, Defeat, Defend, Collect};	// Possible Conditions for winning a specific level
	enum LoseCondition	{Death, Turn, Surrender};					// Possible Conditions for losing a specific level
	enum Phase			{SelectUnit, SelectMove, ExecuteMove, SelectAction, SelectTarget, ExecuteAction, EnemyTurn};
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
	void	CalcShortestPathAStar(Position start, Position end, int unitMove, list<Position> &path, int options);
	int		CalcPathHeuristic(Position p, Position target, int pathNum, int unitMove);
	int 	CheckListContainsTravelNode(vector<TravelNode*> &list, TravelNode* node);
	void	MarkTiles(bool undo, Position start, int range, int markType, vector<Position> skillRange = vector<Position>());
	bool	MoveUnit(Position start, Position end);
	bool	IsObstructed(Position p);
	bool	IsObstructedPlayer(Position p);
	bool	IsObstructedEnemy(Position p);
	bool	IsValidPosition(Position p);
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
	// Level
	vector<WinCondition>	winConditions;	// List of any/all conditions that must be met to win a level
	vector<LoseCondition>	loseConditions;	// List of any/all conditions that will result in losing a level
	Phase					currentPhase;	// What current action is to be taken (select a unit to move, select what action to take, etc/)
	int						turn;			// Current Turn of the level (Player Phase + Enemy Phase = 1 turn)

	Unit*					boss;
	int						numAllyDeaths;
	int						maximumDeaths;
	int						maximumTurns;

	// Map
	Tile***					map;			// 2D-Array of all tiles that make up this level
	int**					grid;			// array of ints used in determining shortest paths. declared here for simplicity
	Unit***					unitMap;		// 2D-Array of all Units (mirrors map - empty tiles are NULL)
	vector<Unit*>			unitList;		// List of all units
	int						mapWidth;		// Width of map (in tiles)
	int						mapHeight;		// Height of map (in tiles)
	int						tileSize;		// Dimmension of tiles
	Position				movementBeginning;
	Position				selectedTile;	// Tile clicked this frame
	Position				hoveredTile;	// Tile the mouse is currently hovering over
	Position				target;			// Target location of a skill to be cast
	list<Position>			currentMovementPath;
	list<Position>**		movementMap;
	bool					pathDrawEnabled;
};
#endif

