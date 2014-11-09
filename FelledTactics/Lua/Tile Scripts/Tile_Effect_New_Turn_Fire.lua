--[[
	Level is pointer to current level passed from C++
	Unit is pointer to unit currently on this tile
	Position is a table that holds the coordinates of this tile
--]]

local level = LuaLevel(Level)
local unit = LuaUnit(level:GetUnit(Position["x"], Position["y"]))
local damage = unit:TakeUnscaledDamage(10)

if damage ~= nil then
	level:CreateCombatText(Position["x"], Position["y"], Position["x"], Position["y"], math.abs(damage))
end
