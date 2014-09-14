#include "StdAfx.h"
#include "LuaLevel.h"

// Name to use to address this class in Lua
const char LuaLevel::className[] = "LuaLevel";

// List of class properties that one can get/set from Lua
const Luna<LuaLevel>::PropertyType LuaLevel::properties[] = {{0,0}};

// List of class methods to make available to Lua
const Luna<LuaLevel>::FunctionType LuaLevel::methods[] = {
	method(LuaLevel, GetTile),
	method(LuaLevel, GetUnit),
	{0,0}
};


LuaLevel::LuaLevel(lua_State* L)
{
	realLevel = (Level*)lua_touserdata(L, 1);
}


LuaLevel::~LuaLevel(void)
{
}

int LuaLevel::GetTile(lua_State* L)
{
	lua_pushlightuserdata(L, realLevel->GetTile((int)luaL_checknumber(L, 1), (int)luaL_checknumber(L, 2)));
	return 1;
}

int LuaLevel::GetUnit(lua_State* L)
{
	bool a = lua_isuserdata(L, 1);
	void* blah = lua_touserdata(L, 1);
	int x = (int)lua_tointeger(L, 2);
	int y = (int)lua_tointeger(L, 3);
	int z = lua_gettop(L);
	lua_pushlightuserdata(L, (void*)realLevel->GetUnit(x, y));
	return 1;
}
