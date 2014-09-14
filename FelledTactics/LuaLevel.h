#pragma once
#ifndef LUALEVEL_H
#define LUALEVEL_H

#include "Luna.h"
#include "Level.h"

class LuaLevel
{
public:
	static const char className[];
	static const Luna<LuaLevel>::PropertyType properties[];
	static const Luna<LuaLevel>::FunctionType methods[];

	LuaLevel(lua_State* L);
	~LuaLevel(void);

	// Functions Lua needs to access
	int GetTile(lua_State* L);
	int GetUnit(lua_State* L);

private:
	Level* realLevel;	// Pointer to real Level wrapped by this class
};
#endif
