#include "StdAfx.h"
#include "Unit.h"

int Unit::unitCounter = 0;
float Unit::BASE_MOVE_TIME = 0.25f;
D3DXVECTOR4 Unit::highlightFinishedTurn = D3DXVECTOR4(0.328f, 0.328f, 0.328f, 1.0f);

Unit::Unit(WCHAR* filename, int layer, int width, int height, int posX, int posY, bool ally/*=true*/) : 
	VisualElement(filename, layer, width, height, posX, posY), felled(false), finishedTurn(false), secBetweenTiles(BASE_MOVE_TIME), movementTimer(0),
	movementFinished(true), status(0), numAbilities(0), numActionAbilities(0), numBattleAbilities(0)
{
	InitProficiency();
	position.x = posX / width;		// width and height are set to TileSize
	position.y = posY / height;		// PosX and PosY are units from origin to bottom-left corner of tile they are at
	unitID = unitCounter++;			// each unit has ID number in order of their creation
	
	if(ally)
		ApplyStatus(UNIT_STATUS_ALLY);
	else
		ApplyStatus(UNIT_STATUS_ENEMY);

	// Test stats
	movement = 8;
	health = maximumHealth = 100;
	abilityPoints = maximumAbilityPoints = 50;
	strength = magic = skill = agility = defense = resistance = 10;
	attackRange = 5;
	inventory += new Weapon(Weapon::WeaponClass::Bow, 30, 0, 5);

	// Test Abilities
	numAbilities = numActionAbilities = numBattleAbilities = 0;
	LearnAbility("Damage", 3);
	LearnAbility("Heal");
	LearnAbility("Double Strike");
	LearnAbility("Fire");
	LearnAbility("Vector Plate");

	// Initialize matrix, buffers and textures for HP/AP bars
	hpapHeight = height * 0.1f;
	D3DXMatrixIdentity(&hpapWorld);
	InitializeHPAPBuffers();
	hr = D3DX10CreateShaderResourceViewFromFile(Direct3D::gpInfo->gpDevice, L"../FelledTactics/Textures/health.png", 0, 0, &hpBarTexture, 0);
	hr = D3DX10CreateShaderResourceViewFromFile(Direct3D::gpInfo->gpDevice, L"../FelledTactics/Textures/ap.png", 0, 0, &apBarTexture, 0);
	updateHPAPBuffers = true;	// always do update on first draw
	drawBars = false;
}

Unit::Unit(int layer, int width, int height, int posX, int posY, const char* name, json_spirit::mObject unitMap, json_spirit::mObject abilityMap) :
	VisualElement(L"", layer, width, height, posX, posY), felled(false), finishedTurn(false), secBetweenTiles(BASE_MOVE_TIME), movementTimer(0),
	movementFinished(true), status(0), numAbilities(0), numActionAbilities(0), numBattleAbilities(0)
{
	strcpy_s(this->name, name);
	string unitClass = unitMap["Class"].get_str();
	experience = unitMap["Experience"].get_int();
	health = maximumHealth = unitMap["Health"].get_int();
	abilityPoints = maximumAbilityPoints = unitMap["AP"].get_int();
	strength = unitMap["Strength"].get_int();
	magic = unitMap["Magic"].get_int();
	skill = unitMap["Skill"].get_int();
	agility = unitMap["Agility"].get_int();
	defense = unitMap["Defense"].get_int();
	resistance = unitMap["Resistance"].get_int();
	movement = unitMap["Movement"].get_int();

	// Abilities
	string abilities = unitMap["Abilities"].get_str();
	int x = 0, y = -1, rank = 1;
	while(x >= 0)
	{
		x = abilities.find("|");
		string ability = abilities.substr(0, x);
		y = ability.find(":");
		if(y != -1)
		{
			rank = atoi(ability.substr(y+1).c_str());
			ability = ability.substr(0, y);
		}
		LearnAbility(ability.c_str(), abilityMap.find(ability)->second.get_obj(), rank);
		abilities = abilities.substr(x+1);
		rank = 1;
	}

	// temporary weapon for now
	attackRange = 5;
	inventory += new Weapon(Weapon::WeaponClass::Bow, 30, 0, 5);

	InitProficiency();
	ApplyStatus(UNIT_STATUS_ALLY);
	SetTexture(L"../FelledTactics/Textures/Units/" + wstring(unitClass.begin(), unitClass.end()) + L".png");
	position.x = posX / width;		// width and height are set to TileSize
	position.y = posY / height;		// PosX and PosY are units from origin to bottom-left corner of tile they are at
	unitID = unitCounter++;			// each unit has ID number in order of their creation

	// Initialize matrix, buffers and textures for HP/AP bars
	hpapHeight = height * 0.1f;
	D3DXMatrixIdentity(&hpapWorld);
	InitializeHPAPBuffers();
	hr = D3DX10CreateShaderResourceViewFromFile(Direct3D::gpInfo->gpDevice, L"../FelledTactics/Textures/health.png", 0, 0, &hpBarTexture, 0);
	hr = D3DX10CreateShaderResourceViewFromFile(Direct3D::gpInfo->gpDevice, L"../FelledTactics/Textures/ap.png", 0, 0, &apBarTexture, 0);
	updateHPAPBuffers = true;	// always do update on first draw
	drawBars = false;
}

