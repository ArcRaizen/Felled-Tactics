#pragma once
#ifndef LUAUNIT_H
#define LUAUNIT_H

#include "Luna.h"
#include "Unit.h"

class LuaUnit
{
public:
	static const char className[];
	static const Luna<LuaUnit>::PropertyType properties[];
	static const Luna<LuaUnit>::FunctionType methods[];

	LuaUnit(lua_State* L);
	~LuaUnit(void);

	// Functions Lua needs to access
	int ApplyStatus(lua_State* L);
	int TakeDamage(lua_State* L);
	int GetCorner(lua_State* L);

private:
	Unit* realUnit;		// Pointer to real Unit wrapped by this class
};
#endif
