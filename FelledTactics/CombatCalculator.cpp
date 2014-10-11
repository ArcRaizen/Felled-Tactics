#include "StdAfx.h"
#include "CombatCalculator.h"

const float CombatCalculator::COMBAT_TEXT_LIFE = 1.0f;
const float CombatCalculator::PRE_COMBAT_WAIT = 0.5f;
const float CombatCalculator::MID_COMBAT_WAIT = 1.0f;
const float CombatCalculator::POST_COMBAT_WAIT = 2.0f;
const float CombatCalculator::MULTI_HIT_WAIT = 1.0f;

const std::string CombatCalculator::BASE_CALC_COMBAT_SCRIPT = "Lua\\Combat\\Base_CalcCombat.lua";
const std::string CombatCalculator::BASE_COMBAT_ATTACKER_STRIKES_SCRIPT = "Lua\\Combat\\BaseCombat_AttackerStrikes.lua";
const std::string CombatCalculator::BASE_COMBAT_DEFENDER_STRIKES_SCRIPT = "Lua\\Combat\\BaseCombat_DefenderStrikes.lua";

CombatCalculator::CombatCalculator(void)
{
	attacker = NULL;
	defender = NULL;
	doCombat = false;
	range = 0;
	combatTimer = 0.0f;
}

CombatCalculator::~CombatCalculator(void)
{
}

// Calculate statistics for Combat
void CombatCalculator::CalculateCombat()
{
	if(attacker == NULL || defender == NULL)
		return;

	// Get range between combaters
	range = attacker->UnitPosition.DistanceTo(defender->UnitPosition);;

	// Calculate damage delt by attacker
	attacker->CalculateCombatDamage(basePhysicalDamageA, baseMagicalDamageA, range);
	physicalDamageA = defender->Defense > basePhysicalDamageA ? 0 : basePhysicalDamageA - defender->Defense;
	magicalDamageA = defender->Resistance > baseMagicalDamageA ? 0 : baseMagicalDamageA - defender->Resistance;

	// Calculate if attacker will hit defender
	hitA = (float)(attacker->Skill * 2) + ((float)attacker->Agility * 0.5f) + /*WEAPON HIT*/60;
	avoidA = (float)(defender->Agility * 2) + ((float)defender->Skill * 0.5f);
	accuracyA = hitA > avoidA ? hitA - avoidA : 0;

	// Calculate damage delt by defender
	defender->CalculateCombatDamage(basePhysicalDamageD, baseMagicalDamageD, range);
	physicalDamageD = defender->Defense > basePhysicalDamageD ? 0 : basePhysicalDamageD - defender->Defense;
	magicalDamageD = defender->Resistance > baseMagicalDamageD ? 0 : baseMagicalDamageD - defender->Resistance;

	// Calculate if defender will hit attacker
	hitD = (float)(defender->Skill * 2) + ((float)defender->Agility * 0.5f) + /*WEAPON HIT*/60;
	avoidD = (float)(attacker->Agility *2) + ((float)attacker->Skill * 0.5f);
	accuracyD = hitD > avoidD ? hitD - avoidD : 0;
}