Unit::~Unit(void)
{
	hpVertexBuffer->Release(); 	hpVertexBuffer = 0;
	apVertexBuffer->Release(); 	apVertexBuffer = 0;
	hpBarTexture->Release();    hpBarTexture = 0;
	apBarTexture->Release();    apBarTexture = 0;

	for(UINT i = 0; i < activeAbilityList.size(); i++)
	{
		delete activeAbilityList[i];
		activeAbilityList[i] = NULL;
	}
	for(UINT j = 0; j < passiveAbilityList.size(); j++)
	{
		delete passiveAbilityList[j];
		passiveAbilityList[j] = NULL;
	}
	activeAbilityList.clear();
	passiveAbilityList.clear();
}

// Pre-set all efficiency values to -1, denoting that a unit cannot use any skills of those classes.
// Proper efficiencies are later set separately per class
void Unit::InitProficiency()
{
	bladedgeProficiency = axereaverProficiency = pheylanceProficiency = rangerProficiency = riderProficiency = bruiserProficiency = elementalistProficiency = 
	kinectorProficieny = arcaneweaverProficiency = blooddancerProficiency = shamanProficiency = _proficiency = healerProficiency = inflicterProficiency = 
	enchanterProficiency = enforcerProficiency = -1.0f;
}

void Unit::CalculateCombatDamage(int& physicalDamage, int& magicalDamage, int range = 0)
{
	// Non-bow weapons cannot do damage at range
	Weapon* equippedWeapon = inventory[0]->Class == Item::Weapon ? dynamic_cast<Weapon*>(inventory[0]) : NULL;
	if(range > 1 && equippedWeapon->GetWeaponClass() != Weapon::Bow)
	{
		physicalDamage = magicalDamage = 0;
		return;
	}

	// Calculate base damage
	int baseDamage = strength + inventory[0]->CalculateBaseDamage();

	// Factor in range if necessary - Bow damage varies by range
	if(equippedWeapon != NULL && equippedWeapon->GetWeaponClass() == Weapon::Bow)
	{
		switch(range)
		{
			case 1:		// Immediate half-damage with direct combat
				baseDamage = baseDamage * 3 / 6;
				break;
			case 2:		// Maximum damage at 2 spaces away
				break;
			case 3:		// Linear drop off of damage 83.33%
				baseDamage = baseDamage * 5 / 6;
				break;
			case 4:		// Linear drop off of damage down to 66.67%
				baseDamage = baseDamage * 4 / 6;
				break;
			case 5:		// Linear drop off of damage down to 50%
				baseDamage = baseDamage * 3 / 6;
			break;
		}
	}

	// Factor in bonus damage/effects here

	// Set final damages
	physicalDamage = baseDamage;
	magicalDamage = 0;
}

// Unit takes damage that is un-affected by their resistances
// Also known as "True Damage"
// Return total damage done to the unit (value is negative if unit died)
int Unit::TakeUnscaledDamage(int damage)
{
	int curHealth = health;		// Save current health if damage taken kills unit
	health -= damage;			// Take the damage
	updateHPAPBuffers = true;

	// Unit has been killed
	if(health <= 0)				
		return -1 * curHealth;		// return actual damage taken

	// Unit survived
	return damage;				// return damage taken
}

