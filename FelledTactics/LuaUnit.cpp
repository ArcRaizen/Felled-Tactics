#include "StdAfx.h"
#include "LuaUnit.h"

// Name to use to address this class in Lua
const char LuaUnit::className[] = "LuaUnit";

// List of class properties that one can get/set from Lua
const Luna<LuaUnit>::PropertyType LuaUnit::properties[] = {
	propertyL(LuaUnit, Health, GetHealth, SetHealth),
	propertyL(LuaUnit, Strength, GetStrength, SetStrength),
	propertyL(LuaUnit, Magic, GetMagic, SetMagic),
	propertyL(LuaUnit, Agility, GetAgility, SetAgility),
	propertyL(LuaUnit, Skill, GetSkill, SetSkill),
	propertyL(LuaUnit, Defense, GetDefense, SetDefense),
	propertyL(LuaUnit, Resistance, GetResistance, SetResistance),
	propertyL(LuaUnit, Movement, GetMovement, SetMovement),
	{0,0}
};

// List of class methods to make available to Lua
const Luna<LuaUnit>::FunctionType LuaUnit::methods[] = {
	method(LuaUnit, ApplyStatus),
	method(LuaUnit, TakeDamage),
	method(LuaUnit, Heal),
	method(LuaUnit, CalculateBaseCombatDamage),
	method(LuaUnit, GetPosition),
	method(LuaUnit, SetCombatCalcAbilityScript),
	method(LuaUnit, SetCombatExecutionAbilityScript),
	{0,0}
};

LuaUnit::LuaUnit(lua_State* L)
{
	realUnit = (Unit*)lua_touserdata(L, 1);
}

LuaUnit::~LuaUnit(void)
{
	realUnit = NULL;
}

#pragma region Stats
int LuaUnit::GetHealth(lua_State* L)	{ lua_pushnumber(L, realUnit->Health); return 1; }
int LuaUnit::GetStrength(lua_State* L)	{ lua_pushnumber(L, realUnit->Strength); return 1; }
int LuaUnit::GetMagic(lua_State* L)		{ lua_pushnumber(L, realUnit->Magic); return 1; }
int LuaUnit::GetAgility(lua_State* L)	{ lua_pushnumber(L, realUnit->Agility); return 1; }
int LuaUnit::GetSkill(lua_State* L)		{ lua_pushnumber(L, realUnit->Skill); return 1; }
int LuaUnit::GetDefense(lua_State* L)	{ lua_pushnumber(L, realUnit->Defense); return 1; }
int LuaUnit::GetResistance(lua_State* L){ lua_pushnumber(L, realUnit->Resistance); return 1; }
int LuaUnit::GetMovement(lua_State* L)	{ lua_pushnumber(L, realUnit->Movement); return 1; }
int LuaUnit::SetHealth(lua_State* L)	{ realUnit->Health = lua_tointeger(L, 2); return 0; }
int LuaUnit::SetStrength(lua_State* L)	{ realUnit->Strength = lua_tointeger(L, 2); return 0; }
int LuaUnit::SetMagic(lua_State* L)		{ realUnit->Magic = lua_tointeger(L, 2); return 0; }
int LuaUnit::SetAgility(lua_State* L)	{ realUnit->Agility = lua_tointeger(L, 2); return 0; }
int LuaUnit::SetSkill(lua_State* L)		{ realUnit->Skill = lua_tointeger(L, 2); return 0; }
int LuaUnit::SetDefense(lua_State* L)	{ realUnit->Defense = lua_tointeger(L, 2); return 0; }
int LuaUnit::SetResistance(lua_State* L){ realUnit->Resistance = lua_tointeger(L, 2); return 0; }
int LuaUnit::SetMovement(lua_State* L)	{ realUnit->Movement = lua_tointeger(L, 2); return 0; }

int LuaUnit::GetPosition(lua_State* L)
{
	lua_pushnumber(L, realUnit->UnitPosition.x);
	lua_pushnumber(L, realUnit->UnitPosition.y);
	return 2;
}
#pragma endregion

int LuaUnit::CalculateBaseCombatDamage(lua_State* L)
{
	int phys, mag;
	realUnit->CalculateCombatDamage(phys, mag, lua_tointeger(L, 2));
	lua_pushnumber(L, phys);
	lua_pushnumber(L, mag);
	return 2;
}

int LuaUnit::TakeDamage(lua_State* L)
{
	if(realUnit == NULL)
		lua_pushnil(L);
	else
		lua_pushnumber(L, realUnit->TakeDamage(lua_tointeger(L, 2), lua_tointeger(L, 3)));

	return 1;
}

int LuaUnit::ApplyStatus(lua_State* L)
{
	realUnit->ApplyStatus((int)lua_tointeger(L, 2));
	return 0;
}

int LuaUnit::SetCombatCalcAbilityScript(lua_State* L)
{
	realUnit->CombatCalcScript = lua_tostring(L, 2);
	return 0;
}

int LuaUnit::SetCombatExecutionAbilityScript(lua_State* L)
{
	realUnit->CombatExecuteScript = lua_tostring(L, 2);
	return 0;
}

int LuaUnit::Heal(lua_State* L)
{
	if(realUnit == NULL)
		lua_pushnil(L);
	else
		lua_pushnumber(L, realUnit->Heal(lua_tointeger(L, 2)));

	return 1;
}