void CombatCalculator::CalculateCombat(lua_State* L)
{
	// Get range between combatants
	range = attacker->UnitPosition.DistanceTo(defender->UnitPosition);

	// Attacker
	lua_pushlightuserdata(L, (void*)this);
	lua_setglobal(L, "CombatCalc");
	lua_pushlightuserdata(L, (void*)attacker);
	lua_setglobal(L, "Combatant");
	lua_pushlightuserdata(L, (void*)defender);
	lua_setglobal(L, "Target");
	lua_pushinteger(L, range);
	lua_setglobal(L, "Range");
	lua_pushboolean(L, true);
	lua_setglobal(L, "isAttacker");
	if(attacker->CombatCalcScript == "")
		luaL_dofile(L, BASE_CALC_COMBAT_SCRIPT.c_str());
	else
		luaL_dofile(L, attacker->CombatCalcScript.c_str());

	// Defender
	lua_pushlightuserdata(L, (void*)this);
	lua_setglobal(L, "CombatCalculator");
	lua_pushlightuserdata(L, (void*)defender);
	lua_setglobal(L, "Combatant");
	lua_pushlightuserdata(L, (void*)attacker);
	lua_setglobal(L, "Target");
	lua_pushinteger(L, range);
	lua_setglobal(L, "Range");
	lua_pushboolean(L, false);
	lua_setglobal(L, "isAttacker");
	if(defender->CombatCalcScript == "")
		luaL_dofile(L, BASE_CALC_COMBAT_SCRIPT.c_str());
	else
		luaL_dofile(L, defender->CombatCalcScript.c_str());

	accuracyA = hitA - avoidD;
	accuracyD = hitD - avoidA;
}

// Do combat between 2 units. Return is who died: 0 = Neither, 1 = Attacker, 2 = Defender
void CombatCalculator::DoCombat()
{
	if(attacker == NULL || defender == NULL)
		return;
	
	doCombat = true;
	combatTimer = 0.0f;
	combatPhase = 1;
	defenderDied = attackerDied = false;
}

void CombatCalculator::DefenderDied()
{
	defenderDied = true;
}

void CombatCalculator::AttackerDied()
{
	attackerDied = true;
}

void CombatCalculator::Reset(bool onlyDefender/* = false*/)
{
	if(!onlyDefender)
		attacker = NULL;
	defender = NULL;
	basePhysicalDamageA = basePhysicalDamageD = 0;
	baseMagicalDamageA = baseMagicalDamageD = 0;
	physicalDamageA = physicalDamageD = 0;
	magicalDamageA = magicalDamageD = 0;
	physicalModifier = magicalModifier = 1.0f;
	numAttackerHits = numDefenderHits = 1;
	attHitCount = defHitCount = 0;
	hitA = hitD = 0.0f;
	avoidA = avoidD = 0.0f;
	accuracyA = accuracyD = 0;
	RNG = -1;

	doCombat = false;
}

void CombatCalculator::ResetDefender()
{
	Reset(true);
}

void CombatCalculator::SetCombatParametersAttacker(int attPhys, int attMag, int attHit, int attAvoid, int attNumHits)
{
	physicalDamageA = attPhys;
	magicalDamageA = attMag;
	hitA = attHit;
	avoidA = attAvoid;
	numAttackerHits = attNumHits;
}

void CombatCalculator::SetCombatParametersDefender(int defPhys, int defMag, int defHit, int defAvoid, int defNumHits)
{
	physicalDamageD = defPhys;
	magicalDamageD = defMag;
	hitD = defHit;
	avoidD = defAvoid;
	numDefenderHits = defNumHits;
}

void CombatCalculator::SetCombatTextCallback(Level* l, void (Level::*func)(Position, Position, const char*, int))
{
	level = l;
	CreateCombatText = func;
}

