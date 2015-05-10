local combatManager = LuaCombatManager(CombatMan)
local level = LuaLevel(Level)

local attacker = LuaUnit(Attacker)
local defender = LuaUnit(Defender)

-- Get Positions of the combatants
attackerX, attackerY = attacker:GetPosition()
defenderX, defenderY = defender:GetPosition()

-- Execute combat
math.randomseed(os.time())
RNG = math.random(100)
if RNG < Accuracy then
	damage = defender:TakeDamage(PhysicalDamage, MagicalDamage)
	level:CreateCombatText(defenderX, defenderY, attackerX, attackerY, math.abs(damage), 0)
else
	level:CreateCombatText(defenderX, defenderY, attackerX, attackerX, "MISS!", 0)
end
