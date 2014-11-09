--[[
	Level is pointer to current level passed from C++
	Source is location of Unit activating this Ability

	Set proper Combat Scripts for the Activating Unit for this Ability
--]]

local level = LuaLevel(Level)
local unit = LuaUnit(level:GetAllyUnit(Source["x"], Source["y"]))

unit:SetCombatCalcAbilityScript("Lua\\Combat Scripts\\DoubleStrike_CalcCombat.lua");
