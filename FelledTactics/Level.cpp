#include "StdAfx.h"
#include "Level.h"

template <typename T>
void Delete(T*& t)
{
	t->Delete();
	delete t;
	t = NULL;
}

Level::Level(lua_State* luaState, int width, int height, int tSize = 50) : mapWidth(width), mapHeight(height), tileSize(tSize), GameMaster(luaState)
{
	pathDrawEnabled = false;
	actionMenu = secondaryMenu = NULL;

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
		Position p;
		result = unitList[i]->Update(dt);

		// Move unit on map - If during movement it enters an active hazard, we resolve that now
		if(result == UNIT_UPDATE_MOVEMENT_END || result == UNIT_UPDATE_TILE_REACHED)
		{
			Position p = unitList[i]->UnitPosition;

			// Do effect of tile unit is currently on
			if(map[p.x][p.y]->HasEffect())
			{
				activatedTiles.push_back(p);	// Save list of tiles with effects this unit has passed through this turn
				lua_pushlightuserdata(L, (void*)this);
				lua_setglobal(L, "Level");
				lua_pushlightuserdata(L, (void*)unitList[i]);
				lua_setglobal(L, "Unit");
				map[p.x][p.y]->ActivateMovementEffect(L);
			}

			if(result == UNIT_UPDATE_MOVEMENT_END)
			{
				// Don't call DoMovementEnd if the unit has renewed motion (from ability/tile effct)
				if(!unitList[i]->CheckStatus(UNIT_STATUS_MOVING | UNIT_STATUS_FORCED_MOVING))
				{
					// Move unit from original to new tile
					unitMap[p.x][p.y] = unitMap[movementBeginning.x][movementBeginning.y];
					unitMap[movementBeginning.x][movementBeginning.y] = NULL;

					// Change tile statuses to reflect
					map[p.x][p.y]->TileStatus = map[movementBeginning.x][movementBeginning.y]->TileStatus;
					map[movementBeginning.x][movementBeginning.y]->TileStatus = Tile::Status::Empty;

					// Re-sort the VisualElements list only on the layer that just changed
					SortVisualElementsInLayer(unitMap[p.x][p.y]->Layer);

					// Set current unit position for this action
					currentUnitPosition = p;
					currentMovementPath.clear();

					// Re-enable tile effects (they disabled themselves after activation)
					for(auto j = activatedTiles.begin(); j != activatedTiles.end(); j++)
						map[j->x][j->y]->ReenableEffect();
					activatedTiles.clear();

					if(unitList[i]->CheckStatus(UNIT_STATUS_ALLY))
					{	
						CheckWin(WIN_CONDITION_CAPTURE); // TODO
						// Create Action Menu now that unit's movement phase is over
						CreateActionMenu();
					}
					else
					{
						CheckLoss(LOSE_CONDITION_SURRENDER); // TODO
					}
					selectedTile.x = -1;
				}
			}
		}
		else if(result == UNIT_UPDATE_DEAD)
		{
			// Leave felled-ally unit's body of possible resurrection
			if(unitList[i]->IsAlly())
			{
				map[unitList[i]->UnitPosition.x][unitList[i]->UnitPosition.y]->TileStatus = Tile::Status::AllyFelled;
				numAllyDeaths++;
				CheckLoss(LOSE_CONDITION_DEATH);	// TODO
				CheckLoss(LOSE_CONDITION_SURVIVE);  // TODO
			}
			else	// Remove dead enemy from the level permanently
			{
				if(CheckWin(WIN_CONDITION_DEFEAT)) {/*TODO*/}
				else
				{
					Position p = unitList[i]->UnitPosition;				// Save position temporarily
					Delete<Unit>(unitList[i]);							// Delete unit	
					unitList.erase(unitList.begin() + i--);				// Remove from unit list
					map[p.x][p.y]->TileStatus = Tile::Status::Empty;	// Clear tile it was on
					unitMap[p.x][p.y] = NULL;							// Reset tile
					numEnemyDeaths++;
					numEnemies--;
					CheckWin(WIN_CONDITION_ELIMINATE);
					CheckWin(WIN_CONDITION_ROUT);
				}
			}

			// Allow user to interact again now that gameplay has restarted
			RestoreUserInput();
		}
	}
#pragma endregion

#pragma region Combat Text Update
	for(int i = 0; i < combatText.size(); i++)
	{
		result = combatText[i]->Update(dt);

		if(result == COMBAT_TEXT_UPDATE_DEAD)
		{
			Delete<TextElement>(combatText[i]);
			combatText.erase(combatText.begin() + i--);
		}
	}
#pragma endregion

	if(actionMenu)
		actionMenu->Update(dt);
	if(secondaryMenu)
		secondaryMenu->Update(dt);

