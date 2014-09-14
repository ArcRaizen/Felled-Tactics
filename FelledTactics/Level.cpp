#include "StdAfx.h"
#include "Level.h"


Level::Level(lua_State* luaState, int width, int height, int tSize = 50) : mapWidth(width), mapHeight(height), tileSize(tSize), GameMaster(luaState)
{
	turn = 0;
	numEnemyDeaths = numAllyDeaths = 0;
	numAllies = numEnemies = 0;
	numUnitsMoved = 0;
	currentPhase = SelectUnit;
	selectedTile.x = selectedTile.y = -1;
	pathDrawEnabled = false;

	GenerateLevel();
}


Level::~Level(void)
{
	for(int i = 0; i < mapWidth; i++)
	{
		for(int j = 0; j < mapHeight; j++)
		{
			delete map[i][j];
			delete unitMap[i][j];
		}

		delete[] map[i];
		delete[] unitMap[i];
		delete[] movementMap[i];
	}

	delete[] map;
	delete[] unitMap;
	delete[] movementMap;

	delete boss;
	delete actionMenu;
	delete secondaryMenu;
}

int Level::Update(float dt, HWND hWnd)
{
	GameMaster::Update(dt, hWnd);
	if(InputManager::RightMouseUpEvent())
		HandleRightClick();

#pragma region Unit Update
	// Update all units on the map
	int result = -1;
	for(int i = 0; i < unitList.size(); i++)
	{
		result = unitList[i]->Update(dt);

		// Move unit on map - If during movement it enters an active hazard, we resolve that now
		if(result == 1)
		{
			bool unitMoved = DoMovementEnd(movementBeginning, currentMovementPath.back());
			selectedTile.x = -1;
		}
		else if(result == 2)
		{
			// Leave felled-ally unit's body of possible resurrection
			if(unitList[i]->IsAlly())
			{
				map[unitList[i]->UnitPosition.x][unitList[i]->UnitPosition.y]->TileStatus = Tile::Status::AllyFelled;
				numAllyDeaths++;
			}
			else	// Remove dead enemy from the level permanently
			{
				unitList[i]->Delete();
				map[unitList[i]->UnitPosition.x][unitList[i]->UnitPosition.y]->TileStatus = Tile::Status::Empty;
				unitMap[unitList[i]->UnitPosition.x][unitList[i]->UnitPosition.y] = NULL;
				unitList.erase(unitList.begin() + i--);
				numEnemyDeaths++;
			}

			// Allow user to interact again now that gameplay has restarted
			RestoreUserInput();
		}
	}
#pragma endregion

#pragma region Phase Update
	// Do different updates depending on the current phase
	switch(currentPhase)
	{
/**/	case Phase::SelectUnit:
			// Turn on/off HP/AP bars on hovered units
			if(lastHoveredTile != hoveredTile)
			{	// Turn off drawing HP/AP bars on old tile
				if(unitMap[lastHoveredTile.x][lastHoveredTile.y] != NULL)
					unitMap[lastHoveredTile.x][lastHoveredTile.y]->DrawBars = false;
	
				// Turn on drawing HP/AP bars on new tile
				if(unitMap[hoveredTile.x][hoveredTile.y] != NULL)
					unitMap[hoveredTile.x][hoveredTile.y]->DrawBars = true;
			}

			// If no tile is selected yet, we have nothing more to do
			if(selectedTile.x == -1)	break;

			if(map[selectedTile.x][selectedTile.y]->TileStatus == Tile::Status::Empty ||
				map[selectedTile.x][selectedTile.y]->TileStatus == Tile::Status::AllyFelled)
			{ 
				/*Do options menu or nothing*/ 
				break; 
			}
			else if(map[selectedTile.x][selectedTile.y]->TileStatus == Tile::Status::EnemyUnit)
			{
				/* Do Enemy Unit info Display */ 
				/* Line intentionally left blank */
			}
			else // Ally Unit - Initiate Movement
			{
				if(unitMap[selectedTile.x][selectedTile.y]->FinishedTurn)
				{ 
					/*Do options menu or nothing*/ 
					break; 
				}

				movementBeginning = selectedTile;
				MarkTiles(false, movementBeginning, unitMap[movementBeginning.x][movementBeginning.y]->Movement, 0);
				unitMap[selectedTile.x][selectedTile.y]->DrawBars = false;

				selectedTile.x = -1;
				currentPhase = SelectMove; // Jump to next phase
			}

			break;
/**/	case Phase::SelectMove:
			// Determine if the user is starting to try to draw a movement path for one of his units
			if(!pathDrawEnabled && map[movementBeginning.x][movementBeginning.y]->IsMouseDown)
			{
				currentMovementPath.clear();						// Clear last player-drawn movement path
				currentMovementPath.push_back(movementBeginning);	// Start the new one
				pathDrawEnabled = true;
			}

			// Update path once mouse moves to hover over new tile
			if(lastHoveredTile != hoveredTile)
			{
				if(pathDrawEnabled)		// Player drawing own path?
					PlayerDrawPath();
				else					// or not?
					AutoDrawPath();
			}

			// If no tile is selected yet, we have nothing more to do
			if(selectedTile.x == -1)	break;

			// Tile selected for movement
			// Selected Tile is occupied by ally unit or the path to Selected Tile is invalid (too long)
			if(selectedTile != movementBeginning && (map[selectedTile.x][selectedTile.y]->TileMark == Tile::Mark::AllyMovePathFail || 
				map[selectedTile.x][selectedTile.y]->TileStatus == Tile::Status::AllyUnit))
			{
				// Reset player drawn movement path
				std::list<Position>::const_iterator i;
				if(currentMovementPath.size() > 0)
				{
					i = currentMovementPath.begin();
					i++;
					for(; i != currentMovementPath.end(); i++)
						map[i->x][i->y]->TileMark = Tile::Mark::AllyMove;
				}

				// Activate pre-calculated movement path
				currentMovementPath.clear();
				currentMovementPath = movementMap[selectedTile.x][selectedTile.y];
				selectedTile = currentMovementPath.front();
				i = currentMovementPath.begin();
				i++;
				for(; i != currentMovementPath.end(); i++)
					map[i->x][i->y]->TileMark = Tile::Mark::AllyMovePath;
			} // Selected Tile is valid. Proceed with movement
			else if(map[selectedTile.x][selectedTile.y]->TileMark == Tile::Mark::AllyMovePath)
			{
				// Unmark movement tiles
				MarkTiles(true, movementBeginning, unitMap[movementBeginning.x][movementBeginning.y]->Movement, 0);

				// No movement for this Unit
				if(selectedTile == movementBeginning)
				{
#ifdef ALLOW_ZERO_TILE_MOVEMENT
					DoMovementEnd(movementBeginning, movementBeginning);
#else
					currentPhase = SelectUnit;
#endif
					selectedTile.x = -1;
					break;
				}
		
				// Tell the unit to move
				unitMap[movementBeginning.x][movementBeginning.y]->SetMovePath(currentMovementPath);
				currentPhase = ExecuteMove;	// Go to next phase
			}
			break;
//		case Phase::ExecuteMove: 
/**/	case Phase::SelectPrimaryAction:
			break;
/**/	case Phase::SelectSecondaryAction:
			break;
/**/	case Phase::SelectTarget:
			if(lastHoveredTile != hoveredTile)
			{
				if(map[hoveredTile.x][hoveredTile.y]->TileMark == Tile::Mark::Attack)
				{
					target = hoveredTile;
					combatCalculator.Defender = unitMap[target.x][target.y];
					combatCalculator.CalculateCombat();

					// CREATE COMBAT RESULTS UI
				}
				else
				{
					combatCalculator.ResetDefender();
					// REMOVE COMBAT AI
				}
			}

			// If no tile is selected yet, we have nothing more to do
			if(selectedTile.x == -1)	break;

			// Selected valid target - FIGHT!
			if(map[selectedTile.x][selectedTile.y]->TileMark == Tile::Mark::Attack)
			{
				combatCalculator.SetCombatModifiers(1.0f, 1.0f, 1, 1);
				combatCalculator.DoCombat();

				// Unmark all enemies in range of unit
				MarkTiles(true, currentUnitPosition, unitMap[currentUnitPosition.x][currentUnitPosition.y]->AttackRange, 1);

				PauseUserInputIndefinite();
				currentPhase = ExecuteAction;
			}
			break;
/**/	case Phase::SelectSkillTarget:
			if(selectedTile.x == -1)	break;
			MarkTiles(false, hoveredTile, 0 /*get range of skill selected*/, 2 /*get aoe of skill selected*/);

			if(map[selectedTile.x][selectedTile.y]->TileMark == Tile::Mark::AllySkillRange)
			{
				target = selectedTile;
				currentPhase = ExecuteAction;
			}
			break;
/**/	case Phase::ExecuteAction:
		{	
			int combatResult; combatResult = combatCalculator.Update(dt);		

			switch(combatResult)
			{
				case 0: break;	// combat still occuring
				case 3:			// no death after combat finished
					RestoreUserInput();
					ActivateEndTurn();
					break;
				default:		// combatant has been killed
					Position deathPosition = (combatResult == 1 ? currentUnitPosition : target);	// combatResult == 1 -> attacker died
																								// combatResult == 2 -> defender died
					// Activate death animation of defeated unit and pause user input until it completes
					PauseUserInput(unitMap[deathPosition.x][deathPosition.y]->Die());
					ActivateEndTurn();
					break;
			}

			break;
		}
/**/	case Phase::EnemyTurn:
			StartNewTurn();
			if(selectedTile.x == -1)	break;
			break;
	}
#pragma endregion

	lastSelectedTile = selectedTile;
	lastHoveredTile = hoveredTile;
	selectedTile.Reset();
	return 1;
}

