#pragma once
#ifndef LUACOMBAT_H
#define LUACOMBAT_H

#include "Luna.h"
#include "CombatManager.h"

class LuaCombatManager
{
public:
	static const char className[];
	static const Luna<LuaCombatManager>::PropertyType properties[];
	static const Luna<LuaCombatManager>::FunctionType methods[];

	LuaCombatManager(lua_State* L);
	~LuaCombatManager(void);

	int GetAttacker(lua_State* L);
	int	GetDefender(lua_State* L);
	int SetCombatParametersAttacker(lua_State* L);
	int SetCombatParametersDefender(lua_State* L);
	int SetCombatTimers(lua_State* L);
	int	DefenderDied(lua_State* L);
	int AttackerDied(lua_State* L);
	int UnitKilledByAbility(lua_State* L);

private:
	CombatManager* manager;
};
#endif
