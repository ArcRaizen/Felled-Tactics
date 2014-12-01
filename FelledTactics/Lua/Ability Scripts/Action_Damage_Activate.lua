--[[
	Level is pointer to current level passed from C++
	AreaOfEffect is a list of all the Positions on the map this ability
		has an effect on. Whether it's a tile and/or a unit is up to the ability
	Rank is an int passed from C++ that specifies the current rank of the ability

	Using Level and AreaOfEffect, loop through and access each respective Tile/Unit
	and apply the rank-appropriate effects of this ability
--]]

-- Get proper pointer to current Level
local combatManager = LuaCombatManager(CombatMan)
local level = LuaLevel(Level)
local unit = nil
local damage = 0

-- Loop through AoE and apply ability effects
for key, value in pairs(AreaOfEffect) do
	unit = LuaUnit(level:GetEnemyUnit(value["x"], value["y"]))
	if Rank == 1 then
		damage = unit:TakeUnscaledDamage(20)
	elseif Rank == 2 then
		damage = unit:TakeUnscaledDamage(50)
	elseif Rank == 3 then
		damage = unit:TakeUnscaledDamage(100)
	end

	-- Create combat text
	if damage ~= nil then
		level:CreateCombatText(value["x"], value["y"], Source["x"], Source["y"], math.abs(damage), 0)

		-- Unit died - Notify Comat Calc
		if damage < 0 then
			combatManager:UnitKilledByAbility(value["x"], value["y"])
		end
	end
end
