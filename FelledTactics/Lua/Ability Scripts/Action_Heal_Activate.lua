--[[
	Level is pointer to current level passed from C++
	AreaOfEffect is a list of all the Positions on the map this ability
		has an effect on. Whether it's a tile and/or a unit is up to the ability

	Using Level and AreaOfEffect, loop through and access each respective Tile/Unit
	and apply the effects of this ability
--]]

-- Get proper pointer to current Level
local level = LuaLevel(Level)
local unit = nil
local heal = 0

-- Loop through AoE and apply ability effects
for key, value in pairs(AreaOfEffect) do
	unit = LuaUnit(level:GetAllyUnit(value["x"], value["y"]))
	heal = unit:Heal(50)
	if heal ~= nil then
		level:CreateCombatText(value["x"], value["y"], Source["x"], Source["y"], heal, 1)
	end
end