#pragma region Phase Update
	// Do different updates depending on the current phase
	switch(currentPhase)
	{
/**/	case Phase::SelectUnit:

			// Turn on/off HP/AP bars on hovered units
			if(lastHoveredTile != hoveredTile)
			{	// Turn off drawing HP/AP bars on old tile
				if(lastHoveredTile.x >= 0 && unitMap[lastHoveredTile.x][lastHoveredTile.y] != NULL)
					unitMap[lastHoveredTile.x][lastHoveredTile.y]->DrawBars = false;

				// Turn on drawing HP/AP bars on new tile
				if(hoveredTile.x >= 0 && unitMap[hoveredTile.x][hoveredTile.y] != NULL)
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
				CalcMovementFlood(movementBeginning, unitMap[movementBeginning.x][movementBeginning.y]->Movement, TEST_OBSTRUCTION_ALLY_UNITS);
				unitMap[selectedTile.x][selectedTile.y]->DrawBars = false;

				selectedTile.x = -1;
				currentPhase = Phase::SelectMove; // Jump to next phase
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
						map[i->x][i->y]->SetMark(Tile::Mark::AllyMove);
				}

				// Activate pre-calculated movement path
				currentMovementPath.clear();
				currentMovementPath = movementMap[selectedTile.x][selectedTile.y];
				selectedTile = currentMovementPath.front();
				i = currentMovementPath.begin();
				i++;
				for(; i != currentMovementPath.end(); i++)
					map[i->x][i->y]->SetMark(Tile::Mark::AllyMovePath);
			} // Selected Tile is valid. Proceed with movement
			else if(map[selectedTile.x][selectedTile.y]->TileMark == Tile::Mark::AllyMovePath)
			{
				// Unmark movement tiles
				MarkTilesInRange(true, movementBeginning, unitMap[movementBeginning.x][movementBeginning.y]->Movement, TILE_MARK_TYPE_MOVEMENT);

				// No movement for this Unit
				if(selectedTile == movementBeginning)
				{
					// Create Action Menu now that unit's movement phase is over
					currentUnitPosition = selectedTile;
					CreateActionMenu();
					currentMovementPath.clear();

					// Clear movement map
					for(int i = 0; i < mapWidth; i++)
					{
						for(int j = 0; j < mapHeight; j++)
							movementMap[i][j].clear();
					}

					selectedTile.x = -1;
					break;
				}
		
				// Tell the unit to move
				unitMap[movementBeginning.x][movementBeginning.y]->SetMovePath(currentMovementPath);
				currentMovementPath.pop_front();

				// Clear movement map
				for(int i = 0; i < mapWidth; i++)
				{
					for(int j = 0; j < mapHeight; j++)
						movementMap[i][j].clear();
				}

				currentPhase = Phase::ExecuteMove;	// Go to next phase
			}
			break;
//		case Phase::ExecuteMove: 
/**/	case Phase::SelectPrimaryAction:
			break;
/**/	case Phase::SelectSecondaryAction:
			break;
/**/	case Phase::SelectTarget:
			if(hoveredTile.x < 0) break;

			if(lastHoveredTile != hoveredTile)
			{
				if(map[hoveredTile.x][hoveredTile.y]->TileMark == Tile::Mark::Attack)
				{
					target = hoveredTile;
					combatManager.SetDefender(unitMap[target.x][target.y]);
					combatManager.CalculateCombat(L);

					// CREATE COMBAT RESULTS UI
				}
				else
				{
					combatManager.ResetDefender();
					// REMOVE COMBAT AI
				}
			}

			// If no tile is selected yet, we have nothing more to do
			if(selectedTile.x == -1)	break;

			// Selected valid target - FIGHT!
			if(map[selectedTile.x][selectedTile.y]->TileMark == Tile::Mark::Attack)
			{
				combatManager.DoCombat();

				// Unmark all enemies in range of unit
				MarkTilesInRange(true, currentUnitPosition, unitMap[currentUnitPosition.x][currentUnitPosition.y]->AttackRange, TILE_MARK_TYPE_ATTACK);

				PauseUserInputIndefinite();
				currentPhase = Phase::ExecuteAttack;
			}
			break;
/**/	case Phase::SelectAbilityTarget:
			// Target location for skill has moved
			if(lastHoveredTile != hoveredTile)
			{
				// Remove AoE marks now that the target for the skill has changed (if marks still exist)
				if(lastHoveredTile.x >= 0 && (map[lastHoveredTile.x][lastHoveredTile.y]->TileMark == Tile::Mark::AllyAbilityRange || 
					map[lastHoveredTile.x][lastHoveredTile.y]->TileMark == Tile::Mark::AllyAbilityAoE))
					MarkTiles(true, TILE_MARK_TYPE_ALLY_ABILITY_AOE, unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityAoE(lastHoveredTile-currentUnitPosition), lastHoveredTile);

				// Re-draw AoE marks when the target is a new valid tile
				if(hoveredTile.x >= 0 && map[hoveredTile.x][hoveredTile.y]->TileMark == Tile::Mark::AllyAbilityRange)
					MarkTiles(false, TILE_MARK_TYPE_ALLY_ABILITY_AOE, unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityAoE(hoveredTile-currentUnitPosition), hoveredTile);

				// Special Considerations must be made for Battle Abilities that tie-in to regular combat
				if(unitMap[currentUnitPosition.x][currentUnitPosition.y]->SelectedBattleAbility())
				{
					if(map[hoveredTile.x][hoveredTile.y]->TileStatus == Tile::Status::EnemyUnit)
					{
						target = hoveredTile;
						combatManager.SetDefender(unitMap[target.x][target.y]);
						combatManager.CalculateCombat(L);

						// Create combat results UI
					}
					else
					{
						combatManager.ResetDefender();
						// Remove combat UI
					}
				}
			}

			if(selectedTile.x == -1)	break;

			// Skill has been activated at target location
			if(unitMap[currentUnitPosition.x][currentUnitPosition.y]->SelectedAbilityHasDynamicAoE())
			{
				vector<Position> ranges = unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityDynamicAoERange();
				bool isInRanges = false;
				for(int i = 0; i < ranges.size(); i++)
				{
					if(ranges[i] == selectedTile-currentUnitPosition)
					{
						isInRanges = true;
						break;
					}
				}

				if(isInRanges)
				{
					target = selectedTile;

					// Remove marks from tiles
					MarkTiles(true, TILE_MARK_TYPE_ALLY_ABILITY_AOE, unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityAoE(selectedTile-currentUnitPosition), selectedTile);
					MarkTiles(true, TILE_MARK_TYPE_ALLY_ABILITY_RANGE, unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityDynamicAoERange(), currentUnitPosition);

					// Activate Skill
					combatManager.SetCombatTimers(unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityTimers());
					combatManager.DoAbility(unitMap[currentUnitPosition.x][currentUnitPosition.y], selectedTile);
					currentPhase = Phase::ExecuteAbility;
				}

			}
			else if(selectedTile.DistanceTo(currentUnitPosition) <= unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityRange())
			{
				// Battle Abilities proceed to Combat Phase when their target is selected
				if(unitMap[currentUnitPosition.x][currentUnitPosition.y]->SelectedBattleAbility())
				{
					if(map[selectedTile.x][selectedTile.y]->TileStatus == Tile::Status::EnemyUnit)
					{
						combatManager.DoCombat();

						// Remove marks from tiles
						MarkTiles(true, TILE_MARK_TYPE_ALLY_ABILITY_AOE, unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityAoE(), hoveredTile);
						MarkTilesInRange(true, currentUnitPosition, unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityRange(), TILE_MARK_TYPE_ALLY_ABILITY_RANGE);

						PauseUserInputIndefinite();
						currentPhase = Phase::ExecuteAttack;
					}
				}
				else
				{
					Ability::CastType ct = unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityCastType();
					Tile::Status ts = map[selectedTile.x][selectedTile.y]->TileStatus;

					// Check if Ability CastType is being respected
					if((ct == Ability::CastType::Ally && ts == Tile::Status::AllyUnit) || 
						(ct == Ability::CastType::Enemy && ts == Tile::Status::EnemyUnit) || 
						(ct == Ability::CastType::SelfCast && selectedTile == currentUnitPosition) ||
						ct == Ability::CastType::Free)
					{
						target = selectedTile;

						// Remove marks from tiles
						MarkTiles(true, TILE_MARK_TYPE_ALLY_ABILITY_AOE, unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityAoE(), hoveredTile);
						MarkTilesInRange(true, currentUnitPosition, unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityRange(), TILE_MARK_TYPE_ALLY_ABILITY_RANGE);

						// Activate Skill
						combatManager.SetCombatTimers(unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityTimers());
						combatManager.DoAbility(unitMap[currentUnitPosition.x][currentUnitPosition.y], selectedTile);
						currentPhase = Phase::ExecuteAbility;
					}
				}
			}
			break;