// Unit takes damage that is countered by it's resistances
// Return total damage done to the unit (value is negative if unit died)
int Unit::TakeDamage(int physDamage, int magDamage)
{
	int curHealth = health;				// Save current health if damage taken kills unit
	int phys = physDamage - defense;	// Calculate actual damage done
	int mag  = magDamage - resistance;	//		against defense/resistance

	// Don't take negative damage if defenses are higher than damage given
	if(phys < 0) phys = 0;
	if(mag < 0) mag = 0;

	health -= phys + mag;				// Take the damage
	updateHPAPBuffers = true;

	// Unit has been killed
	if(health <= 0)
		return -curHealth;			// return actual damage taken

	// Unit survived
	return phys + mag;
}

// Unit regains HP and returns the amount gained
int Unit::Heal(int hp)
{
	int healed = maximumHealth - health;
	updateHPAPBuffers = true;

	health += hp;
	if(health > maximumHealth)
	{
		health = maximumHealth;
		return healed;
	}

	return hp;
}

void Unit::Revive(int health)
{
	this->health = health;
	RemoveStatus(UNIT_STATUS_FELLED);
	felled = false;
}

float Unit::Die()
{
	ApplyStatus(UNIT_STATUS_DYING);
	felled = true;

	// Play death animation

	// Return death animation time
	return 1.5f;
}

void Unit::GainExperience()
{
}

// A Unit has surpassed 100 experience and levels up.
// Increase Stats based on percentages.
// Different classes will have different amounts that their stats can level up by at one time
void Unit::LevelUp()
{
	int result = -1, index = 0;

	while(index < 8)
	{
		// Obtain the next roll for stat growth
		result = rand() % 100;

		switch(index)
		{
		case 0:			// Health
			if(result < hpGrowth)
				health++;
			break;
		case 1:			// Ability Points
			if(result < apGrowth)
				abilityPoints++;
			break;
		case 2:			// Strength
			if(result < strGrowth)
				strength++;
			break;
		case 3:			// Magic
			if(result < magGrowth)
				magic++;
			break;
		case 4:			// Agility
			if(result < aglGrowth)
				agility++;
			break;
		case 5:			// Dexterity
			if(result < sklGrowth)
				skill++;
			break;
		case 6:			// Defence
			if(result < defGrowth)
				defense++;
			break;
		case 7:			// Resistance
			if(result < resGrowth)
				resistance++;
			break;
		}
	}
}

// Save the path a unit is told to move for its movement phase
void Unit::SetMovePath(list<Position> path, float moveTime/*=0*/) 
{ 
	// convert path from tile units to pixels and save
	for(std::list<Position>::const_iterator iterator = path.begin(); iterator != path.end(); ++iterator)
		movementPath.push_back(*iterator * tileSize);

	movementFinished = false;
	if(moveTime != 0)
		secBetweenTiles = moveTime;
	ApplyStatus(UNIT_STATUS_MOVING);
}

// Force a unit to move to a specific tile (by ability/tile effect)
void Unit::ForceMovement(Position p, float moveTime)
{
	// Only allow forced movement in a specific direction
	if(position.x - p.x != 0 && position.y - p.y != 0)
		return;

	Position move = p - position, temp = position;
	if(move.x != 0)	move.x /= abs(move.x);
	if(move.y != 0) move.y /= abs(move.y);

	movementPath.clear();
	movementPath.push_back(temp * tileSize);
	while(temp != p)
	{
		temp += move;
		movementPath.push_back(temp * tileSize);
	}

	// Save new movement speed for this forced movement
	secBetweenTiles = moveTime;
	ApplyStatus(UNIT_STATUS_FORCED_MOVING);
}

// Force a unit to end its movement phase prematurely
void Unit::ForceEndMovement()
{
	movementPath.clear();
	movementPath.push_back(position * tileSize);
}

void Unit::FinishTurn()
{
	finishedTurn = true;
	if(!CheckStatus(UNIT_STATUS_FELLED))
		highlightColor = highlightFinishedTurn;
}

void Unit::NewTurn(lua_State* L)
{
	finishedTurn = false;
	if(!CheckStatus(UNIT_STATUS_FELLED))
		highlightColor = highlightNone;
}

