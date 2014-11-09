--[[
	Level is pointer to current level passed from C++
	Unit is pointer to unit currently on this tile
	Position is a table that holds the coordinates of this tile
--]]

local level = LuaLevel(Level)
local unit = LuaUnit(Unit)

-- Push the unit up to 3 tiles to theleft (stop at occupied tiles)
local isFirstOccupied = level:IsOccupied(Position["x"] + 1, Position["y"])
local isSecondOccuPied = level:IsOccupied(Position["x"] + 2, Position["y"])
local isThirdOccupied = level:IsOccupied(Position["x"] + 3, Position["y"])

if isFirstOccupied == false then
	if isSecondOccuPied== false then
		if isThirdOccupied == false then
			unit:ForceMovement(Position["x"] + 3, Position["y"], 0.0625)
		else
			unit:ForceMovement(Position["x"] + 2, Position["y"], 0.0625)
		end
	else
		unit:ForceMovement(Position["x"] + 1, Position["y"], 0.0625)
	end
else
	unit:ForceEndMovement()
end