void Level::HandleRightClick()
{
	int size;
	switch(currentPhase)
	{
/**/	case Phase::SelectUnit:				// Do nothing
			return;
/**/	case Phase::SelectMove:				// Cancel move, return to SelectUnit
			// Unmark movement tiles
			MarkTiles(true, movementBeginning, unitMap[movementBeginning.x][movementBeginning.y]->Movement, 0);
			currentPhase = SelectUnit;
			selectedTile.x = -1;
			return;
/**/	case Phase::ExecuteMove:			// Do nothing
			return;
/**/	case Phase::SelectPrimaryAction:	// Default to "End Turn"
			ActivateEndTurn();
			return;
/**/	case Phase::SelectSecondaryAction:	// Return to SelectPrimaryAction
			secondaryMenu->Delete();
			//actionMenu->EnableDraw();
			RemoveActiveLayer(SECONDARY_MENU_LAYER);
			AddActiveLayer(ACTION_MENU_LAYER);
			currentPhase = SelectPrimaryAction;
			break;
/**/	case Phase::SelectTarget:			// Cancel Action, return to SelectAction
			// Unmark all enemies in range of unit
			MarkTiles(true, currentUnitPosition, unitMap[currentUnitPosition.x][currentUnitPosition.y]->AttackRange, 1);
			actionMenu->EnableDraw();
			RemoveActiveLayer(TILE_LAYER);
			AddActiveLayer(ACTION_MENU_LAYER);
			currentPhase = SelectPrimaryAction;
			return;
/**/	case Phase::SelectSkillTarget:		// Cancel Secondary Action, return to SelectSecondary Action
			// Unmark Skill Range/AoE
			// MarkTiles(true, currentUnitPosition, /*SkillRange*/ 0, 2);
			
			secondaryMenu->EnableDraw();
			actionMenu->EnableDraw();
			RemoveActiveLayer(TILE_LAYER);
			AddActiveLayer(SECONDARY_MENU_LAYER);
			currentPhase = SelectSecondaryAction;
			break;
/**/	case Phase::ExecuteAction:			// Do Nothing
			return;
/**/	case Phase::EnemyTurn:				// Skip Enemy Turn (maybe)
			return;
	}
}