#pragma region Abilities
void Unit::LearnAbility(const char* name, int forceRank/*=1*/)
{
	if(forceRank == 0)
	{
		for(auto i = activeAbilityList.begin(); i != activeAbilityList.end(); i++)
		{
			if(!strcmp((*i)->Name, name))
			{
				if((*i)->RankUp())	// ranked up
				{
				}
				else{}				// cannot rank up anymore

				return;
			}
		}

		for(auto i = passiveAbilityList.begin(); i != passiveAbilityList.end(); i++)
		{
			if(!strcmp((*i)->Name, name))
			{
				if((*i)->RankUp())	// ranked up
				{
				}
				else{}				// cannot rank up anymore

				return;
			}
		}
	}

	// LEARN NEW ABILITY HERE
	Ability* a = new Ability(name, forceRank);

	numAbilities++;
	switch(a->AbilityType)
	{
		case Ability::Type::Passive:
			passiveAbilityList.push_back(a);
			break;
		case Ability::Type::Action:
			numActionAbilities++;
			numBattleAbilities--;
		case Ability::Type::Battle:
			numBattleAbilities++;
			activeAbilityList.push_back(a);
			break;
	}
	a = NULL;
}

void Unit::LearnAbility(const char* name, json_spirit::mObject abilityMap, int forceRank/*=1*/)
{
	if(forceRank == 0)
	{
		for(auto i = activeAbilityList.begin(); i != activeAbilityList.end(); i++)
		{
			if(!strcmp((*i)->Name, name))
			{
				if((*i)->RankUp())	// ranked up
				{
				}
				else{}				// cannot rank up anymore

				return;
			}
		}

		for(auto i = passiveAbilityList.begin(); i != passiveAbilityList.end(); i++)
		{
			if(!strcmp((*i)->Name, name))
			{
				if((*i)->RankUp())	// ranked up
				{
				}
				else{}				// cannot rank up anymore

				return;
			}
		}
	}

	// LEARN NEW ABILITY HERE
	Ability* a = new Ability(name, abilityMap, forceRank);

	numAbilities++;
	switch(a->AbilityType)
	{
		case Ability::Type::Passive:
			passiveAbilityList.push_back(a);
			break;
		case Ability::Type::Action:
			numActionAbilities++;
			numBattleAbilities--;
		case Ability::Type::Battle:
			numBattleAbilities++;
			activeAbilityList.push_back(a);
			break;
	}
	a = NULL;
}

// Check if the selected ability is a Battle ability (not action or passive)
bool Unit::SelectedBattleAbility() const { return activeAbilityList[selectedAbility]->AbilityType == Ability::Type::Battle; }

// Check if selected ability has a Dynamic AoE
bool Unit::SelectedAbilityHasDynamicAoE() const { return activeAbilityList[selectedAbility]->HasDynamicAoE(); }

// Set a new ability as the currently selected ability
void Unit::SetSelectedAbility(int index) { selectedAbility = index; }

// Get specific parameters of the currently selected ability
char* Unit::GetSelectedAbilityName() const { return activeAbilityList[selectedAbility]->Name; }
int   Unit::GetSelectedAbilityCost() const { return activeAbilityList[selectedAbility]->APCost; }
int	  Unit::GetSelectedAbilityRange() const { return activeAbilityList[selectedAbility]->AbilityType == Ability::Type::Battle ? attackRange : activeAbilityList[selectedAbility]->Range; }
Ability::CastType Unit::GetSelectedAbilityCastType() const { return activeAbilityList[selectedAbility]->AbilityCastType; }
vector<Position> Unit::GetSelectedAbilityAoE(Position p/*=Position(0, 0)*/) const { return activeAbilityList[selectedAbility]->GetAoE(p); }
vector<Position> Unit::GetSelectedAbilityDynamicAoERange() const { return activeAbilityList[selectedAbility]->GetDynamicAoERange(); }
const float* Unit::GetSelectedAbilityTimers() const { return activeAbilityList[selectedAbility]->GetTimers(); }

// Activate ability (if possible) at target location.
void Unit::ActivateAbility(lua_State* L, Position target)
{
	activeAbilityList[selectedAbility]->Activate(L, target, position);
	abilityPoints -= activeAbilityList[selectedAbility]->APCost;
	updateHPAPBuffers = true;
}

// Battle Abilities are activated BEFORE their effects go through and can be canceled before as well
// Call this function to refund their cost after activation if they are canceled before combat occurs
void Unit::RefundAP()
{
	abilityPoints += activeAbilityList[selectedAbility]->APCost;
}

// Reset Battle Ability Scripts after they have been used
void Unit::ClearBattleScripts()
{
	combatCalculationAbilityScript = "Lua\\Combat\\Base_CalcCombat.lua";
	combatExecutionAbilityScript = "";
}

