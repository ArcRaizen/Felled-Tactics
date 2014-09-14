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

}

int LuaUnit::ApplyStatus(lua_State* L)
{
	realUnit->ApplyStatus((int)luaL_checknumber(L, 1));
	return 0;
}

int LuaUnit::TakeDamage(lua_State* L)
{
	int a = (int)lua_tointeger(L, 2);
	int b = (int)lua_tointeger(L, 3);
	lua_pushnumber(L, realUnit->TakeDamage(a,b));
	return 1;
}