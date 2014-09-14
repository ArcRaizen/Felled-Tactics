--[[
	Level comes from the global parameters and is a pointer to
	the REAL C++ data. This is saved as lightuserdata in Lua and
	we preserve this pointer in the LuaGameObject. From there, we
	can manipulate ANYTHING from this pointer
--]]

-- Start up a new LuaLevel wrapper class and pass the global Level
-- C++ lightuserdata pointer into it
local level = LuaLevel(Level)
local unit = LuaUnit(level:GetUnit(2,2))
unit:TakeDamage(20, 10)
