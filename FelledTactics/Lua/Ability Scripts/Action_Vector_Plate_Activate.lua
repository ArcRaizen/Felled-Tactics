--[[
	Level is pointer to current level passed from C++
	AreaOfEffect is a list of all the Positions on the map this ability
		has an effect on. Whether it's a tile and/or a unit is up to the ability

	Using Level and AreaOfEffect, loop through and access each respective Tile/Unit
	and apply the effects of this ability
--]]

-- Get proper pointer to current Level
local level = LuaLevel(Level)
local tile = nil

-- Loop through AoE and apply ability effects
for key, value in pairs(AreaOfEffect) do
	tile = LuaTile(level:GetTile(value["x"], value["y"]))
	math.randomseed(os.time())
	local rand = math.random(2,5)
	tile:SetEffect(rand,5)
end
