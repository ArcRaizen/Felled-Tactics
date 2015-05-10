#include "StdAfx.h"
#include "LuaCombatManager.h"

// Luna setup
const char LuaCombatManager::className[] = "LuaCombatManager";
const Luna<LuaCombatManager>::PropertyType LuaCombatManager::properties[] = {{0,0}};
const Luna<LuaCombatManager>::FunctionType LuaCombatManager::methods[] = {
	method(LuaCombatManager, SetCombatParametersAttacker),
	method(LuaCombatManager, SetCombatParametersDefender),
	method(LuaCombatManager, SetCombatTimers),
	method(LuaCombatManager, UnitKilledByAbility),
	{0,0}
};

LuaCombatManager::LuaCombatManager(lua_State* L)
{
	manager = (CombatManager*)lua_touserdata(L, 1);
}


LuaCombatManager::~LuaCombatManager(void) {}

int LuaCombatManager::SetCombatParametersAttacker(lua_State* L)
{
	manager->SetCombatParametersAttacker(lua_tointeger(L, 2), lua_tointeger(L, 3), 
		lua_tointeger(L, 4), lua_tointeger(L, 5), lua_tointeger(L, 6));
	return 0;
}

int LuaCombatManager::SetCombatParametersDefender(lua_State* L)
{
	manager->SetCombatParametersDefender(lua_tointeger(L, 2), lua_tointeger(L, 3),
		lua_tointeger(L, 4), lua_tointeger(L, 5), lua_tointeger(L, 6));
	return 0;
}

int LuaCombatManager::SetCombatTimers(lua_State* L)
{
	manager->SetCombatTimers(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
	return 0;
}

int LuaCombatManager::UnitKilledByAbility(lua_State* L)
{
	manager->UnitKilledByAbility(lua_tointeger(L, 2), lua_tointeger(L, 3));
	return 0;
}