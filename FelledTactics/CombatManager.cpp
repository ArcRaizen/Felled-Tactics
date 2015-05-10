#include "StdAfx.h"
#include "CombatManager.h"
#include "Level.h"

const float CombatManager::COMBAT_TEXT_LIFE = 1.0f;
const float CombatManager::BASE_PRE_COMBAT_WAIT = 0.5f;
const float CombatManager::BASE_MID_COMBAT_WAIT = 1.0f;
const float CombatManager::BASE_POST_COMBAT_WAIT = 2.0f;
const float CombatManager::BASE_MULTI_HIT_WAIT = 1.0f;

const std::string CombatManager::BASE_CALC_COMBAT_SCRIPT = "Lua\\Combat Scripts\\Base_CalcCombat.lua";
const std::string CombatManager::BASE_COMBAT_SCRIPT = "Lua\\Combat Scripts\\BaseCombat.lua";


CombatManager::CombatManager(void) : doCombat(false), range(0), combatTimer(0.0f), preCombatWait(BASE_PRE_COMBAT_WAIT), 
									midCombatWait(BASE_MID_COMBAT_WAIT), postCombatWait(BASE_POST_COMBAT_WAIT), multiHitWait(BASE_MULTI_HIT_WAIT),
									deathCombatWait(0.0f)
{

}

CombatManager::~CombatManager(void) { }

void CombatManager::CalculateCombat(lua_State* L)
{
	// Get range between combatants
	range = attacker->UnitPosition.DistanceTo(defender->UnitPosition);

	// Attacker
	lua_pushlightuserdata(L, (void*)this);
	lua_setglobal(L, "CombatMan");
	lua_pushlightuserdata(L, (void*)&attacker);
	lua_setglobal(L, "Combatant");
	lua_pushlightuserdata(L, (void*)&defender);
	lua_setglobal(L, "Target");
	lua_pushinteger(L, range);
	lua_setglobal(L, "Range");
	lua_pushboolean(L, true);
	lua_setglobal(L, "isAttacker");
	int test = attacker->CombatCalcScript == "" ? luaL_dofile(L, BASE_CALC_COMBAT_SCRIPT.c_str()) :
														luaL_dofile(L, attacker->CombatCalcScript.c_str());
#ifdef DEV_DEBUG
	if(test)
	{
		std::string error = lua_tostring(L, -1);
		test++;
	}
#endif

	// Defender
	lua_pushlightuserdata(L, (void*)this);
	lua_setglobal(L, "CombatMan");
	lua_pushlightuserdata(L, (void*)&defender);
	lua_setglobal(L, "Combatant");
	lua_pushlightuserdata(L, (void*)&attacker);
	lua_setglobal(L, "Target");
	lua_pushinteger(L, range);
	lua_setglobal(L, "Range");
	lua_pushboolean(L, false);
	lua_setglobal(L, "isAttacker");
	test = defender->CombatCalcScript == "" ? luaL_dofile(L, BASE_CALC_COMBAT_SCRIPT.c_str()) :
												luaL_dofile(L, defender->CombatCalcScript.c_str());

#ifdef DEV_DEBUG
	if(test)
	{
		std::string error = lua_tostring(L, -1);
		test++;
	}
#endif

	accuracyA = hitA - avoidD;
	accuracyD = hitD - avoidA;
}

// Do combat between 2 units. Return is who died: 0 = Neither, 1 = Attacker, 2 = Defender
void CombatManager::DoCombat()
{
	if(attacker == nullptr || defender == nullptr)
		return;

	combatTimer = 0.0f;
	combatPhase = 1;
	defenderDied = attackerDied = false;
}

// An ability has been cast
void CombatManager::DoAbility(UnitPtr u, Position p)
{
	combatTimer = 0.0f;
	combatPhase = 1;
	attacker = u;
	target = p;
	unitsKilledByAbility.clear();
}

void CombatManager::Reset(bool onlyDefender/* = false*/)
{
	if(!onlyDefender)
		attacker.Reset();
	defender.Reset();
	physicalDamageA = physicalDamageD = 0;
	magicalDamageA = magicalDamageD = 0;
	numAttackerHits = numDefenderHits = 1;
	attHitCount = defHitCount = 0;
	hitA = hitD = 0.0f;
	avoidA = avoidD = 0.0f;
	accuracyA = accuracyD = 0;

	// Reset combat timers
	preCombatWait = BASE_PRE_COMBAT_WAIT;
	midCombatWait = BASE_MID_COMBAT_WAIT;
	postCombatWait = BASE_POST_COMBAT_WAIT;
	multiHitWait = BASE_MULTI_HIT_WAIT;

	doCombat = false;
}

void CombatManager::ResetDefender()
{
	Reset(true);
}

// Save the combat parameters of the attacker
void CombatManager::SetCombatParametersAttacker(int attPhys, int attMag, int attHit, int attAvoid, int attNumHits)
{
	physicalDamageA = attPhys;
	magicalDamageA = attMag;
	hitA = attHit;
	avoidA = attAvoid;
	numAttackerHits = attNumHits;

	if(combatAugmentationScript != "")
	{
		int x = 5;
		x++;
	}
}

