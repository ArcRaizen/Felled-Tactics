#include "StdAfx.h"
#include "Tile.h"

#pragma region Highlight Color Initializations
D3DXVECTOR4 Tile::highlightAllyMove = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
D3DXVECTOR4 Tile::highlightAllyMoveAlly = D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f);
D3DXVECTOR4 Tile::highlightAllySkillRange = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
D3DXVECTOR4 Tile::highlightAllySkillAoE = D3DXVECTOR4(1.0f, 0.647f, 0.0f, 1.0f);
D3DXVECTOR4 Tile::highlightEnemyMove = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
D3DXVECTOR4 Tile::highlightAttack = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
#pragma endregion

Tile::Tile(WCHAR* filename, int layer, int width, int height, int posX, int posY, LevelPtrW l, Position gp) :
	VisualElement(filename, layer, width, height, posX, posY), level(l), gridPosition(gp), effectTimer(0), 
	status(Status::Empty), mark(Mark::Blank), prevMark(Mark::Blank), effect(Effect::None), effectEnabled(false)
{
	// Save the path of the tiles default texture (removing the ".png" portion)
	defaultTexturePath = filename;
	defaultTexturePath = defaultTexturePath.substr(0, defaultTexturePath.size()-4);
}


Tile::~Tile(void) {}

// Update Tile for new turn: update/clear any tile effects and change texture appropriately
void Tile::NewTurn(lua_State* L)
{
	if(effectTimer > 1)			// Update effect timer for each new turn
	{
		effectTimer--;
		if(effectNewTurnScript != "")
			ActivateNewTurnEffect(L);
	}
	else if(effectTimer == 1)	// Clear tile effect once its timer has elapsed
	{
		effectTimer = 0;
		effect = Effect::None;
		effectMovementScript = effectNewTurnScript = "";
		std::wstring texturePath = defaultTexturePath + L"" + PNG;
		hr = D3DX10CreateShaderResourceViewFromFile(Direct3D::gpInfo->gpDevice, texturePath.c_str(), 0, 0, &texture, 0);
	}
}

void Tile::Clear()
{
	status = Status::Empty;
	mark = Mark::Blank;
	effect = Effect::None;
	effectTimer = 0;
}

void Tile::ActivateMovementEffect(lua_State* L)
{
	// Create table to tell script where the tile is
	lua_createtable(L, 2, 0);
	lua_pushinteger(L, gridPosition.x);
	lua_setfield(L, -2, "x");
	lua_pushinteger(L, gridPosition.y);
	lua_setfield(L, -2, "y");
	lua_setglobal(L, "Position");

	// Run the script
#ifdef DEV_DEBUG
	int test = luaL_dofile(L, effectMovementScript.c_str());
	if(test == 1)
	{
		std::string error = lua_tostring(L, -1);
		test++;
	}
#else
	luaL_dofile(L, effectScript.c_str());
#endif

	effectEnabled = false;
}

void Tile::ActivateNewTurnEffect(lua_State* L)
{
	lua_pushlightuserdata(L, (void*)&level);
	lua_setglobal(L, "Level");

	// Create table to tell script where the tile is
	lua_createtable(L, 2, 0);
	lua_pushinteger(L, gridPosition.x);
	lua_setfield(L, -2, "x");
	lua_pushinteger(L, gridPosition.y);
	lua_setfield(L, -2, "y");
	lua_setglobal(L, "Position");

	// Run the script
#ifdef DEV_DEBUG
	int test = luaL_dofile(L, effectNewTurnScript.c_str());
	if(test)
	{
		std::string error = lua_tostring(L, -1);
		test++;
	}
#else
	luaL_dofile(L, effectScript.c_str());
#endif
}

// A Tile Effect can only activate on a moving unit once for its movement phase
// We disable the effect after activation to prevent this
// Re-enable it now
void Tile::ReenableEffect()
{
	effectEnabled = true;
}