void Level::GenerateLevel()
{
	// Initialize map and unit array to appropriate size
	int i,j;
	map = new Tile**[mapWidth];
	unitMap = new Unit**[mapWidth];
	movementMap = new list<Position>*[mapWidth];
	for(i = 0; i < mapWidth; i++)
	{
		map[i] =  new Tile*[mapHeight];
		unitMap[i] = new Unit*[mapHeight];
		movementMap[i] = new list<Position>[mapHeight];
	}

	// Create map and set units to NULL initially
	for(i = 0; i < mapWidth; i++)
	{
		for(j = 0; j < mapHeight; j++)
		{
			map[i][j] = new Tile(L"../FelledTactics/Textures/Tile.png", TILE_LAYER, tileSize, tileSize, i*tileSize, j*tileSize, this, Position(i,j));
			unitMap[i][j] = NULL;
			AddVisualElement(map[i][j]);
		}
	}

	// Create units
	unitMap[0][0] = new Unit(L"../FelledTactics/Textures/Units/Bladedge.png", UNIT_LAYER, tileSize, tileSize,0,0);
	unitMap[1][1] = new Unit(L"../FelledTactics/Textures/Units/Bladedge.png", UNIT_LAYER, tileSize, tileSize,50,50);
	unitMap[2][2] = new Unit(L"../FelledTactics/Textures/Units/Bladedge.png", UNIT_LAYER, tileSize, tileSize,100,100);
	unitMap[3][3] = new Unit(L"../FelledTactics/Textures/Units/Axereaver.png", UNIT_LAYER, tileSize, tileSize,150,150,false);
	unitMap[4][4] = new Unit(L"../FelledTactics/Textures/Units/Bladedge.png", UNIT_LAYER, tileSize, tileSize,200,200);
	unitMap[7][7] = new Unit(L"../FelledTactics/Textures/Units/Axereaver.png", UNIT_LAYER, tileSize, tileSize,350,350,false);
	unitMap[7][6] = new Unit(L"../FelledTactics/Textures/Units/Axereaver.png", UNIT_LAYER, tileSize, tileSize,350,300,false);
	unitMap[2][8] = new Unit(L"../FelledTactics/Textures/Units/Axereaver.png", UNIT_LAYER, tileSize, tileSize,100,400,false);
	unitMap[3][8] = new Unit(L"../FelledTactics/Textures/Units/Axereaver.png", UNIT_LAYER, tileSize, tileSize,150,400,false);
	unitList.push_back(unitMap[0][0]);
	unitList.push_back(unitMap[1][1]);
	unitList.push_back(unitMap[2][2]);
	unitList.push_back(unitMap[3][3]);
	unitList.push_back(unitMap[4][4]);
	unitList.push_back(unitMap[7][7]);
	unitList.push_back(unitMap[7][6]);
	unitList.push_back(unitMap[2][8]);
	unitList.push_back(unitMap[3][8]);
	map[0][0]->TileStatus = Tile::Status::AllyUnit;
	map[1][1]->TileStatus = Tile::Status::AllyUnit;
	map[2][2]->TileStatus = Tile::Status::AllyUnit;
	map[3][3]->TileStatus = Tile::Status::EnemyUnit;
	map[4][4]->TileStatus = Tile::Status::AllyUnit;
	map[7][7]->TileStatus = Tile::Status::EnemyUnit;
	map[7][6]->TileStatus = Tile::Status::EnemyUnit;
	map[2][8]->TileStatus = Tile::Status::EnemyUnit;
	map[3][8]->TileStatus = Tile::Status::EnemyUnit;

	numAllies = 4;
	numEnemies = 5;

	// Add units to the VisualElements list
	for(int i = 0; i < unitList.size(); i++)
		AddVisualElement(unitList[i]);

	SortVisualElements();
	AddActiveLayer(TILE_LAYER);

	StartNewTurn();
}