// Complete a phase of combat between 2 units and return result
// 0 = no one died (yet), 1 = attacker died, 2 = defender died, 3 = no deaths
int CombatCalculator::Update(float dt)
{
	if(!doCombat)
		return -1;

	switch(combatPhase)
	{
/**/	case 1:	// Wait period before combat occurs
			if(combatTimer < PRE_COMBAT_WAIT)
			{
				combatTimer += dt;
				return 0;
			}
			combatPhase = 2;
/**/	case 2: // Attacker executing combat
			if(attHitCount > 0 && combatTimer < MULTI_HIT_WAIT)
			{
				combatTimer += dt;
				return 0;
			}

			// Attacker hits defender
			int damage;
			RNG = rand() % 100;
			if(RNG < accuracyA)
			{
				// Defender takes damage
				damage = defender->TakeDamage(physicalDamageA * physicalModifier, magicalDamageA * magicalModifier);
				itoa(damage, battleText, 10);
				((level)->*(CreateCombatText))(defender->UnitPosition, attacker->UnitPosition, battleText, 0);

				// Defender killed - Only check after all attacks have finished
				if(defender->Health == 0)
					defenderDied = true;
			}
			else	// WHIFF
				((level)->*(CreateCombatText))(defender->UnitPosition, attacker->UnitPosition, "MISS!", 0);

			// If the attacker is executing multiple hits, repeat this step appropriately
			if(numAttackerHits - attHitCount > 1)
			{
				attHitCount++;
				combatTimer = 0.0f;
			}
			else	// Proceed to next combat phase
			{
				combatTimer = 0.0f;
				combatPhase = 3;
			}

			return 0;
/**/	case 3:	// Wait period before defender retaliates
			if(combatTimer < MID_COMBAT_WAIT)
			{
				combatTimer += dt;
				return 0;
			}

			// Check if defender died
			if(defenderDied)
				return 2;

			combatPhase = 4;
/**/	case 4: // Defender executing combat
			if(defender->AttackRange >= range) // Defender might not be able to retaliate
			{
				RNG = rand() % 100;
				if(RNG < accuracyD)
				{
					// Attacker takes damage
					damage = attacker->TakeDamage(physicalDamageD, magicalDamageD);
					itoa(damage, battleText, 10);
					((level)->*(CreateCombatText))(attacker->UnitPosition, defender->UnitPosition, battleText, 0);

					// Attacker killed
					if(attacker->Health == 0)
						attackerDied = true;
				}
				else // WHIFF
					((level)->*(CreateCombatText))(attacker->UnitPosition, defender->UnitPosition, "MISS!", 0);
			}

			// If attacker has post-combat skill, proceed to phase to execute it
			if(/*attacker->HasSkill("Follow-Up Cut")*/true)
			{
				combatPhase = 5;
				combatTimer = 0.0f;
				return 0;
			}

			combatTimer = 0.0f;
			combatPhase = 6;
/**/	case 6: // Wait period before combat finishes
			if(combatTimer < POST_COMBAT_WAIT)
			{
				combatTimer += dt;
				return 0;
			}

			// Combat has ended
			doCombat = false;
			combatTimer = 0.0f;
			combatPhase = 0;

			// Check if attacker died
			if(attackerDied)
				return 1;
			else if(defenderDied)
				return 2;

			// No one died
			return 3;
/**/	case 5:	// Optional wait for post-combat skills
			if(combatTimer < MID_COMBAT_WAIT)
			{
				combatTimer += dt;
				return 0;
			}

			// Check which post-combat skill and execute it


			RNG = rand() % 100;
			if(RNG < accuracyA)
			{
				// Defender takes damage
				damage = defender->TakeDamage(physicalDamageA * physicalModifier, magicalDamageA * magicalModifier);
				itoa(damage, battleText, 10);
				((level)->*(CreateCombatText))(defender->UnitPosition, attacker->UnitPosition, battleText, 0);

				// Defender killed - Only check after all attacks have finished
				if(defender->Health == 0)
					defenderDied = true;
			}
			else	// WHIFF
				((level)->*(CreateCombatText))(defender->UnitPosition, attacker->UnitPosition, "MISS!", 0);

			// Proceed to end of combat
			combatTimer = 0.0f;
			combatPhase = 6;
		default: return 0;
	}
}

