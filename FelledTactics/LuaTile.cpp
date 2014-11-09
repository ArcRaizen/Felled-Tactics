#include "StdAfx.h"
#include "LuaTile.h"

// Name to use to address this class in Lua
const char LuaTile::className[] = "LuaTile";

// List of class properties that one can get/set from Lua
const Luna<LuaTile>::PropertyType LuaTile::properties[] = {
	{0,0}
};

// List of class methods to make available to Lua
const Luna<LuaTile>::FunctionType LuaTile::methods[] = {
	method(LuaTile, SetEffect),
	{0,0}
};

LuaTile::LuaTile(lua_State* L)
{
	realTile = (Tile*)lua_touserdata(L, 1);
}


LuaTile::~LuaTile(void)
{
	realTile = NULL;
}

int LuaTile::SetEffect(lua_State* L)
{
	int a = lua_tointeger(L, 2);
	int b = lua_tointeger(L, 3);
	realTile->SetTileEffect(a, b);
	return 0;
}