/**/	case Phase::ExecuteAttack:
		{	
			int combatResult; combatResult = combatManager.UpdateCombat(dt, L);		

			switch(combatResult)
			{
				case COMBAT_MANAGER_UPDATE_NULL: break;	// combat still occuring
				case COMBAT_MANAGER_UPDATE_COMBAT_END:	// combat has ended
					RestoreUserInput();
					ActivateEndTurn();
					break;
				default:		// combatant has been killed
					Position deathPosition = (combatResult == COMBAT_MANAGER_UPDATE_ATTACKER_DEAD ? // combatResult == 1 -> attacker died
																	currentUnitPosition : target);	// combatResult == 2 -> defender died
																									
					// Activate death animation of defeated unit and pause user input until it completes
					PauseUserInput(unitMap[deathPosition.x][deathPosition.y]->Die());
					break;
			}
			break;
		}
		case Phase::ExecuteAbility:
		{
			Position p; p = combatManager.UpdateAbility(dt, L);

			switch(p.x)
			{
				case -1: break;		// Ability still executing or wait period between unit deaths
				case -2:			// Ability execution is over
					RestoreUserInput();
					ActivateEndTurn();
					break;
				default:			// Unit was killed by Ability
					PauseUserInput(unitMap[p.x][p.y]->Die());
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
			MarkTilesInRange(true, movementBeginning, unitMap[movementBeginning.x][movementBeginning.y]->Movement, TILE_MARK_TYPE_MOVEMENT);
			// Clear movement map
			for(int i = 0; i < mapWidth; i++)
			{
				for(int j = 0; j < mapHeight; j++)
					movementMap[i][j].clear();
			}
			currentPhase = Phase::SelectUnit;
			selectedTile.x = -1;
			return;
/**/	case Phase::ExecuteMove:			// Do nothing
			return;
/**/	case Phase::SelectPrimaryAction:	// Default to "End Turn"
			ActivateEndTurn();
			return;
/**/	case Phase::SelectSecondaryAction:	// Return to SelectPrimaryAction
			Delete<MenuBox>(secondaryMenu);
			actionMenu->EnableDraw();
			RemoveActiveLayer(SECONDARY_MENU_LAYER);
			AddActiveLayer(ACTION_MENU_LAYER);
			currentPhase = Phase::SelectPrimaryAction;
			break;
/**/	case Phase::SelectTarget:			// Cancel Action, return to SelectAction
			// Unmark all enemies in range of unit
			MarkTilesInRange(true, currentUnitPosition, unitMap[currentUnitPosition.x][currentUnitPosition.y]->AttackRange, TILE_MARK_TYPE_ATTACK);
			actionMenu->EnableDraw();
			RemoveActiveLayer(TILE_LAYER);
			AddActiveLayer(ACTION_MENU_LAYER);
			currentPhase = Phase::SelectPrimaryAction;
			return;
/**/	case Phase::SelectAbilityTarget:		// Cancel Secondary Action, return to SelectSecondary Action
			// Unmark Skill Range/AoE
			MarkTiles(true, TILE_MARK_TYPE_ALLY_ABILITY_AOE, unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityAoE(hoveredTile-currentUnitPosition), hoveredTile);
			if(unitMap[currentUnitPosition.x][currentUnitPosition.y]->SelectedAbilityHasDynamicAoE())	// Dynamic Ability
				MarkTiles(true, TILE_MARK_TYPE_ALLY_ABILITY_RANGE, unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityDynamicAoERange(), currentUnitPosition);
			else	// Static Ability
				MarkTilesInRange(true, currentUnitPosition, unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityRange(), TILE_MARK_TYPE_ALLY_ABILITY_RANGE);
			unitMap[currentUnitPosition.x][currentUnitPosition.y]->ClearBattleScripts();
			if(unitMap[currentUnitPosition.x][currentUnitPosition.y]->SelectedBattleAbility())
				unitMap[currentUnitPosition.x][currentUnitPosition.y]->RefundAP();
			secondaryMenu->EnableDraw();
			actionMenu->EnableDraw();
			RemoveActiveLayer(TILE_LAYER);
			AddActiveLayer(SECONDARY_MENU_LAYER);
			currentPhase = Phase::SelectSecondaryAction;
			break;
/**/	case Phase::ExecuteAttack:			// Do Nothing
			return;
/**/	case Phase::ExecuteAbility:			// Do Nothing
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

	// Starting Positions of the Units on this level
	Position startingPositions[4] = {Position(0,0), Position(1,1), Position(2,2), Position(4,4)};
	Position enemyStartingPositions[5] = {Position(3,3), Position(7,7), Position(7,6), Position(2,8), Position(3,8)};

	ifstream is(UNIT_JSON);
	ifstream is2(ABILITY_JSON);
	json_spirit::mValue value, value2;
	json_spirit::read(is, value);
	json_spirit::read(is2, value2);
	json_spirit::mObject units = value.get_obj();
	json_spirit::mObject abilities = value2.get_obj();

	// Create Units
	i = 0, numAllies = 0;
	for(auto it = units.begin(); it != units.end(); it++)
	{
		unitMap[startingPositions[i].x][startingPositions[i].y] = new Unit(UNIT_LAYER, tileSize, tileSize, startingPositions[i].x*tileSize, startingPositions[i].y*tileSize,
			it->first.c_str(), it->second.get_obj(), abilities);
		unitList.push_back(unitMap[startingPositions[i].x][startingPositions[i].y]);
		map[startingPositions[i].x][startingPositions[i].y]->TileStatus = Tile::Status::AllyUnit;
		i++;
		numAllies++;
	}

	for(i = 0; i < 5; i++)
	{
		unitMap[enemyStartingPositions[i].x][enemyStartingPositions[i].y] = new Unit(L"../FelledTactics/Textures/Units/Axereaver.png", UNIT_LAYER, tileSize, tileSize, 
			enemyStartingPositions[i].x*tileSize, enemyStartingPositions[i].y*tileSize, false);
		unitList.push_back(unitMap[enemyStartingPositions[i].x][enemyStartingPositions[i].y]);
		map[enemyStartingPositions[i].x][enemyStartingPositions[i].y]->TileStatus = Tile::Status::EnemyUnit;
		numEnemies++;
	}

	// Add units to the VisualElements list
	for(int i = 0; i < unitList.size(); i++)
		AddVisualElement(unitList[i]);

	SortVisualElements();
	AddActiveLayer(TILE_LAYER);
	combatManager.SetCombatTextCallback(this, &Level::CreateCombatText);
	CombatText::SetTileSize(tileSize);

	// Set basic parameters
	turn = 0;
	numEnemyDeaths = numAllyDeaths = 0;
	numUnitsMoved = 0;

	// Set win/lose conditions
	boss = survivor = NULL;
	targetTile = defenseTile = NULL;
	targetEnemyDeaths = 1000;
	maximumDeaths = 1000;
	maximumTurns = 1000;

	// Test outputting save file
/*	json_spirit::Object save;
	json_spirit::Object jsonUnits;
	for(int i = 0; i < unitList.size(); i++)
	{
		if(unitList[i]->CheckStatus(UNIT_STATUS_ALLY))
			jsonUnits.push_back(json_spirit::Pair(unitList[i]->Name, unitList[i]->Serialize()));
	}
	save.push_back(json_spirit::Pair("Units", jsonUnits));

	ofstream os(SAVE_JSON, ofstream::binary);
	json_spirit::write(jsonUnits, os, json_spirit::pretty_print);
	os.close();
*/
	StartNewTurn();
}

// Update and reset all LevelEntities (Units and Tiles) for the start of a new turn
void Level::StartNewTurn()
{
	for(int i = 0; i < unitList.size(); i++)	// Update all Units
		unitList[i]->NewTurn(L);
	for(int i = 0; i < mapWidth; i++)			// Update all Tiles
	{
		for(int j = 0; j < mapHeight; j++)
			map[i][j]->NewTurn(L);
	}

	// Update parameters and reset Level's current state for the new turn
	currentUnitPosition.Reset();
	selectedTile.Reset();
	numUnitsMoved = 0;
	turn++;
	currentPhase = Phase::SelectUnit;

	CheckWin(WIN_CONDITION_DEFEND);
	CheckLoss(LOSE_CONDITION_TURN);
}

bool Level::CheckWin(int cond)
{
	if(!(cond & winConditions))
		return false;

	switch(cond)
	{
		case WIN_CONDITION_ELIMINATE:
			if(numEnemies == 0)
				winConditionCheck |= WIN_CONDITION_ELIMINATE;
			break;
		case WIN_CONDITION_ROUT:
			if(numEnemyDeaths >= targetEnemyDeaths)
				winConditionCheck |= WIN_CONDITION_ROUT;
			break;
		case WIN_CONDITION_CAPTURE:
			if(targetTile->TileStatus == Tile::Status::AllyUnit)
				winConditionCheck |= WIN_CONDITION_CAPTURE;
			break;
		case WIN_CONDITION_DEFEAT:
			if(boss->CheckStatus(UNIT_STATUS_FELLED))
				winConditionCheck |= WIN_CONDITION_DEFEAT;
			break;
		case WIN_CONDITION_DEFEND:
			if(turn >= maximumTurns && defenseTile->TileStatus == Tile::Status::Empty)
				winConditionCheck |= WIN_CONDITION_DEFEND;
			break;
		case WIN_CONDITION_COLLECT:
			break;
	}

	if(winConditionCheck == winConditions) {/*TODO*/}
	return false;
}

bool Level::CheckLoss(int cond)
{
	if(!(cond & loseConditions)) 
		return false;

	bool result = false;
	switch(cond)
	{
		case LOSE_CONDITION_DEATH:
			if(numAllyDeaths > maximumDeaths)
				result = true;
			break;
		case LOSE_CONDITION_TURN:
			if(turn > maximumTurns)
				result = true;
			break;
		case LOSE_CONDITION_SURRENDER:
			if(defenseTile->TileStatus == Tile::Status::EnemyUnit)
				result = true;
			break;
		case LOSE_CONDITION_SURVIVE:
			if(survivor->CheckStatus(UNIT_STATUS_FELLED))
				result = true;
			break;
	}

	if(result) {/*TODO*/}
	return false;
}

// Allow player to draw a custom movement path for the selected Unit to follow for their movement phase
void Level::PlayerDrawPath()
{
	if(hoveredTile.x < 0) return;

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
				map[currentMovementPath.back().x][currentMovementPath.back().y]->SetMark(Tile::Mark::AllyMove);
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
								map[it->x][it->y]->SetMark(Tile::Mark::AllyMove);
								
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
				map[npIT->x][npIT->y]->SetMark(Tile::Mark::AllyMovePath);
			else
				map[npIT->x][npIT->y]->SetMark(Tile::Mark::AllyMovePathFail);
		}
	}// Selected tile already in the path (overlap) - find it in the list and remove all entries after it
	else if(map[hoveredTile.x][hoveredTile.y]->TileMark == Tile::Mark::AllyMovePath || map[hoveredTile.x][hoveredTile.y]->TileMark == Tile::Mark::AllyMovePathFail)
	{
		for(std::list<Position>::const_iterator i = currentMovementPath.begin(); i != currentMovementPath.end(); ++i)
		{	
			if(hoveredTile == *i)
			{	// Reset the color of the tiles being removed
				for(std::list<Position>::const_iterator j = ++i; j != currentMovementPath.end(); j++)
					map[(*j).x][(*j).y]->SetMark(Tile::Mark::AllyMove);

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
	if(hoveredTile.x < 0) return;

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
				map[j.x][j.y]->SetMark(Tile::Mark::AllyMove);
			}
		}
		currentMovementPath.clear();
		
		// Retrieve pre-calculated shortest path
		currentMovementPath = movementMap[hoveredTile.x][hoveredTile.y];
		
		// Color the new path
		list<Position>::const_iterator it = currentMovementPath.begin();
		it++;
		for(; it != currentMovementPath.end(); it++)
			map[it->x][it->y]->SetMark(Tile::Mark::AllyMovePath);
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
	if(currentPhase == Phase::SelectMove && map[p.x][p.y]->TileMark == Tile::Mark::Blank)
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

void Level::CalcMovementFlood(Position start, int unitMove, int options)
{
	queue<Position> pathQueue;		// Queue of tiles we are going to calculate the movement path to
	vector<Position> test;
	Position cur = start, p;		// Position of current tile being examined and temp position
	Position directions[4];			// Tell which direction we check 1st when calculation path
	int i, curLength;

	// pointer to evaluation function
	bool (Level::*ObstructedFunction)(Position) = &Level::IsObstructed;
	if(options & TEST_OBSTRUCTION_ALLY_UNITS)
		ObstructedFunction = &Level::IsObstructedPlayer;
	else if(options & TEST_OBSTRUCTION_ENEMY_UNITS)
		ObstructedFunction = &Level::IsObstructedEnemy;

	// Fill in base-case movementMap and add 1st entry to queue
	map[start.x][start.y]->SaveMark();
	map[start.x][start.y]->SetMark(Tile::Mark::AllyMovePath);
	movementMap[start.x][start.y].push_front(start);

	// Push 4 tiles surrounding current tile onto queue
	if(IsValidPosition(cur.x+1, cur.y) && !(*this.*ObstructedFunction)(Position(cur.x+1, cur.y))) 
		pathQueue.push(Position(cur.x+1, cur.y));
	if(IsValidPosition(cur.x-1, cur.y) && !(*this.*ObstructedFunction)(Position(cur.x-1, cur.y))) 
		pathQueue.push(Position(cur.x-1, cur.y));
	if(IsValidPosition(cur.x, cur.y+1) && !(*this.*ObstructedFunction)(Position(cur.x, cur.y+1))) 
		pathQueue.push(Position(cur.x, cur.y+1));
	if(IsValidPosition(cur.x, cur.y-1) && !(*this.*ObstructedFunction)(Position(cur.x, cur.y-1))) 
		pathQueue.push(Position(cur.x, cur.y-1));

	while(!pathQueue.empty())
	{
		// Take next tile in queue
		cur = pathQueue.front();

		// Choose the correct direction priorities
		CalcDirectionPriority(start, cur, directions);

		// Pick shortest already calculated path from the surrounding tiles as a start
		curLength = 1000;
		for(i = 0; i < 4; i++)
		{
			p = cur - directions[i];
			directions[i].x = directions[i].y = 0;		// Reset directions before next tile
			if(!IsValidPosition(p) || p.DistanceTo(start) > unitMove) continue;			// Tile cannot be moved to, skip it
			else if(!(*this.*ObstructedFunction)(p))									// Is tile open to be moved onto?
			{
				if(movementMap[p.x][p.y].empty())										// Has path to tile has  been calculated yet?
				{																			// No
					if(std::find(test.begin(), test.end(), p) == test.end())				// Has this tile already been added to the queue?
					{																			// No
						pathQueue.push(p);													// Push it onto queue to calculate later
						test.push_back(p);													// Track it
					}
				}																			// Yes
				else if(movementMap[p.x][p.y].size() > 0 &&								// Does path to p exist? (it won't if p is out of movement range)
					movementMap[p.x][p.y].size() <  curLength)							// Is path to p the shortest path adjacent to current tile?
				{

					movementMap[cur.x][cur.y] = movementMap[p.x][p.y];					// Set path for now equal to the shortest adjacent path
					curLength = movementMap[cur.x][cur.y].size();						// Save length of current path
				}
			}
		}

		// Add current tile onto end of path to itself, mark tile 
		//	(only if the path to it isn't greater than the unit's movement)
		if(movementMap[cur.x][cur.y].size() <= unitMove && movementMap[cur.x][cur.y].size() > 0)
		{
			// Push current position onto end of path to current tile and mark it
			movementMap[cur.x][cur.y].push_back(cur);
			if(map[cur.x][cur.y]->TileMark == Tile::Mark::Blank)
			{
				map[cur.x][cur.y]->SaveMark();
				if(options & TILE_MARK_TYPE_ENEMY_MOVEMENT)
					map[cur.x][cur.y]->SetMark(Tile::Mark::EnemyMove);
				else
					map[cur.x][cur.y]->SetMark(Tile::Mark::AllyMove);
			}
		}
		else // Otherwise, it can't be moved to, clear it
			movementMap[cur.x][cur.y].clear();

		// Pop current tile off queue, path to it has been calculated
		pathQueue.pop();
	}

}

// Decide which directions to favor for path-construction based on start/end locations - makes it "more dynamic" because I feel like it
void Level::CalcDirectionPriority(Position start, Position end, Position* directions)
{
	// Direction selection prioritization based off difference between desired position and current position
	Position diff = end - start;

	if(abs(diff.x) > abs(diff.y))	// left or right - prioritize vertical > horizontal
	{
		directions[0].y = diff.y != 0 ? ((diff.y) / (abs(diff.y))) * -1 : ((diff.x) / (abs(diff.x))) * -1;
		directions[1].y = diff.y != 0 ? diff.y / abs(diff.y) : diff.x / abs(diff.x);
		directions[2].x = ((diff.x) / (abs(diff.x))) * -1;
		directions[3].x = diff.x / abs(diff.x);
	}
	else if(abs(diff.x) < abs(diff.y)) // up or down - prioritize horizontal > vertical
	{
		directions[0].x = diff.x != 0 ? ((diff.x) / (abs(diff.x))) * -1 : ((diff.y) / (abs(diff.y))) * -1;
		directions[1].x = diff.x != 0 ? diff.x / abs(diff.x) : diff.y / abs(diff.y);
		directions[2].y = ((diff.y) / (abs(diff.y))) * -1;
		directions[3].y = diff.y / abs(diff.y);
	}
	else if(diff.x == diff.y && diff.x > 0) // diagonal up/right - prioritize down > left > up > right
	{	directions[0].y = directions[1].x = -1; directions[2].y = directions[3].x = 1; }
	else if(diff.x == diff.y && diff.x < 0) // diagonal down/left - prioritize up > right > down > left
	{	directions[0].y = directions[1].x = 1; directions[2].y = directions[3].x = -1; }
	else if(diff.x == (diff.y * -1) && diff.x > 0) // diagonal down/right - prioritize left > up > right > down
	{	directions[0].x = -1; directions[1].y = 1; directions[2].x = 1; directions[3].y = -1; }
	else if(diff.x == (diff.y * -1) && diff.x < 0) // diagonal up/left - prioritize right > down > left > up
	{	directions[0].x = 1; directions[1].y = -1; directions[2].x = -1; directions[3].y = 1; }
}

// Visual Mark all tiles in range of a given action (movement, ability cast, etc.)
// Mark Type 0 = Attack, 1 = Ally Ability Range, 2 = Ally Ability AoE, 3 = Enemy Movement
void Level::MarkTilesInRange(bool undo, Position start, int range, int markType)
{
	// Search through all tiles in range
	for(int i = start.x - range; i <= start.x + range; i++)
	{
		if(i < 0) continue; if(i >= mapWidth) break;
		for(int j = start.y - range; j <= start.y + range; j++)
		{
			if(j < 0) continue; if(j >= mapHeight) break;

			// Mark/Unmark tiles within the movement range of the currently selected unit
			if(Position(i, j).DistanceTo(start) <= range)
			{ 
				if(!undo)
				{
					// Save mark for simple undo later
					map[i][j]->SaveMark();

					// Mark tile - Visual Change will be done by the tile itself
					switch(markType)
					{
						case TILE_MARK_TYPE_ATTACK:
							if(map[i][j]->TileStatus == Tile::Status::EnemyUnit)
								map[i][j]->SetMark(Tile::Mark::Attack);
							break;
						case TILE_MARK_TYPE_ALLY_ABILITY_RANGE:
							map[i][j]->SetMark(Tile::Mark::AllyAbilityRange);
							break;
						case TILE_MARK_TYPE_ALLY_BATTLE_ABILITY_RANGE:
							if(map[i][j]->TileStatus == Tile::Status::EnemyUnit)
								map[i][j]->SetMark(Tile::Mark::AllyAbilityRange);
							break;
					}
				}
				else	// Undo any markings - Just reset to prev tile status
					map[i][j]->ResetMark();
			}
		}
	}
}

// Mark a set of tiles with a given markType. The set can be absolute Positions or relative Positions given a starting Position
void Level::MarkTiles(bool undo, int markType, vector<Position> tileSet, Position start/*=Position(0, 0)*/)
{
	Tile::Mark mark;
	switch(markType)
	{
		case TILE_MARK_TYPE_MOVEMENT:	mark = Tile::Mark::AllyMove;	break;
		case TILE_MARK_TYPE_ATTACK:		mark = Tile::Mark::Attack;		break;
		case TILE_MARK_TYPE_ALLY_ABILITY_RANGE:
		case TILE_MARK_TYPE_ALLY_BATTLE_ABILITY_RANGE:	mark = Tile::Mark::AllyAbilityRange;	break;
		case TILE_MARK_TYPE_ALLY_ABILITY_AOE:	mark = Tile::Mark::AllyAbilityAoE;	break;
		case TILE_MARK_TYPE_ENEMY_MOVEMENT:		mark = Tile::Mark::EnemyMove;		break;
	}

	if(!undo)
	{
		for(int k = 0; k < tileSet.size(); k++)
		{
			if(!IsValidPosition(start.x + tileSet[k].x, start.y + tileSet[k].y)) continue;
			map[start.x + tileSet[k].x][start.y + tileSet[k].y]->SaveMark();
			map[start.x + tileSet[k].x][start.y+  tileSet[k].y]->SetMark(mark);
		}
	}
	else
	{
		for(int k = 0; k < tileSet.size(); k++)
		{
			if(!IsValidPosition(start.x + tileSet[k].x,start.y + tileSet[k].y)) continue;
			map[start.x + tileSet[k].x][start.y + tileSet[k].y]->ResetMark();
		}
	}
}

void Level::CreateActionMenu()
{
	actionMenu = new MenuBox(this, L"../FelledTactics/Textures/MenuBackground.png", ACTION_MENU_LAYER, 100, 200, currentUnitPosition.x*tileSize + tileSize*2, currentUnitPosition.y*tileSize, 1, 4);
	actionMenu->CreateElement(&Level::SelectAttack, L"../FelledTactics/Textures/MenuAttack.png");//, 80, 45, 10, 145);
	actionMenu->CreateElement(&Level::SelectAbility, L"../FelledTactics/Textures/MenuSkills.png");//, 80, 45, 10, 100);
	actionMenu->CreateElement(&Level::SelectItem, L"../FelledTactics/Textures/MenuItems.png");//, 80, 45, 10, 55);
	actionMenu->CreateElement(&Level::ActivateEndTurn, L"../FelledTactics/Textures/MenuEnd.png");//, 80, 45, 10, 10);
	
	AddVisualElement(actionMenu);
//	SortVisualElements();

	// Set active layers to ignore everything but this Menu
	RemoveActiveLayer(TILE_LAYER);
	AddActiveLayer(ACTION_MENU_LAYER);
	currentPhase = Phase::SelectPrimaryAction;
}

// Player has selected for a Unit to attack
void Level::SelectAttack()
{
	// Mark all enemies in range of unit - Loop through all tiles in attack range of unit and check for enemies
	MarkTilesInRange(false, currentUnitPosition, unitMap[currentUnitPosition.x][currentUnitPosition.y]->AttackRange, TILE_MARK_TYPE_ATTACK);

	// Set 1st Combatant in the CombatCalculator
	combatManager.SetAttacker(unitMap[currentUnitPosition.x][currentUnitPosition.y]);

	// Go to Next Phase - Select Target for attack
	currentPhase = Phase::SelectTarget;
	actionMenu->DisableDraw();
	RemoveActiveLayer(ACTION_MENU_LAYER);	// Change active layers
	AddActiveLayer(TILE_LAYER);				//	to the tiles to allow player to select a target
}

void Level::SelectAbility()
{
	int numAbilities = unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetNumActiveAbilities();
	secondaryMenu = new MenuBox(this, L"../FelledTactics/Textures/MenuBackground.png", SECONDARY_MENU_LAYER, 100, 200, currentUnitPosition.x*tileSize + tileSize*3, currentUnitPosition.y*tileSize + tileSize/2, 1, numAbilities);
	
	for(int i = 0; i < numAbilities; i++)
	{
		secondaryMenu->CreateElement(&Level::ActivateAbility, i, 
			L"../FelledTactics/Textures/MenuBackgrounds.png", 
			unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetAbilityName(i));
	}

	currentPhase = Phase::SelectSecondaryAction;
	AddVisualElement(secondaryMenu);
//	SortVisualElements();

	// Set active layers to ignore everything but this Menu
	RemoveActiveLayer(ACTION_MENU_LAYER);
	AddActiveLayer(SECONDARY_MENU_LAYER);
}

void Level::SelectItem()
{
	secondaryMenu = new MenuBox(this, L"../FelledTactics/Textures/MenuBackground.png", SECONDARY_MENU_LAYER, 100, 200, 950, 100, 1, 4);

	currentPhase = Phase::SelectSecondaryAction;
	AddVisualElement(secondaryMenu);
//	SortVisualElements();

	// Set active layers to ignore everything but this Menu
	RemoveActiveLayer(ACTION_MENU_LAYER);
	AddActiveLayer(SECONDARY_MENU_LAYER);
}

void Level::ActivateEndTurn()
{
	// End turn of current unit
	if(
#if defined GOD_MODE_ALLY && defined GOD_MODE_ENEMY
		false)
#elif defined GOD_MODE_ALLY
		!unitMap[currentUnitPosition.x][currentUnitPosition.y]->CheckStatus(UNIT_STATUS_ALLY))
#elif defined GOD_MODE_ENEMY
		!unitMap[currentUnitPosition.x][currentUnitPosition.y]->CheckStatus(UNIT_STATUS_ENEMY))