// Set a new effect to occur on this tile for a specified number of turns
// Update the Tile's texture accordingly for each effect
void Tile::SetTileEffect(int e, int effectLength)
{
	// Set the new effect and save the proper filepath extension for the effect
	std::wstring fileExtension = L"";
	int scriptType = 0;	// 0 = no script, 1 = movement script only, 2 = new turn script only, 3 = both scripts

	switch(e)
	{
		case 0: effect = Effect::None; fileExtension = L""; scriptType = 0; break;
		case 1: effect = Effect::Fire; fileExtension = L"_Fire"; scriptType = 3; break;
		case 2: effect = Effect::Vector_Plate_Left; fileExtension = L"_Vector_Plate_Left"; scriptType = 1; break;
		case 3: effect = Effect::Vector_Plate_Right; fileExtension = L"_Vector_Plate_Right"; scriptType = 1; break;
		case 4: effect = Effect::Vector_Plate_Up; fileExtension = L"_Vector_Plate_Up"; scriptType = 1; break;
		case 5: effect = Effect::Vector_Plate_Down; fileExtension = L"_Vector_Plate_Down"; scriptType = 1; break;
	}
	effectTimer = effectLength;		// Save the effect length
	effectEnabled = true;

	// Get script for tile effect
	std::string s(fileExtension.begin(), fileExtension.end());
	if(scriptType == 1 || scriptType == 3)
		effectMovementScript = "Lua\\Tile Scripts\\Tile_Effect_Movement" + s + ".lua";
	if(scriptType > 1)
		effectNewTurnScript = "Lua\\Tile Scripts\\Tile_Effect_New_Turn" + s + ".lua";

	// Update the texture for the given effect
	std::wstring texturePath = defaultTexturePath + fileExtension + PNG;
	hr = D3DX10CreateShaderResourceViewFromFile(Direct3D::gpInfo->gpDevice, texturePath.c_str(), 0, 0, &texture, 0);
}

bool Tile::HasEffect()
{
	return effectEnabled && (effect != Effect::None || effectTimer > 0);
}

bool Tile::IsObstructed()
{
	return !(status == Status::Empty /*|| mark == None*/);
}

bool Tile::IsObstructedPlayer()
{
	/*if(IsObstructed())
		return true;
	else*/
		return status == Status::EnemyUnit || status == Status::AllyFelled;
}

bool Tile::IsObstructedEnemy()
{
	/*if(IsObstructed())
		return true;
	else*/
		return status == Status::AllyUnit || status == Status::AllyFelled;
}

#pragma region MouseEvents
void Tile::MouseUp()
{
	// Only do MouseUp event if a down even already occured
	if(mouseDown)
	{
		mouseDown = false;
		level.Lock()->SetSelectedTile(gridPosition);
	}
}
void Tile::MouseOver() 
{ 
	mouseEntered = true;
	level.Lock()->SetHoveredTile(gridPosition);
}
void Tile::MouseOut() 
{ 
	mouseEntered = false; 
	mouseDown = false;
	level.Lock()->ClearHoveredTile(gridPosition);
}
#pragma endregion

#pragma region Properties
void Tile::SetStatus(Tile::Status s) { status = s; }
Tile::Status Tile::GetStatus() { return status; }
Tile::Effect Tile::GetEffect() { return effect; }
Tile::Mark Tile::GetMark() { return mark; }
void Tile::SetMark(Tile::Mark m)
{
	mark = m; 

	// Change how the tile is being drawn by the shader (just a highlight color at this point)
	switch(mark)
	{
		case Tile::Mark::AllyMove:
			highlightColor = highlightAllyMove;

			if(status == Status::AllyUnit)
				highlightColor = highlightAllyMoveAlly;
			break;
		case Tile::Mark::AllyMovePath:
			highlightColor = highlightAllySkillRange;
			break;
		case Tile::Mark::AllyMovePathFail:
			highlightColor = highlightEnemyMove;
			break;
		case Tile::Mark::AllyAbilityAoE:
			highlightColor = highlightAllySkillAoE;
			break;
		case Tile::Mark::AllyAbilityRange:
			highlightColor = highlightAllySkillRange;
			break;
		case Tile::Mark::EnemyMove:
			highlightColor = highlightEnemyMove;
			break;
		case Tile::Mark::Attack:
			highlightColor = highlightAttack;
			break;
		case Tile::Mark::Blank:
			highlightColor = highlightNone;
			break;
	}
}
void Tile::SaveMark() { prevMark = mark; }
void Tile::ResetMark() { SetMark(prevMark); prevMark = Mark::Blank; }
#pragma endregion