void Level::StartNewTurn()
{
	for(int i = 0; i < unitList.size(); i++)
		unitList[i]->NewTurn();

	currentUnitPosition.Reset();
	selectedTile.Reset();
	numUnitsMoved = 0;
	turn++;
	currentPhase = SelectUnit;
}

bool Level::CheckWin()
{
	return false;
}

bool Level::CheckLoss()
{
	bool hasLost = false;

	for(int i = 0; i < loseConditions.size(); i++)
	{
		switch(loseConditions[i])
		{
			case Death:
				if(numAllyDeaths > maximumDeaths)
					hasLost = true;
				break;
			case Turn:
				if(turn > maximumTurns)
					hasLost = true;
				break;
			case Surrender:
				break;
		}
	}

	return hasLost;
}

// Allow player to draw a custom movement path for the selected Unit to follow for their movement phase
void Level::PlayerDrawPath()
{
	// If the selected tile (new tile the mouse is hovering over) being added is valid
	if(map[hoveredTile.x][hoveredTile.y]->TileMark == Tile::Mark::AllyMove)
	{
		list<Position> newPath;		// Path being added to currentMovementPath this turn
		list<Position>::iterator npIT, cpIT;// Iterator for later

		// Next tile is not adjacent to the last tile, we moved diagonally or around something. Account for that and make a path between the two
		if(hoveredTile.DistanceTo(currentMovementPath.back()) > 1)
		{	// Calc the new path between the last tile in the path and the destination tile 
			CalcShortestPathAStar(currentMovementPath.back(), hoveredTile, unitMap[movementBeginning.x][movementBeginning.y]->Movement - (currentMovementPath.size() - 1),
				newPath, TEST_OBSTRUCTION_ALLY_UNITS);
					
			// Test newPath for doubling back and remove redundant tile in both paths
			if(currentMovementPath.size() != 1)
			{
				map[currentMovementPath.back().x][currentMovementPath.back().y]->TileMark = Tile::Mark::AllyMove;
				currentMovementPath.pop_back();		// delete last tile in path (it already exists in newPath)
						
				bool finished = false;
				cpIT = currentMovementPath.begin(); 
				for(; cpIT != currentMovementPath.end(); cpIT++)
				{
					for(npIT = newPath.begin(); npIT != newPath.end(); npIT++)
					{
						if(*cpIT == *npIT)
						{
							// Reset Mark for tiles no longer in the path
							for(list<Position>::iterator it = cpIT; it != currentMovementPath.end(); it++)
								map[it->x][it->y]->TileMark = Tile::Mark::AllyMove;
								
							currentMovementPath.erase(cpIT, currentMovementPath.end());	// remove tiles from path
							newPath.erase(newPath.begin(), npIT);
							finished = true;
							break;
						}
					}
					if(finished) break;
				}
			}
			else	// newPath is exactly what we want currentMovementPath to be, so clear it now before we combine them
				currentMovementPath.clear();
		}
		else // Add tile to path
			newPath.push_back(hoveredTile);


		// Mark the new path
		for(npIT = newPath.begin(); npIT != newPath.end(); npIT++)
		{
			currentMovementPath.push_back(*npIT);

			if(*npIT == selectedTile)
				continue;

			// Change color to show player the path he has drawn (red if the path is too large for the unit moving)
			if(currentMovementPath.size() <= unitMap[movementBeginning.x][movementBeginning.y]->Movement+1)
				map[npIT->x][npIT->y]->TileMark = Tile::Mark::AllyMovePath;
			else
				map[npIT->x][npIT->y]->TileMark = Tile::Mark::AllyMovePathFail;
		}
	}// Selected tile already in the path (overlap) - find it in the list and remove all entries after it
	else if(map[hoveredTile.x][hoveredTile.y]->TileMark == Tile::Mark::AllyMovePath || map[hoveredTile.x][hoveredTile.y]->TileMark == Tile::Mark::AllyMovePathFail)
	{
		for(std::list<Position>::const_iterator i = currentMovementPath.begin(); i != currentMovementPath.end(); ++i)
		{	
			if(hoveredTile == *i)
			{	// Reset the color of the tiles being removed
				for(std::list<Position>::const_iterator j = ++i; j != currentMovementPath.end(); j++)
					map[(*j).x][(*j).y]->TileMark = Tile::Mark::AllyMove;

				// Remove tiles
				currentMovementPath.erase(i, currentMovementPath.end());
				break;
			}
		}
	}	
}	

