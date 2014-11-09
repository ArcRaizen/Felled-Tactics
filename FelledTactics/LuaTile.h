#pragma once
#ifndef LUATILE_H
#define LUATILE_H

#include "Luna.h"
#include "Tile.h"

class LuaTile
{
public:
	static const char className[];
	static const Luna<LuaTile>::PropertyType properties[];
	static const Luna<LuaTile>::FunctionType methods[];

	LuaTile(lua_State* L);
	~LuaTile(void);

	int SetEffect(lua_State* L);

private:
	Tile* realTile;		// Pointer to real Tile wrapped by this class
};
#endif
