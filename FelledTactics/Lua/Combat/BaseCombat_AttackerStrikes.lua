local combatCalculator = LuaCombatCalculator(CombatCalc)
local level = LuaLevel(Level)

local attacker = LuaUnit(combatCalculator:GetAttacker())
local defender = LuaUnit(combatCalculator:GetDefender())

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
		combatCalculator:DefenderDies()
	end
else
	level:CreateCombatText(defenderX, defenderY, attackerX, attackerX, "MISS!", 0)
end