// Draw pre-calculated path between Moving Unit's tile and the tile the mouse is currently hovering over
void Level::AutoDrawPath()
{
	// Only change path if mouse is hovering over a tile that can be moved too
	if(map[hoveredTile.x][hoveredTile.y]->TileMark == Tile::Mark::AllyMove || map[hoveredTile.x][hoveredTile.y]->TileMark == Tile::Mark::AllyMovePath)
	{
		// Un-color the path to the previous hovered tile
		Position j;
		if(currentMovementPath.size() > 1)
		{
			int size = currentMovementPath.size()-1;
			for(int i = 0; i < size; i++)
			{
				currentMovementPath.pop_front();
				j = currentMovementPath.front();
				map[j.x][j.y]->TileMark = Tile::Mark::AllyMove;
			}
		}
		currentMovementPath.clear();
		
		// Retrieve pre-calculated shortest path
		currentMovementPath = movementMap[hoveredTile.x][hoveredTile.y];
		
		// Color the new path
		list<Position>::const_iterator it = currentMovementPath.begin();
		it++;
		for(; it != currentMovementPath.end(); it++)
			map[it->x][it->y]->TileMark = Tile::Mark::AllyMovePath;
	}
}

// Return a path between 'start' and 'end' that can be traversed
void Level::CalcShortestPathAStar(Position start, Position end, int unitMove, list<Position> &path, int options = 0)
{
	vector<TravelNode*> openList, closedList;
	TravelNode* startNode = new TravelNode(NULL, start, 0, CalcPathHeuristic(start, end, 0, unitMove));
	TravelNode* curNode;
	Position p;
	int index, minF;
	path.clear();

#pragma region Direction Priority
	// Decide which directions to favor for path-construction based on start/end locations - makes it "more dynamic" because I feel like it
	Position diff = end - start;
	Position directions[4];
	if(abs(diff.x) > abs(diff.y) && diff.x > 0 && diff.y > 0)			// horizontal - right, up, left, down
	{	directions[0].x = directions[1].y = 1;	directions[2].x = directions[3].y = -1;	}
	else if(abs(diff.x) > abs(diff.y) && diff.x > 0 && diff.y <= 0)		// horizontal - right, down, left, up
	{	directions[0].x = directions[3].y = 1;	directions[2].x = directions[1].y = -1; }
	else if(abs(diff.x) > abs(diff.y) && diff.x <= 0 && diff.y > 0)		// horizontal - left, up, right, down
	{	directions[2].x = directions[1].y = 1;	directions[0].x = directions[3].y = -1;	}
	else if(abs(diff.x) > abs(diff.y) && diff.x <= 0 && diff.y <= 0)	// horizontal - left, down, right, up
	{	directions[2].x = directions[3].y = 1;	directions[0].x = directions[1].y = -1;	}
	else if(abs(diff.x) < abs(diff.y) && diff.x > 0 && diff.y > 0)		// vertical - up, right, down, left
	{	directions[1].x = directions[0].y = 1;	directions[3].x = directions[2].y = -1;	}
	else if(abs(diff.x) < abs(diff.y) && diff.x <= 0 && diff.y > 0)		// veritcal - up, left, down, right
	{	directions[3].x = directions[0].y = 1;	directions[1].x = directions[2].y = -1;	}		
	else if(abs(diff.x) < abs(diff.y) && diff.x > 0 && diff.y <= 0)		// veritcal - down, right, up, left
	{	directions[1].x = directions[0].y = 1;	directions[3].x = directions[2].y = -1;	}
	else if(abs(diff.x) < abs(diff.y) && diff.x <= 0 && diff.y <= 0)	// veritcal - down, left, right, up
	{	directions[2].x = directions[3].y = 1;	directions[1].x = directions[0].y = -1;	}
	else if(diff.x > 0 && diff.y > 0)									// right, up, left, down															
	{	directions[0].x = directions[1].y = 1;	directions[2].x = directions[3].y = -1;	}
	else if(diff.x < 0 && diff.y > 0)									// up, left, down, right															
	{	directions[3].x = directions[0].y = 1;	directions[1].x = directions[2].y = -1;	}
	else if(diff.x < 0 && diff.y < 0)									// left, down, right, up															
	{	directions[2].x = directions[3].y = 1;	directions[0].x = directions[1].y = -1;	}
	else if(diff.x > 0 && diff.y < 0)									// down, right, up, left															
	{	directions[1].x = directions[2].y = 1;	directions[3].x = directions[0].y = -1;	}
#pragma endregion

	// pointer to evaluation function
	bool (Level::*ObstructedFunction)(Position) = &Level::IsObstructed;
	if(options & TEST_OBSTRUCTION_ALLY_UNITS)
		ObstructedFunction = &Level::IsObstructedPlayer;
	else if(options & TEST_OBSTRUCTION_ENEMY_UNITS)
		ObstructedFunction = &Level::IsObstructedEnemy;

	// Add starting point to open list
	openList.push_back(startNode);

	while(openList.size() > 0)
	{
		// Get node with lowest F (total travel cost) in open list
		index = -1; minF = 1000000;
		for(int i = 0; i < openList.size(); i++)
		{
			if(openList[i]->f <= minF)
			{
				minF = openList[i]->f;
				index = i;
			}
		}
		curNode = openList[index];

		// Current node is the target we are moving to, stop searching
		if(curNode->p == end)
			break;

		// Create nodes for traveling from current node in each direction
		for(int i = 0; i < 4; i++)
		{
			p = curNode->p + directions[i];

			// If next destination node is valid
			if(p.x >= 0 && p.x < mapWidth && p.y >= 0 && p.y < mapHeight && !(*this.*ObstructedFunction)(p))
			{
				TravelNode* newNode = new TravelNode(curNode, p, 10, CalcPathHeuristic(p, end, curNode->pathNum+1, unitMove));
				int openIndex = CheckListContainsTravelNode(openList, newNode);
				int closedIndex = CheckListContainsTravelNode(closedList, newNode);

				// Check if newNode already exists in the open or closed lists
				// If it does and either is "better" than the current one, skip it
				if(openIndex != -1)
				{
					if(openList[openIndex]->f <= newNode->f)
						continue;
				}
				if(closedIndex != -1)
				{
					if(closedList[closedIndex]->f <= newNode->f)
						continue;
				}

				// Remove existing node in open/closed lists if new one is better
				if(openIndex != -1)
					openList.erase(openList.begin() + openIndex);
				if(closedIndex != -1)
					closedList.erase(closedList.begin() + closedIndex);

				openList.push_back(newNode);
			}
		}

		// Add current node to closed list now that we explored all options from it
		closedList.push_back(curNode);
		for(index = 0; index < openList.size(); index++)
		{
			if(openList[index] == curNode)
				break;
		}
		openList.erase(openList.begin() + index);
	}

	// Construct actual path
	path.push_front(curNode->p);
	while(curNode->parent != NULL)
	{
		curNode = curNode->parent;
		path.push_front(curNode->p);
	}

	// Delete pointers created
	for(int i = 0; i < openList.size(); i++)
		delete openList[i];
	for(int j = 0; j < closedList.size(); j++)
		delete closedList[j];
}

