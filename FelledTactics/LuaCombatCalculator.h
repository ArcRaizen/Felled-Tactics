#pragma once
#ifndef LUACOMBAT_H
#define LUACOMBAT_H

#include "Luna.h"
#include "CombatCalculator.h"

class LuaCombatCalculator
{
public:
	static const char className[];
	static const Luna<LuaCombatCalculator>::PropertyType properties[];
	static const Luna<LuaCombatCalculator>::FunctionType methods[];

	LuaCombatCalculator(lua_State* L);
	~LuaCombatCalculator(void);

	int GetAttacker(lua_State* L);
	int	GetDefender(lua_State* L);
	int SetCombatParametersAttacker(lua_State* L);
	int SetCombatParametersDefender(lua_State* L);
	int	DefenderDied(lua_State* L);
	int AttackerDied(lua_State* L);

private:
	CombatCalculator* calc;
};
#endif
