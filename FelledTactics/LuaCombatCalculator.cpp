#include "StdAfx.h"
#include "LuaCombatCalculator.h"

// Luna setup
const char LuaCombatCalculator::className[] = "LuaCombatCalculator";
const Luna<LuaCombatCalculator>::PropertyType LuaCombatCalculator::properties[] = {{0,0}};
const Luna<LuaCombatCalculator>::FunctionType LuaCombatCalculator::methods[] = {
	method(LuaCombatCalculator, GetAttacker),
	method(LuaCombatCalculator, GetDefender),
	method(LuaCombatCalculator, SetCombatParametersAttacker),
	method(LuaCombatCalculator, SetCombatParametersDefender),
	method(LuaCombatCalculator, DefenderDied),
	method(LuaCombatCalculator, AttackerDied),
	{0,0}
};

LuaCombatCalculator::LuaCombatCalculator(lua_State* L)
{
	calc = (CombatCalculator*)lua_touserdata(L, 1);
}


LuaCombatCalculator::~LuaCombatCalculator(void)
{
	calc = NULL;
}

int LuaCombatCalculator::GetAttacker(lua_State* L)
{
	lua_pushlightuserdata(L, calc->Attacker);
	return 1;
}

int LuaCombatCalculator::GetDefender(lua_State* L)
{
	lua_pushlightuserdata(L, calc->Defender);
	return 1;
}

int LuaCombatCalculator::SetCombatParametersAttacker(lua_State* L)
{
	calc->SetCombatParametersAttacker(lua_tointeger(L, 2), lua_tointeger(L, 3), 
		lua_tointeger(L, 4), lua_tointeger(L, 5), lua_tointeger(L, 6));
	return 0;
}

int LuaCombatCalculator::SetCombatParametersDefender(lua_State* L)
{
	calc->SetCombatParametersDefender(lua_tointeger(L, 2), lua_tointeger(L, 3),
		lua_tointeger(L, 4), lua_tointeger(L, 5), lua_tointeger(L, 6));
	return 0;
}

int LuaCombatCalculator::DefenderDied(lua_State* L)
{
	calc->DefenderDied();
	return 0;
}

int LuaCombatCalculator::AttackerDied(lua_State* L)
{
	calc->AttackerDied();
	return 0;
}