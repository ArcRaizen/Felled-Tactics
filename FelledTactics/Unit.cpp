#include "StdAfx.h"
#include "Unit.h"

int Unit::unitCounter = 0;
D3DXVECTOR4 Unit::highlightFinishedTurn = D3DXVECTOR4(0.328f, 0.328f, 0.328f, 1.0f);

Unit::Unit(WCHAR* filename, int layer, int width, int height, int posX, int posY, bool ally/*=true*/) : 
	VisualElement(filename, layer, width, height, posX, posY)
{
	InitProficiency();
	felled = false;
	finishedTurn = false;
	position.x = posX / width;		// width and height are set to TileSize
	position.y = posY / height;		// PosX and PosY are units from origin to bottom-left corner of tile they are at
	secBetweenTiles = 0.25f;
	movementTimer = 0;
	movementFinished = true;
	status = 0;
	unitID = unitCounter++;			// each unit has ID number in order of their creation
	
	if(ally)
		ApplyStatus(ALLY);
	else
		ApplyStatus(ENEMY);

	// Test stats
	movement = 8;
	health = maximumHealth = 100;
	abilityPoints = maximumAbilityPoints = 50;
	strength = magic = skill = agility = defense = resistance = 10;
	attackRange = 5;
	inventory += new Weapon(Weapon::WeaponClass::Bow, 40, 0, 5);
	a = new Ability("Damage");

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
	hpVertexBuffer->Release(); delete hpVertexBuffer; hpVertexBuffer = 0;
	apVertexBuffer->Release(); delete apVertexBuffer; apVertexBuffer = 0;
	hpBarTexture->Release(); delete hpBarTexture; hpBarTexture = 0;
	apBarTexture->Release(); delete apBarTexture; apBarTexture = 0;
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

int Unit::TakeDamage(int physDamage, int magDamage)
{
	health -= physDamage + magDamage;
	if(health < 0)
		health = 0;

	updateHPAPBuffers = true;
	return physDamage + magDamage;	// return damage taken
}

void Unit::Revive(int health)
{
	this->health = health;
	RemoveStatus(FELLED);
	felled = false;
}

float Unit::Die()
{
	ApplyStatus(FELLED);
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

void Unit::SetMovePath(list<Position> path) 
{ 
	// convert path from tile units to pixels and save
	for(std::list<Position>::const_iterator iterator = path.begin(); iterator != path.end(); ++iterator)
		movementPath.push_back(*iterator * tileSize);

	movementFinished = false;
}

// Activate ability (if possible) at target location.
// Return 1 is ability is cast, 0 if not
int Unit::ActivateAbility(lua_State* L, Position target)
{
	if(abilityPoints < a->APCost)
		return 0;

	a->Activate(L, target, position);
	abilityPoints -= a->APCost;
	updateHPAPBuffers = true;
	return 1;
}

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

// Update Statuses: 0 - None, 1 - Movement between 2 tiles finished, 2 - Dead
int Unit::Update(float dt)
{
	if(CheckStatus(FELLED))
	{
		highlightColor.w -= (dt / 1.5f);
		if(highlightColor.w <= 0)
		{
			if(CheckStatus(ALLY))
			{
				highlightColor.w = 0.2;
				RemoveStatus(FELLED);
				felled = true;
			}
			return 2;
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

			// Movement finished
			if(movementPath.size() == 1)
			{
				movementFinished = true;
				movementPath.clear();
				updateHPAPBuffers = true;
				return 1;
			}
		}

		// Update vertex buffers on movement
		updateHPAPBuffers = true;
	}

	return 0;
}

void Unit::ApplyStatus(int s) {	status |= s; }
void Unit::RemoveStatus(int s) { status &= ~s; }
bool Unit::CheckStatus(int s) { return status & s; }
bool Unit::IsAlly() { return status & ALLY; }
bool Unit::IsEnemy() { return status & ENEMY; }

void Unit::FinishTurn()
{
	finishedTurn = true;
	highlightColor = highlightFinishedTurn;
}

void Unit::NewTurn()
{
	finishedTurn = false;
	highlightColor = highlightNone;
}

bool Unit::UpdateHPAPBuffers()
{
	float left, right, bottom, top;										// Boarders of each bard
	float hpRatio = (float)health / (float)maximumHealth;				// Ratio of HP determines how filled HP Bar is	
	float apRatio = (float)abilityPoints / (float)maximumAbilityPoints;	// Ratio of AP determines how filled AP Bar is
	void* vertsPtr = 0;

	// Update HP Vertex Buffer
	if(CENTERED_ORIGIN) {}
	else
	{
		left = -(float)(screenWidth / 2) + (float)leftCorner.x;		// Left even with Unit
		right = left + ((float)width * hpRatio);					// Right scales to even with Unit at max
		bottom = -(float)(screenHeight / 2) + (float)leftCorner.y +  height + hpapHeight; // Bottom slightly above unit
		top = bottom + hpapHeight;		// HP Bar is 1/10 height of unit
	}

	Vertex verts[] =
	{	// Cut off percentage of texture proportional to current HP
		{D3DXVECTOR3(left, top, 0.0f),		D3DXVECTOR2(0.0, 0.0f)},
		{D3DXVECTOR3(right, top, 0.0f),		D3DXVECTOR2(1.0 * hpRatio, 0.0f)},
		{D3DXVECTOR3(left, bottom, 0.0f),	D3DXVECTOR2(0.0, 1.0f)},
		{D3DXVECTOR3(right, bottom, 0.0f),	D3DXVECTOR2(1.0 * hpRatio, 1.0f)}
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
		//left = leftCorner.x;						// Left doesn't change
		right = left + ((float)width * apRatio);	// Right scales to even with Unit at max
		top = bottom;								// Top starts at bottom of HP Bar
		bottom = top - hpapHeight;					// Stops at top of Unit
	}

	Vertex verts2[] = 
	{	// Cut off percentage of texture proportional to current AP
		{D3DXVECTOR3(left, top, 0.0f),		D3DXVECTOR2(0.0, 0.0f)},
		{D3DXVECTOR3(right, top, 0.0f),		D3DXVECTOR2(1.0 * apRatio, 0.0f)},
		{D3DXVECTOR3(left, bottom, 0.0f),	D3DXVECTOR2(0.0, 1.0f)},
		{D3DXVECTOR3(right, bottom, 0.0f),	D3DXVECTOR2(1.0 * apRatio, 1.0f)}
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
void  Unit::SetDrawBars(bool db) { drawBars = db; }
int	  Unit::GetUnitID() const { return unitID; }

bool  Unit::operator==(const Unit& other) const { return other.GetUnitID() == unitID; }
bool  Unit::operator==(const Unit* &other) const { return other->GetUnitID() == this->unitID; }
#pragma endregion