#pragma once
#ifndef COMBATMANAGER_H
#define COMBATMANAGER_H

#ifndef LEVEL_H
//#include "Level.h"
#endif
#ifndef UNIT_H
#include "Unit.h"
#endif

#ifndef COMBATMANAGERUPDATECODES_C
#include "CombatManagerUpdateCodes.h"
#endif

#include "Luna.h"
#include <vector>

class Level;
class CombatManager
{
public:
	CombatManager(void);
	~CombatManager(void);

	void	CalculateCombat(lua_State* L);
	void	DoCombat();
	void	DoAbility(SmartPointer<Unit> u, Position p);
	void	Reset(bool onlyDefender = false);
	void	ResetDefender();
	void	SetCombatParametersAttacker(int attPhys, int attMag, int attHit, int attAvoid, int attNumHits);
	void	SetCombatParametersDefender(int defPhys, int defMag, int defHit, int defAvoid, int defNumHits);
	void	SetCombatTimers(float pre, float mid, float post, float multi);
	void	SetCombatTimers(const float timers[4]);
	void	SaveLevelPointer(SmartPointer<Level> l);

	void	DefenderDied();
	void	AttackerDied();
	void	UnitKilledByAbility(int x, int y);

	int		 UpdateCombat(float dt, lua_State* L);
	Position UpdateAbility(float dt, lua_State* L);

#pragma region Properties
	__declspec(property(get=GetAttacker, put=SetAttacker)) SmartPointer<Unit> Attacker;	SmartPointer<Unit> GetAttacker();	void SetAttacker(SmartPointer<Unit> a);
	__declspec(property(get=GetDefender, put=SetDefender)) SmartPointer<Unit> Defender;	SmartPointer<Unit> GetDefender();	void SetDefender(SmartPointer<Unit> d);
	__declspec(property(get=GetDamageA)) int DamageA;		int		GetDamageA();
	__declspec(property(get=GetDaamgeD)) int DamageD;		int		GetDamageD();
	__declspec(property(get=GetAccuracyA)) float AccuracyA;	float	GetAccuracyA();
	__declspec(property(get=GetAccuracyD)) float AccuracyD; float	GetAccuracyD();
#pragma endregion

private:
	SmartPointer<Unit>	attacker;				// attacking unit
	SmartPointer<Unit>	defender;				// defending unit
	bool				doCombat;				// is combat occuring right now? (do update function?)

	// Combat Text displays
	SmartPointer<Level>	level;
	char				battleText[10];			// buffer to hold text form of damage numbers during combat

	// Combat timer values
	static const float  COMBAT_TEXT_LIFE;		// how long each combat text entry lasts / is displayed
	static const float  BASE_PRE_COMBAT_WAIT;	// time after combat is initiated before attacker first strikes
	static const float	BASE_MID_COMBAT_WAIT;	// time after attacker first strikes before defender retaliates
	static const float	BASE_POST_COMBAT_WAIT;	// time after final attack before combat completely ends
	static const float  BASE_MULTI_HIT_WAIT;	// time between multiple hits from a single combatant
	float	preCombatWait;
	float	midCombatWait;
	float	postCombatWait;
	float	multiHitWait;

	// Default Combat scripts
	static const std::string BASE_CALC_COMBAT_SCRIPT;
	static const std::string BASE_COMBAT_ATTACKER_STRIKES_SCRIPT;
	static const std::string BASE_COMBAT_DEFENDER_STRIKES_SCRIPT;

	// Optional script for Augmentator Abilities
	std::string combatAugmentationScript;

	// Combat Phase + timer
	float combatTimer;	// timer to keep track of when each combat phase initiates/ends
	int	  combatPhase;	// keep track of which combat phase we're in during update (for a switch statement)
	
	// Combat results (for return values)
	bool defenderDied;
	bool attackerDied;
	vector<Position> unitsKilledByAbility;
	
	// Battle Statistics
	int		physicalDamageA, physicalDamageD;			// Physical Damage delt
	int		magicalDamageA, magicalDamageD;				// Magical Damage delt
	int		numAttackerHits, numDefenderHits;			// Num of times each combatant attacks (default 1)
	int		attHitCount, defHitCount;					// Counter to track number of attacks on each side
	float	hitA, hitD;									// Percent chance to hit with attack
	float	avoidA, avoidD;								// Perecent chance to dodge attack
	int		range;

	// Derived Statistics
	float	accuracyA, accuracyD;						// Overall accuracy of attack (hit - avoid)

	// Ability Casting
	Position target;
};
#endif