// Save the combat parameters of the defender
void CombatManager::SetCombatParametersDefender(int defPhys, int defMag, int defHit, int defAvoid, int defNumHits)
{
	physicalDamageD = defPhys;
	magicalDamageD = defMag;
	hitD = defHit;
	avoidD = defAvoid;
	numDefenderHits = defNumHits;
}

// Set the timers for the timers between combat phases
void CombatManager::SetCombatTimers(const float timers[4]) { SetCombatTimers(timers[0], timers[1], timers[2], timers[3]); }
void CombatManager::SetCombatTimers(float pre, float mid, float post, float multi)
{
	if(pre >= 0) preCombatWait = pre;
	if(mid >= 0) midCombatWait = mid;
	if(post >=0) postCombatWait = post;
	if(multi >=0)multiHitWait = multi;
}
void CombatManager::SetDeathCombatTimer(float deathTimer) { if(deathTimer > deathCombatWait) deathCombatWait = deathTimer; }

void CombatManager::SaveLevelPointer(LevelPtrW l) { level = l; }
void CombatManager::UnitKilledByAbility(int x, int y) { unitsKilledByAbility.push_back(Position(x,y)); }

// Complete a phase of combat between 2 units and return result
// 0 = no one died (yet), 1 = attacker died, 2 = defender died, 3 = no deaths
int CombatManager::UpdateCombat(float dt, lua_State* L)
{
	int test;
	std::string error;

	switch(combatPhase)
	{
		case 1:		// Wait period before combat occurs
			if(combatTimer < preCombatWait)
			{
				combatTimer += dt;
				return COMBAT_MANAGER_UPDATE_NULL;
			}
			combatPhase = 2;
		case 2:		// Attacker executing combat
			if(attHitCount > 0 && combatTimer < multiHitWait)
			{
				combatTimer += dt;
				return COMBAT_MANAGER_UPDATE_NULL;
			}

			// Attacker attacks defender
			lua_pushlightuserdata(L, (void*)this);
			lua_setglobal(L, "CombatMan");
			lua_pushlightuserdata(L, (void*)&level);
			lua_setglobal(L, "Level");
			lua_pushlightuserdata(L, (void*)&attacker);
			lua_setglobal(L, "Attacker");
			lua_pushlightuserdata(L, (void*)&defender);
			lua_setglobal(L, "Defender");
			lua_pushinteger(L, physicalDamageA);
			lua_setglobal(L, "PhysicalDamage");
			lua_pushinteger(L, magicalDamageA);
			lua_setglobal(L, "MagicaDamage");
			lua_pushinteger(L, accuracyA);
			lua_setglobal(L, "Accuracy");
			test = !attacker->CombatExecuteScript.compare("") ? luaL_dofile(L, BASE_COMBAT_SCRIPT.c_str()) :
																luaL_dofile(L, attacker->CombatExecuteScript.c_str());

#ifdef DEV_DEBUG
			if(test)
			{
				std::string error = lua_tostring(L, -1);
				test++;
			}
#endif

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

			return COMBAT_MANAGER_UPDATE_NULL;
		case 3:		// Wait period before defender retaliates
			if(combatTimer < midCombatWait)
			{
				combatTimer += dt;
				return COMBAT_MANAGER_UPDATE_NULL;
			}

			// End combat if defender died
			if(defender->CheckStatus(UNIT_STATUS_KILLED_IN_COMBAT))
			{
				defender->ClearBattleScripts();
				combatPhase = 8;
				combatTimer = 0.0f;
				Reset();
				return COMBAT_MANAGER_UPDATE_DEFENDER_DEAD;
			}

			combatPhase = 4;
		case 4:		// Defender executing combat
			if(defender->AttackRange >= range)
			{
				// Defender retaliates against Attacker
				lua_pushlightuserdata(L, (void*)this);
				lua_setglobal(L, "CombatMan");
				lua_pushlightuserdata(L, (void*)&level);
				lua_setglobal(L, "Level");
				lua_pushlightuserdata(L, (void*)&defender);
				lua_setglobal(L, "Attacker");
				lua_pushlightuserdata(L, (void*)&attacker);
				lua_setglobal(L, "Defender");
				lua_pushinteger(L, physicalDamageD);
				lua_setglobal(L, "PhysicalDamage");
				lua_pushinteger(L, magicalDamageD);
				lua_setglobal(L, "MagicaDamage");
				lua_pushinteger(L, accuracyD);
				lua_setglobal(L, "Accuracy");
				test = !defender->CombatExecuteScript.compare("") ? luaL_dofile(L, BASE_COMBAT_SCRIPT.c_str()) :
																		luaL_dofile(L, defender->CombatExecuteScript.c_str());
#ifdef DEV_DEBUG
				if(test)
				{
					std::string error = lua_tostring(L, -1);
					test++;
				}
#endif
			}

			combatTimer = 0.0f;
			combatPhase = 5;
		case 5:		// Wait period before attacker dies (if applicable) or attacks again
			if(combatTimer < midCombatWait)
			{
				combatTimer += dt;
				return COMBAT_MANAGER_UPDATE_NULL;
			}

			combatPhase = 7;	// End combat phase now by default

			// Attacker died - end combat phase and return his death
			if(attacker->CheckStatus(UNIT_STATUS_KILLED_IN_COMBAT))
			{
				attacker->ClearBattleScripts();
				combatTimer = 0.0f;
				Reset();
				combatPhase = 8;
				return COMBAT_MANAGER_UPDATE_ATTACKER_DEAD;
			}
			else if(false)		// Attacker has post-combat skill, proceed to phase to execute it
			{
				combatPhase = 6;
				return COMBAT_MANAGER_UPDATE_NULL;
			}
		case 6:	// Attack has extra attack
			return COMBAT_MANAGER_UPDATE_NULL;
		case 7:		// Wait period before combat finishes
			if(combatTimer < postCombatWait)
			{
				combatTimer += dt;
				return COMBAT_MANAGER_UPDATE_NULL;
			}

			// Combat has ended
			doCombat = false;
			combatTimer = 0.0f;
			combatPhase = 0;

			// Reset combat ability scripts
			if(attacker->CheckStatus(UNIT_STATUS_KILLED_IN_COMBAT))
				attacker->ClearBattleScripts();
			if(defender->CheckStatus(UNIT_STATUS_KILLED_IN_COMBAT))
				defender->ClearBattleScripts();
			combatAugmentationScript = "";

			Reset();
		case 8:		// Combat end
			if(combatTimer < deathCombatWait)
			{
				combatTimer += dt;
				return COMBAT_MANAGER_UPDATE_NULL;
			}

			combatTimer = 0.0f;
			deathCombatWait = 0.0f;
			combatPhase = 0;
			return COMBAT_MANAGER_UPDATE_COMBAT_END;
		default: return COMBAT_MANAGER_UPDATE_NULL;
	}
}

