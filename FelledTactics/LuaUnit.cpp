#include "StdAfx.h"
#include "LuaUnit.h"

// Name to use to address this class in Lua
const char LuaUnit::className[] = "LuaUnit";

// List of class properties that one can get/set from Lua
const Luna<LuaUnit>::PropertyType LuaUnit::properties[] = {{0,0}};

// List of class methods to make available to Lua
const Luna<LuaUnit>::FunctionType LuaUnit::methods[] = {
	method(LuaUnit, ApplyStatus),
	method(LuaUnit, TakeDamage),
	{0,0}
};

LuaUnit::LuaUnit(lua_State* L)
{
	realUnit = (Unit*)lua_touserdata(L, 1);
}

LuaUnit::~LuaUnit(void)
{
	realUnit = NULL;
}

int LuaUnit::ApplyStatus(lua_State* L)
{
	realUnit->ApplyStatus((int)lua_tointeger(L, 2));
	return 0;
}

int LuaUnit::TakeDamage(lua_State* L)
{
	if(realUnit == NULL)
		lua_pushnil(L);
	else
		lua_pushnumber(L, realUnit->TakeDamage(lua_tointeger(L, 2), lua_tointeger(L, 3)));

	return 1;
}

int LuaUnit::GetCorner(lua_State* L)
{
	lua_pushnumber(L, realUnit->Corner.x);
	lua_pushnumber(L, realUnit->Corner.y);
	return 2;
}