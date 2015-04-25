--[[
	Level is pointer to current level passed from C++
	Unit is pointer to unit currently on this tile
	Position is a table that holds the coordinates of this tile
--]]

local level = LuaLevel(Level)
local unit = LuaUnit(Unit)
local unitID = unit:GetUnitID()

-- Push the unit up to 3 tiles up (stop at occupied tiles)
local isFirstOccupied = level:IsOccupied(Position["x"], Position["y"] + 1) and (unitID ~= level:GetOccupantID(Position["x"], Position["y"] + 1))
local isSecondOccuPied = level:IsOccupied(Position["x"], Position["y"] + 2) and (unitID ~= level:GetOccupantID(Position["x"], Position["y"] + 2))
local isThirdOccupied = level:IsOccupied(Position["x"], Position["y"] + 3) and (unitID ~= level:GetOccupantID(Position["x"], Position["y"] + 3))

if isFirstOccupied == false then
	if isSecondOccuPied == false then
		if isThirdOccupied == false then
			unit:ForceMovement(Position["x"], Position["y"] + 3, 0.0625)
		else
			unit:ForceMovement(Position["x"], Position["y"] + 2, 0.0625)
		end
	else
		unit:ForceMovement(Position["x"], Position["y"] + 1, 0.0625)
	end
else
	unit:ForceEndMovement()
end