bool Unit::HasAbility(const char* name)
{

	return false;
}

char* Unit::GetAbilityName(int index) const { return activeAbilityList[index]->Name; }

// Get number of abilities
int Unit::GetNumActiveAbilities() const { return numActionAbilities + numBattleAbilities; }
int Unit::GetNumAbilities() const { return numAbilities; }
#pragma endregion

bool Unit::InitializeHPAPBuffers()
{
	Vertex* verts = new Vertex[4];
	memset(verts, 0, sizeof(Vertex) * 4);

	// Describes the vertex buffer we are creating
	D3D10_BUFFER_DESC vBufferDesc;
		ZeroMemory(&vBufferDesc, sizeof(vBufferDesc));
		vBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
		vBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vBufferDesc.ByteWidth = sizeof(Vertex) * 4;
		vBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		vBufferDesc.MiscFlags = 0;

	// Specify the data we are initializing the vertex buffer with
	D3D10_SUBRESOURCE_DATA vInitData;
		vInitData.pSysMem = verts;

	// Create the vertex buffer
	hr = Direct3D::gpInfo->gpDevice->CreateBuffer(&vBufferDesc, &vInitData, &hpVertexBuffer);
	if(FAILED(hr)) return hr;
	hr = Direct3D::gpInfo->gpDevice->CreateBuffer(&vBufferDesc, &vInitData, &apVertexBuffer);
	if(FAILED(hr)) return hr;


	delete [] verts;
	verts = 0;

	return true;
}

// Update Statuses: 0 - None, 1 - Movement between 2 tiles fini shed, 2 - Dead
int Unit::Update(float dt)
{
	if(CheckStatus(UNIT_STATUS_DYING))
	{
		highlightColor.w -= (dt / 1.5f) * 0.8f;
		if(highlightColor.w <= 0.2f)
		{
			if(CheckStatus(UNIT_STATUS_ALLY | UNIT_STATUS_BOSS))
			{
				highlightColor.w = 0.2f;
				RemoveStatus(UNIT_STATUS_DYING);
				ApplyStatus(UNIT_STATUS_FELLED);
				if(CheckStatus(UNIT_STATUS_BOSS))
					return UNIT_UPDATE_DEAD;
				felled = true;
			}
			return UNIT_UPDATE_DEAD;
		}
		return 0;
	}

	// Do movement
	if(movementPath.size() > 1)
	{
		movementTimer += dt;
		std::list<Position>::const_iterator currentTile = movementPath.begin();
		std::list<Position>::const_iterator nextTile = currentTile; ++nextTile;

		// Move between 2 tiles.
		if(movementTimer < secBetweenTiles)
		{
			Position newPos = (*currentTile) + ((*nextTile - *currentTile) * (movementTimer / secBetweenTiles));
			Translate(D3DXVECTOR3(newPos.x - leftCorner.x, newPos.y - leftCorner.y, 0));
		}
		else  // 2nd tile is reached, prepare to move to next tile
		{
			Translate(D3DXVECTOR3(nextTile->x - leftCorner.x, nextTile->y - leftCorner.y, 0));
			leftCorner = *nextTile;		// set position exactly on next tile
			movementPath.pop_front();	// remove the tile we just moved from
			movementTimer = 0;			// reset timer for next set of movement
			position = movementPath.front() / tileSize;

			// Movement finished
			if(movementPath.size() == 1)
			{
				movementFinished = true;
				secBetweenTiles = BASE_MOVE_TIME;
				movementPath.clear();
				updateHPAPBuffers = true;
				RemoveStatus(UNIT_STATUS_MOVING);
				RemoveStatus(UNIT_STATUS_FORCED_MOVING);
				return UNIT_UPDATE_MOVEMENT_END;
			}

			return UNIT_UPDATE_TILE_REACHED;
		}

		// Update vertex buffers on movement
		updateHPAPBuffers = true;
	}

	return UNIT_UPDATE_NULL;
}

void Unit::ApplyStatus(int s) {	status |= s; }
void Unit::RemoveStatus(int s) { status &= ~s; }
bool Unit::CheckStatus(int s) { return status & s; }
bool Unit::IsAlly() { return status & UNIT_STATUS_ALLY; }
bool Unit::IsEnemy() { return status & UNIT_STATUS_ENEMY; }

