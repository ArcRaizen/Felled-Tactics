--[[
		Calculate basic combat statistics for a single unit.
		Unit is passed in to script as Combatant.
		Unit being attacked is passed in to script as Target
		CombatCalculator is passed in to script as CombatCalc
		Integer "Range" is passed in telling the distance between the Combatant and Target
		Boolean "isAttacker" is passed in to signify if the combatant is the attacker or defender
--]]

local combatCalculator = LuaCombatCalculator(CombatCalc)
local combatant = LuaUnit(Combatant)
local target = LuaUnit(Target)

-- Get base combat damage done by the attacker
physicalDamage, magicalDamage = combatant:CalculateBaseCombatDamage(Range)

-- Calculate actual damage done to target
physicalDamage = physicalDamage - target.Defense
magicalDamage = magicalDamage - target.Resistance
physicalDamage = physicalDamage < 0 and 0 or physicalDamage		-- damage done cannot be negative
magicalDamage = magicalDamage < 0 and 0 or magicalDamage		-- http://bit.ly/1w13X4A

-- Calculate attacker hit chance
hit = (combatant.Skill * 2) + (combatant.Agility * 0.5) + 60

-- Calculate avoid chance
avoid = (combatant.Agility * 2) + (combatant.Skill * 0.5)

-- Set
if isAttacker == true then
	combatCalculator:SetCombatParametersAttacker(physicalDamage/2, magicalDamage/2, hit, avoid, 5)
else
	combatCalculator:SetCombatParametersDefender(physicalDamage, magicalDamage, hit, avoid, 1)
end