#else
		true)
#endif
	{
		unitMap[currentUnitPosition.x][currentUnitPosition.y]->FinishTurn();
		numUnitsMoved++;
	}
	else
		unitMap[currentUnitPosition.x][currentUnitPosition.y]->NewTurn(L);

	currentUnitPosition.Reset();

	// Delete action menu
	Delete<MenuBox>(actionMenu);
	if(secondaryMenu)
		Delete<MenuBox>(secondaryMenu);
	RemoveActiveLayer(ACTION_MENU_LAYER);
	AddActiveLayer(TILE_LAYER);

	lua_settop(L, 0);

	// Go to next phase
	if(numUnitsMoved == numAllies)
		currentPhase = Phase::EnemyTurn;
	else
		currentPhase = Phase::SelectUnit;
}

void Level::CreateCombatUI()
{

}

void Level::ActivateAbility(int selectedAbility)
{
	// Set Selected Ability
	unitMap[currentUnitPosition.x][currentUnitPosition.y]->SetSelectedAbility(selectedAbility);

	// Check if unit has enough AP to cast the selected ability
	if(unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityCost() > unitMap[currentUnitPosition.x][currentUnitPosition.y]->AbilityPoints)
		return;

	// Activate Battle Abilities NOW. The costs/effects are reset if they are canceled before a target is selected
	if(unitMap[currentUnitPosition.x][currentUnitPosition.y]->SelectedBattleAbility())
	{
		lua_pushlightuserdata(L, (void*)this);
		lua_setglobal(L, "Level");
		unitMap[currentUnitPosition.x][currentUnitPosition.y]->ActivateAbility(L, currentUnitPosition);

		// Set 1st Combatant in the CombatCalculator
		combatManager.SetAttacker(unitMap[currentUnitPosition.x][currentUnitPosition.y]);

		// Mark Range of Battle Ability
		MarkTilesInRange(false, currentUnitPosition, unitMap[currentUnitPosition.x][currentUnitPosition.y]->AttackRange, TILE_MARK_TYPE_ALLY_BATTLE_ABILITY_RANGE);
	}
	else if(unitMap[currentUnitPosition.x][currentUnitPosition.y]->SelectedAbilityHasDynamicAoE())	// Mark Range of Dynamic Ability
		MarkTiles(false, TILE_MARK_TYPE_ALLY_ABILITY_RANGE, unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityDynamicAoERange(), currentUnitPosition);
	else	// Mark Range of regular Ability
		MarkTilesInRange(false, currentUnitPosition, unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityRange(), TILE_MARK_TYPE_ALLY_ABILITY_RANGE);

	// Mark AoE of ability - Range = Attack Range if Battle Ability and no AoE
	if(map[hoveredTile.x][hoveredTile.y]->TileMark == Tile::Mark::AllyAbilityRange)
		MarkTiles(false, TILE_MARK_TYPE_ALLY_ABILITY_AOE, unitMap[currentUnitPosition.x][currentUnitPosition.y]->GetSelectedAbilityAoE(hoveredTile-currentUnitPosition), hoveredTile);

	// Allow targeting on tiles
	RemoveActiveLayer(SECONDARY_MENU_LAYER);
	AddActiveLayer(TILE_LAYER);

	// Turn off menus for targeting
	actionMenu->DisableDraw();
	secondaryMenu->DisableDraw();

	// Move to proper gameplay phase
	currentPhase = Phase::SelectAbilityTarget;
}