// Calculate the heuristic value of tile 'p' as part of a path to tile 'target'
int Level::CalcPathHeuristic(Position p, Position target, int pathNum, int unitMove)
{
	// This check is only relevant when the user is trying to draw a path outside a unit's movement range
	if(currentPhase == SelectMove && map[p.x][p.y]->TileMark == Tile::Mark::None)
		return 100000;

	if(p.DistanceTo(movementBeginning) > unitMove)
		return 10000;

	if(pathNum > unitMove)
		return 1000;

	// Heuristic is Manhatten distance to target
	return p.DistanceTo(target) * 10;
}

// Check if list contains node. If existing entry exists, return index of existing entry
int Level::CheckListContainsTravelNode(vector<TravelNode*> &list, TravelNode* node)
{
	for(int i = 0; i < list.size(); i++)
	{
		if(list[i]->p == node->p)
			return i;
	}

	return -1;
}

// Visual Mark all tiles in range of a given action (movement, ability cast, etc.)
// Mark Type 0 = Movement, 1 = Ally Skill Range, 2 = Ally Skill AoE, 3 = Enemy Movement
void Level::MarkTiles(bool undo, Position start, int range, int markType, vector<Position> skillRange)
{
	if(markType == 2)
	{
		if(!undo)
		{
			map[start.x][start.y]->PrevTileMark = map[start.x][start.y]->TileMark;	// Save mark for simple undo later
			map[start.x][start.y]->TileMark = Tile::Mark::AllySkillAoE;				// Mark tile - Visual Change will be done by the tile itself (hopefully with shaders)

			// A skill can have a custom Area of Effect (aoe) that requires extra markings
			for(int k = 0; k < skillRange.size(); k++)
				map[start.x+skillRange[k].x][start.y+skillRange[k].y]->TileMark = Tile::Mark::AllySkillAoE;
		}
		else	// Undo the change as the cast location of the skill moves or the skill is cast or cancelled
		{
			map[start.x][start.y]->TileMark = map[start.x][start.y]->PrevTileMark;
			for(int k = 0; k < skillRange.size(); k++)
				map[start.x+skillRange[k].x][start.y+skillRange[k].y]->TileMark = map[start.x+skillRange[k].x][start.y+skillRange[k].y]->PrevTileMark;
		}

		return;	// Do not bother with the rest, this section is for AllySkillAoE only
	}

	// Search through all tiles in range
	for(int i = start.x - range; i <= start.x + range; i++)
	{
		for(int j = start.y - range; j <= start.y + range; j++)
		{
			// Mark/Unmark tiles within the movement range of the currently selected unit
			if(IsValidPosition(i, j) && Position(i, j).DistanceTo(start) <= range)
			{ 
				if(!undo)
				{
					// Save mark for simple undo later
					map[i][j]->PrevTileMark = map[i][j]->TileMark;

					// Mark tile - Visual Change will be done by the tile itself
					if(markType == 0/* && map[i][j]->TileStatus == Tile::Status::Empty*/)
					{
						// Mark starting movement position as part of the path
						if(Position(i,j) == start)
						{
							map[i][j]->TileMark = Tile::Mark::AllyMovePath;
							movementMap[i][j].clear();
							movementMap[i][j].push_back(Position(i,j));
							continue;
						}

						CalcShortestPathAStar(start, Position(i,j), unitMap[start.x][start.y]->Movement, movementMap[i][j], TEST_OBSTRUCTION_ALLY_UNITS);
						if(movementMap[i][j].size() <= range+1)	// +1 because the path includes the starting location of the movement
							map[i][j]->TileMark = Tile::Mark::AllyMove;
						else
							movementMap[i][j].clear();
					}
					else if(markType == 1)
						map[i][j]->TileMark = Tile::Mark::AllySkillRange;
					else if(markType == 3)
						map[i][j]->TileMark = Tile::Mark::EnemyMove;
				}
				else	// Undo any markings - Just reset to prev tile status
					map[i][j]->TileMark = map[i][j]->PrevTileMark;
			}
		}
	}
}


