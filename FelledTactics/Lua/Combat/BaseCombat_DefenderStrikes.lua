local combatCalculator = LuaCombatCalculator(CombatCalc)
local level = LuaLevel(Level)

local attacker = LuaUnit(combatCalculator:GetDefender())
local defender = LuaUnit(combatCalculator:GetAttacker())

-- Get Positions of the combatants
attackerX, attackerY = attacker:GetPosition()
defenderX, defenderY = defender:GetPosition()

-- Execute combat
math.randomseed(os.time())
RNG = math.random(100)
if RNG < Accuracy then
	damage = defender:TakeDamage(PhysicalDamage, MagicalDamage)
	level:CreateCombatText(defenderX, defenderY, attackerX, attackerY, damage, 0)

	if(defender.Health == 0) then
		combatCalculator:AttackerDies()
	end
else
	level:CreateCombatText(defenderX, defenderY, attackerX, attackerY, "MISS!", 0)
end