void Level::CreateCombatText(Position target, Position source, const char* t, int damageType)
{
	combatText.push_back(new CombatText(COMBAT_TEXT_LAYER, CombatText::CalcCombatTextPosition(target, source), CombatText::CalcCombatTextMovement(target, source), t, damageType));
	AddVisualElement(combatText[combatText.size()-1]);
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
//	if(lastHoveredTile.y == -1)
//		lastHoveredTile = p;
}

void Level::ClearHoveredTile(Position p)
{
	if(hoveredTile == p)
		hoveredTile.x = hoveredTile.y = -1;
}

void Level::Draw()
{
	GameMaster::Draw();
}

#pragma region Properties
Tile*** Level::GetMap() { return map; }
Unit*** Level::GetUnits() { return unitMap; }
int	  Level::GetWidth() { return mapWidth; }
void  Level::SetWidth(int w) { mapWidth = w; }
int	  Level::GetHeight() { return mapHeight; }
void  Level::SetHeight(int h) { mapHeight = h; }
Tile* Level::GetTile(int x, int y) { return IsValidPosition(x,y) ? map[x][y] : NULL; }
Unit* Level::GetUnit(int x, int y) { return IsValidUnit(x,y) ? unitMap[x][y] : NULL; }
Unit* Level::GetEnemyUnit(int x, int y) { return IsValidUnit(x,y) && unitMap[x][y]->IsEnemy() ? unitMap[x][y] : NULL; }
Unit* Level::GetAllyUnit(int x, int y) { return IsValidUnit(x,y) && unitMap[x][y]->IsAlly() ? unitMap[x][y] : NULL; }
bool  Level::IsOccupied(int x, int y) { return IsValidPosition(x,y) ? map[x][y]->IsObstructed() : true; }
int   Level::GetOccupantID(int x, int y) { return IsOccupied(x,y) ? unitMap[x][y]->GetUnitID() : -1; }
#pragma endregion                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            