// Move a unit from the start to end, change all properties and statuses to reflect
bool Level::DoMovementEnd(Position start, Position end)
{
	if(start != end)
	{
		// Attempting to move unit to occupied space
		if(unitMap[end.x][end.y] != NULL || map[end.x][end.y]->TileStatus != Tile::Status::Empty)
			return false;

		// Attempting to move non-existant or felled unit
		if(unitMap[start.x][start.y] == NULL || map[start.x][start.y]->TileStatus == Tile::Status::Empty || map[start.x][start.y]->TileStatus == Tile::Status::AllyFelled)
			return false;

		// Move unit from start to end
		unitMap[end.x][end.y] = unitMap[start.x][start.y];
		unitMap[start.x][start.y] = NULL;
		unitMap[end.x][end.y]->UnitPosition = end;

		// Change tile statuses to reflect
		map[end.x][end.y]->TileStatus = map[start.x][start.y]->TileStatus;
		map[start.x][start.y]->TileStatus = Tile::Status::Empty;

		// Re-sort the VisualElements list only on the layer that just changed
		SortVisualElementsInLayer(unitMap[end.x][end.y]->Layer);

		// DO EFFECTS FROM TILES MOVED THROUGH OR STOPPED ON VIA CURRENTMOVEMENTPATH

		currentMovementPath.clear();
	}

	// Set current unit position for this action
	currentUnitPosition = end;

	// Create Action Menu
	CreateActionMenu();

	currentPhase = SelectPrimaryAction;
	return true;
}

void Level::CreateActionMenu()
{
	actionMenu = new MenuBox(this, L"../FelledTactics/Textures/MenuBackground.png", ACTION_MENU_LAYER, 100, 200, currentUnitPosition.x*tileSize + tileSize*2, currentUnitPosition.y*tileSize);
	actionMenu->CreateElement(&Level::ActivateAttack, L"../FelledTactics/Textures/MenuAttack.png", 80, 45, 10, 145);
	actionMenu->CreateElement(&Level::ActivateSkill, L"../FelledTactics/Textures/MenuSkills.png", 80, 45, 10, 100);
	actionMenu->CreateElement(&Level::ActivateItem, L"../FelledTactics/Textures/MenuItems.png", 80, 45, 10, 55);
	actionMenu->CreateElement(&Level::ActivateEndTurn, L"../FelledTactics/Textures/MenuEnd.png", 80, 45, 10, 10);
	
	AddVisualElement(actionMenu);
//	SortVisualElements();

	// Set active layers to ignore everything but this Menu
	RemoveActiveLayer(TILE_LAYER);
	AddActiveLayer(ACTION_MENU_LAYER);
}