int CombatCalculator::Update2(float dt, lua_State* L)
{
	if(!doCombat)
		return -1;

	int test;
	std::string error;

	switch(combatPhase)
	{
		case 1:		// Wait period before combat occurs
			if(combatTimer < PRE_COMBAT_WAIT)
			{
				combatTimer += dt;
				return 0;
			}
			combatPhase = 2;
		case 2:		// Attacker executing combat
			if(attHitCount > 0 && combatTimer < MULTI_HIT_WAIT)
			{
				combatTimer += dt;
				return 0;
			}

			// Attacker attacks defender
			lua_pushlightuserdata(L, (void*)this);
			lua_setglobal(L, "CombatCalc");
			lua_pushlightuserdata(L, (void*)level);
			lua_setglobal(L, "Level");
			lua_pushinteger(L, physicalDamageA);
			lua_setglobal(L, "PhysicalDamage");
			lua_pushinteger(L, magicalDamageA);
			lua_setglobal(L, "MagicaDamage");
			lua_pushinteger(L, accuracyA);
			lua_setglobal(L, "Accuracy");
			if(attacker->CombatExecuteScript == "")
				luaL_dofile(L, BASE_COMBAT_ATTACKER_STRIKES_SCRIPT.c_str());
			else
				luaL_dofile(L, attacker->CombatExecuteScript.c_str());

			// If attacker is executing multiple hits, repeate this step appropriately
			if(numAttackerHits - attHitCount > 1)
			{
				attHitCount++;
				combatTimer = 0.0f;
			}
			else	// Proceed to next combat phase
			{
				combatTimer = 0.0f;
				combatPhase = 3;
			}

			return 0;
		case 3:		// Wait period before defender retaliates
			if(combatTimer < MID_COMBAT_WAIT)
			{
				combatTimer += dt;
				 return 0;
			}

			// End combat if defender died
			if(defenderDied)
				return 2;

			combatPhase = 4;
		case 4:		// Defender executing combat
			if(defender->AttackRange >= range)
			{
				// Defender retaliates against Attacker
				lua_pushlightuserdata(L, (void*)this);
				lua_setglobal(L, "CombatCalc");
				lua_pushlightuserdata(L, (void*)level);
				lua_setglobal(L, "Level");
				lua_pushinteger(L, physicalDamageD);
				lua_setglobal(L, "PhysicalDamage");
				lua_pushinteger(L, magicalDamageD);
				lua_setglobal(L, "MagicaDamage");
				lua_pushinteger(L, accuracyD);
				lua_setglobal(L, "Accuracy");
				if(defender->CombatExecuteScript == "")
					luaL_dofile(L, BASE_COMBAT_DEFENDER_STRIKES_SCRIPT.c_str());
				else
					luaL_dofile(L, defender->CombatExecuteScript.c_str());
			}

			// If Attacker has post-combat skill, proceed to phase to execute it
			if(false)
			{
				combatPhase = 5;
				combatTimer = 0.0f;
				return 0;
			}

			combatTimer = 0.0f;
			combatPhase = 6;
		case 6:		// Wait period before combat finishes
			if(combatTimer < POST_COMBAT_WAIT)
			{
				combatTimer += dt;
				return 0;
			}

			// Combat has ended
			doCombat = false;
			combatTimer = 0.0f;
			combatPhase = 0;

			// Reset combatant combat ability scripts
			attacker->ClearBattleScripts();
			defender->ClearBattleScripts();

			// Check for deaths
			if(attackerDied)
				return 1;
			else if(defenderDied)
				return 2;

			// No one died
			return 3;
		case 5:
			return 0;
		default: return 0;
	}
}

#pragma region Properties
Unit*	CombatCalculator::GetAttacker() { return attacker; }
void	CombatCalculator::SetAttacker(Unit* a) { attacker = a; }
Unit*	CombatCalculator::GetDefender() { return defender; }
void	CombatCalculator::SetDefender(Unit* d) { defender = d; }
int		CombatCalculator::GetDamageA() { return physicalDamageA + magicalDamageA; }
int		CombatCalculator::GetDamageD() { return physicalDamageD + magicalDamageD; }
float	CombatCalculator::GetAccuracyA() { return accuracyA; }
float	CombatCalculator::GetAccuracyD() { return accuracyD; }
#pragma endregion