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

//	~~~~ Functions Lua needs to access ~~~~
	// Stats
	int GetHealth(lua_State* L);			int SetHealth(lua_State* L);
	int GetStrength(lua_State* L);			int SetStrength(lua_State* L);
	int GetMagic(lua_State* L);				int SetMagic(lua_State* L);
	int GetAgility(lua_State* L);			int SetAgility(lua_State* L);
	int GetSkill(lua_State* L);				int SetSkill(lua_State* L);
	int GetDefense(lua_State* L);			int SetDefense(lua_State* L);
	int GetResistance(lua_State* L);		int SetResistance(lua_State* L);
	int GetMovement(lua_State* L);			int SetMovement(lua_State* L);
	int GetPosition(lua_State* L);

	// Combat
	int CalculateBaseCombatDamage(lua_State* L);
	int TakeDamage(lua_State* L);
	int SetCombatCalcAbilityScript(lua_State* L);
	int SetCombatExecutionAbilityScript(lua_State* L);

	int ApplyStatus(lua_State* L);
	int Heal(lua_State* L);


private:
	Unit* realUnit;		// Pointer to real Unit wrapped by this class
};
#endif