// Complete executing of an ability and return the positions
//		of units that were killed by the ability
// Return -1 during wait periods, -2 when ability is over
Position CombatManager::UpdateAbility(float dt, lua_State* L)
{
	switch(combatPhase)
	{
		case 1:		// Wait Period before ability executes
			if(combatTimer < preCombatWait)
			{
				combatTimer += dt;
				return Position(-1, -1);
			}
			combatPhase = 2;
		case 2:		// Execute ability
			lua_pushlightuserdata(L, (void*)&level);
			lua_setglobal(L, "Level");
			lua_pushlightuserdata(L, (void*)this);
			lua_setglobal(L, "CombatMan");
			attacker->ActivateAbility(L, target);
			
			// If no units died, end combat
			if(unitsKilledByAbility.size() == 0)
			{
				combatPhase = 5;
				combatTimer = 0.0f;
				return Position(-1, -1);
			}

			// Units died, proceed
			combatTimer = 0.0f;		// reset timer
			combatPhase = 3;
		case 3:		// Interim wait period between deaths
			if(combatTimer < midCombatWait)
			{
				combatTimer += dt;
				return Position(-1, -1);
			}

			// If more dead units, repeat previous phase
			if(unitsKilledByAbility.size() > 0)
				combatPhase = 4;
			else	// No more dead units, end combat
			{
				if(deathCombatWait > 0)
					combatPhase = 6;
				else
					combatPhase = 5;
				break;
			}
		case 4:		// Return position of next unit killed by Ability
		{
			combatTimer = 0.0f;		// Reset timer
			
			// Return position of dead unit for it to be removed
			Position p = unitsKilledByAbility.front();
			unitsKilledByAbility.erase(unitsKilledByAbility.begin());
			combatPhase = 3;		// return to wait phase
			return p;
		}
		case 5:		// Post ability/combat wait
			if(combatTimer < postCombatWait)
			{
				combatTimer += dt;
				return Position(-1, -1);
			}

			Reset();

			// End
			return Position(-2, -2);
		case 6:
			if(combatTimer < deathCombatWait)
			{
				combatTimer += dt;
				return Position(-1, -1);
			}

			Reset();
			deathCombatWait = 0.0f;

			// End
			return Position(-2, -2);
	}

	// Default return
	return Position(-1, -1);
}

#pragma region Properties
void	CombatManager::SetAttacker(UnitPtr a) { attacker = a; }
void	CombatManager::SetDefender(UnitPtr d) { defender = d; }
int		CombatManager::GetDamageA() { return physicalDamageA + magicalDamageA; }
int		CombatManager::GetDamageD() { return physicalDamageD + magicalDamageD; }
float	CombatManager::GetAccuracyA() { return accuracyA; }
float	CombatManager::GetAccuracyD() { return accuracyD; }
#pragma endregion