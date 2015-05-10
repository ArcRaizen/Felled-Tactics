#include "StdAfx.h"
#include "LuaLevel.h"

// Name to use to address this class in Lua
const char LuaLevel::className[] = "LuaLevel";

// List of class properties that one can get/set from Lua
const Luna<LuaLevel>::PropertyType LuaLevel::properties[] = {{0,0}};

// List of class methods to make available to Lua
const Luna<LuaLevel>::FunctionType LuaLevel::methods[] = {
	method(LuaLevel, CreateCombatText),
	method(LuaLevel, GetTile),
	method(LuaLevel, GetUnit),
	method(LuaLevel, GetEnemyUnit),
	method(LuaLevel, GetAllyUnit),
	method(LuaLevel, IsOccupied),
	method(LuaLevel, GetOccupantID),
	{0,0}
};


LuaLevel::LuaLevel(lua_State* L)
{
	void* test = lua_touserdata(L, 1);
	realLevel = ((test == nullptr) ? nullptr : *static_cast<LevelPtr*>(test));
}


LuaLevel::~LuaLevel(void) {}

int LuaLevel::CreateCombatText(lua_State* L)
{
	realLevel->CreateCombatText(Position(lua_tointeger(L,2), lua_tointeger(L,3)),	// Target
								Position(lua_tointeger(L,4), lua_tointeger(L,5)),	// Source
								lua_tostring(L,6), lua_tointeger(L,7));				// Text / Damage Type
	return 0;
}

int LuaLevel::GetTile(lua_State* L)
{
	lua_pushlightuserdata(L, (void*)realLevel->GetTile(lua_tointeger(L, 2), lua_tointeger(L, 3)));
	return 1;
}

int LuaLevel::GetUnit(lua_State* L)
{
	lua_pushlightuserdata(L, (void*)realLevel->GetUnit(lua_tointeger(L, 2), lua_tointeger(L, 3)));
	return 1;
}

int LuaLevel::GetEnemyUnit(lua_State* L)
{
	lua_pushlightuserdata(L, (void*)realLevel->GetEnemyUnit(lua_tointeger(L, 2), lua_tointeger(L, 3)));
	return 1;
}

int LuaLevel::GetAllyUnit(lua_State* L)
{
	lua_pushlightuserdata(L, (void*)realLevel->GetAllyUnit(lua_tointeger(L, 2), lua_tointeger(L, 3)));
	return 1;
}

int LuaLevel::IsOccupied(lua_State* L)
{
	lua_pushboolean(L, realLevel->IsOccupied(lua_tointeger(L, 2), lua_tointeger(L, 3)));
	return 1;
}

int LuaLevel::GetOccupantID(lua_State* L)
{
	lua_pushinteger(L, realLevel->GetOccupantID(lua_tointeger(L, 2), lua_tointeger(L, 3)));
	return 1;
}