json_spirit::Object Unit::Serialize()
{
	json_spirit::Object o;
	o.push_back(json_spirit::Pair("Class", "Bladedge"));
	o.push_back(json_spirit::Pair("Experience", experience));
	o.push_back(json_spirit::Pair("Health", maximumHealth));
	o.push_back(json_spirit::Pair("AP", maximumAbilityPoints));
	o.push_back(json_spirit::Pair("Strength", strength));
	o.push_back(json_spirit::Pair("Magic", magic));
	o.push_back(json_spirit::Pair("Skill", skill));
	o.push_back(json_spirit::Pair("Agility", agility));
	o.push_back(json_spirit::Pair("Defense", defense));
	o.push_back(json_spirit::Pair("Resistance", resistance));
	o.push_back(json_spirit::Pair("Movement", movement));

	string abilities;
	int i, rank;
	for(i = 0; i < activeAbilityList.size(); i++)
	{
		abilities.append(activeAbilityList[i]->Name);
		rank = activeAbilityList[i]->Rank;
		if(rank > 1)
			abilities.append(":" + std::to_string(static_cast<long long>(rank)));
		abilities.append("|");
	}
	for(i = 0; i < passiveAbilityList.size(); i++)
	{
		abilities.append(passiveAbilityList[i]->Name);
		rank = passiveAbilityList[i]->Rank;
		if(rank > 1)
			abilities.append(":" + std::to_string(static_cast<long long>(rank)));
		abilities.append("|");
	}

	o.push_back(json_spirit::Pair("Abilities", abilities.substr(0, abilities.size()-1)));
	return o;
}

bool Unit::UpdateHPAPBuffers()
{
	float left, right, bottom, top;										// Borders of each bar
	float hpRatio = (float)health / (float)maximumHealth;				// Ratio of HP determines how filled HP Bar is	
	float apRatio = (float)abilityPoints / (float)maximumAbilityPoints;	// Ratio of AP determines how filled AP Bar is
	void* vertsPtr = 0;

	// Update HP Vertex Buffer
	if(CENTERED_ORIGIN) {}
	else
	{
		left = Left();								// Left even with Unit
		right = left + (Width()* hpRatio);			// Right scales to even with Unit at max
		bottom = Top() + hpapHeight;				// Bottom slightly above unit
		top = bottom + hpapHeight;					// HP Bar is 1/10 height of unit
	}

	Vertex verts[] =
	{	// Cut off percentage of texture proportional to current HP
		{D3DXVECTOR3(left, top, 0.0f),		D3DXVECTOR2(0.0f, 0.0f)},
		{D3DXVECTOR3(right, top, 0.0f),		D3DXVECTOR2(1.0f * hpRatio, 0.0f)},
		{D3DXVECTOR3(left, bottom, 0.0f),	D3DXVECTOR2(0.0f, 1.0f)},
		{D3DXVECTOR3(right, bottom, 0.0f),	D3DXVECTOR2(1.0f * hpRatio, 1.0f)}
	};
	
	// Set new updated HP Bar Buffer
	hr = hpVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&vertsPtr);
	if(FAILED(hr)) return hr;
	memcpy(vertsPtr, (void*)verts, sizeof(Vertex) * 4);
	hpVertexBuffer->Unmap();

	// Update AP Vertex Buffer
	if(CENTERED_ORIGIN) {}
	else
	{
		//left = Left();						// Left doesn't change
		right = left + (Width() * apRatio);		// Right scales to even with Unit at max
		top = bottom;							// Top starts at bottom of HP Bar
		bottom = top - hpapHeight;				// Stops at top of Unit
	}

	Vertex verts2[] = 
	{	// Cut off percentage of texture proportional to current AP
		{D3DXVECTOR3(left, top, 0.0f),		D3DXVECTOR2(0.0f, 0.0f)},
		{D3DXVECTOR3(right, top, 0.0f),		D3DXVECTOR2(1.0f * apRatio, 0.0f)},
		{D3DXVECTOR3(left, bottom, 0.0f),	D3DXVECTOR2(0.0f, 1.0f)},
		{D3DXVECTOR3(right, bottom, 0.0f),	D3DXVECTOR2(1.0f * apRatio, 1.0f)}
	};

	// Set new updated AP Bar Buffer
	hr = apVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&vertsPtr);
	if(FAILED(hr)) return hr;
	memcpy(vertsPtr, (void*)verts2, sizeof(Vertex) * 4);
	apVertexBuffer->Unmap();

	updateHPAPBuffers = false;	// Update Complete!
	return true;
}