// Player has selected for a Unit to attack
void Level::ActivateAttack()
{
	// Mark all enemies in range of unit - Loop through all tiles in attack range of unit and check for enemies
	int range = unitMap[currentUnitPosition.x][currentUnitPosition.y]->AttackRange;
	for(int i = currentUnitPosition.x - range; i <= currentUnitPosition.x + range; i++)
	{
		for(int j = currentUnitPosition.y - range; j <= currentUnitPosition.y + range; j++)
		{
			if(i < 0 || j < 0) continue;

			if(Position(i,j).DistanceTo(currentUnitPosition) <= range && map[i][j]->TileStatus == Tile::Status::EnemyUnit)
				map[i][j]->TileMark = Tile::Mark::Attack;
		}
	}

	// Create Combat Calculator
	combatCalculator.SetAttacker(unitMap[currentUnitPosition.x][currentUnitPosition.y]);

	// Go to Next Phase - Select Target for attack
	currentPhase = SelectTarget;
	actionMenu->DisableDraw();
	RemoveActiveLayer(ACTION_MENU_LAYER);	// Change active layers
	AddActiveLayer(TILE_LAYER);				//	to the tiles to allow player to select a target
}

void Level::ActivateSkill()
{
/*	secondaryMenu = new MenuBox(this, L"../FelledTactics/Textures/MenuBackground.png", SECONDARY_MENU_LAYER, 100, 200, 950, 100);

	currentPhase = SelectSecondaryAction;
	AddVisualElement(secondaryMenu);
//	SortVisualElements();

	// Set active layers to ignore everything but this Menu
	RemoveActiveLayer(ACTION_MENU_LAYER);
	AddActiveLayer(SECONDARY_MENU_LAYER);*/

	lua_pushlightuserdata(L, (void*)this);
	lua_setglobal(L, "Level");
	unitMap[currentUnitPosition.x][currentUnitPosition.y]->ActivateAbility(L, currentUnitPosition);
	ActivateEndTurn();
}

void Level::ActivateItem()
{
	secondaryMenu = new MenuBox(this, L"../FelledTactics/Textures/MenuBackground.png", SECONDARY_MENU_LAYER, 100, 200, 950, 100);

	currentPhase = SelectSecondaryAction;
	AddVisualElement(secondaryMenu);
//	SortVisualElements();

	// Set active layers to ignore everything but this Menu
	RemoveActiveLayer(ACTION_MENU_LAYER);
	AddActiveLayer(SECONDARY_MENU_LAYER);
}

void Level::ActivateEndTurn()
{
	// End turn of current unit
	unitMap[currentUnitPosition.x][currentUnitPosition.y]->FinishTurn();
	currentUnitPosition.Reset();
	numUnitsMoved++;

	// Delete action menu
	actionMenu->Delete();
	RemoveActiveLayer(ACTION_MENU_LAYER);
	AddActiveLayer(TILE_LAYER);

	// Go to next phase
	if(numUnitsMoved == numAllies)
		currentPhase = EnemyTurn;
	else
		currentPhase = SelectUnit;
}

void Level::CreateCombatUI()
{

}

bool Level::IsObstructed(Position p) { 	return map[p.x][p.y]->IsObstructed(); }
bool Level::IsObstructedPlayer(Position p) { return map[p.x][p.y]->IsObstructedPlayer(); }
bool Level::IsObstructedEnemy(Position p) {	return map[p.x][p.y]->IsObstructedEnemy(); }

// Save the selected tile (called from the tile itself)
void Level::SetSelectedTile(Position p)
{
	selectedTile = p;
	pathDrawEnabled = false;
//	if(currentMovementPath.size() == 1)
//		currentMovementPath.clear();
}

// Save the hovered tile (called from the tile itself)
void Level::SetHoveredTile(Position p)
{
	hoveredTile = p;
	if(lastHoveredTile.y == -1)
		lastHoveredTile = p;
}

void Level::Draw()
{
	GameMaster::Draw();

	combatCalculator.Draw();
}

#pragma region Properties
Tile*** Level::GetMap() { return map; }
Unit*** Level::GetUnits() { return unitMap; }
int	  Level::GetWidth() { return mapWidth; }
void  Level::SetWidth(int w) { mapWidth = w; }
int	  Level::GetHeight() { return mapHeight; }
void  Level::SetHeight(int h) { mapHeight = h; }
Tile* Level::GetTile(int x, int y) { return map[x][y]; }
Unit* Level::GetUnit(int x, int y) { return unitMap[x][y]; }
#pragma endregion                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            