bool Unit::Draw()
{
	// Draw Unit first
	VisualElement::Draw();

	// Only draw HP/AP bars if mouse is hovering over unit
	if(!drawBars)
		return true;
	
	// Update HP/AP Buffers if necessary
	if(updateHPAPBuffers)
		UpdateHPAPBuffers();

	Direct3D::gpInfo->gpShaderWorldMatrix->SetMatrix((float*)&hpapWorld);

	// Draw HP Bar
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Direct3D::gpInfo->gpDevice->IASetVertexBuffers(0, 1, &hpVertexBuffer, &stride, &offset);
	Direct3D::gpInfo->gpShaderTexture->SetResource(hpBarTexture);

	D3D10_TECHNIQUE_DESC techDesc;
	Direct3D::gpInfo->gpTechnique->GetPassByIndex(0/*i*/)->Apply(0);
	Direct3D::gpInfo->gpDevice->DrawIndexed(6, 0, 0);

	// Draw AP Bar
	Direct3D::gpInfo->gpDevice->IASetVertexBuffers(0, 1, &apVertexBuffer, &stride, &offset);
	Direct3D::gpInfo->gpShaderTexture->SetResource(apBarTexture);
	Direct3D::gpInfo->gpTechnique->GetPassByIndex(0/*i*/)->Apply(0);
	Direct3D::gpInfo->gpDevice->DrawIndexed(6, 0, 0);

	// Reset default Vertex Buffer
	Direct3D::gpInfo->gpDevice->IASetVertexBuffers(0, 1, &Direct3D::gpInfo->gpVertexBuffer, &stride, &offset);

	return true;
}

#pragma region Properties
Position Unit::GetPosition() { return position; }
void  Unit::SetPosition(Position p) { position = p; }
char* Unit::GetName() { return name; }
void  Unit::SetName(char* n) { strcpy(name, n); }
int	  Unit::GetExperience() { return experience; }
void  Unit::SetExperience(int xp) { experience = xp; }
int	  Unit::GetHealth() { return health; }
void  Unit::SetHealth(int h) { health = h; updateHPAPBuffers = true; if(health < 0) health = 0; }
int   Unit::GetAbilityPoints() { return abilityPoints; }
void  Unit::SetAbilityPoints(int ap) { abilityPoints = ap; }
int   Unit::GetStrength() { return strength; }
void  Unit::SetStrength(int s) { strength = s; }
int   Unit::GetMagic() { return magic; }
void  Unit::SetMagic(int m) { magic = m; }
int   Unit::GetAgility() { return agility; }
void  Unit::SetAgility(int a) { agility = a; }
int   Unit::GetSkill() { return skill; }
void  Unit::SetSkill(int s) { skill = s; }
int   Unit::GetDefense() { return defense; }
void  Unit::SetDefense(int d) { defense = d; }
int   Unit::GetResistance() { return resistance; }
void  Unit::SetResistance(int r) { resistance = r; }
int   Unit::GetMovement() { return movement; }
void  Unit::SetMovement(int m) { movement = m; }
int   Unit::GetAttackRange() { return attackRange; }
void  Unit::SetAttackRange(int r) { attackRange = r; }
Unit::Phylum Unit::GetPhylum() { return phylum; }
void  Unit::SetFinished(bool f) { finishedTurn = f; }
bool  Unit::GetMovementFinished() { return movementFinished; }
bool  Unit::GetFinished() { return finishedTurn; }
void  Unit::SetCombatCalcScript(std::string s) { combatCalculationAbilityScript = s; }
std::string Unit::GetCombatCalcScript() { return combatCalculationAbilityScript; }
void  Unit::SetCombatExecuteScript(std::string s) { combatExecutionAbilityScript = s; }
std::string Unit::GetCombatExecuteScript() { return combatExecutionAbilityScript; }
void  Unit::SetDrawBars(bool db) { drawBars = db; }
int	  Unit::GetUnitID() const { return unitID; }

bool  Unit::operator==(const Unit& other) const { return other.GetUnitID() == unitID; }
bool  Unit::operator==(const Unit* &other) const { return other->GetUnitID() == this->unitID; }
#pragma endregion